#include "Polygon3D.h"

//
// Default constructor
//
Polygon3D::Polygon3D() : _indices { 0 }
{ }

//
// Constructs a polygon from 3 indices.
//
Polygon3D::Polygon3D(const int& index0, const int& index1, const int& index2) : _indices { 0 }
{
	_indices[0] = index0;
	_indices[1] = index1;
	_indices[2] = index2;
}

//
// Destroys this polygon.
//
Polygon3D::~Polygon3D()
{ }

//
// Copies the values of the passed polygon to this one.
//
Polygon3D::Polygon3D(const Polygon3D& other) : _indices{ 0 }
{
	_indices[0] = other._indices[0];
	_indices[1] = other._indices[1];
	_indices[2] = other._indices[2];
}

//
// Returns the vertex index stored at the i location.
//
int Polygon3D::GetIndex(const int& i) const
{
	if (i < 0 || i > INDICES_COUNT)
	{
		return -1;
	}

	return _indices[i];
}

//
// This polygon's pre-calculated normal.
//
const Vector3& Polygon3D::GetNormal() const
{
	return _normal;
}

//
// The depth of this polygon.
//
const float& Polygon3D::GetDepth() const
{
	return _depth;
}

const Colour& Polygon3D::GetColour() const
{
	return _finalColour;
}

//
// Calculates the normal for this polygon.
//
void Polygon3D::CalculateNormal(const Vertex& a, const Vertex& b, const Vertex& c)
{
	Vector3 aTob((b - a).AsVector());
	Vector3 aToc((c - a).AsVector());

	_normal = Vector3::NormaliseVector(Vector3::Cross(aTob, aToc));
}

//
// Calculates the center of the polygon.
//
const Vertex Polygon3D::CalculateCenter(const std::vector<Vertex>& vertices) const
{
	const Vertex& a = vertices[_indices[0]];
	const Vertex& b = vertices[_indices[1]];
	const Vertex& c = vertices[_indices[2]];

	return Vertex::GetAverage(a, b, c);
}

//
// Calculates a temporary normal and returns a copy without storing its reference.
//
const Vector3 Polygon3D::TemporaryNormal(const std::vector<Vertex>& vertices) const
{
	const Vertex& a = vertices[_indices[0]];
	const Vertex& b = vertices[_indices[1]];
	const Vertex& c = vertices[_indices[2]];

	Vector3 aTob((b - a).AsVector());
	Vector3 aToc((c - a).AsVector());

	return Vector3::NormaliseVector(Vector3::Cross(aTob, aToc));
}

//
// Calculates the average depth of all vertices in the polygon.
//
void Polygon3D::CalculateDepth(const std::vector<Vertex>& vertices)
{
	const Vertex& a = vertices[_indices[0]];
	const Vertex& b = vertices[_indices[1]];
	const Vertex& c = vertices[_indices[2]];

	_depth = (a.GetDepth() + b.GetDepth() + c.GetDepth()) / 3;
}

//
// Assigns the values of the passed polygon to this one, then returns this object.
//
Polygon3D& Polygon3D::operator=(const Polygon3D& rhs)
{
	_indices[0] = rhs._indices[0];
	_indices[1] = rhs._indices[1];
	_indices[2] = rhs._indices[2];

	_depth = rhs._depth;
	_normal = rhs._normal;
	_finalColour = rhs._finalColour;

	return *this;
}

//
// Compares two polygons depth-wise.
//
bool Polygon3D::operator<(const Polygon3D& rhs)
{
	return _depth < rhs._depth;
}

//
// Compares two polygons depth wise.
//
bool Polygon3D::operator>(const Polygon3D& rhs)
{
	return _depth > rhs._depth;
}

//
// Compares the depth of two polygons.
//
const bool DepthTest::operator()(Polygon3D* lhs, Polygon3D* rhs) const
{
	return *lhs > *rhs;
}
