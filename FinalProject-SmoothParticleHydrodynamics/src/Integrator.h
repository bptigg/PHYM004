//Definition of the integrator class, im using a velocity verlet integrator

#ifndef SPH_INTEGRATOR
#define SPH_INTEGRATOR

#include <vector>
#include <memory>
#include "MultithreadingLockGuards.h"
#include "FileReader.h"
#include "particle.h"

//using ButcherTable = std::vector<std::pair<double, std::vector<double>>>;
//typedef double(*RungeKuttaFunction)(double, double, double&);


typedef void(*EvaluationFunction)(int);

class VelocityVerlet
{
private:
    double m_Timestep;
    double m_CurrentTimeStep;
    double m_SystemStep;

    std::shared_ptr<IntergatorLockGuard> m_LockGaurd;
    std::shared_ptr<FileOutput> m_Output;

    EvaluationFunction m_KernalEvaluation;
    EvaluationFunction m_DensityEvaluation;
    EvaluationFunction m_PressureEvaluation;
    EvaluationFunction m_AccelerationEvaluation;
    EvaluationFunction m_ThermalEvaluation;
    EvaluationFunction m_KineticEvaluation;

    int m_ParticleID;
    std::shared_ptr<Particle> m_Particle;

    double m_TempV;
    double m_TempU;
    double m_TempKE;

public:
    struct SetupData
    {
        int ParticleId;
        std::shared_ptr<Particle> ParticlePtr;
        double Timestep;
        std::shared_ptr<FileOutput> Output;
    };
public:
    void DoStep(bool UpdatePositions = true, bool UpdatePressure = false, bool Energy = true);
    //void ThermalEnergyEvaluation();
    
    VelocityVerlet(SetupData dat, std::shared_ptr<IntergatorLockGuard> ILG, std::vector<EvaluationFunction> Functions);
    ~VelocityVerlet() = default;
    
};

#endif
