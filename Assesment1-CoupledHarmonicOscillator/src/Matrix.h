//Description of the Matrix class (was planning on using this matrix class in other projects hence the slighty over engineered nature of it(i.e column major and row major formats))
//function definitions
//Allocate memory : will either allocate memory for the object itself or a ptr that you provide it, allowing you to create a array of the same size and shape without having to make a new matrix object and dealing with the memeory overheads of that
//Free memory : works the same way as allocate memory except we're freeing the memory rather than allocating it
//[] overload : allows me to access the matrix class in the same way as a vector of vectors (or array of arrays)
//Matrix : the constructor, takes a setup struct as a parameter. The reasoning behind using a struct is that it allows for very quick changes to be made to the code without having to masssivly rewrite functions
//~Matrix : when the matrix objects gets deleted this calls upon free memory
//fill : fils the matrix with values, this works in a similar way to eigen sparse matracies where you specify the value position of matrix elements
//Copy array : copies the values from one array of matrixdatatype to another
//set system tolerance : sets the tolarance for the QU decomposition
//geteigenvalues : will trigger the QU decomposition if m_eigenvalues is empty, otherwise it just returns that vector
//QU decompositon : performs the QU decompostion, this is done recursivley 


#include <complex>
#include <vector>

enum class MatrixFormat
{
	COLUMNMAJOR = 0,
	ROWMAJOR,
	DEFAULT //Default will be column major as it's easier to implement QR decomposition for a column major matrix
};

using MatDataType = long double;

class Matrix
{
private:
	MatrixFormat m_Format;
	
	MatDataType** m_Data; 
	uint32_t m_Rows;
	uint32_t m_Cols;

	std::vector<MatDataType> m_EigenValues;
	
public: //lets me get a const reference to the variable without using get functions 
	const MatrixFormat& Format = m_Format;
	const uint32_t& Rows = m_Rows;
	const uint32_t& Cols = m_Cols;

private:

	bool AllocateMemory(uint32_t dim1, uint32_t dim2, MatDataType InitialValue, MatDataType*** DataPtr = nullptr, bool ExternalPtr = false);
	bool FreeMemory(uint32_t dim1, uint32_t dim2, MatDataType** DataPtr = nullptr);

public: 
	struct MatrixSetup
	{
		uint32_t rows;
		uint32_t cols;
		MatrixFormat type;
		MatDataType InitialValue = 0; //0 by default
	};

	struct Triplet
	{
		uint32_t row;
		uint32_t col;
		MatDataType Value;
	};

	const MatDataType* operator[](int); //overloading the [] operator allows the code to be written like obj[2][3];

	Matrix(MatrixSetup);
	~Matrix();

	bool Fill(std::vector<Triplet>);

	//matrix methods
	bool QUdecomposition(Matrix*, std::vector<long double> = {});
	bool CopyArray(MatDataType*, uint32_t, MatDataType*, uint32_t); //arr1 - array to be copied, arr2 - array to be copied to

	void SetSystemTolerance(double tol);

	std::vector<long double> GetEigenValues();
};