//Implementation of the Matrix class
//This .cpp file contains the functions that use the 
#include "Matrix.h"

#include<iostream>

static double s_Tolerance = 0.0;

bool Matrix::QUdecomposition(Matrix* _ReturnMat, std::vector<long double> values)
{
	if (m_Format == MatrixFormat::ROWMAJOR)
	{
		std::cout << "MatrixFormat::ROWMAJOR is not supported by QR decomposition" << std::endl;
		_ReturnMat = nullptr;
		return false;
	}

	if (m_Rows != m_Cols)
	{
		std::cout << "Matrix is not square, not possible to find eigen values" << std::endl;
		_ReturnMat = nullptr;
		return false;
	}

	MatDataType** f = nullptr;
	if (!AllocateMemory(m_Cols, m_Rows, 0.0, &f, true))
	{
		_ReturnMat = nullptr;
		return false;
	}

	auto scalar = [](MatDataType* arr, uint32_t arrSize, double scalar)
		{
			for (int i = 0; i < arrSize; i++)
			{
				arr[i] = arr[i] * scalar;
			}
		};

	auto subtract = [](MatDataType* arr1, MatDataType* arr2, uint32_t size)
		{
			for (int i = 0; i < size; i++)
			{
				arr1[i] = arr1[i] - arr2[i];
			}
		};

	auto MaginitudeSquared = [](MatDataType* arr, uint32_t size)
		{
			long double SumSquared = 0;
			for (int i = 0; i < size; i++)
			{
				SumSquared += std::pow(arr[i], 2);
			}

			return SumSquared;
		};

	auto DotProduct = [](MatDataType* arr1, MatDataType* arr2, uint32_t size)
		{
			long double DotProd = 0.0;
			for (int i = 0; i < size; i++)
			{
				DotProd += arr1[i] * arr2[i];
			}

			return DotProd;
		};

	CopyArray(m_Data[0], m_Rows, f[0], m_Rows);
	for (int i = 1; i < m_Cols; i++)
	{
		CopyArray(m_Data[i], m_Rows, f[i], m_Rows);
		for (int e = 0; e < i; e++)
		{
			long double factor = DotProduct(m_Data[i], f[e], m_Rows);
			long double factor2 = MaginitudeSquared(f[e], m_Rows);
			factor = factor / factor2;

			MatDataType* tmp = nullptr;
			tmp = (MatDataType*)malloc(m_Rows * sizeof(MatDataType));
			if (tmp == nullptr)
			{
				std::cout << "[ERROR]: MEMORY NOT ALLOCATED" << std::endl;
				return false;
			}

			CopyArray(f[e], m_Rows, tmp, m_Rows);
			scalar(tmp, m_Rows, factor);
			subtract(f[i], tmp, m_Rows);
			
			free(tmp);
		}
	}

	MatDataType** q = nullptr;
	if(!AllocateMemory(m_Rows, m_Cols, 0.0, &q, true))
	{
		_ReturnMat = nullptr;
		return false;
	}

	for (int i = 0; i < m_Cols; i++)
	{
		long double factor2 = MaginitudeSquared(f[i], m_Rows);
		CopyArray(f[i], m_Rows, q[i], m_Rows);
		scalar(q[i], m_Rows, 1.0 / std::sqrt(factor2));
	}

	//Q matrix is done
	//setting up the U matrix
	
	MatDataType** u = nullptr;
	if (!AllocateMemory(m_Cols, m_Cols, 0.0, &u, true))
	{
		_ReturnMat = nullptr;
		return false;
	}

	for (int i = 0; i < m_Cols; i++)
	{
		long double mag = std::sqrt(MaginitudeSquared(f[i], m_Rows));
		for (int e = 0; e <= i; e++)
		{
			if (e == i)
			{
				u[i][e] = mag;
				break;
			}
			else
			{
				long double dot = DotProduct(m_Data[i], q[e], m_Rows);
				u[i][e] = dot;
			}
		}
		free(f[i]);
	}
	free(f);

	//testing
	//for (int i = 0; i < m_Rows; i++)
	//{
	//	for (int e = 0; e < m_Cols; e++)
	//	{
	//		std::cout << q[e][i] << " ";
	//	}
	//
	//	std::cout << std::endl;
	//}

	//for (int i = 0; i < m_Cols; i++)
	//{
	//	for (int e = 0; e < m_Cols; e++)
	//	{
	//		std::cout << u[e][i] << " ";
	//	}
	//	std::cout << std::endl;
	//}
	//
	for (uint32_t i = 0; i < m_Cols; i++)
	{
		for (uint32_t j = 0; j < m_Rows; j++)
		{
			long double MatrixEntry = 0.0;
			for (uint32_t k = 0; k < m_Cols; k++)
			{
				MatrixEntry += u[k][i] * q[j][k];
			}
			_ReturnMat->Fill({ {i,j, MatrixEntry} });
		}
	}
	
	long double diff2 = 0;
	std::vector<long double> val = {};
	for (int i = 0; i < m_Cols; i++)
	{
		for (int e = 0; e < m_Rows; e++)
		{
			//std::cout << _ReturnMat->operator[](e)[i] << " ";

			if (i == e)
			{
				val.push_back(_ReturnMat->operator[](i)[i]);
			}
			
			if (values.size() == 0)
			{
				diff2 = 1;
				continue;
			}

			if (i == e)
			{
				diff2 += std::pow(_ReturnMat->operator[](i)[i] - values[i], 2);
			}
		}
	
		//std::cout << std::endl;
	}

	diff2 = std::sqrt(diff2);

	for (int i = 0; i < m_Cols; i++)
	{
		free(u[i]);
		free(q[i]);
	}
	free(q);
	free(u);

	if (diff2 > s_Tolerance)
	{
		Matrix A({ m_Rows,m_Cols,MatrixFormat::DEFAULT });
		_ReturnMat->QUdecomposition(&A, val);
	}
	else
	{
		for (int i = 0; i < m_Cols; i++)
		{
			m_EigenValues.push_back(_ReturnMat->operator[](i)[i]);
		}
		return true;
	}

	auto vec = _ReturnMat->m_EigenValues;
	for (int i = 0; i < vec.size(); i++)
	{
		m_EigenValues.push_back(vec[i]);
	}

	return true;
}


void Matrix::SetSystemTolerance(double tol)
{
	s_Tolerance = tol;
}

std::vector<long double> Matrix::GetEigenValues()
{
	if (m_EigenValues.size() == 0)
	{
		Matrix mat({ m_Rows,m_Cols,MatrixFormat::DEFAULT });
		this->QUdecomposition(&mat);
	}

	return m_EigenValues;
}



