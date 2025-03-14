#include "Observer.h"
#include <algorithm>
#include <iostream>
#include <math.h>

#define G 6.67430e-11

void EnergyObserver::operator()(const std::pair<Body::vec &, Body::vec &> rv, double mass, double t, int id)
{
    int index = m_positions.size() - 1;
    m_OperatorMutex.lock();  
    this->m_positions[index].push_back(rv.first);
    this->m_velocities[index].push_back(rv.second);
    this->m_Masses[index].push_back(mass);
    this->m_Index[index].push_back(id);
    if(std::find(m_Timesteps.begin(), m_Timesteps.end(), t) == m_Timesteps.end()) //each integrator is attatched to this observer so without this line, the timestep array would be n times bigger 
        this->m_Timesteps.push_back(t);
    m_OperatorMutex.unlock();
}

void EnergyObserver::CalculateEnergy()
{
    //kinetic energy
    double kinetic = 0;

    m_OperatorMutex.lock(); //grabs the values from the vectors, to prevent race conditions 
    std::vector<Body::vec> positions = m_positions[m_loop];
    std::vector<Body::vec> velocities = m_velocities[m_loop];
    std::vector<double> masses = m_Masses[m_loop];
    m_OperatorMutex.unlock();

    for(int i = 0; i < m_bodies; i++) //calculates kinetic energy
    {
        double temp = 0;
        temp = masses[i] * velocities[i].magnitude(); 
        kinetic += temp;
    }


    double gravitational = 0; 

    for(int i = 0; i < m_bodies; i++)  //calculates graviational energy 
    {
        for (int e = 0; e < m_bodies; e++)
        {
            if (e == i) { continue; }

            double MassProduct = masses[i] * masses[e];
            
            auto b2Vec = positions[e];
            auto b1Vec = positions[i];
            Body::vec rVec = {b2Vec.x - b1Vec.x, b2Vec.y - b1Vec.y, b2Vec.z - b1Vec.z};
            gravitational += MassProduct / std::sqrt(std::pow(rVec.magnitude(),2) + std::pow(m_Epsilon,2));
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

    m_OperatorMutex.lock();
    std::vector<Body::vec> positions = m_positions[m_loop]; //grabs the values from the vectors, to prevent race conditions
    std::vector<Body::vec> velocities = m_velocities[m_loop];
    std::vector<double> masses = m_Masses[m_loop];
    m_OperatorMutex.unlock();

    for (int i = 0; i < m_bodies; i++) //calculates angular momentum
    {
        Body::vec Momentum = velocities[i] * masses[i];
        Body::vec SingleAngularMomentum = positions[i].CrossProduct(Momentum);
        AngularMomentum += SingleAngularMomentum; 
    }

    //std::cout << AngularMomentum.x << " , " << AngularMomentum.y << " , " << AngularMomentum.z << std::endl;
    m_AngularMomentum.push_back(AngularMomentum);
}

bool EnergyObserver::Reset()
{
    //this->m_positions.clear();
    //this->m_velocities.clear();
    //this->m_Masses.clear();
    //this->m_Index.clear();
    m_OperatorMutex.lock(); //adds a new vector into the main vectors for values to be out into 
    this->m_positions.push_back({});
    this->m_velocities.push_back({});
    this->m_Masses.push_back({});
    this->m_Index.push_back({});
    m_OperatorMutex.unlock();
    return true;
}

void EnergyObserver::Pop()
{
    m_OperatorMutex.lock();
    m_positions.erase(m_positions.begin()); //removed the bottom vector
    m_velocities.erase(m_velocities.begin());
    m_Masses.erase(m_Masses.begin());
    m_Index.erase(m_Index.begin());
    m_OperatorMutex.unlock();
}

EnergyObserver::EnergyObserver(int bodies, int timesteps)
    :m_bodies(bodies), m_NumTimesteps(timesteps)
{
}

EnergyObserver::~EnergyObserver()
{
    m_OutputFile->close();
}

void EnergyObserver::AtttachLockGuard(std::shared_ptr<OutputLockGuard> olg)
{
    m_OLG = olg;
}

void EnergyObserver::AttachOutputFile(std::string OutputFile)
{
    m_OutputFile = std::make_shared<std::ofstream>(OutputFile);
    WriteHeader();
}

void EnergyObserver::WriteToFile()
{
    m_loop = 0;
    while(!m_OLG->kill) //whilst the main code is still running m_OLG->Kill will be false
    {
        while(m_loop < m_OLG->writeLimit)
        {
            Calculate();

            m_OperatorMutex.lock();
            std::vector<int> indexVec = m_Index[m_loop];
            std::vector<Body::vec> positions = m_positions[m_loop];
            m_OperatorMutex.unlock();

            (*m_OutputFile.get()) << m_loop << " " << m_Timesteps[m_loop] << " " << m_Energy[m_loop] << " " << m_AngularMomentum[m_loop];//writes the the file
            for(int i = 0; i < m_bodies; i++) //iterates through the different bodies 
            {
                int index = 0;
                auto it = std::find(indexVec.begin(), indexVec.end(), i);
                index = it - indexVec.begin();
                (*m_OutputFile.get()) << " " << positions[index];
            }
            (*m_OutputFile.get()) << std::endl;
            m_loop++;
            //Pop();
        }
    }
}

void EnergyObserver::WriteHeader() //writes a header on the data file so the python file knows how to handle the file 
{
    (*m_OutputFile.get()) << "Number_of_bodies:" << m_bodies << std::endl << "Number_of_steps:" << m_NumTimesteps << std::endl;
    (*m_OutputFile.get()) << "step timestep(s) energy angular_momentum_magnitude L_x L_y L_z";
    for(int i = 0; i < m_bodies; i++)
    {
        (*m_OutputFile.get()) << " body" << i << "_radius" << " body" << i << ".x" << " body" << i << ".y" << " body" << i << ".z";
    }
    (*m_OutputFile.get()) << std::endl;
}

void EnergyObserver::Calculate() 
{
    CalculateEnergy();
    CalculateAngularMomentum();
}
