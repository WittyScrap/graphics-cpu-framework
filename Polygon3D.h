#pragma once
#include "Vector.h"
#include "Vertex.h"
#include "Colour.h"
#include <vector>

//
// Indices supported by polygon.
//
constexpr auto INDICES_COUNT = 3;

//
// 3D polygon composed of 3 indices that
// point to vertices in a mesh.
//
class Polygon3D
{
public:
	//
	// Constructors, destructors
	//
	Polygon3D();
	Polygon3D(const int& p0, const int& p1, const int& p2, const int& u0, const int& u1, const int u2);
	virtual ~Polygon3D();

	//
	// Copy constructor.
	//
	Polygon3D(const Polygon3D& other);

	int GetVertex(const int& i) const;
	int GetUVCoord(const int& i) const;

	const Vector3& GetObjectNormal() const;
	const Vector3& GetWorldNormal() const;
	const Vector3& GetClipNormal() const;

	const float& GetDepth() const;
	const Colour& GetColour() const;

	const Vertex CalculateCenter(const std::vector<Vertex>& vertices) const;
	void CalculateDepth(const std::vector<Vertex>& vertices);

	void CalculateObjectNormal(const std::vector<Vertex>& objectSpace);
	void CalculateWorldNormal(const std::vector<Vertex>& worldSpace);
	void CalculateClipNormal(const std::vector<Vertex>& clipSpace);

	Polygon3D& operator=(const Polygon3D& rhs);

	bool operator<(const Polygon3D& rhs);
	bool operator>(const Polygon3D& rhs);

private:
	int _vertices[INDICES_COUNT];
	int _uvcoords[INDICES_COUNT];

	float _depth;
	Colour _finalColour;

	Vector3 _objectNormal;
	Vector3 _worldNormal;
	Vector3 _clipNormal;
};


//
// Compares the depth of two polygons
//
struct DepthTest
{
	const bool operator()(Polygon3D* lhs, Polygon3D* rhs) const;
};