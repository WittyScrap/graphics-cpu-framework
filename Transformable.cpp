#include "Transformable.h"

//
// Default constructor
//
Transformable::Transformable() : _position(Matrix::IdentityMatrix()),
								 _rotation(Matrix::IdentityMatrix()),
								 _scale(Matrix::IdentityMatrix())
{ }

//
// Returns the combined transformation matrix.
//
const Matrix Transformable::GetTransform() const
{
	return _position * _rotation * _scale;
}

//
// Updates the position matrix.
//
void Transformable::SetPosition(const Vector3& position)
{
	_position = Matrix::TranslationMatrix(position.X, position.Y, position.Z);
}

//
// Updates the rotation matrix.
//
void Transformable::SetRotation(const Vector3& rotation)
{
	_rotation = Matrix::RotationMatrix(rotation.X, rotation.Y, rotation.Z);
}

//
// Updates the scale matrix.
//
void Transformable::SetScale(const Vector3& scale)
{
	_scale = Matrix::ScaleMatrix(scale.X, scale.Y, scale.Z);
}

//
// Translates the shape by a given amount.
//
void Transformable::Translate(const Vector3& amount)
{
	Matrix translation = _position * Matrix::TranslationMatrix(amount.X, amount.Y, amount.Z);
	_position = translation;
}

//
// Rotates the shape by a given amount.
//
void Transformable::Rotate(const Vector3& amount)
{
	Matrix rotation = _rotation * Matrix::RotationMatrix(amount.X, amount.Y, amount.Z);
	_rotation = rotation;
}

//
// Scales the shape by a given amount.
//
void Transformable::Scale(const Vector3& amount)
{
	Matrix scale = _scale * Matrix::ScaleMatrix(amount.X, amount.Y, amount.Z);
	_scale = scale;
}