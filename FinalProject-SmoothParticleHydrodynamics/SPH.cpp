#include <iostream>

#include "Integrator.h"
#include "particle.h"
#include "ThreadPool.h"
#include "FileReader.h"

#define _USE_MATH_DEFINES
#include <cmath>

static std::vector<std::shared_ptr<Particle>> s_AllParticles;
static float s_h;
static float s_Gamma = 1.4;
static float s_K = 1.0;

int DiracDelta(int a, int b)
{
    return a==b;
}

void KernalFunction(double q, double h, double& ReturnValue, double& derivative) //(pos of a - pos of b, smoothing length)
{
    if(q <= 1)
    {
        ReturnValue = 1 - (3.0/2.0 * std::pow(q,2)) + (3.0/4.0 * std::pow(q,3));
        derivative =  -1 * (3.0 * q + (9.0/4.0)*std::pow(q,2));
    }
    else if(q <= 2)
    {
        ReturnValue = 0.25 * std::pow(2.0-q,3);
        derivative = -0.75 * std::pow(2.0 - q, 2);
    }
    else
    {
        ReturnValue = 0;
        derivative = 0;
    }

    ReturnValue = ReturnValue * (2.0/(3*h));
    derivative = derivative * (-2.0/(3*std::pow(h,2))); 
}

void KernalEvaulation(int a)
{
    auto TargetParticle = s_AllParticles[a];
    std::vector<Kernal> KernalEvaulations;
    TargetParticle->ClearKernal();
    for(int b = 0; b < s_AllParticles.size(); b++)
    {
        double RadiusDiff = TargetParticle->GetX() - s_AllParticles[b]->GetX();
        double ReturnValue = 0;
        double DerivativeValue = 0;
        double q = std::abs(RadiusDiff) / s_h;
        KernalFunction(q, s_h, ReturnValue, DerivativeValue);
        KernalEvaulations.push_back({ReturnValue, DerivativeValue});
    }
    TargetParticle->UpdateKernal(KernalEvaulations);
}

void EnergyEvaluation(int a)
{
    //In 1D
    //Pressure/Density * sum(mass of b * (velocity of a - velocity of b) * (kernal vector / magnitude of kernal)
    auto TargetParticle = s_AllParticles[a];
    auto Kernal = TargetParticle->GetKernal();
    auto Pressure = TargetParticle->GetP();
    auto Density = TargetParticle->GetRho();
    auto VelocityA = TargetParticle->GetV();
    double Energy = 0.0;
    double r = 0.0;
    for(int b = 0; b < s_AllParticles.size(); b++)
    {
        if(Kernal[b].first == 0.0) {continue;}
        r = s_AllParticles[b]->GetX() - TargetParticle->GetX();
        if(r == 0.0) {continue;}
        double Mass = s_AllParticles[b]->GetMass();
        double VelocityB = s_AllParticles[b]->GetV();
        double EAB = Mass * (VelocityA - VelocityB) * Kernal[b].second * (r / std::abs(r)) * (1 - DiracDelta(a,b));
        Energy += EAB;
    }
    Energy = Energy * (Pressure * std::pow(Density,-2));

    //TargetParticle->UpdateThermalEnergy(Energy);
    TargetParticle->TemporyInternalEnergyGradient = Energy;
}

void InitialEnergyEvaluation(int a, double* ReturnValue)
{
    auto TargetParticle = s_AllParticles[a];
    double P = TargetParticle->GetP();
    double Rho = TargetParticle->GetRho();
    double U = P / Rho;

    TargetParticle->UpdateKineticEnergy(0.0);
    TargetParticle->TemporyInternalEnergyGradient = 0.0;
    TargetParticle->UpdateThermalEnergy(U);

    *ReturnValue = U;
}

void KineticEvaluation(int a)
{
    auto TargetParticle = s_AllParticles[a];
    double Velocity = TargetParticle->GetV();
    double Mass = TargetParticle->GetMass();
    double KE = 0.5 * Mass * std::pow(Velocity,2);
    TargetParticle->UpdateKineticEnergy(KE);
}

