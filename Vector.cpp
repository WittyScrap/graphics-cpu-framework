#include "Vector.h"

//
// Assignment operator.
//
Vector3& Vector3::operator=(const Vector3& rhs)
{
	if (rhs != *this)
	{
		X = rhs.X;
		Y = rhs.Y;
		Z = rhs.Z;
	}

	return *this;
}

//
// Addition operator.
//
Vector3 Vector3::operator+(const Vector3& rhs) const
{
	return {
		X + rhs.X,
		Y + rhs.Y,
		Z + rhs.Z
	};
}

//
// Subtraction operator.
//
Vector3 Vector3::operator-(const Vector3& rhs) const
{
	return {
		X - rhs.X,
		Y - rhs.Y,
		Z - rhs.Z
	};
}

//
// Multiplication operator.
//
Vector3 Vector3::operator*(const Vector3& rhs) const
{
	return {
		X * rhs.X,
		Y * rhs.Y,
		Z * rhs.Z
	};
}

//
// Division operator.
//
Vector3 Vector3::operator/(const Vector3& rhs) const
{
	return {
		X / rhs.X,
		Y / rhs.Y,
		Z / rhs.Z
	};
}

//
// Concat addition operator.
//
Vector3 Vector3::operator+=(const Vector3& rhs) const
{
	return *this + rhs;
}

//
// Concat subtraction operator.
//
Vector3 Vector3::operator-=(const Vector3& rhs) const
{
	return *this - rhs;
}

//
// Concat multiplication operator.
//
Vector3 Vector3::operator*=(const Vector3& rhs) const
{
	return *this * rhs;
}

//
// Concat division operator.
//
Vector3 Vector3::operator/=(const Vector3& rhs) const
{
	return *this / rhs;
}

//
// Negation operator.
//
Vector3 Vector3::operator-() const
{
	return {
		-X,
		-Y,
		-Z
	};
}

//
// Equality operator.
//
bool Vector3::operator==(const Vector3& rhs) const
{
	return	X == rhs.X &&
			Y == rhs.Y &&
			Z == rhs.Z;
}

//
// Inequality operator.
//
bool Vector3::operator!=(const Vector3& rhs) const
{
	return !(*this == rhs);
}
