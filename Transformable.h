#pragma once
#include "Matrix.h"

//
// Represents any object that can be transformed.
//
class Transformable
{
public:
	//
	// Default constructor.
	//
	Transformable();

	//
	// Gets a TRS (Translation Rotation Scale) matrix from
	// the shape's current location, rotation, and scale
	// matrices.
	//
	virtual const Matrix GetTransform() const;

	//
	// Transformation overrides, these will overwrite
	// the related current transformation matrices.
	//
	virtual void SetPosition(const Vector3& position);
	virtual void SetRotation(const Vector3& rotation);
	virtual void SetScale(const Vector3& scale);

	//
	// Transformation alterations, these will multiply
	// the related current transformation matrices by
	// the supplied matrices.
	//
	virtual void Translate(const Vector3& amount);
	virtual void Rotate(const Vector3& amount);
	virtual void Scale(const Vector3& amount);

private:
	//
	// Transformation matrices
	//
	Matrix _position;
	Matrix _rotation;
	Matrix _scale;
};

