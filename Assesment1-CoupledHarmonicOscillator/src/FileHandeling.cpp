#include "FileHandeling.h"


FileHandeling::FileHandeling(std::string filepath)// std::vector<std::string> var)
	:m_Filepath(filepath)//, m_Var(var)
{
}

bool FileHandeling::LoadFile()
{
	this->m_Input.open(m_Filepath);

	std::string buffer = "";
	std::string FileLine = "";

	bool Variable = false;
	std::string v = "";

	std::vector<char> SpecialChar = { '{', '}', ';', '=', '\t', ' ', '\r'};


	while (std::getline(this->m_Input, FileLine))
	{
		for (auto c = FileLine.cbegin(); c != FileLine.cend(); c++)
		{
			if (std::find(SpecialChar.begin(), SpecialChar.end(), (*c)) == SpecialChar.end())
			{
				buffer += (*c); //only adds a character to the buffer if it's not a special character 
			}

			if ((*c) == '{' || (*c) == '}')
			{
				buffer.clear(); //clears the buffer as  { or } indicates the start of a block in the setup file
			}

			if ((*c) == ';')
			{
				if (Variable == true)
				{
					m_Data.push_back({ v, buffer }); //finds the value of the variable and uses the string on the lhs of the = sign as the variable name
				}
				buffer.clear();
				v = "";
				Variable = false;
			}

			if ((*c) == '=') //indicates that this line contains a variable
			{
				v = buffer;
				Variable = true;
				buffer.clear();
			}
		}
	}

	m_Input.close();

	return true;
}

bool FileHandeling::WriteToFile(std::string out)
{	
	if (!this->m_Output.is_open()) //checks wether the output file is open before writing to it
	{
		m_Output.open(this->m_Outpath); 
	}

	m_Output << out;
	return true;
}
