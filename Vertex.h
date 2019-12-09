#pragma once
#include <initializer_list>
#include "Colour.h"
#include "VertexData.h"

//
// Represents a vertex, with X, Y, Z, and W components.
//
class Vertex
{
public:
	Vertex();
	Vertex(float x, float y, float z);
	Vertex(float x, float y, float z, float w);
	Vertex(const Vertex& other);

	const float& GetX() const;
	const float& GetY() const;
	const float& GetZ() const;
	const float& GetW() const;

	const float& GetDepth() const;

	void SetX(const float& x);
	void SetY(const float& x);
	void SetZ(const float& x);
	void SetW(const float& x);

	const Vertex operator+(const Vertex& rhs) const;
	const Vertex operator-(const Vertex& rhs) const;
	const Vertex& operator =(const Vertex& rhs);

	template <typename ...TVertices>
	static Vertex GetAverage(const TVertices& ...vertices);

	const Vector3 AsVector() const;
	void Dehomogenise();

	VertexData& GetVertexData();
	const VertexData& GetVertexData() const;

	static Vertex Lerp(const Vertex& lhs, const Vertex& rhs, const float& alpha);

private:
	static Vertex GetAverage(const std::initializer_list<Vertex>& vertices);

private:
	float _x;
	float _y;
	float _z;
	float _w;

	float _depth;

	VertexData _vertexData;
};

//
// Returns the average of all passed vertices.
//
template<typename ...TVertices>
inline Vertex Vertex::GetAverage(const TVertices& ...vertices)
{
	return GetAverage({ vertices... });
}

