#include "FileReader.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>

bool FileReader::LoadFile(std::string& filename)
{
     //open the file
    std::ifstream SetupFile(filename);
    if(!SetupFile.good())
    {
        std::cout << "File: " << filename << " doesn't exist, exiting program" << std::endl;
    }

    enum class Object{
        Region = 0,
        Setup,
        Default
    };

    Object mode = Object::Default;

    std::string file;
    std::string buffer;
    std::vector<char> Ignore = {'\n', '\t'};
    while(getline(SetupFile,buffer))
    {
        std::string LineString = "";
        for(auto c = buffer.begin(); c != buffer.end(); c++)
        {
            if(std::find(Ignore.begin(), Ignore.end(), (*c)) != Ignore.end())
            {
                continue;
            }
            if((*c) == ' ' && LineString.empty())
            {
                continue;
            }

            file = file + (*c);
            LineString = LineString + (*c);
        }
    }

    std::cout << file << std::endl;
    buffer.clear();
    bool InObject = false;
    int RegionComponents = 0;
    int SetupComponents = 0; 
    bool Polytropic = false;
    bool Thermal = false;
    bool Density = false;
    for(auto c = file.begin(); c != file.end(); c++)
    {
        if((*c) == '{' && mode != Object::Default)
        {
            InObject = true;
            buffer.clear();
            continue;
        }

        if((*c) == '}' && mode != Object::Default)
        {
            InObject = false;
            buffer.clear();
            mode = Object::Default;
            continue;
        }
        
        if((*c) == ';')
        {
            continue;
        }

        buffer += (*c);

        if(mode == Object::Default and InObject == false)
        {
            if(buffer.compare("Region") == 0 || buffer.compare("region") == 0)
            {
                mode = Object::Region;
                m_Regions.push_back(RegionData(0.0,{}));
            }
            if(buffer.compare("Setup") == 0 || buffer.compare("setup") == 0)
            {
                mode = Object::Setup;
            }
            else
            {
                continue;
            }
            buffer.clear();
            continue;
        }

        auto ReadString = [](std::string::iterator& c) {
            std::string strbuffer;
            while((*c) != '=')
            {
                c++;
            }
            c++;
            while((*c) != ';')
            {
                if((*c) == ' ') { c++; continue; }
                if((*c) == '"') { c++; continue; }
                strbuffer += (*c);
                c++;
            }
            return strbuffer;
        };

        if(mode == Object::Region)
        {
            if(!InObject) {continue;}

            if(buffer.compare("boundary") == 0 || buffer.compare("Boundary") == 0)
            {
                //input should look like this "1 0 0" i.e. if two boundary conditions objects where in the file e.g. "-2 0 0" and "2 0 0" the region would look like this |**| in 1D
                //In 2D they represent points so 4 boundary poitns would be needed to represent a square e.g. "-1 -1 0" "-1 1 0" "1 -1 0" "1 1 0" would be 
                /*  
                    |-|
                    |*|
                    |-|
                */      
                std::array<double,3> Boundary = {0.0, 0.0, 0.0};
                buffer.clear();
                while ((*c) != '"') {c++;}
                c++;
                while ((*c) != '"') {
                    buffer += (*c);
                    c++;
                };
                std::vector<std::string> coord = SplitString(buffer, " ");
                for(int i = 0; i < 3; i++)
                {
                    Boundary[i] = std::stod(coord[i]);
                }
                buffer.clear();
                m_Regions[m_Regions.size() - 1].Boundaries.push_back(Boundary);
                RegionComponents += 1;
            }
            if(buffer.compare("InternalEnergy") == 0 || buffer.compare("internal energy") == 0) //not required if density is provided
            {
                buffer.clear();
                while ((*c) != '"') {c++;}
                c++;
                while ((*c) != '"') {
                    buffer += (*c);
                    c++;
                };
                m_Regions[m_Regions.size() - 1].InitialInternalEnergy = std::stod(buffer);
                buffer.clear();
                RegionComponents += 1;
                Thermal = true;
            }
            if(buffer.compare("Density") == 0 || buffer.compare("density") == 0)
            {
                m_Regions[m_Regions.size() - 1].InitialDensity = std::stod(ReadSingleValue(c));
                buffer.clear();
                RegionComponents += 1;
                Density = true;
            }
        }
        if(mode == Object::Setup)
        {
            if(!InObject) {continue;}
            if(buffer.compare("Particles") == 0 || buffer.compare("particles") == 0)
            {
                m_Setup.Particles = std::stod(ReadSingleValue(c));
                buffer.clear();
                SetupComponents += 1;
            }
            if(buffer.compare("InitialTimestep") == 0 || buffer.compare("initial timestep") == 0 )
            {
                m_Setup.timestep = std::stod(ReadSingleValue(c));
                buffer.clear();
                SetupComponents += 1;
            }
            if(buffer.compare("Steps") == 0 || buffer.compare("steps") == 0)
            {
                m_Setup.steps = std::stod(ReadSingleValue(c));
                buffer.clear();
                SetupComponents += 1;
            }
            if(buffer.compare("SmoothingLength") == 0 || buffer.compare("smoothing length") == 0)
            {
                m_Setup.SmoothingLength = std::stod(ReadSingleValue(c));
                buffer.clear();
                SetupComponents += 1;
            }
            if(buffer.compare("Dimensions") == 0 || buffer.compare("dimensions") == 0)
            {
                m_Setup.Dimensions = std::stoi(ReadSingleValue(c));
                buffer.clear();
                SetupComponents += 1;
            }
            if(buffer.compare("Gamma") == 0 || buffer.compare("gamma") == 0)
            {
                m_Setup.Gamma = std::stod(ReadSingleValue(c));
                buffer.clear();
                //SetupComponents += 1;
                Polytropic = true;
            }
            if(buffer.compare("BoundaryConditions") == 0 || buffer.compare("boundary conditions") == 0)
            {
                buffer = ReadString(c);
                if(buffer.compare("true") == 0)
                {
                    m_Setup.BoundaryConditions = true;
                }
                else
                {
                    m_Setup.BoundaryConditions = false;
                }
                buffer.clear();
                SetupComponents += 1;
            }
            if(buffer.compare("OutputFile") == 0 || buffer.compare("output file") == 0)
            {
                m_Setup.OutputFile = ReadString(c);
                buffer.clear();
                SetupComponents += 1;
            }
        }
    }

    //Evaluate physical boundaries
    if(m_Setup.Dimensions == 1) //boundary positions represent points
    {
        std::vector<std::array<double,3>> AllBoundaries;
        for(int i = 0; i < m_Regions.size(); i++) {
            for(int j = 0; j < m_Regions[i].Boundaries.size(); j++)
            {
                AllBoundaries.push_back(m_Regions[i].Boundaries[j]);
            }
        }

        std::set<std::array<double,3>> TempSet(AllBoundaries.begin(), AllBoundaries.end());
        m_Setup.PhysicalBoundaries.assign(TempSet.begin(), TempSet.end());
    }
    if(m_Setup.Dimensions != 1)
    {
        std::cout << "More than 1 dimension not supported" << std::endl;
        return false;
    }

    if(Polytropic && !Density)
    {
        std::cout << "Density not provided for Polytropic Equation of State" << std::endl;
        return false;
    }

    if(!Polytropic && !Thermal && !Density)
    {
        std::cout << "Polytropic constant not provided" << std::endl;
        return false;
    }

    if(RegionComponents <= 2*m_Regions.size())
    {
        std::cout << "Sufficient region data not provided" << std::endl;
        return false;
    }
    if(SetupComponents != 7)
    {
        std::cout << "Sufficient setup data not provided" << std::endl;
        return false;
    }
    
    return true;
}