void AccelerationEvaluation(int a)
{
    //In 1D
    //-1 * sum(mass of b * (Pressure of a/density of a^2 + Pressure of b/density of b^2) * (kernal vector / magnitude of kernal))
    auto TargetParticle = s_AllParticles[a];
    auto Kernal = TargetParticle->GetKernal();
    auto Pressure = TargetParticle->GetP();
    auto Density = TargetParticle->GetRho();
    double ValA = Pressure * (1 / std::pow(Density,2));
    double acceleration = 0.0;
    double r = 0.0;
    for(int b = 0; b < s_AllParticles.size(); b++)
    {
        if(Kernal[b].first == 0.0) {continue;}
        r = s_AllParticles[b]->GetX() - TargetParticle->GetX();
        if(r == 0.0) {continue;}
        double Mass = s_AllParticles[b]->GetMass();
        double ValB = s_AllParticles[b]->GetP() * (1 / std::pow(s_AllParticles[b]->GetRho(),2));
        double AccAB = Mass * (ValA + ValB) * Kernal[b].second * (r / std::abs(r)) * (1 - DiracDelta(a,b));
        acceleration += AccAB;
    }

    TargetParticle->UpdateA(-1.0 * acceleration);
}

void DensityEvauluation(int a)
{
    //In ID
    //sum(mass of b * kernal function)
    auto TargetParticle = s_AllParticles[a];
    auto Kernal = TargetParticle->GetKernal();
    double density = 0.0;
    for(int b = 0; b < s_AllParticles.size(); b++)
    {
        double Mass = s_AllParticles[b]->GetMass();
        density += Kernal[b].first * Mass;
    }
    TargetParticle->UpdateRho(density);
}

void PressureEvaulation(int a)
{
    //Pressure = rho * internal energy
    auto TargetParticle = s_AllParticles[a];
    double density = TargetParticle->GetRho();
    auto[KineticEnergy, ThermalEnergy] = TargetParticle->GetRecentEnergy();
    double Pressure = density * ThermalEnergy;
    TargetParticle->UpdateP(Pressure);
}

void PolytropicPressureEvaluation(int a)
{
    //P = K * rho^gamma
    auto TargetParticle = s_AllParticles[a];
    double density = TargetParticle->GetRho();
    double Pressure = s_K * std::pow(density, s_Gamma);
    TargetParticle->UpdateP(Pressure);
}

void InitialConditions(int a)
{
    KernalEvaulation(a);
    DensityEvauluation(a);
    PolytropicPressureEvaluation(a);
    //AccelerationEvaluation(a);
}

//void Step(int FunctionId, std::unique_ptr<VelocityVerlet> Integrator, bool Positions, bool Kernal, bool Energy) //FunctionId serves no purpose but to work with the ThreadPool
//{
//    Integrator->DoStep(Positions, Kernal, Energy);
//}

