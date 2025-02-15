#include "Vector.h"
#include "Vertex.h"
#include <cmath>

//
// Default constructor.
//
Vector3::Vector3() : _x{0}, _y{0}, _z{0}
{ }

//
// Copy constructor.
//
Vector3::Vector3(const Vector3& copy) : _x{copy._x}, _y{copy._y}, _z{copy._z}
{ }

//
// "Copy" constructor.
//
Vector3::Vector3(const Vertex& copy) : _x{copy.GetX()}, _y{copy.GetY()}, _z{copy.GetZ()}
{ }

//
// Full constructor.
//
Vector3::Vector3(const float& x, const float& y, const float& z) : _x{x}, _y{y}, _z{z}
{ }

//
// Assignment operator.
//
Vector3& Vector3::operator=(const Vector3& rhs)
{
	if (rhs != *this)
	{
		_x = rhs._x;
		_y = rhs._y;
		_z = rhs._z;
	}

	return *this;
}

//
// Addition operator.
//
Vector3 Vector3::operator+(const Vector3& rhs) const
{
	return {
		_x + rhs._x,
		_y + rhs._y,
		_z + rhs._z
	};
}

//
// Subtraction operator.
//
Vector3 Vector3::operator-(const Vector3& rhs) const
{
	return {
		_x - rhs._x,
		_y - rhs._y,
		_z - rhs._z
	};
}

//
// Multiplication operator.
//
Vector3 Vector3::operator*(const Vector3& rhs) const
{
	return {
		_x * rhs._x,
		_y * rhs._y,
		_z * rhs._z
	};
}

//
// Division operator.
//
Vector3 Vector3::operator/(const Vector3& rhs) const
{
	return {
		_x / rhs._x,
		_y / rhs._y,
		_z / rhs._z
	};
}

//
// Concat addition operator.
//
Vector3& Vector3::operator+=(const Vector3& rhs)
{
	_x += rhs._x;
	_y += rhs._y;
	_z += rhs._z;

	return *this;
}

//
// Concat subtraction operator.
//
Vector3& Vector3::operator-=(const Vector3& rhs)
{
	_x -= rhs._x;
	_y -= rhs._y;
	_z -= rhs._z;

	return *this;
}

//
// Concat multiplication operator.
//
Vector3& Vector3::operator*=(const Vector3& rhs)
{
	_x *= rhs._x;
	_y *= rhs._y;
	_z *= rhs._z;

	return *this;
}

//
// Concat division operator.
//
Vector3& Vector3::operator/=(const Vector3& rhs)
{
	_x /= rhs._x;
	_y /= rhs._y;
	_z /= rhs._z;

	return *this;
}

//
// Negation operator.
//
Vector3 Vector3::operator-() const
{
	return {
		-_x,
		-_y,
		-_z
	};
}

//
// Assigns all values from the vertex into this vector.
//
Vector3& Vector3::operator=(const Vertex& rhs)
{
	_x = rhs.GetX();
	_y = rhs.GetY();
	_z = rhs.GetZ();

	return *this;
}

//
// Adds the vertex components to this vector's.
//
Vector3 Vector3::operator+(const Vertex& rhs) const
{
	return Vector3(rhs.GetX() + _x, rhs.GetY() + _y, rhs.GetZ() + _z);
}

//
// Subtracts the vertex components from this vector's.
//
Vector3 Vector3::operator-(const Vertex& rhs) const
{
	return Vector3(_x - rhs.GetX(), _y - rhs.GetY(), _z - rhs.GetZ());
}

//
// Multiplies this vector by the vertex.
//
Vector3 Vector3::operator*(const Vertex& rhs) const
{
	return Vector3(_x * rhs.GetX(), _y * rhs.GetY(), _z * rhs.GetZ());
}

//
// Divides this vector by the vertex.
//
Vector3 Vector3::operator/(const Vertex& rhs) const
{
	return Vector3(_x / rhs.GetX(), _y / rhs.GetY(), _z / rhs.GetZ());
}

//
// Concat adds the vertex's components to this vector's.
//
Vector3& Vector3::operator+=(const Vertex& rhs)
{
	_x += rhs.GetX();
	_y += rhs.GetY();
	_z += rhs.GetZ();

	return *this;
}

//
// Concat subtracts the vertex's components from this vector's.
//
Vector3& Vector3::operator-=(const Vertex& rhs)
{
	_x -= rhs.GetX();
	_y -= rhs.GetY();
	_z -= rhs.GetZ();

	return *this;
}

//
// Concat multiplies the vertex's components to this vector's.
//
Vector3& Vector3::operator*=(const Vertex& rhs)
{
	_x *= rhs.GetX();
	_y *= rhs.GetY();
	_z *= rhs.GetZ();

	return *this;
}

