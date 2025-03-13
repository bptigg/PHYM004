#include "Integrator.h"
#include <cmath>

void VelocityVerlet::DoStep(bool UpdatePositions, bool UpdatePressure, bool Energy)
{
    if(UpdatePositions)
    {
        //get intial conditions (velocity, acceleration and postition)
        auto[Velocity, Accerleration, Position] = m_Particle->GetInitialConditions();
        auto[KE,U] = m_Particle->GetRecentEnergy();
        //half timestep v = initial v + 1/2 * timestep * inital a
        double HalfV = Velocity + 0.5 * m_Timestep * Accerleration;
        double HalfU = U + 0.5 * m_Timestep * m_Particle->TemporyInternalEnergyGradient;
        //double HalfKE = KE + 0.5 * m_Timestep * m_Particle->TemporyKineticEnergyGradient;
        //new position = inital r + timestep * half timestep v
        double NewX = Position + m_Timestep * HalfV;
        m_Particle->UpdateX(NewX);
        m_LockGaurd->UpdateCurrentChecks();
        m_TempV = HalfV;
        m_TempU = HalfU;
        return;
    }

    //wait for all particles positions to be updated

    if(UpdatePressure)
    {
        //evaluate kernals
        m_KernalEvaluation(m_ParticleID);
        //evaluate density
        m_DensityEvaluation(m_ParticleID);
        //evaluate acceleration
        //update pressure
        m_PressureEvaluation(m_ParticleID);
        double Prhosquared = m_Particle->GetP() * (1/std::pow(m_Particle->GetRho(), 2));
        m_Particle->GetCache().PressureOverDensitySquared = Prhosquared;
        m_LockGaurd->UpdateCurrentChecks();
        return;
    }

    if(!Energy)
    {    
        m_AccelerationEvaluation(m_ParticleID);
        //update velocity using new acceleration
        double NewV = m_TempV + 0.5 * m_Timestep * m_Particle->GetA();
        m_Particle->UpdateV(NewV);
        m_LockGaurd->UpdateCurrentChecks();
        return;
    }

    m_ThermalEvaluation(m_ParticleID);
    m_KineticEvaluation(m_ParticleID);
    double NewEnergy = m_TempU + 0.5 * m_Timestep * m_Particle->TemporyInternalEnergyGradient;
    m_Particle->UpdateThermalEnergy(NewEnergy);
    //m_Particle->UpdateKineticEnergy(NewKE);
    m_LockGaurd->UpdateCurrentChecks();

    FileOutput::ParticleData data;
    auto[Velocity, Accerleration, Position] = m_Particle->GetInitialConditions();
    data.Density = m_Particle->GetRho();
    data.Position = Position;
    data.Velocity = Velocity;
    auto[KE,U] = m_Particle->GetRecentEnergy();
    data.KineticEnergy = KE;
    data.ThermalEnergy = U;
    data.step = m_SystemStep;
    m_SystemStep++;

    m_Output->UpdateData(data,m_ParticleID);
    m_Particle->ClearCache();
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
