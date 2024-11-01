// CoupledHarmonicOscillator.cpp
//This program aims to find the eignevalues of the coupled harmnonic oscillator via the QR iterative method. 
//It reads a file in "setup.txt" that specifies how the system should be defined, currently it assumes the mass of all the particles is the same and that the spring constants are all the same
//the code loops over all the values of m for every value of k
//the code then runs the Simulation function that creates tbe matrix and calls on the eigenvalue solver
//the code then outputs this to a csv that is specified by setup.txt

//function definitions
//Simulation: calls the other functions that run the bulk of the code
//main: reads the input file and sets the system up, also writes to the output file

#include <iostream>

#include "Matrix.h"
#include "FileHandeling.h"

#include <sstream>


std::vector<MatDataType> Simulation(long double k, long double m, long double tol)
{
	long double OneOverM = 1 / m; //division is really slow, so do it once rather than 4 times
	Matrix M({ 2,2,MatrixFormat::COLUMNMAJOR });
	M.Fill({ {0, 0, -2.0 * k * OneOverM }, {0, 1,k * OneOverM },
			 {1, 0, k * OneOverM}, {1,1, -2.0 * k * OneOverM} });

	for (int i = 0; i < 2; i++) //prints out the matrix but in coloumn major form
	{
		for (int e = 0; e < 2; e++)
		{
			std::cout << M[e][i] << " ";
		}
		std::cout << std::endl;
	}
	M.SetSystemTolerance(tol);
	M.GetEigenValues();

	for (int i = 0; i < M.GetEigenValues().size(); i++)
	{
		std::cout << M.GetEigenValues()[i] << std::endl;
	}

	return M.GetEigenValues();
}

int main()
{
	//std::vector<std::string> var = { "K1", "K2", "MinK", "MaxK", "SyncK", "StepK", "M1", "M2", "MinM", "MaxM", "SyncM", "StepM", "SystemTol"};
	FileHandeling setup("Setup.txt");//, var);

	struct sys
	{
		long double k;
		std::pair<long double, long double> RangeK;
		long double StepK;

		long double m;
		std::pair<long double, long double> RangeM;
		long double StepM;

		long double tol;
		std::string Output;
	};

	sys def;
	std::vector<std::pair<std::string, void*>> VarList = { {"K", (void*)&def.k}, {"MinK", (void*)&def.RangeK.first}, {"MaxK", (void*)&def.RangeK.second}, {"StepK", (void*)&def.StepK },
													   {"M", (void*)&def.m}, {"MinM", (void*)&def.RangeM.first}, {"MaxM", (void*)&def.RangeM.second}, {"StepM", (void*)&def.StepM },
													   {"SystemTolerance", (void*)&def.tol}, {"OutputFile", (void*)&def.Output} }; //avoids me using multiple if statements though could potentially cause issues on systems with void* of a different size

	setup.LoadFile();
	auto dat = setup.GetData();
	for (unsigned int i = 0; i < dat.size(); i++)
	{
		long double v = 0;
		try {
			v = std::stod(dat[i].second);
		}
		catch (std::invalid_argument& e) {
			if (dat[i].first != "OutputFile") //all variabels should be a double apart from output file
			{
				std::cout << "Variable " << dat[i].first << " is not a number. Quiting program" << std::endl;
				return 0;
			}
		}

		for (unsigned int a = 0; a < VarList.size(); a++)
		{
			if (dat[i].first == "OutputFile")
			{
				if (VarList[a].first == "OutputFile")
				{
					*(std::string*)VarList[a].second = dat[i].second; //explicit casting of the void* to a string pointer and the derefencing it
					break;
				}
			}
			if (dat[i].first == VarList[a].first)
			{
				*(long double*)VarList[a].second = v; //explicit casting of the void* to a double pointer and the derefencing it
				break;
			}
		}
	}

	setup.SetOutputFile(def.Output); 
	setup.WriteToFile("step,k,m,w1,w2\n");

	int step = 0;
	for (long double k = def.RangeK.first; k <= def.RangeK.second + def.tol; k = k + def.StepK)
	{
		for (long double m = def.RangeM.first; m <= def.RangeM.second + def.tol; m = m + def.StepM)
		{
			step = step + 1;
			auto eig = Simulation(k, m, def.tol);

			std::stringstream ss;
			ss << step << "," << k << "," << m << "," << std::sqrt(-1*eig[0]) << "," << std::sqrt(-1*eig[1]) << "\n";
			std::string out = ss.str();
			setup.WriteToFile(out);
		}
	}

	setup.CloseOutputFile();

	return 1;

}

