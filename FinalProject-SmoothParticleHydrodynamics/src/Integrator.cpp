#include "Integrator.h"

bool VelocityVerlet::DoStep(bool UpdatePositions)
{
    if(UpdatePositions)
    {
    //get intial conditions (velocity, acceleration and postition)
    //half timestep v = initial v + 1/2 * timestep * inital a
    //new position = inital r + timestep * half timestep v
        m_LockGaurd->UpdateCurrentChecks();
        return true;
    }
    
    //wait for all particles positions to be updated
    //evaluate kernals
    m_KernalEvaluation(m_ParticleID);
    //evaluate density
    m_DensityEvaluation(m_ParticleID);
    //evaluate acceleration
    m_AccelerationEvaluation(m_ParticleID);
    //evaluate energy
    m_EnergyEvaluation(m_ParticleID);
    //update velocity using new acceleration
    return true;
}
