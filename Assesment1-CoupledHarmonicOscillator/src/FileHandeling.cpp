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
				buffer += (*c);
			}

			if ((*c) == '{' || (*c) == '}')
			{
				buffer.clear();
			}

			if ((*c) == ';')
			{
				if (Variable == true)
				{
					m_Data.push_back({ v, buffer });
				}
				buffer.clear();
				v = "";
				Variable = false;
			}

			if ((*c) == '=')
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
	if (!this->m_Output.is_open())
	{
		m_Output.open(this->m_Outpath);
	}

	m_Output << out;
	return true;
}
