#pragma once
#include "Point.h"

template<typename TNumeric>
struct Point3D
{
	TNumeric X;
	TNumeric Y;
	TNumeric Z;

	// Multiply
	inline Point3D<TNumeric> operator*(const Point3D<TNumeric>& rhs) const
	{
		return {
			X * rhs.X,
			Y * rhs.Y,
			Z * rhs.Z
		};
	}

	// Multiply by point
	inline Point3D<TNumeric> operator*(const TNumeric& rhs) const
	{
		return {
			X * rhs,
			Y * rhs,
			Z * rhs
		};
	}

	// Negate
	inline Point3D<TNumeric> operator-() const
	{
		return *this * -1;
	}
};
