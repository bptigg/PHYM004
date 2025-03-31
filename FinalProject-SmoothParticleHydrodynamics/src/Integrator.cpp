#include "Integrator.h"
#include <cmath>

void VelocityVerlet::DoStep(int step)
{
    if(step == 1)
    {
        //get intial conditions (velocity, acceleration and postition)
        auto[Velocity, Accerleration, Position] = m_Particle->GetInitialConditions();
        auto[KE,U] = m_Particle->GetRecentEnergy();
        //half timestep v = initial v + 1/2 * timestep * inital a
        double HalfV = Velocity + 0.5 * m_Timestep * Accerleration;
        double HalfU = U + 0.5 * m_Timestep * m_Particle->TemporyInternalEnergyGradient;// * m_Particle->GetMass();
        //double HalfKE = KE + 0.5 * m_Timestep * m_Particle->TemporyKineticEnergyGradient;
        //new position = inital r + timestep * half timestep v
        double NewX = Position + m_Timestep * HalfV;
        m_Particle->UpdateX(NewX);
        m_Particle->UpdateV(HalfV);
        m_Particle->UpdateThermalEnergy(HalfU);
        m_LockGaurd->UpdateCurrentChecks();
        m_TempV = HalfV;
        m_TempU = HalfU;
        return;
    }

    //wait for all particles positions to be updated

    if(step == 2)
    {
        //evaluate kernals
        m_KernalEvaluation(m_ParticleID);
        //evaluate density
        m_DensityEvaluation(m_ParticleID);
        //evaluate acceleration
        //update pressure
        m_PressureEvaluation(m_ParticleID);
        //double Prhosquared = m_Particle->GetP() * (1/std::pow(m_Particle->GetRho(), 2));
        //m_Particle->GetCache().PressureOverDensitySquared = Prhosquared;
        m_LockGaurd->UpdateCurrentChecks();
        return;
    }

    if(step == 3)
    {    
        m_AccelerationEvaluation(m_ParticleID);
        //m_ThermalEvaluation(m_ParticleID);
        //m_KineticEvaluation(m_ParticleID);
        //update velocity using new acceleration
        m_LockGaurd->UpdateCurrentChecks();
        return;
    }

    //m_ThermalEvaluation(m_ParticleID);
    auto[KE1,U1] = m_Particle->GetRecentEnergy();
    double NewV = m_Particle->GetV() + 0.5 * m_Timestep * m_Particle->GetA();
    double NewEnergy = U1 + 0.5 * m_Timestep * m_Particle->TemporyInternalEnergyGradient;// * m_Particle->GetMass();
    //m_KineticEvaluation(m_ParticleID);
    m_Particle->UpdateV(NewV);
    m_Particle->UpdateThermalEnergy(NewEnergy);
    m_KineticEvaluation(m_ParticleID);
    //m_Particle->UpdateKineticEnergy(NewKE);

    FileOutput::ParticleData data;
    auto[Velocity, Accerleration, Position] = m_Particle->GetInitialConditions();
    data.Density = m_Particle->GetRho();
    data.Position = Position;
    data.Velocity = Velocity;
    auto[KE,U] = m_Particle->GetRecentEnergy();
    data.KineticEnergy = KE;
    data.ThermalEnergy = U;
    data.Pressure = m_Particle->GetP();
    data.step = m_SystemStep;
    m_SystemStep++;

    m_Output->UpdateData(data,m_ParticleID);
    m_Particle->ClearCache();
    //data.Density = m_Particle->GetRho();
    m_LockGaurd->UpdateCurrentChecks();

    return;

}

//void VelocityVerlet::ThermalEnergyEvaluation()
//{
//    //evaluate energy
//    m_ThermalEvaluation(m_ParticleID);
//    double EnergyChange = m_Particle->TemporyInternalEnergyGradient * m_Timestep; //eular integration is sufficient for this as energy is a not a function of itself
//    auto[KE,U] = m_Particle->GetRecentEnergy();
//    m_Particle->UpdateThermalEnergy(U + EnergyChange);
//    m_LockGaurd->UpdateCurrentChecks();
//
//    m_KineticEvaluation(m_ParticleID);
//    return;
//}

VelocityVerlet::VelocityVerlet(SetupData dat, std::shared_ptr<IntergatorLockGuard> ILG, std::vector<EvaluationFunction> Functions)
{
    m_ParticleID = dat.ParticleId;
    m_Particle = dat.ParticlePtr;
    m_Timestep = dat.Timestep;
    m_LockGaurd = ILG;
    m_KernalEvaluation = Functions[0];
    m_DensityEvaluation = Functions[1];
    m_PressureEvaluation = Functions[2];
    m_AccelerationEvaluation = Functions[3];
    m_ThermalEvaluation = Functions[4];
    m_KineticEvaluation = Functions[5];
    m_Output = dat.Output;
    m_SystemStep = 0;
}