//
// Concat divides the vertex's components from this vector's.
//
Vector3& Vector3::operator/=(const Vertex& rhs)
{
	_x /= rhs.GetX();
	_y /= rhs.GetY();
	_z /= rhs.GetZ();

	return *this;
}

//
// Assigns a scalar to all components of the vector.
//
Vector3& Vector3::operator=(const float& rhs)
{
	_x = rhs;
	_y = rhs;
	_z = rhs;

	return *this;
}

//
// Adds a scalar to each component of the vector.
//
Vector3 Vector3::operator+(const float& rhs) const
{
	return Vector3(_x + rhs, _y + rhs, _z + rhs);
}

//
// Subtracts a scalar from all components of the vector;
//
Vector3 Vector3::operator-(const float& rhs) const
{
	return Vector3(_x - rhs, _y - rhs, _z - rhs);
}

//
// Multiplies each component of a copy of this vector by a scalar.
//
Vector3 Vector3::operator* (const float& rhs) const
{
	Vector3 mul(*this);

	mul._x *= rhs;
	mul._y *= rhs;
	mul._z *= rhs;

	return mul;
}

//
// Divides each component of a copy of this vector by a scalar.
//
Vector3 Vector3::operator/ (const float& rhs) const
{
	Vector3 div(*this);

	div._x /= rhs;
	div._y /= rhs;
	div._z /= rhs;

	return div;
}

//
// Adds a scalar to each component of the vector.
//
Vector3& Vector3::operator+=(const float& rhs)
{
	_x += rhs;
	_y += rhs;
	_z += rhs;

	return *this;
}

//
// Subtracts a scalar from all components of the vector;
//
Vector3& Vector3::operator-=(const float& rhs)
{
	_x -= rhs;
	_y -= rhs;
	_z -= rhs;

	return *this;
}

//
// Multiplies each component of this vector by a scalar.
//
Vector3& Vector3::operator*=(const float& rhs)
{
	_x *= rhs;
	_y *= rhs;
	_z *= rhs;

	return *this;
}

//
// Divides each component of this vector by a scalar.
//
Vector3& Vector3::operator/=(const float& rhs)
{
	_x /= rhs;
	_y /= rhs;
	_z /= rhs;

	return *this;
}

//
// Performs the dot product from the lhs and rhs vectors.
//
const float Vector3::Dot(const Vector3& lhs, const Vector3& rhs)
{
	return lhs._x * rhs._x + lhs._y * rhs._y + lhs._z * rhs._z;
}

//
// Performs the cross product from the lhs and rhs vectors.
//
Vector3 Vector3::Cross(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3(
		lhs._y * rhs._z - lhs._z * rhs._y,
		lhs._z * rhs._x - lhs._x * rhs._z,
		lhs._x * rhs._y - lhs._y * rhs._x
	);
}

//
// Normalises the vector.
//
Vector3 Vector3::NormaliseVector(const Vector3& vector)
{
	Vector3 copy(vector);
	copy.Normalise();
	return copy;
}

//
// Interpolates lineraly between the two vectors.
//
Vector3 Vector3::Lerp(const Vector3& lhs, const Vector3& rhs, const float& alpha)
{
	return rhs * alpha + lhs * (1 - alpha);
}

//
// Clears the components of this vector without
// allocating a new vector.
//
void Vector3::Clear()
{
	_x = 0;
	_y = 0;
	_z = 0;
}

//
// Normalises this vector.
//
void Vector3::Normalise()
{
	const float& magnitude = GetMagnitude();

	_x /= magnitude;
	_y /= magnitude;
	_z /= magnitude;
}

//
// The magnitude (length) of this vector, squared. Faster, as it
// does not need to perform the square root, but not accurate.
// Valid for comparisons, not for exact measurements.
//
const float Vector3::GetSqrMagnitude() const
{
	return _x * _x + _y * _y + _z * _z;
}

//
// The magnitude (length) of this vector.
//
const float Vector3::GetMagnitude() const
{
	return std::sqrt(GetSqrMagnitude());
}

//
// Equality operator.
//
bool Vector3::operator==(const Vector3& rhs) const
{
	return	_x == rhs._x &&
			_y == rhs._y &&
			_z == rhs._z;
}

//
// Inequality operator.
//
bool Vector3::operator!=(const Vector3& rhs) const
{
	return !(*this == rhs);
}

//
// Converts this vector to a vertex.
//
const Vertex Vector3::ToVertex() const
{
	return Vertex(_x, _y, _z, 0);
}
