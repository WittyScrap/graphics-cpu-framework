#include "Shape.h"
#include "Camera.h"


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
// Returns a vertex group containing every vertex in the internal shape, transformed by the MVP matrix.
//
void Shape::CalculateTransformations()
{
	const size_t  verticesCount = _shapeData.size();

	Matrix mv = GetMVP(MV);
	Matrix p = GetMVP(P);
	Matrix p2c = GetP2C();

	for (size_t i = 0; i < verticesCount; ++i)
	{
		_worldSpaceData[i] = mv * _shapeData[i];
		_clipSpaceData[i] = ObjectToClipSpace(p, p2c, _worldSpaceData[i]);
	}
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
// Projection-To-Clip matrix.
//
const Matrix Shape::GetP2C() const
{
	if (const Camera * const mainCamera = Camera::GetMainCamera())
	{
		return mainCamera->GetProjectionToClipMatrix();
	}

	return Matrix::IdentityMatrix();
}

//
// Applies the MVP transformation, dehomogenisation, and screen transformation.
//
const Vertex Shape::ObjectToClipSpace(const Matrix& p, const Matrix& p2c, const Vertex& vertex)
{
	Vertex transformed = p * vertex;
	transformed.Dehomogenise();

	// Apply projection-to-clip matrix...
	transformed = p2c * transformed;


	return transformed;
}

//
// The fully tranformed vertices, after all projection and clip space transformations have been applied.
//
const std::vector<Vertex>& Shape::GetClipSpaceVertices() const
{
	return _clipSpaceData;
}

//
// The world-space vertices without any projection/clip space transformation applied to them.
//
const std::vector<Vertex>& Shape::GetWorldSpaceVertices() const
{
	return _worldSpaceData;
}

//
// Pushes a vertex
//
void Shape::CreateVertex(const Vertex& vertex)
{
	_shapeData.push_back(vertex);

	_clipSpaceData.resize(_shapeData.size(), Vertex(vertex));
	_worldSpaceData.resize(_shapeData.size(), Vertex(vertex));
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
const Colour Shape::GetColour() const
{
	return Colour(_shapeColour);
}

//
// The shape's colour.
//
void Shape::SetColour(const Colour& colour)
{
	_shapeColour = colour.AsColor();
}

//
// Full equality operator.
//
const bool operator==(const Shape& lhs, const Shape& rhs)
{
	return &lhs == &rhs;
}
