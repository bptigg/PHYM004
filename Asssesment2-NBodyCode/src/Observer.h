//Definition of the observer object, this object gets the infomation from the integrator 
#pragma once
#include "Body.h"
#include <mutex>
#include <vector>

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
    std::vector<Body::vec> m_positions;
    std::vector<Body::vec> m_velocities;
    std::vector<double> m_Masses;
    std::vector<double> m_Energy;
    std::vector<double> m_AngularMomentum;
    std::mutex m_OperatorMutex;
    std::vector<double> m_Timesteps;
    double m_bodies;
public:
    void operator() (const std::pair<Body::vec&, Body::vec&> rv, double mass, double t); //summation so order doesn't really matter 
    void CalculateEnergy();
    void CalculateAngularMomentum();
    bool Reset();

    EnergyObserver(int);
};

class TrajactoryObserver
{
private:
    std::vector<Body::vec> m_Positions;
    std::mutex m_OperatorMutex;
    std::vector<double> m_Timesteps;
public:
    void operator() (const Body::vec& r, const int id, double t); // order does matter because this is printing the results 
    bool Reset();
};