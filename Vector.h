#pragma once
#include "Point.h"

template<typename TNumeric>
struct Vector3D
{
	TNumeric X;
	TNumeric Y;
	TNumeric Z;

	// Multiply
	inline Vector3D<TNumeric> operator*(const Vector3D<TNumeric>& rhs) const
	{
		return {
			X * rhs.X,
			Y * rhs.Y,
			Z * rhs.Z
		};
	}

	// Multiply by point
	inline Vector3D<TNumeric> operator*(const TNumeric& rhs) const
	{
		return {
			X * rhs,
			Y * rhs,
			Z * rhs
		};
	}

	// Negate
	inline Vector3D<TNumeric> operator-() const
	{
		return *this * -1;
	}

	// Addition
	inline Vector3D<TNumeric> operator+(const Vector3D<TNumeric>& rhs) const
	{
		return {
			X + rhs.X,
			Y + rhs.Y,
			Z + rhs.Z
		};
	}

	// Concatenation / addition
	inline Vector3D<TNumeric> operator+=(const Vector3D<TNumeric>& rhs) const
	{
		return *this + rhs;
	}
};

//
// Standard definition of 3D vector.
//
using Vector3 = Vector3D<float>;