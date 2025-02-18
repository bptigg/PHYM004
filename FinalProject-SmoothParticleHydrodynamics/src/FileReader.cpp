#include "FileReader.h"
#include <fstream>
#include <iostream>
#include <algorithm>

bool FileReader::LoadFile(std::string& filename)
{
     //open the file
    std::ifstream SetupFile(filename);
    if(!SetupFile.good())
    {
        std::cout << "File: " << filename << " doesn't exist, exiting program" << std::endl;
    }

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
}