#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class FileHandeling
{
private:
	std::string m_Filepath;
	std::string m_Outpath;
	std::vector<std::string> m_Var;
	std::vector<std::pair<std::string, std::string>> m_Data;

	std::ofstream m_Output;
	std::ifstream m_Input;

public:
	FileHandeling(std::string);// , std::vector<std::string>);
	void SetOutputFile(std::string out) { this->m_Outpath = out; };

	bool LoadFile();
	bool WriteToFile(std::string);
	
	void CloseOutputFile() { m_Output.close(); };
	std::vector<std::pair<std::string, std::string>> GetData() { return this->m_Data; };
};