std::vector<std::string> FileReader::SplitString(std::string WholeString, std::string delimiter)
{
    std::vector<std::string> SplitString = {};
    size_t DelimeterPosition = 0;
    std::string PartialString;
    while((DelimeterPosition = WholeString.find(delimiter)) != std::string::npos)
    {
        PartialString = WholeString.substr(0, DelimeterPosition);
        SplitString.push_back(PartialString);
        WholeString.erase(0,DelimeterPosition + delimiter.length());
    }
    SplitString.push_back(WholeString);
    return SplitString;
}

std::string FileReader::ReadSingleValue(std::string::iterator& c)
{
    std::string ReturnBuffer;
    while((*c) != '"') {c++;}
    c++;
    while((*c) != '"') {
        ReturnBuffer += (*c);
        c++;
    };
    return ReturnBuffer;
}

FileOutput::FileOutput(std::string filename, OutputLockGuard* OLG)
{
    std::array<std::string,5> Files = {"ENERGY_MOMENTUM", "POSITION", "DENSITY", "VELOCITY", "ENERGY_POSITION"};
    for(auto extension : Files)
    {
        std::string ModifiedFilename = filename + "_" + extension + ".txt";
        auto Outfile = new std::ofstream(ModifiedFilename);
        //Outfile.close();
        m_Files.push_back(Outfile);
    }
    m_OLG = OLG;
    //m_ParticleData.push_back({});

}

FileOutput::~FileOutput()
{
    for (auto& it : m_Files)
    {
        it->close();
    }
    m_Files.clear();
    m_OLG = nullptr;
}

void FileOutput::UpdateData(ParticleData data, int id)
{
    m_DataLock.lock();
    {
        //m_ParticleData[m_ParticleData.size()-1].push_back({data,id});
        m_ParticleData.push_back({{data,id},data.step});
    }
    m_DataLock.unlock();
}

void FileOutput::WriteData()
{
    while(!m_OLG->kill)
    {
        while(m_CurrentStep < m_OLG->writeLimit)
        {
            double KE = 0;
            double U = 0;
            double Momentum = 0;
            std::vector<std::array<double,4>> Position_Density_Velocity;
            std::vector<std::pair<FileOutput::ParticleData, int>> data;

            {
                m_DataLock.lock();
                std::vector<std::pair<std::pair<ParticleData,int>,int>> copy = m_ParticleData;
                std::vector<int> indexs;
                m_DataLock.unlock();
                for(int i = 0; i < copy.size(); i++)
                {
                    if(copy[i].second == m_CurrentStep)
                    {
                        data.push_back(copy[i].first);
                        indexs.push_back(i);
                    }
                }
                m_DataLock.lock();
                int removed = 0;
                for(auto i : indexs)
                {
                    m_ParticleData.erase(m_ParticleData.begin() + i - removed);
                    removed++;
                }
                m_DataLock.unlock();
            }
            for(int i = 0; i < data.size(); i++)
            {
                auto pData = data[i].first;
                KE += pData.KineticEnergy;
                U  += pData.ThermalEnergy;
                Momentum += pData.Velocity;
            
                Position_Density_Velocity.push_back({pData.Position, pData.Density, pData.Velocity, pData.ThermalEnergy});
            }
        
            for (auto& it : m_Files)
            {
                auto file = &it - &m_Files[0];
                (*m_Files[file]) << m_CurrentStep;
                if(file == 0)
                {
                    (*m_Files[file]) << " " << KE << " " << U << " " << Momentum << " " << std::endl;
                    continue;
                }
                
                for(auto d : Position_Density_Velocity)
                {
                    (*m_Files[file]) << " " << d[file - 1];
                }
                (*m_Files[file]) << " " << std::endl;
            }
            m_CurrentStep++;
        }
    }   
}
