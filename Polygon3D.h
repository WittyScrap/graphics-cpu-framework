#pragma once
#include "Vector.h"
#include "Vertex.h"

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
	Polygon3D(const int& index0, const int& index1, const int& index2);
	virtual ~Polygon3D();

	//
	// Copy constructor.
	//
	Polygon3D(const Polygon3D& other);
	int GetIndex(const int& i) const;

	const Vector3& GetNormal() const;
	const Vector3& CalculateNormal(const Vertex& a, const Vertex& b, const Vertex& c);

	Polygon3D& operator=(const Polygon3D& rhs);

private:
	int _indices[INDICES_COUNT];
	Vector3 _normal;
};

