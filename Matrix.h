#pragma once
#include "Point3D.h"
#include "Vertex.h"

// Class for a 4 x 4 matrix object.  

const int ROWS = 4;
const int COLS = 4;

#include <initializer_list>

//
// Transformation matrix
//
class Matrix
{
	public:
		// Default constructor
		Matrix();
		Matrix(std::initializer_list<float> list);
		Matrix(const Matrix& other) noexcept;

		~Matrix() noexcept;
		
		float GetM(const int row, const int column) const;
		void SetM(const int row, const int column, const float value);

		Matrix& operator= (const Matrix &rhs) noexcept;
		bool	operator==(const Matrix&other) const;

		const Matrix operator*(const Matrix &other) const;
		const Vertex operator*(const Vertex &other) const;

		const Matrix Inverse() const;

		//
		// Information retrival
		//
		const Point3D<float> GetPosition() const;
		const Point3D<float> GetRotation() const;
		const Point3D<float> GetScale() const;

		// Default matrix
		static Matrix IdentityMatrix();
		static Matrix ZeroMatrix();

		//
		// Transformation matrices
		//
		static Matrix TranslationMatrix (const float& x, const float& y, const float& z);
		static Matrix RotationMatrix (const float& x, const float& y, const float& z);
		static Matrix ScaleMatrix (const float& x, const float& y, const float& z);

		// Combined transformation matrix
		static Matrix TRS(const Point3D<float>& translation, const Point3D<float>& rotation, const Point3D<float>& scale);

	private:
		//
		// Rotation matrix axis
		//
		static Matrix RotationMatrixX(const float& angle);
		static Matrix RotationMatrixY(const float& angle);
		static Matrix RotationMatrixZ(const float& angle);

	private:
		float _m[ROWS][COLS];

		// Private method to copy contents of one matrix
		// to another
		void Copy(const Matrix& other);
};

