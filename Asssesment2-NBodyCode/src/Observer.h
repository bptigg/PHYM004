//Definition of the observer object, this object gets the infomation from the integrator 
#pragma once
#include "Body.h"
#include "MultithreadingLockGuards.h"
#include <mutex>
#include <vector>
#include <memory>
#include <fstream>

//class ObserverBase
//{
//public:
//    virtual void operator() (const std::pair<Body::vec&, Body::vec&> rv, double t) = 0;
//    virtual bool Reset() = 0;
//
//    ObserverBase(int);
//    ObserverBase(const ObserverBase& base);
//    virtual ~ObserverBase() {};
//protected:
//    std::mutex m_OperatorMutex;
//    std::vector<double> m_Timesteps;
//    double m_bodies;
//};

class EnergyObserver
{
private:
    std::vector<std::vector<Body::vec>> m_positions;
    std::vector<std::vector<Body::vec>> m_velocities;
    std::vector<std::vector<int>> m_Index;
    std::vector<std::vector<double>> m_Masses;
    std::vector<double> m_Energy;
    std::vector<Body::vec> m_AngularMomentum;
    std::vector<double> m_Timesteps;
    double m_bodies;
    int m_NumTimesteps;
    double m_Epsilon;
    int m_loop = 0;
    
    std::mutex m_OperatorMutex;
    std::shared_ptr<OutputLockGuard> m_OLG;
    std::shared_ptr<std::ofstream> m_OutputFile; 
    
public:
    void operator() (const std::pair<Body::vec&, Body::vec&> rv, double mass, double t, int id); //summation so order doesn't really matter 
    void CalculateEnergy();
    void CalculateAngularMomentum();
    bool Reset();
    void Pop();

    EnergyObserver(int, int);
    ~EnergyObserver();

    void AtttachLockGuard(std::shared_ptr<OutputLockGuard> olg);
    void AttachOutputFile(std::string OutputFile);
    inline void SetEpsilon(double e) { m_Epsilon = e;}
    void WriteToFile();

private:
    void WriteHeader();
    void Calculate();
};

