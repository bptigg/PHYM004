#include "FileReader.h"
#include <fstream>
#include <iostream>

bool FileReader::LoadFile(std::string& filename)
{
    //open the file
    std::ifstream SetupFile(filename);
    if(!SetupFile.good())
    {
        std::cout << "File: " << filename << " doesn't exist, exiting program" << std::endl;
    }

    enum class Object { //lets the file reader work as a state machine 
        Body = 0,
        Setup,
        Default
    };

    Object mode = Object::Default;

    std::string file;
    std::string buffer;
    std::vector<char> Ignore = {'\n', '\t'};
    while(getline(SetupFile,buffer))
    {
        for(auto c = buffer.begin(); c != buffer.end(); c++)
        {
            if(std::find(Ignore.begin(), Ignore.end(), (*c)) != Ignore.end())
            {
                continue;
            }

            file = file + (*c);
        }
    }

    std::cout << file << std::endl;
    buffer.clear();
    bool InObject = false;
    for(auto c = file.begin(); c != file.end(); c++)
    {
        if((*c) == ' ')
            continue;
        
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
        if(mode == Object::Default)
        {
            if(buffer.compare("Body") == 0 || buffer.compare("body") == 0)
            {
                mode = Object::Body;
                BodyData b;
                m_Bodies.push_back(b);
            }
            else if(buffer.compare("Setup") == 0 || buffer.compare("setup") == 0)
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
        if(mode == Object::Body)
        {
            if(!InObject)
            {
                continue;
            }

            if(buffer.compare("InitialPosition") == 0)
            {
                std::array<double,3> values = {0.0, 0.0, 0.0};
                buffer.clear();
                while((*c) != '"')
                {
                    c++;
                }
                c++;
                while((*c) != '"')
                {
                    buffer += (*c);
                    c++;
                }
                //std::cout << buffer << std::endl;
                std::string numberbuffer = "";
                int num = 0;
                for(auto c : buffer)
                {
                    if(c == '"')
                        continue;
                    if(c == ' ')
                    {
                        double val = std::stod(numberbuffer);
                        values[num] = val;
                        num++;
                        numberbuffer.clear();
                    }
                    else
                    {
                        numberbuffer += c;
                    }
                }

                m_Bodies[m_Bodies.size()-1].pos = values;
                buffer.clear();
            }

            if(buffer.compare("InitialVelocity") == 0)
            {
                std::array<double,3> values = {0.0, 0.0, 0.0};
                buffer.clear();
                while((*c) != '"')
                {
                    c++;
                }
                c++;
                while((*c) != '"')
                {
                    buffer += (*c);
                    c++;
                }
                //std::cout << buffer << std::endl;
                std::string numberbuffer = "";
                int num = 0;
                for(auto c : buffer)
                {
                    if(c == '"')
                        continue;
                    if(c == ' ')
                    {
                        double val = std::stod(numberbuffer);
                        values[num] = val;
                        num++;
                        numberbuffer.clear();
                    }
                    else
                    {
                        numberbuffer += c;
                    }
                }

                m_Bodies[m_Bodies.size()-1].vel = values;
                buffer.clear();
            }

            if(buffer.compare("Mass") == 0 || buffer.compare("mass") == 0)
            {
                std::string numbuffer;
                while((*c) != '=')
                {
                    c++;
                }
                c++;
                while((*c) != ';')
                {
                    if((*c) == ' ') { c++; continue;}
                    numbuffer += (*c);
                    c++;
                }
                m_Bodies[m_Bodies.size() - 1].mass = std::stod(numbuffer);
                buffer.clear();
            }
        }
        if(mode == Object::Setup)
        {
            auto ReadValue = [&c]() {
                std::string numbuffer;
                while((*c) != '=')
                {
                    c++;
                }
                c++;
                while((*c) != ';')
                {
                    if((*c) == ' ') { c++; continue;}
                    numbuffer += (*c);
                    c++;
                }
                return std::stod(numbuffer);
            };

            auto ReadString = [&c]() {
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

            if(buffer.compare("Timestep") == 0 || buffer.compare("timestep") == 0)
            {
                m_Setup.timestep = ReadValue();
                buffer.clear();
            }
            if(buffer.compare("Epsilon") == 0 || buffer.compare("epsilon") == 0)
            {
                m_Setup.epsilon = ReadValue();
                buffer.clear();
            }
            if(buffer.compare("Steps") == 0 || buffer.compare("steps") == 0)
            {
                m_Setup.steps = ReadValue();
                buffer.clear();
            }
            if(buffer.compare("OutputFile") == 0)
            {
                m_Setup.OutputFile = ReadString();
                buffer.clear();
            }
        }
    }

}