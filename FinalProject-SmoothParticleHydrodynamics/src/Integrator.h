//Definition of the integrator class, im using a velocity verlet integrator

#ifndef SPH_INTEGRATOR
#define SPH_INTEGRATOR

#include <vector>
#include <memory>
#include "MultithreadingLockGuards.h"
#include "particle.h"

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
    EvaluationFunction m_PressureEvaluation;
    EvaluationFunction m_AccelerationEvaluation;
    EvaluationFunction m_ThermalEvaluation;
    EvaluationFunction m_KineticEvaluation;

    int m_ParticleID;
    std::shared_ptr<Particle> m_Particle;
    double m_TempV;

public:
    void DoStep(bool UpdatePositions = true);
    void EnergyEvaluation();
};

#endif
