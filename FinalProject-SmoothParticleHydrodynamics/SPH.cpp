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
        derivative =  -1 * ((3.0 * q) - (9.0/4.0)*std::pow(q,2));
    }
    else if(q <= 2)
    {
        ReturnValue = 0.25 * std::pow(2.0-q,3);
        derivative = -0.75 * std::pow(2.0 - q, 2);
    }
    else
    {
        ReturnValue = 0.0;
        derivative = 0.0;
    }

    ReturnValue = ReturnValue * (2.0/(3*h));
    //derivative = derivative * (-2.0/(3*std::pow(h,2))); 
    derivative = derivative * (2.0/(3*std::pow(h,2))); 
}

void KernalEvaulation(int a)
{
    auto TargetParticle = s_AllParticles[a];
    std::vector<int> KernalIndex;
    std::vector<Kernal> KernalEvaulations;
    TargetParticle->ClearKernal();
    for(int b = 0; b < s_AllParticles.size(); b++)
    {
        double RadiusDiff = TargetParticle->GetX() - s_AllParticles[b]->GetX();
        double ReturnValue = 0;
        double DerivativeValue = 0;
        double q = std::abs(RadiusDiff) / s_h;
        KernalFunction(q, s_h, ReturnValue, DerivativeValue);
        if(ReturnValue != 0.0 )//|| DerivativeValue != 0.0)
        {
            KernalEvaulations.push_back({ReturnValue, DerivativeValue});
            KernalIndex.push_back(b);
        }
    }
    TargetParticle->UpdateKernal(KernalEvaulations);
    TargetParticle->GetCache().KernalResults = KernalIndex;
}

void SpeedOfSound(int a)
{
    //Cs = sqrt(K/P) = sqrt(rho*(dP/drho)/P)
    auto TargetParticle = s_AllParticles[a];
    double density = TargetParticle->GetRho();
    double Pressure = TargetParticle->GetP();
    double dPdrho = TargetParticle->GetdPdrho();
    
    double BulkModulus = density * dPdrho;
    double SpeedOfSound = std::sqrt(BulkModulus/Pressure);
    TargetParticle->UpdateCs(SpeedOfSound);
}

