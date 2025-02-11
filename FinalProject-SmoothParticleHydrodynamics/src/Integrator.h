//Definition of the integrator class, im using a velocity verlet integrator

#ifndef SPH_INTEGRATOR
#define SPH_INTEGRATOR

#include <vector>
#include <memory>
#include "MultithreadingLockGuards.h"

//using ButcherTable = std::vector<std::pair<double, std::vector<double>>>;
//typedef double(*RungeKuttaFunction)(double, double, double&);


typedef void(*EvaluationFunction)(int);

class VelocityVerlet
{
private:
    double m_Timestep;
    double m_CurrentTimeStep;

    std::shared_ptr<IntergatorLockGuard> m_LockGaurd;

    EvaluationFunction m_KernalEvaluation;
    EvaluationFunction m_DensityEvaluation;
    EvaluationFunction m_AccelerationEvaluation;
    EvaluationFunction m_EnergyEvaluation;

    int m_ParticleID;
public:
    bool DoStep(bool UpdatePositions = true);
};

#endif
