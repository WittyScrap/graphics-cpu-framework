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
	_position = Matrix::TranslationMatrix(position.GetX(), position.GetY(), position.GetZ());
}

//
// Updates the rotation matrix.
//
void Transformable::SetRotation(const Vector3& rotation)
{
	_rotation = Matrix::RotationMatrix(rotation.GetX(), rotation.GetY(), rotation.GetZ());
}

//
// Updates the scale matrix.
//
void Transformable::SetScale(const Vector3& scale)
{
	_scale = Matrix::ScaleMatrix(scale.GetX(), scale.GetY(), scale.GetZ());
}

//
// Translates the shape by a given amount.
//
void Transformable::Translate(const Vector3& amount)
{
	Matrix translation = _position * Matrix::TranslationMatrix(amount.GetX(), amount.GetY(), amount.GetZ());
	_position = translation;
}

//
// Rotates the shape by a given amount.
//
void Transformable::Rotate(const Vector3& amount)
{
	Matrix rotation = _rotation * Matrix::RotationMatrix(amount.GetX(), amount.GetY(), amount.GetZ());
	_rotation = rotation;
}

//
// Scales the shape by a given amount.
//
void Transformable::Scale(const Vector3& amount)
{
	Matrix scale = _scale * Matrix::ScaleMatrix(amount.GetX(), amount.GetY(), amount.GetZ());
	_scale = scale;
}