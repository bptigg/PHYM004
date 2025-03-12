//definition of the filereader object
#ifndef SPH_FILEREADER
#define SPH_FILEREADER

#include <vector>
#include <string>
#include <array>

#include "MultithreadingLockGuards.h"

class FileReader
{
public:
    struct SetupData
    {
        double Particles;
        double SmoothingLength;
        double timestep;
        double steps;
        double Gamma;
        std::string OutputFile;
        bool BoundaryConditions;
        int Dimensions;
        std::vector<std::array<double,3>> PhysicalBoundaries;
    };
    struct RegionData
    {
        double InitialInternalEnergy;
        double InitialDensity;
        std::vector<std::array<double,3>> Boundaries;
    };

private:
    SetupData m_Setup;
    std::vector<RegionData> m_Regions;
public:
    bool LoadFile(std::string&);
    inline const std::vector<RegionData> GetRegionData() {
        return m_Regions;
    }
    inline const SetupData GetSetupData() {
        return m_Setup;
    } 
private:
    std::vector<std::string> SplitString(std::string WholeString, std::string delimiter);
    std::string ReadSingleValue(std::string::iterator& c);
};

class FileOutput
{
public:
    struct ParticleData
    {
        int step;
        double Position;
        double Velocity;
        double Density;
        double ThermalEnergy;
        double KineticEnergy;
    };
private:
    OutputLockGuard* m_OLG;
    std::mutex m_DataLock;
    //std::vector<std::vector<std::pair<ParticleData, int>>> m_ParticleData;
    std::vector<std::pair<std::pair<ParticleData,int>,int>> m_ParticleData;

    int m_CurrentStep = 0;
    std::string m_OutputFile;
    std::vector<std::ofstream*> m_Files;

public:
    FileOutput(std::string, OutputLockGuard*);
    ~FileOutput();
    void UpdateData(ParticleData, int);
    void WriteData();


};
#endif