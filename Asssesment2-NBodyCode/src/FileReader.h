//definition of the filereader object, this reads the setup file
#pragma once
#include <vector>
#include <string>
#include <array>

class FileReader
{
public:
    struct SetupData
    {
        double timestep;
        int steps;
        double epsilon;
        std::string OutputFile;
    };

    struct BodyData
    {
        std::array<double,3> pos;
        std::array<double,3> vel;
        double mass;
    };
private:
    std::vector<BodyData> m_Bodies;
    SetupData m_Setup;
public:
    bool LoadFile(std::string&);
    
    inline std::vector<BodyData> GetBodyData() {return m_Bodies;}
    inline SetupData GetSetupData() {return m_Setup;}
};