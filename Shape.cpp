#include "Shape.h"
#include "Camera.h"

//
// Default constructor
//
Shape::Shape() : _position(Matrix::IdentityMatrix()),
				 _rotation(Matrix::IdentityMatrix()),
				 _scale(Matrix::IdentityMatrix()),
				 _shapeColour(RGB(0, 0, 0))
{ }

//
// Destructor - clears vertices vector.
//
Shape::~Shape()
{
	_shapeData.clear();
}

//
// Returns a vertex group containing every vertex in the internal shape, transformed by the matrix.
//
const std::vector<Vertex>& Shape::GetShape()
{
	if (_shapeData.size() != _transformedShape.size())
	{
		Realign();
	}

	Matrix MVP = GetMVP();

	for (size_t vertex = 0; vertex < _shapeData.size(); ++vertex)
	{
		Vertex currentVertex = _shapeData[vertex];
		Vertex transformed = MVP * currentVertex;

		// Save vertex
		_transformedShape[vertex] = transformed;
	}

	return _transformedShape;
}

//
// Model, View, Projection matrix.
//
const Matrix Shape::GetMVP() const
{
	Matrix M = GetTransform();

	if (Camera::GetMainCamera())
	{
		Matrix V = Camera::GetMainCamera()->GetWorldToCameraMatrix();
		Matrix P = Camera::GetMainCamera()->GetProjectionMatrix();

		return P * V * M;
	}

	return M;
}

//
// Ensures the transformation vector and the model vector contain
// the same number of elements.
//
void Shape::Realign()
{
	_transformedShape.clear();
	_transformedShape.resize(_shapeData.size());

	for (size_t i = 0; i < _shapeData.size(); ++i)
	{
		_transformedShape[i] = _shapeData[i];
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
	return _shapeColour;
}

//
// The shape's colour.
//
void Shape::SetColour(const COLORREF& colour)
{
	_shapeColour = colour;
}

//
// Returns the combined transformation matrix.
//
const Matrix Shape::GetTransform() const
{
	return _position * _rotation * _scale;
}

//
// Updates the position matrix.
//
void Shape::SetPosition(const Vector3& position)
{
	_position = Matrix::TranslationMatrix(position.X, position.Y, position.Z);
}

//
// Updates the rotation matrix.
//
void Shape::SetRotation(const Vector3& rotation)
{
	_rotation = Matrix::RotationMatrix(rotation.X, rotation.Y, rotation.Z);
}

//
// Updates the scale matrix.
//
void Shape::SetScale(const Vector3& scale)
{
	_scale = Matrix::ScaleMatrix(scale.X, scale.Y, scale.Z);
}

//
// Translates the shape by a given amount.
//
void Shape::Translate(const Vector3& amount)
{
	Matrix translation = _position * Matrix::TranslationMatrix(amount.X, amount.Y, amount.Z);
	_position = translation;
}

//
// Rotates the shape by a given amount.
//
void Shape::Rotate(const Vector3& amount)
{
	Matrix rotation = _rotation * Matrix::RotationMatrix(amount.X, amount.Y, amount.Z);
	_rotation = rotation;
}

//
// Scales the shape by a given amount.
//
void Shape::Scale(const Vector3& amount)
{
	Matrix scale = _scale * Matrix::ScaleMatrix(amount.X, amount.Y, amount.Z);
	_scale = scale;
}
