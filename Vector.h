#pragma once
#include "Point.h"

struct Vector3
{
public:
	//
	// Main member variables are exposed for easier initialisation
	// and because they are simple holders that do not need any kind
	// of explicit getter/setter and can be set to any valid float
	// value without any repercussions.
	//
	float X;
	float Y;
	float Z;

	//
	// Mathematics operators
	//
	Vector3& operator= (const Vector3& rhs);
	Vector3  operator+ (const Vector3& rhs) const;
	Vector3  operator- (const Vector3& rhs) const;
	Vector3  operator* (const Vector3& rhs) const;
	Vector3  operator/ (const Vector3& rhs) const;
	Vector3  operator+=(const Vector3& rhs) const;
	Vector3  operator-=(const Vector3& rhs) const;
	Vector3  operator*=(const Vector3& rhs) const;
	Vector3  operator/=(const Vector3& rhs) const;
	Vector3  operator- () const;
	
	//
	// Comparison operators
	//
	bool operator==(const Vector3& rhs) const;
	bool operator!=(const Vector3& rhs) const;
};