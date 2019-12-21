#include "Polygon3D.h"

//
// Default constructor
//
Polygon3D::Polygon3D() : _vertices { 0 }, _uvcoords { 0 }
{ }

//
// Constructs a polygon from 3 indices.
//
Polygon3D::Polygon3D(const int& p0, const int& p1, const int& p2, const int& u0, const int& u1, const int u2) : _vertices { 0 }, _uvcoords { 0 }
{
	_vertices[0] = p0;
	_vertices[1] = p1;
	_vertices[2] = p2;

	_uvcoords[0] = u0;
	_uvcoords[1] = u1;
	_uvcoords[2] = u2;
}

//
// Destroys this polygon.
//
Polygon3D::~Polygon3D()
{ }

//
// Copies the values of the passed polygon to this one.
//
Polygon3D::Polygon3D(const Polygon3D& other) : _vertices{ 0 }
{
	_vertices[0] = other._vertices[0];
	_vertices[1] = other._vertices[1];
	_vertices[2] = other._vertices[2];
	_uvcoords[0] = other._uvcoords[0];
	_uvcoords[1] = other._uvcoords[1];
	_uvcoords[2] = other._uvcoords[2];
	_depth = other._depth;
	_finalColour = other._finalColour;
	_objectNormal = other._objectNormal;
	_worldNormal = other._worldNormal;
	_clipNormal = other._clipNormal;
}

//
// Returns the vertex index stored at the i location.
//
int Polygon3D::GetVertex(const int& i) const
{
	if (i < 0 || i > INDICES_COUNT)
	{
		return -1;
	}

	return _vertices[i];
}

//
// Returns the UV index stored at the i location.
//
int Polygon3D::GetUVCoord(const int& i) const
{
	if (i < 0 || i > INDICES_COUNT)
	{
		return -1;
	}

	return _uvcoords[i];
}

//
// This polygon's precalculated OBJECT SPACE normal.
//
const Vector3& Polygon3D::GetObjectNormal() const
{
	return _objectNormal;
}

//
// This polygon's precalculated OBJECT SPACE normal.
//
const Vector3& Polygon3D::GetWorldNormal() const
{
	return _worldNormal;
}

//
// This polygon's precalculated OBJECT SPACE normal.
//
const Vector3& Polygon3D::GetClipNormal() const
{
	return _clipNormal;
}

//
// The depth of this polygon.
//
const float& Polygon3D::GetDepth() const
{
	return _depth;
}

//
// The calculated colour of this polygon.
//
const Colour& Polygon3D::GetColour() const
{
	return _finalColour;
}

//
// Calculates the center of the polygon.
//
const Vertex Polygon3D::CalculateCenter(const std::vector<Vertex>& vertices) const
{
	const Vertex& a = vertices[_vertices[0]];
	const Vertex& b = vertices[_vertices[1]];
	const Vertex& c = vertices[_vertices[2]];

	return Vertex::GetAverage(a, b, c);
}

//
// Calculates the average depth of all vertices in the polygon.
//
void Polygon3D::CalculateDepth(const std::vector<Vertex>& vertices)
{
	const Vertex& a = vertices[_vertices[0]];
	const Vertex& b = vertices[_vertices[1]];
	const Vertex& c = vertices[_vertices[2]];

	_depth = (a.GetDepth() + b.GetDepth() + c.GetDepth()) / 3;
}

//
// Calculates the OBJECT SPACE normal for this polygon.
//
void Polygon3D::CalculateObjectNormal(const std::vector<Vertex>& objectSpace)
{
	const Vertex& a = objectSpace[_vertices[0]];
	const Vertex& b = objectSpace[_vertices[1]];
	const Vertex& c = objectSpace[_vertices[2]];

	Vector3 aTob((b - a).AsVector());
	Vector3 aToc((c - a).AsVector());

	_objectNormal = Vector3::NormaliseVector(Vector3::Cross(aTob, aToc));
}

//
// Calculates the WORLD SPACE normal for this polygon.
//
void Polygon3D::CalculateWorldNormal(const std::vector<Vertex>& worldSpace)
{
	const Vertex& a = worldSpace[_vertices[0]];
	const Vertex& b = worldSpace[_vertices[1]];
	const Vertex& c = worldSpace[_vertices[2]];

	Vector3 aTob((b - a).AsVector());
	Vector3 aToc((c - a).AsVector());

	_worldNormal = Vector3::NormaliseVector(Vector3::Cross(aTob, aToc));
}

//
// Calculates the CLIP SPACE normal for this polygon.
//
void Polygon3D::CalculateClipNormal(const std::vector<Vertex>& clipSpace)
{
	const Vertex& a = clipSpace[_vertices[0]];
	const Vertex& b = clipSpace[_vertices[1]];
	const Vertex& c = clipSpace[_vertices[2]];

	Vector3 aTob((b - a).AsVector());
	Vector3 aToc((c - a).AsVector());

	_clipNormal = Vector3::NormaliseVector(Vector3::Cross(aTob, aToc));
}

//
// Assigns the values of the passed polygon to this one, then returns this object.
//
Polygon3D& Polygon3D::operator=(const Polygon3D& rhs)
{
	_vertices[0] = rhs._vertices[0];
	_vertices[1] = rhs._vertices[1];
	_vertices[2] = rhs._vertices[2];
	_uvcoords[0] = rhs._uvcoords[0];
	_uvcoords[1] = rhs._uvcoords[1];
	_uvcoords[2] = rhs._uvcoords[2];
	_depth = rhs._depth;
	_objectNormal = rhs._objectNormal;
	_worldNormal = rhs._worldNormal;
	_clipNormal = rhs._clipNormal;
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
