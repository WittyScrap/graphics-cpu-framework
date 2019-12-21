#pragma once
#include "Point.h"

//
// Vertex forward declaration.
//
class Vertex;

struct Vector3
{
public:
	Vector3();
	Vector3(const Vector3& copy);
	Vector3(const Vertex& copy);
	Vector3(const float& x, const float& y, const float& z);

	// --- Setters --- //
	inline void SetX(const float& value);
	inline void SetY(const float& value);
	inline void SetZ(const float& value);

	// --- Getters --- //
	inline const float& GetX() const;
	inline const float& GetY() const;
	inline const float& GetZ() const;

	//
	// Mathematics operators
	//
	Vector3& operator= (const Vector3& rhs);
	Vector3  operator+ (const Vector3& rhs) const;
	Vector3  operator- (const Vector3& rhs) const;
	Vector3  operator* (const Vector3& rhs) const;
	Vector3  operator/ (const Vector3& rhs) const;
	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator*=(const Vector3& rhs);
	Vector3& operator/=(const Vector3& rhs);
	Vector3  operator- () const;

	//
	// Operators on vertices
	//
	Vector3& operator= (const Vertex& rhs);
	Vector3  operator+ (const Vertex& rhs) const;
	Vector3  operator- (const Vertex& rhs) const;
	Vector3  operator* (const Vertex& rhs) const;
	Vector3  operator/ (const Vertex& rhs) const;
	Vector3& operator+=(const Vertex& rhs);
	Vector3& operator-=(const Vertex& rhs);
	Vector3& operator*=(const Vertex& rhs);
	Vector3& operator/=(const Vertex& rhs);

	//
	// Operators on scalars
	//
	Vector3  operator* (const float& rhs) const;
	Vector3  operator/ (const float& rhs) const;

	Vector3& operator*=(const float& rhs);
	Vector3& operator/=(const float& rhs);

	//
	// Static Vector aritmetics
	//
	static const float Dot(const Vector3& lhs, const Vector3& rhs);
	static Vector3 Cross(const Vector3& lhs, const Vector3& rhs);
	static Vector3 NormaliseVector(const Vector3& vector);
	static Vector3 Lerp(const Vector3& lhs, const Vector3& rhs, const float& alpha);

	//
	// Vector aritmetics
	//
	void Clear();
	void Normalise();
	const float GetMagnitude() const;
	const float GetSqrMagnitude() const;
	
	//
	// Comparison operators
	//
	bool operator==(const Vector3& rhs) const;
	bool operator!=(const Vector3& rhs) const;

	const Vertex ToVertex() const;

private:
	//
	// Main member variables.
	//
	float _x;
	float _y;
	float _z;

};

//
// Sets the X component of the vector.
//
inline void Vector3::SetX(const float& value)
{
	_x = value;
}

//
// Sets the Y component of the vector.
//
inline void Vector3::SetY(const float& value)
{
	_y = value;
}

//
// Sets the Z component of the vector.
//
inline void Vector3::SetZ(const float& value)
{
	_z = value;
}

//
// The X component of the vector.
//
inline const float& Vector3::GetX() const
{
	return _x;
}

//
// The Y component of the vector.
//
inline const float& Vector3::GetY() const
{
	return _y;
}

//
// The Z component of the vector.
//
inline const float& Vector3::GetZ() const
{
	return _z;
}