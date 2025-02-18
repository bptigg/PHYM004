//definition of the filereader object
#ifndef SPH_FILEREADER
#define SPH_FILEREADER

#include <vector>
#include <string>
#include <array>

class FileReader
{
public:
    struct SetupData
    {
        double timestep;
        double steps;
        std::string OutputFile;
    };

private:
    SetupData m_Setup;

public:
    bool LoadFile(std::string&);

    inline const SetupData GetSetupData() {
        return m_Setup;
    } 
};
#endif