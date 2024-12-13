#include "Integrator.h"
#include <chrono>
#include <iostream>

Integrator::Integrator(InitialConditions initial, ForceCalculation calc, double timestep, double maxtime, int id, std::shared_ptr<IntergatorLockGuard> LockGuard,std::shared_ptr<EnergyObserver>& obs )
    :m_SystemState(initial), m_PrevSystemState(initial), m_ForceFunction(calc), m_TimeStep(timestep), m_MaxTime(maxtime), m_LockGaurd(LockGuard),m_obs(obs), m_id(id)
{
    m_CurrentTime = 0;
}

Integrator::Integrator(double timestep, double maxtime, std::shared_ptr<IntergatorLockGuard> LockGaurd, int id)
    :m_TimeStep(timestep), m_MaxTime(maxtime), m_LockGaurd(LockGaurd), m_id(id)
{
    m_CurrentTime = 0;
}

void Integrator::DoStep()
{
    Body::vec a = m_PrevSystemState.acc;
    
    Body::vec tempv = m_SystemState.vel + (0.5 * m_TimeStep * a);
    Body::vec AdjustedV = m_TimeStep * tempv;
    m_SystemState.pos = m_SystemState.pos + AdjustedV;
    m_BodyVec[m_id]->SetGlobalPositon(m_SystemState.pos);

    //std::cout << "step 1 done " << m_id << std::endl;
    m_LockGaurd->UpdateCurrentChecks();
    while (!m_LockGaurd->Wait()) {std::this_thread::sleep_for(std::chrono::microseconds(10)); continue;}
    //std::cout << "step 2 done " << m_id << std::endl;
    
    a = m_ForceFunction(m_id, m_BodyVec);
    double OneOverStep = 1 / m_TimeStep;
    m_SystemState.vel = tempv + (0.5 * m_TimeStep * a);
    m_SystemState.acc = a;
    m_PrevSystemState = m_SystemState;
    m_BodyVec[m_id]->setVelocity(m_SystemState.vel);
    m_obs->operator()({m_SystemState.pos,m_SystemState.vel},m_BodyVec[m_id]->GetMass(),m_CurrentTime, m_id);
    m_CurrentTime += m_TimeStep;

    //std::cout << m_id << "," << m_SystemState.pos.x << "," << m_SystemState.pos.y << "," << m_SystemState.pos.z << std::endl;
}

void Integrator::SetBodyVec(std::vector<std::shared_ptr<Body>>BodyVec)
{
    m_BodyVec = BodyVec;
}

