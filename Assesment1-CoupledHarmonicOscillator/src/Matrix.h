//Description of the Matrix class

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
	
public:
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