#include "Matrix.h"
#include <cmath>

Matrix::Matrix() : _m{ 0 }
{
}

// Constructor that initialises all elements.  Note that an initialiser list is used
// to pass in the values.  This is a new C++ feature.
Matrix::Matrix(std::initializer_list<float> list) : _m{ 0 }
{
	// Check that the initialiser list is the same size as the 
	// size of the array
	if (list.size() != ROWS * COLS)
	{
		// If not, throw an exception
		throw "Insufficient values supplied";
	}
	// Use iterator to work through the initialiser list, 
	// retrieving each value in turn.
	auto iterator = list.begin();
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			_m[i][j] = *iterator++;
		}
	}
}

// Copy constructor
Matrix::Matrix(const Matrix& other) noexcept
{
	Copy(other);
}

// Destructor
Matrix::~Matrix() noexcept
{
}

// Retrieve value in matrix at specified row and column
float Matrix::GetM(const int row, const int column) const
{
	return _m[row][column];
}

// Set value in matrix at specified row and column
void Matrix::SetM(const int row, const int column, const float value)
{
	_m[row][column] = value;
}

// Copy assignment operator
Matrix& Matrix::operator= (const Matrix &rhs) noexcept
{
	if (this != &rhs)
	{
		// Only do if not assigning to ourselves (remember that a = b is the same as a.operator=(b))
		Copy(rhs);
	}
	return *this;
}

// Test for two matrices being equivalent
bool Matrix::operator==(const Matrix &other) const
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (_m[i][j] != other.GetM(i, j))
			{
				return false;
			}
		}
	}
	return true;
}

// Multiply two matrices together
const Matrix Matrix::operator*(const Matrix &other) const
{
	Matrix result;
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			float resultValue = 0;
			for (int k = 0; k < ROWS; k++)
			{
				resultValue += _m[i][k] * other.GetM(k, j);
			}
			result.SetM(i, j, resultValue);
		}
	}
	return result;
}

const Vertex Matrix::operator*(const Vertex &p) const
{
	Vertex newVertex(p);

	newVertex.SetX(_m[0][0] * p.GetX() + _m[0][1] * p.GetY() + _m[0][2] * p.GetZ() + _m[0][3] * p.GetW());
	newVertex.SetY(_m[1][0] * p.GetX() + _m[1][1] * p.GetY() + _m[1][2] * p.GetZ() + _m[1][3] * p.GetW());
	newVertex.SetZ(_m[2][0] * p.GetX() + _m[2][1] * p.GetY() + _m[2][2] * p.GetZ() + _m[2][3] * p.GetW());
	newVertex.SetW(_m[3][0] * p.GetX() + _m[3][1] * p.GetY() + _m[3][2] * p.GetZ() + _m[3][3] * p.GetW());
	return newVertex;
}

//
// Retrieves the position data stored in this matrix
//
const Point3D<float> Matrix::GetPosition() const
{
	return { _m[0][3], _m[1][3], _m[2][3] };
}

//
// Retrieves the rotation data stored in this matrix
//
const Point3D<float> Matrix::GetRotation() const
{
	if (_m[0][0] == 1.0f || _m[0][0] == -1)
	{
		return { std::atan2f(_m[0][2], _m[2][3]), 0, 0 };
	}
	
	return { std::atan2(-_m[2][0], _m[0][0]),
			 std::asin(_m[1][0]),
			 std::atan2(-_m[1][2], _m[1][1]) };
}

//
// Retrieves the scale data stored in this matrix
//
const Point3D<float> Matrix::GetScale() const
{
	return { std::sqrt(_m[0][0] * _m[0][0] + _m[0][1] * _m[0][1] + _m[0][2] * _m[0][2]),
			 std::sqrt(_m[1][0] * _m[1][0] + _m[1][1] * _m[1][1] + _m[1][2] * _m[1][2]),
			 std::sqrt(_m[2][0] * _m[2][0] + _m[2][1] * _m[2][1] + _m[2][2] * _m[2][2]) };
}

Matrix Matrix::IdentityMatrix()
{
	return Matrix{ 1, 0, 0, 0,
				   0, 1, 0, 0,
				   0, 0, 1, 0,
				   0, 0, 0, 1 };
}

//
// Matrix to translate all vertices of a model by a translation amount
//
Matrix Matrix::TranslationMatrix(const float& translateX, const float& translateY, const float& translateZ)
{
	return Matrix { 1, 0, 0, translateX,
					0, 1, 0, translateY,
					0, 0, 1, translateZ,
					0, 0, 0, 1 };
}

//
// Matrix to rotate a model by a set of angles
//
Matrix Matrix::RotationMatrix(const float& rotationX, const float& rotationY, const float& rotationZ)
{
	return Matrix::RotationMatrixX(rotationX) * Matrix::RotationMatrixY(rotationY) * Matrix::RotationMatrixZ(rotationZ);
}

//
// Matrix to scale a model by a scaling factor
//
Matrix Matrix::ScaleMatrix(const float& x, const float& y, const float& z)
{
	return Matrix { x, 0, 0, 0,
					0, y, 0, 0,
					0, 0, z, 0,
					0, 0, 0, 1 };
}

//
// Translation, Rotation, Scale matrix.
// Combines all three transformations into one function call.
//
Matrix Matrix::TRS(const Point3D<float>& translation, const Point3D<float>& rotation, const Point3D<float>& scale)
{
	return Matrix::TranslationMatrix(translation.X, translation.Y, translation.Z) *
		   Matrix::RotationMatrix(rotation.X, rotation.Y, rotation.Z) *
		   Matrix::ScaleMatrix(scale.X, scale.Y, scale.Z);
}

//
// Rotation along the X axis.
//
Matrix Matrix::RotationMatrixX(const float& angle)
{
	return Matrix { 1,				 0,				   0, 0,
					0, std::cos(angle), -std::sin(angle), 0,
					0, std::sin(angle),  std::cos(angle), 0,
					0,				 0,				   0, 1 };
}

//
// Rotation along the Y axis.
//
Matrix Matrix::RotationMatrixY(const float& angle)
{
	return Matrix { std::cos(angle), 0, std::sin(angle), 0,
								  0, 1,				  0, 0,
				   -std::sin(angle), 0, std::cos(angle), 0,
								  0, 0,				  0, 1 };
}

//
// Rotation along the Z axis.
//
Matrix Matrix::RotationMatrixZ(const float& angle)
{
	return Matrix { std::cos(angle), -std::sin(angle), 0, 0,
					std::sin(angle),  std::cos(angle), 0, 0, 
								  0,				0, 1, 0,
								  0,				0, 0, 1 };
}

// Private method to copy contents of one matrix
// to another
void Matrix::Copy(const Matrix & other)
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			_m[i][j] = other.GetM(i, j);
		}
	}
}