void ArtificialViscosity(double r, double v, std::pair<int,int> Particles, double& viscosity)
{
    double alpha = 1;
    double beta = 2;
    double epsilon = 0.01;

    double mu = 0.0;
    double approach = r * v;

    if(approach >= 0) {viscosity = 0.0; return;}

    double AvgSpeedOfSound = (s_AllParticles[Particles.first]->GetCs() + s_AllParticles[Particles.second]->GetCs()) * 0.5;
    double AvgDensity = (s_AllParticles[Particles.first]->GetRho() + s_AllParticles[Particles.second]->GetRho()) * 0.5;

    
    mu = (s_h * approach) / (std::pow(r,2) + (epsilon*std::pow(s_h,2)));
    viscosity = (-1 * alpha * AvgSpeedOfSound * mu) + (beta * std::pow(mu,2));
    viscosity = viscosity / AvgDensity;
    viscosity = 0.0;
    return;
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
    double PressureOverRhoSquared = Pressure * std::pow(Density,-2);
    auto ParticleIndex = TargetParticle->GetCache().KernalResults;
    //for(int b = 0; b < s_AllParticles.size(); b++)
    for(int i = 0; i < ParticleIndex.size(); i++)
    {
        int b = ParticleIndex[i];
        if(Kernal[b].first == 0.0) {continue;}
        //r = s_AllParticles[b]->GetX() - TargetParticle->GetX();
        r = TargetParticle->GetX() - s_AllParticles[b]->GetX();
        if(r == 0.0) {continue;}
        double Mass = s_AllParticles[b]->GetMass();
        double VelocityB = s_AllParticles[b]->GetV();
        double VelocityDiff = VelocityA - VelocityB;
        //double VelocityDiff = TargetParticle->VelocityApproach[i];
        double GradKernal = Kernal[b].second * (r / std::abs(r)) * (1 - DiracDelta(a,b));
        double EABFirstTerm = Mass * VelocityDiff * GradKernal;
        double EABSecondTerm = 0.5 * Mass * TargetParticle->GetViscosity()[i] * VelocityDiff * GradKernal;
        EABFirstTerm = EABFirstTerm * PressureOverRhoSquared;
        double EAB = EABFirstTerm + EABSecondTerm;
        Energy += EAB;
    }

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
    double ValA = TargetParticle->GetCache().PressureOverDensitySquared;
    //if(ValA == 0.0)
    {
        ValA = Pressure * (1 / std::pow(Density,2));
        TargetParticle->GetCache().PressureOverDensitySquared = ValA;
    }
    double acceleration = 0.0;
    double Energy = 0.0;
    double r = 0.0;
    auto ParticleIndex = TargetParticle->GetCache().KernalResults;
    //TargetParticle->ClearViscosity();
    //TargetParticle->VelocityApproach.clear();
    //for(int b = 0; b < s_AllParticles.size(); b++)
    for(int i = 0; i < ParticleIndex.size(); i++)
    {
        int b = ParticleIndex[i];
        //TargetParticle->UpdateViscosity(0.0);
        //TargetParticle->VelocityApproach.push_back(0.0);
        if(Kernal[i].first == 0.0) {continue;}
        //r = s_AllParticles[b]->GetX() - TargetParticle->GetX();
        r = TargetParticle->GetX() - s_AllParticles[b]->GetX();
        if(r == 0.0) {continue;}
        double Mass = s_AllParticles[b]->GetMass();
        double ValB = s_AllParticles[b]->GetCache().PressureOverDensitySquared;
        //double ApprochVelocity = s_AllParticles[b]->GetV() - TargetParticle->GetV();
        double ApprochVelocity = TargetParticle->GetV() - s_AllParticles[b]->GetV();
        //if(ValB == 0.0)
        {
            ValB = s_AllParticles[b]->GetP() * (1 / std::pow(s_AllParticles[b]->GetRho(),2));
            s_AllParticles[b]->GetCache().PressureOverDensitySquared = ValB;
        }
        double Viscosity = 0.0;
        ArtificialViscosity(r,ApprochVelocity,{a,b},Viscosity);
        //TargetParticle->UpdateViscosity(Viscosity,i);
        //TargetParticle->VelocityApproach[i] = ApprochVelocity;
        double GradKernal = Kernal[i].second * (r / std::abs(r)) * (1 - DiracDelta(a,b));
        double AccAB = Mass * (ValA + ValB + Viscosity) * GradKernal;
        acceleration += AccAB;

        double EABFirstTerm = Mass * ApprochVelocity * GradKernal;
        double EABSecondTerm = 0.5 * Mass * Viscosity * ApprochVelocity * GradKernal;
        EABFirstTerm = EABFirstTerm * ValA;
        double EAB = EABFirstTerm + EABSecondTerm;
        Energy += EAB;
    }
    TargetParticle->UpdateA(-1.0 * acceleration);
    TargetParticle->TemporyInternalEnergyGradient = Energy;
}

