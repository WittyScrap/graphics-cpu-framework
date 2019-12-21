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
	_depth = other._depth;
	_finalColour = other._finalColour;
	_objectNormal = other._objectNormal;
	_worldNormal = other._worldNormal;
	_clipNormal = other._clipNormal;
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
	const Vertex& a = vertices[_indices[0]];
	const Vertex& b = vertices[_indices[1]];
	const Vertex& c = vertices[_indices[2]];

	return Vertex::GetAverage(a, b, c);
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
// Calculates the OBJECT SPACE normal for this polygon.
//
void Polygon3D::CalculateObjectNormal(const std::vector<Vertex>& objectSpace)
{
	const Vertex& a = objectSpace[_indices[0]];
	const Vertex& b = objectSpace[_indices[1]];
	const Vertex& c = objectSpace[_indices[2]];

	Vector3 aTob((b - a).AsVector());
	Vector3 aToc((c - a).AsVector());

	_objectNormal = Vector3::NormaliseVector(Vector3::Cross(aTob, aToc));
}

//
// Calculates the WORLD SPACE normal for this polygon.
//
void Polygon3D::CalculateWorldNormal(const std::vector<Vertex>& worldSpace)
{
	const Vertex& a = worldSpace[_indices[0]];
	const Vertex& b = worldSpace[_indices[1]];
	const Vertex& c = worldSpace[_indices[2]];

	Vector3 aTob((b - a).AsVector());
	Vector3 aToc((c - a).AsVector());

	_worldNormal = Vector3::NormaliseVector(Vector3::Cross(aTob, aToc));
}

//
// Calculates the CLIP SPACE normal for this polygon.
//
void Polygon3D::CalculateClipNormal(const std::vector<Vertex>& clipSpace)
{
	const Vertex& a = clipSpace[_indices[0]];
	const Vertex& b = clipSpace[_indices[1]];
	const Vertex& c = clipSpace[_indices[2]];

	Vector3 aTob((b - a).AsVector());
	Vector3 aToc((c - a).AsVector());

	_clipNormal = Vector3::NormaliseVector(Vector3::Cross(aTob, aToc));
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
