#include "Shape.h"

//
// Default constructor
//
Shape::Shape() : _translation(Matrix::IdentityMatrix()), _rotation(Matrix::IdentityMatrix()), _scale(Matrix::IdentityMatrix()), _shapeColor(RGB(0, 0, 0))
{ }

//
// Destructor - clears vertices vector.
//
Shape::~Shape()
{
	_shapeData.clear();
}

//
// Saves a transformation matrix.
//
void Shape::Transform(const Matrix& transformationMatrix)
{
	Matrix translation = _translation * transformationMatrix;
	Matrix rotation = _rotation * transformationMatrix;
	Matrix scale = _scale * transformationMatrix;

	_translation = translation;
	_rotation = rotation;
	_scale = scale;
}

//
// Resets the transformation matrix to the identity matrix.
//
void Shape::ResetAll()
{
	_translation = Matrix::IdentityMatrix();
	_rotation = Matrix::IdentityMatrix();
	_scale = Matrix::IdentityMatrix();
}

//
// Returns a vertex group containing every vertex in the internal shape, transformed by the matrix.
//
const std::vector<Vertex>& Shape::GetShape()
{
	if (_shapeData.size() != _transform.size())
	{
		Realign();
	}

	Matrix transformationMatrix = GetTransform();

	for (size_t vertex = 0; vertex < _shapeData.size(); ++vertex)
	{
		Vertex currentVertex = _shapeData[vertex];
		Vertex transformed = transformationMatrix * currentVertex;

		_transform[vertex] = transformed;
	}

	return _transform;
}

//
// Ensures the transformation vector and the model vector contain
// the same number of elements.
//
void Shape::Realign()
{
	_transform.clear();
	_transform.resize(_shapeData.size());

	for (size_t i = 0; i < _shapeData.size(); ++i)
	{
		_transform[i] = _shapeData[i];
	}
}

//
// Pushes a vertex
//
void Shape::CreateVertex(const Vertex& vertex)
{
	_shapeData.push_back(vertex);
}

//
// Pushes a list of vertices.
//
void Shape::CreateVertices(const std::vector<Vertex>& vertices)
{
	for (std::vector<Vertex>::const_iterator it = vertices.begin(); it != vertices.end(); ++it)
	{
		CreateVertex(*it);
	}
}

void Shape::ClearVertices()
{
	_shapeData.clear();
}

//
// Returns a read-only reference to the existing vertices.
//
const std::vector<Vertex>& Shape::GetVertices() const
{
	return _shapeData;
}

//
// The number of vertices.
//
const size_t Shape::GetVerticesCount() const
{
	return _shapeData.size();
}

//
// The shape's colour
//
const COLORREF& Shape::GetColour() const
{
	return _shapeColor;
}

//
// The shape's colour.
//
void Shape::SetColour(const COLORREF& colour)
{
	_shapeColor = colour;
}

//
// Returns the combined transformation matrix.
//
const Matrix Shape::GetTransform() const
{
	return _translation * _rotation * _scale;
}

//
// Updates the position matrix.
//
void Shape::SetPosition(const Point3D<float>& position)
{
	_translation = Matrix::TranslationMatrix(position.X, position.Y, position.Z);
}

//
// Updates the rotation matrix.
//
void Shape::SetRotation(const Point3D<float>& rotation)
{
	_rotation = Matrix::RotationMatrix(rotation.X, rotation.Y, rotation.Z);
}

//
// Updates the scale matrix.
//
void Shape::SetScale(const Point3D<float>& scale)
{
	_scale = Matrix::ScaleMatrix(scale.X, scale.Y, scale.Z);
}

//
// Translates the shape by a given amount.
//
void Shape::Translate(const Point3D<float>& amount)
{
	Matrix translation = _translation * Matrix::TranslationMatrix(amount.X, amount.Y, amount.Z);
	_translation = translation;
}

//
// Rotates the shape by a given amount.
//
void Shape::Rotate(const Point3D<float>& amount)
{
	Matrix rotation = _rotation * Matrix::RotationMatrix(amount.X, amount.Y, amount.Z);
	_rotation = rotation;
}

//
// Scales the shape by a given amount.
//
void Shape::Scale(const Point3D<float>& amount)
{
	Matrix scale = _scale * Matrix::ScaleMatrix(amount.X, amount.Y, amount.Z);
	_scale = scale;
}
