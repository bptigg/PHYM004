//Definition of the velocity verlet integrator
#pragma once
#include <vector>
#include <memory>
#include "Body.h"
#include "MultithreadingLockGuards.h"
#include "Observer.h"

class Integrator
{
public:
    typedef Body::vec(*ForceCalculation)(int, std::vector<std::shared_ptr<Body>>); //function ptr for the force calculation 

    struct InitialConditions
    {
        Body::vec pos;
        Body::vec vel;
        Body::vec acc;
    };
private:
    InitialConditions m_SystemState;
    InitialConditions m_PrevSystemState;
    ForceCalculation m_ForceFunction;

    std::shared_ptr<IntergatorLockGuard> m_LockGaurd;
    std::shared_ptr<EnergyObserver> m_obs;
    std::vector<std::shared_ptr<Body>> m_BodyVec;

    double m_TimeStep;
    double m_MaxTime;
    double m_CurrentTime;

    int m_id;
    
public:
    Integrator(InitialConditions, ForceCalculation, double, double, int, std::shared_ptr<IntergatorLockGuard>, std::shared_ptr<EnergyObserver>& obs); //main constructor
    Integrator(double, double, std::shared_ptr<IntergatorLockGuard>, int); //basic constructor for testing 
    void DoStep(); 
    void SetBodyVec(std::vector<std::shared_ptr<Body>>);
};
