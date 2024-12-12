#include "Observer.h"
#include <algorithm>
#include <iostream>

#define G 6.67430e-11

void EnergyObserver::operator()(const std::pair<Body::vec &, Body::vec &> rv, double mass, double t)
{
    m_OperatorMutex.lock();  
    this->m_positions.push_back(rv.first);
    this->m_velocities.push_back(rv.second);
    this->m_Masses.push_back(mass);
    if(std::find(m_Timesteps.begin(), m_Timesteps.end(), t) == m_Timesteps.end())
        this->m_Timesteps.push_back(t);
    m_OperatorMutex.unlock();
}

void EnergyObserver::CalculateEnergy()
{
    //kinetic energy
    double kinetic = 0;
    for(int i = 0; i < m_bodies; i++)
    {
        double temp = 0;
        temp = m_Masses[i] * m_velocities[i].magnitude();
        kinetic += temp;
    }

    double gravitational = 0;

    for(int i = 0; i < m_bodies; i++)
    {
        for (int e = 0; e < m_bodies; e++)
        {
            if (e == i) { continue; }

            double MassProduct = m_Masses[i] * m_Masses[e];
            
            auto b2Vec = m_positions[e];
            auto b1Vec = m_positions[i];
            Body::vec rVec = {b2Vec.x - b1Vec.x, b2Vec.y - b1Vec.y, b2Vec.z - b1Vec.z};
            gravitational += MassProduct / rVec.magnitude();
        }
    }

    gravitational = gravitational * (-1/2 * G);

    double TotalEnergy = kinetic + gravitational;
    m_Energy.push_back(TotalEnergy);
    //std::cout << m_Energy[m_Energy.size()-1] << std::endl;
}

void EnergyObserver::CalculateAngularMomentum()
{
    Body::vec AngularMomentum = {0,0,0};
    for (int i = 0; i < m_bodies; i++)
    {
        Body::vec Momentum = m_velocities[i] * m_Masses[i];
        Body::vec SingleAngularMomentum = m_positions[i].CrossProduct(Momentum);
        AngularMomentum += SingleAngularMomentum; 
    }

    std::cout << AngularMomentum.x << " , " << AngularMomentum.y << " , " << AngularMomentum.z << std::endl;
}

bool EnergyObserver::Reset()
{
    this->m_positions.clear();
    this->m_velocities.clear();
    this->m_Masses.clear();
    return true;
}

EnergyObserver::EnergyObserver(int bodies)
    :m_bodies(bodies)
{
}

void TrajactoryObserver::operator()(const Body::vec &r, const int id, double t)
{
    m_OperatorMutex.lock();
    this->m_Positions.push_back(r);
    if(std::find(m_Timesteps.begin(), m_Timesteps.end(), t) == m_Timesteps.end())
        this->m_Timesteps.push_back(t);
    m_OperatorMutex.unlock();
}

bool TrajactoryObserver::Reset()
{
    this->m_Positions.clear();
    for (int i = 0; i < 0; i++)
    {
        m_Positions.push_back({0.0,0.0,0.0});
    }
    return true;
}