void DensityEvauluation(int a)
{
    //In ID
    //sum(mass of b * kernal function)
    auto TargetParticle = s_AllParticles[a];
    auto Kernal = TargetParticle->GetKernal();
    double density = 0.0;
    auto ParticleIndex = TargetParticle->GetCache().KernalResults;
    //for(int b = 0; b < s_AllParticles.size(); b++)
    for(int i = 0; i < ParticleIndex.size(); i++)
    {
        int b = ParticleIndex[i];
        if(Kernal[i].first == 0.0) {continue;}
        double Mass = s_AllParticles[b]->GetMass();
        density += Kernal[i].first * Mass;
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
    double dPdrho = ThermalEnergy;
    TargetParticle->UpdatePrho(dPdrho);
    SpeedOfSound(a);
}

void PolytropicPressureEvaluation(int a)
{
    //P = K * rho^gamma
    auto TargetParticle = s_AllParticles[a];
    double density = TargetParticle->GetRho();
    double Pressure = s_K * std::pow(density, s_Gamma);
    TargetParticle->UpdateP(Pressure);
    double dPdrho = s_K * s_Gamma * std::pow(density,s_Gamma-1);
    TargetParticle->UpdatePrho(dPdrho);
    SpeedOfSound(a);
}

void InitialConditions(int a)
{
    KernalEvaulation(a);
    DensityEvauluation(a);
    PolytropicPressureEvaluation(a);
    //AccelerationEvaluation(a);
}

bool CheckSymmetry()
{
    for(int i = 0; i < s_AllParticles.size(); i++)
    {
        auto kernal = s_AllParticles[i]->GetKernal();
        auto kernalindex = s_AllParticles[i]->GetCache().KernalResults;
        for (int j = 0; j < kernalindex.size(); j++)
        {
            int index1 = kernalindex[j];
            if (i == index1) {continue;}
            auto kernalindex2 = s_AllParticles[index1]->GetCache().KernalResults;
            int index2 = 0;
            for (int e : kernalindex2)
            {
                if(e == i)
                    break;
                else
                    index2++;
            }
            auto kernal2 = s_AllParticles[index1]->GetKernal();
            if(kernal[j].first != kernal2[index2].first)
            {
                return false;
            }

        }
    }
    return true;
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
    std::vector<double> RegionSizes = {};
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

    std::vector<std::pair<double,int>> Regions = {};
    for (int i = 0; i < RegionData.size(); i++)
    {
        Regions.push_back({RegionSizes[i],i});
    }
    std::sort(Regions.begin(), Regions.end(), [](std::pair<double,int> a, std::pair<double,int> b) {return a.first < b.first;});
    std::vector<std::pair<double,int>> DensityRatio = {};
    std::vector<std::pair<double,int>> SizeRatio = {};
    std::vector<std::pair<double,int>> Density = {}; 
    for(int i = 0; i < RegionSizes.size(); i++)
    {   
        Density.push_back({RegionData[i].InitialDensity,i});
    }
    std::sort(Density.begin(), Density.end(), [](std::pair<double,int> a, std::pair<double,int> b) {return a.first < b.first;});
    for(int i = 0; i < Density.size(); i++)
    {
        DensityRatio.push_back({Density[i].first / Density[0].first, Density[i].second});
        SizeRatio.push_back({Regions[i].first / Regions[0].first, Regions[i].second});
    }
    for (int i = 0; i < SizeRatio.size(); i++)
    {
        int index = 0;
        for(int j = 0; j < DensityRatio.size(); j++)
        {
            if(DensityRatio[j].second = SizeRatio[i].second)
            {
                index = j;
                break;
            }
            index++;
        }
        DensityRatio[index].first = DensityRatio[index].first * SizeRatio[i].first;
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
    for (int i = 0; i < ParticleDensity.size(); i++)
    {
        ParticleDensity[i].first = std::round(ParticleDensity[i].first);
    }

    std::sort(ParticleDensity.begin(), ParticleDensity.end(), [](std::pair<double,int> a, std::pair<double,int> b) {return a.second < b.second;});
    int ParticleIndexOffset = 0;
    for(int i = 0; i < RegionData.size(); i++)
    {
        auto Boundaries = RegionData[i].Boundaries;
        std::sort(Boundaries.begin(), Boundaries.end(), [](std::array<double,3> a, std::array<double,3> b) {return a[0] < b[0];});
        for(int j = ParticleIndexOffset; j < ParticleDensity[i].first + ParticleIndexOffset; j++)
        {
            double StepSize = ((Boundaries[Boundaries.size() - 1][0]) - (Boundaries[0][0])) / (ParticleDensity[i].first);
            s_AllParticles[j]->UpdateX(Boundaries[0][0] + (j - ParticleIndexOffset) * StepSize);
            s_AllParticles[j]->UpdateM(s_h * RegionData[i].InitialDensity/ (ParticleDensity[i].first*s_h/Regions[i].first));
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
    while(Pool.CheckBusyThreads()){};
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
    while(Pool.CheckBusyThreads()){};
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
    while(Pool.CheckBusyThreads()){};
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
            Pool.QueueJob(std::bind(&VelocityVerlet::DoStep, Integrators[j].get(), true, false, false), j);
        }
        Pool.Resume();
        while(Pool.Busy()){};
        while(!ILG->Wait()){};
        while(Pool.CheckBusyThreads()){};
        Pool.Pause();
        ILG->Reset();
        for(int j = 0; j < s_AllParticles.size(); j++)
        {
            Pool.QueueJob(std::bind(&VelocityVerlet::DoStep, Integrators[j].get(), false, true, false), j);
        }
        Pool.Resume();
        while(Pool.Busy()){};
        while(!ILG->Wait()){};
        while(Pool.CheckBusyThreads()){};
        Pool.Pause();
        //if(!CheckSymmetry())
        //{
        //    std::cout << "False" << std::endl;
        //    std::cin.get();
        //}
        ILG->Reset();
        for(int j = 0; j < s_AllParticles.size(); j++)
        {
            Pool.QueueJob(std::bind(&VelocityVerlet::DoStep, Integrators[j].get(), false, false, false), j);
        }
        Pool.Resume();
        while(Pool.Busy()){};
        while(!ILG->Wait()){};
        while(Pool.CheckBusyThreads()){};
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
        //while(Pool.CheckBusyThreads()){};
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