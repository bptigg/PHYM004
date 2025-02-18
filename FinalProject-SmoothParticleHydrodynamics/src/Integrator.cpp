#include "Integrator.h"

void VelocityVerlet::DoStep(bool UpdatePositions)
{
    if(UpdatePositions)
    {
        //get intial conditions (velocity, acceleration and postition)
        auto[Velocity, Accerleration, Position] = m_Particle->GetInitialConditions();
        //half timestep v = initial v + 1/2 * timestep * inital a
        double HalfV = Velocity + 0.5 * m_Timestep * Accerleration;
        //new position = inital r + timestep * half timestep v
        double NewX = Position + m_Timestep * HalfV;
        m_Particle->UpdateX(NewX);
        m_LockGaurd->UpdateCurrentChecks();
        m_TempV = HalfV;
        return;
    }
    
    //wait for all particles positions to be updated
    //evaluate kernals
    m_KernalEvaluation(m_ParticleID);
    //evaluate density
    m_DensityEvaluation(m_ParticleID);
    //evaluate acceleration
    //update pressure
    m_PressureEvaluation(m_ParticleID);
    m_AccelerationEvaluation(m_ParticleID);
    //update velocity using new acceleration
    double NewV = m_TempV + 0.5 * m_Timestep * m_Particle->GetA();
    m_Particle->UpdateV(NewV);
    m_LockGaurd->UpdateCurrentChecks();
    return;
}

void VelocityVerlet::EnergyEvaluation()
{
    //evaluate energy
    m_ThermalEvaluation(m_ParticleID);
    double EnergyChange = m_Particle->TemporyInternalEnergyGradient * m_Timestep;
    auto[KE,U] = m_Particle->GetRecentEnergy();
    m_Particle->UpdateThermalEnergy(U + EnergyChange);

    m_KineticEvaluation(m_ParticleID);
    return;
}
