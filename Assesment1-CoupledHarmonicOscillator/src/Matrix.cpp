//Implementation of the Matrix class
//This .cpp file contains all the functions for interacting with the data of the matrix
/*
* 1. overloading of the [] operator to allow the code to be written like mat[4] which is actually mat.m_data[4]
* 2. Allocate memory, due to slightly over engineering the code the matrix supports both a column major and row major format,
* it therefore seemed like a good idea to have memory allocation done in a function, this also allows for the error flagging code
* to be much more readable
* 3. Free memory, see Allocate Memory
* 4. Matrix - the constructor
* 5. ~Matrix - the deconstructor
* 6. Fill, provided a method of filling the matrix with data, very similar to how the Eigen library fills a matrix
*/
#include "Matrix.h"

#include <iostream>

const MatDataType* Matrix::operator[](int n)
{
	return m_Data[n];
}

bool Matrix::AllocateMemory(uint32_t dim1, uint32_t dim2, MatDataType InitialValue, MatDataType*** DataPtr, bool ExternalPtr)
{
	MatDataType** TempPtr = nullptr;
	TempPtr = (MatDataType**)calloc(dim1, sizeof(MatDataType*));

	if (TempPtr == nullptr)
	{
		std::cout << "[ERROR]: MEMORY NOT ALLOCATED" << std::endl;
		return false;
	}

	for (int i = 0; i < dim1; i++)
	{
		TempPtr[i] = (MatDataType*)calloc(dim2, sizeof(MatDataType));

		if (TempPtr[i] == nullptr)
		{
			std::cout << "[ERROR]: MEMORY NOT ALLOCATED" << std::endl;
			return false;
		}

		if (InitialValue != std::complex<long double>(0))
		{
			for (int e = 0; e < dim2; e++)
			{
				TempPtr[i][e] = InitialValue;
			}
		}
	}

	if (ExternalPtr == false)
	{
		m_Data = TempPtr;
	}
	else
	{
		*DataPtr = TempPtr;
	}

	return true;
}

bool Matrix::FreeMemory(uint32_t dim1, uint32_t dim2, MatDataType** DataPtr)
{
	MatDataType** TempPtr = (DataPtr == nullptr) ? m_Data : DataPtr;
	
	if (TempPtr == nullptr)
	{
		return true;
	}

	for (int i = 0; i < dim1; i++)
	{
		free(TempPtr[i]);
		TempPtr[i] == nullptr;
	}

	free(TempPtr);
	TempPtr = nullptr;

	if (DataPtr == nullptr)
	{
		m_Data == nullptr;
	}
	else
	{
		DataPtr = nullptr;
	}

	return true;
}

Matrix::Matrix(Matrix::MatrixSetup setup)
	:m_Rows(setup.rows), m_Cols(setup.cols), m_Format(setup.type), m_EigenValues({})
{
	//std::cout << "Created" << std::endl;

	int type = 1; //0 - RM, 1 - CM
	bool error = false;
	if (m_Format == MatrixFormat::ROWMAJOR)
		type = 0;

	bool NoError = true;

	if (type == 0) //row major
	{
		NoError = AllocateMemory(m_Rows, m_Cols, setup.InitialValue);
	}
	else //column major
	{
		NoError = AllocateMemory(m_Cols, m_Rows, setup.InitialValue);
	}

}

Matrix::~Matrix()
{
	//std::cout << "Destroyed" << std::endl;
	int type = 1; //0 - RM, 1 - CM
	bool error = false;
	if (m_Format == MatrixFormat::ROWMAJOR)
		type = 0;

	bool NoError = true;

	if (type == 0) //row major
	{
		NoError = FreeMemory(m_Rows, m_Cols);
	}
	else //column major
	{
		NoError = FreeMemory(m_Cols, m_Rows);
	}
}

bool Matrix::Fill(std::vector<Triplet> Data)
{
	for (int i = 0; i < Data.size(); i++)
	{
		if (this->m_Format == MatrixFormat::ROWMAJOR)
		{
			m_Data[Data[i].row][Data[i].col] = Data[i].Value;
		}
		else
		{
			m_Data[Data[i].col][Data[i].row] = Data[i].Value;
		}
	
	}
	return false;
}

bool Matrix::CopyArray(MatDataType* arr1, uint32_t elem1, MatDataType* arr2, uint32_t elem2)
{
	if (elem2 != elem2)
	{
		std::cout << "[ERROR]: SIZE OF ARRAY1 IS NOT THE SAME AS THE SIZE OF ARRAY2" << std::endl;
		return false;
	}

	for (int i = 0; i < elem1; i++)
	{
		arr2[i] = arr1[i];
	}

	return true;
}

