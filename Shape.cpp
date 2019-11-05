#include "Shape.h"
#include "Camera.h"

#define M	0b00000001
#define V	0b00000010
#define P	0b00000100

#define MV	M | V
#define MP	M | P
#define VP	V | P
#define MVP M | V | P

//
// Default constructor
//
Shape::Shape() : _shapeColour(RGB(0, 0, 0))
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
const std::vector<Vertex>& Shape::CalculateTransformations()
{
	const size_t  verticesCount = _shapeData.size();
	Matrix mvp = GetMVP(MVP);

	for (size_t i = 0; i < verticesCount; ++i)
	{
		_transformedShape[i] = mvp * _shapeData[i];
	}

	return _transformedShape;
}

//
// Model, View, Projection matrix.
//
const Matrix Shape::GetMVP(const char& type) const
{
	bool hasM = (type & M) == M;
	bool hasV = (type & V) == V;
	bool hasP = (type & P) == P;

	Matrix _M = hasM ? GetTransform() : Matrix::IdentityMatrix();

	if (const Camera* const mainCamera = Camera::GetMainCamera())
	{
		Matrix _V = hasV ? mainCamera->GetWorldToCameraMatrix() : Matrix::IdentityMatrix();
		Matrix _P = hasP ? mainCamera->GetProjectionMatrix()	: Matrix::IdentityMatrix();

		return _P * _V * _M;
	}

	return _M;
}

//
// Pushes a vertex
//
void Shape::CreateVertex(const Vertex& vertex)
{
	_shapeData.push_back(vertex);
	_transformedShape.resize(_shapeData.size(), Vertex(vertex));
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
// Full equality operator.
//
const bool operator==(const Shape& lhs, const Shape& rhs)
{
	return &lhs == &rhs;
}
