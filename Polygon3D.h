#pragma once
#include "Vector.h"
#include "Vertex.h"
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
	Polygon3D(const int& index0, const int& index1, const int& index2);
	virtual ~Polygon3D();

	//
	// Copy constructor.
	//
	Polygon3D(const Polygon3D& other);
	int GetIndex(const int& i) const;

	const Vector3& GetNormal() const;
	const float& GetDepth() const;
	const Vertex CalculateCenter(const std::vector<Vertex>& vertices) const;

	void CalculateNormal(const Vertex& a, const Vertex& b, const Vertex& c);
	void CalculateDepth(const std::vector<Vertex>& vertices);

	Polygon3D& operator=(const Polygon3D& rhs);
	bool operator<(const Polygon3D& rhs);

private:
	int _indices[INDICES_COUNT];
	float _depth;
	Vector3 _normal;
};