int main(int argc, char* argv[])
{

    //Read in file
    FileReader Input;
    std::string file = "";
    if(argc < 2)
    {
        std::cout << "Please provide a file" << std::endl;
        return 0;
    }
    file = argv[1];
    if(!Input.LoadFile(file))
    {
        std::cout << "Failed to load file" << std::endl;
        return 0;
    }
    //Distribute particles

    auto RegionData = Input.GetRegionData();
    auto SetupData = Input.GetSetupData();

    OutputLockGuard* OLG = new OutputLockGuard;
    std::shared_ptr<FileOutput> Writer = std::make_shared<FileOutput>(SetupData.OutputFile, OLG);

    unsigned int ParticleCount = SetupData.Particles;
    s_h = SetupData.SmoothingLength;

    auto PhysicalBoundaries = SetupData.PhysicalBoundaries;
    std::sort(PhysicalBoundaries.begin(), PhysicalBoundaries.end(), [](std::array<double,3> a, std::array<double,3> b) {return a[0] < b[0];});
    unsigned int SystemSize = PhysicalBoundaries[PhysicalBoundaries.size() - 1][0] - PhysicalBoundaries[0][0];
    std::vector<unsigned int> RegionSizes = {};
    for(auto R : RegionData)
    {
        auto Boundaries = R.Boundaries;
        std::sort(Boundaries.begin(), Boundaries.end(), [](std::array<double,3> a, std::array<double,3> b) {return a[0] < b[0];});
        RegionSizes.push_back(Boundaries[Boundaries.size() - 1][0] - Boundaries[0][0]);
    }
    for (int i = 0; i < ParticleCount; i++)
    {
        s_AllParticles.push_back(std::make_shared<Particle>());
    }

    std::vector<std::pair<double,int>> DensityRatio = {};
    std::vector<std::pair<double,int>> Density = {}; 
    for(int i = 0; i < RegionSizes.size(); i++)
    {   
        Density.push_back({RegionData[i].InitialDensity,i});
    }
    std::sort(Density.begin(), Density.end(), [](std::pair<double,int> a, std::pair<double,int> b) {return a.first < b.first;});
    for(int i = 0; i < Density.size(); i++)
    {
        DensityRatio.push_back({Density[i].first / Density[0].first, Density[i].second});
    }
    double TotalDensityRatio = 0.0;
    for(auto D : DensityRatio)
    {
        TotalDensityRatio += D.first;
    }
    std::vector<std::pair<double,int>> ParticleDensity = {};
    for(int i = 0; i < RegionData.size(); i++)
    {
        ParticleDensity.push_back({DensityRatio[i].first * ParticleCount / TotalDensityRatio, DensityRatio[i].second});
    }
    std::sort(ParticleDensity.begin(), ParticleDensity.end(), [](std::pair<double,int> a, std::pair<double,int> b) {return a.second < b.second;});
    int ParticleIndexOffset = 0;
    for(int i = 0; i < RegionData.size(); i++)
    {
        auto Boundaries = RegionData[i].Boundaries;
        std::sort(Boundaries.begin(), Boundaries.end(), [](std::array<double,3> a, std::array<double,3> b) {return a[0] < b[0];});
        for(int j = ParticleIndexOffset; j < ParticleDensity[i].first + ParticleIndexOffset; j++)
        {
            double StepSize = (Boundaries[Boundaries.size() - 1][0] - Boundaries[0][0]) / (ParticleDensity[i].first + 1);
            s_AllParticles[j]->UpdateX(Boundaries[0][0] + (j+1 - ParticleIndexOffset) * StepSize);
            s_AllParticles[j]->UpdateM(1.0);
            //std::cout << s_All
        }
        ParticleIndexOffset += ParticleDensity[i].first;
    }

    //Setup Initial Conditions
    ThreadPool Pool(16);
    for(int i = 0; i < s_AllParticles.size(); i++)
    {
        Pool.QueueJob(std::bind(InitialConditions, i), i);
        //InitialConditions(i);
    }
    Pool.start();
    while(Pool.Busy()){}
    Pool.Pause();
    //Pool.Stop();
    for(int i = 0; i < s_AllParticles.size(); i++)
    {
        Pool.QueueJob(std::bind(AccelerationEvaluation, i), i);
        //InitialConditions(i);
    }
    Pool.Resume();
    //Pool.start();
    while(Pool.Busy()){}
    //Pool.Stop();
    Pool.Pause();

    double* EnergyValues = (double*)malloc(sizeof(double) * s_AllParticles.size());
    double* EnergyValuesPtr = EnergyValues;
    for(int i = 0; i < s_AllParticles.size(); i++)
    {
        Pool.QueueJob(std::bind(InitialEnergyEvaluation, i, EnergyValuesPtr), i);
        EnergyValuesPtr++;
    }
    Pool.Resume();
    while(Pool.Busy()){}
    Pool.Stop();
    double TotalEnergy = 0.0;
    EnergyValuesPtr = EnergyValues;
    for(int i = 0; i < s_AllParticles.size(); i++)
    {
        //std::cout << *EnergyValuesPtr << std::endl;
        TotalEnergy += *EnergyValuesPtr;
        EnergyValuesPtr++;
    }

    EnergyValuesPtr = nullptr;
    free(EnergyValues);
    std::cout << "Total Initial Energy: " << TotalEnergy << std::endl;

    //Setup Integrators
    std::shared_ptr<IntergatorLockGuard> ILG = std::make_shared<IntergatorLockGuard>(s_AllParticles.size());
    std::vector<std::shared_ptr<VelocityVerlet>> Integrators;
    for(int i = 0; i < s_AllParticles.size(); i++)
    {
        Integrators.push_back(std::make_shared<VelocityVerlet>(
            VelocityVerlet::SetupData{
                i,
                s_AllParticles[i],
                SetupData.timestep,
                Writer
            },
            ILG,
            std::vector<EvaluationFunction>{
                KernalEvaulation,
                DensityEvauluation,
                //PolytropicPressureEvaluation,
                PressureEvaulation,
                AccelerationEvaluation,
                EnergyEvaluation,
                KineticEvaluation
            }
        ));
    }

    //Run Simulation
    Pool.start();
    Pool.Pause();
    auto OutputThread = std::thread(std::bind(&FileOutput::WriteData, Writer.get()));
    for(int i = 0; i < SetupData.steps; i++)
    {
        ILG->Reset();
        for(int j = 0; j < s_AllParticles.size(); j++)
        {
            Pool.QueueJob(std::bind(&VelocityVerlet::DoStep, Integrators[j].get(), true, false, true), j);
        }
        Pool.Resume();
        while(Pool.Busy()){};
        while(!ILG->Wait()){};
        Pool.Pause();
        ILG->Reset();
        for(int j = 0; j < s_AllParticles.size(); j++)
        {
            Pool.QueueJob(std::bind(&VelocityVerlet::DoStep, Integrators[j].get(), false, true, true), j);
        }
        Pool.Resume();
        while(Pool.Busy()){};
        while(!ILG->Wait()){};
        Pool.Pause();
        ILG->Reset();
        for(int j = 0; j < s_AllParticles.size(); j++)
        {
            Pool.QueueJob(std::bind(&VelocityVerlet::DoStep, Integrators[j].get(), false, false, false), j);
        }
        Pool.Resume();
        while(Pool.Busy()){};
        while(!ILG->Wait()){};
        Pool.Pause();
        ILG->Reset();
        for (int j = 0; j < s_AllParticles.size(); j++)
        {
            Pool.QueueJob(std::bind(&VelocityVerlet::DoStep, Integrators[j].get(), false, false, true), j);
            //Integrators[j]->ThermalEnergyEvaluation();
        }
        Pool.Resume();
        while(Pool.Busy()){};
        while(!ILG->Wait()){};
        Pool.Pause();
        ILG->Reset();
        TotalEnergy = 0.0;
        double TotalKE = 0.0;
        double TotalU = 0.0;
        while(Pool.CheckBusyThreads()){};
        for(int j = 0; j < s_AllParticles.size(); j++)
        {
            auto[KE,U] = s_AllParticles[j]->GetRecentEnergy();
            TotalKE += KE;
            TotalU += U;
            TotalEnergy += KE + U;
        }
        std::cout << "Total Energy at step " << i << ": " << TotalEnergy << " , " << TotalKE << " , " << TotalU << std::endl;
        OLG->UpdateWriteLimit(i+1);
    }
    OLG->UpdateWriteLimit(SetupData.steps);
    OLG->UpdateKill(true);
    Pool.Resume();
    Pool.Stop();
    OutputThread.join();
    delete OLG;
    
    //Pool.QueueJob(std::bind(VelocityVerlet::DoStep, true, std::placeholders::_1), 0);


    return 0;
}