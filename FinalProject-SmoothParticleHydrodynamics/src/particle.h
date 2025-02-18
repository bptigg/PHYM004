//Definition of the particle class

#ifndef SPH_PARTICLE
#define SPH_PARTICLE

#include <vector>
#include <tuple>

typedef std::pair<double, double> Kernal;


class Particle
{
private:
    float m_xCoord;
    float m_Velocity;
    float m_Acceleration;

    float m_Density;
    float m_Pressure;

    float m_Mass;

    //std::vector<double> m_KernalFunctionPerParticle;
    std::vector<Kernal> m_KernalFunctionPerParticle;
    
    std::vector<float> m_KineticEnergy;
    std::vector<float> m_ThermalEnergy;
public:
    inline void UpdateX(float NewXCoord) { m_xCoord = NewXCoord; };
    inline float GetX() { return m_xCoord; };

    inline void UpdateV(float NewVelocity) { m_Velocity = NewVelocity; };
    inline float GetV() { return m_Velocity; };

    inline void UpdateA(float NewAcc) { m_Acceleration = NewAcc; };
    inline float GetA() { return m_Acceleration; };

    inline void UpdateRho(float NewDensity) { m_Density = NewDensity; };
    inline float GetRho() { return m_Density; };

    inline void UpdateP(float NewPressure) { m_Pressure = NewPressure; };
    inline float GetP() { return m_Pressure; };

    inline void UpdateM(float NewMass) { m_Mass = NewMass; };
    inline float GetMass() { return m_Mass; };

    inline void UpdateKernal(std::vector<Kernal> KernalResults) { 
        for(int i = 0; i < KernalResults.size(); i++)
        {
            m_KernalFunctionPerParticle.push_back(KernalResults[i]);
        } 
    };

    inline const std::vector<Kernal>& GetKernal() { return m_KernalFunctionPerParticle; };
    inline void ClearKernal() { return m_KernalFunctionPerParticle.clear(); };

    inline void UpdateKineticEnergy(float Energy) { m_KineticEnergy.push_back(Energy); };
    inline void UpdateThermalEnergy(float Energy) { m_ThermalEnergy.push_back(Energy); };


    std::tuple<double, double, double> GetInitialConditions() {
        std::tuple<double, double, double> ReturnValues = {
            GetV(),
            GetA(),
            GetX()
        };
        return ReturnValues;
    }

    std::tuple<double, double> GetRecentEnergy() {
        std::tuple<double, double> ReturnValues = {
            m_KineticEnergy.back(),
            m_ThermalEnergy.back()
        };
        return ReturnValues;
    }

public:
    double TemporyInternalEnergyGradient = 0.0;

};

#endif