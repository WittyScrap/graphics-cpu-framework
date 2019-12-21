#include "Vector.h"
#include "Vertex.h"
#include <numeric>

Vertex::Vertex() : _x{ 0 }, _y{ 0 }, _z{ 0 }, _w{ 0 }, _depth{ 0 }
{ }

Vertex::Vertex(float x, float y, float z) : _x{ x }, _y{ y }, _z{ z }, _w{ 1 }, _depth{ 0 }
{ }

Vertex::Vertex(float x, float y, float z, float w) : _x{ x }, _y{ y }, _z{ z }, _w{ w }, _depth{ 0 }
{ }

Vertex::Vertex(float x, float y, float z, float w, const Vector3& normal) : _x{ x }, _y{ y }, _z{ z }, _w{ w }, _depth{ 0 }, _vertexData{ VertexData() }
{
	_vertexData.SetNormal(normal);
}

Vertex::Vertex(const Vertex& other) : _x{ other._x }, _y{ other._y }, _z{ other._z }, _w{ other._w }, _depth{ other._depth }, _vertexData{ other._vertexData }
{ }

Vertex::Vertex(const Vector3& copy) : _x{ copy.GetX() }, _y{ copy.GetY() }, _z{ copy.GetZ() }, _w { 0 }, _depth { 0 }, _vertexData { VertexData() }
{ }

const float& Vertex::GetX() const
{
	return _x;
}

const float& Vertex::GetY() const
{
	return _y;
}

const float& Vertex::GetZ() const
{
	return _z;
}

const float& Vertex::GetW() const
{
	return _w;
}

const float& Vertex::GetDepth() const
{
	return _depth;
}

void Vertex::SetX(const float& x)
{
	_x = x;
}

void Vertex::SetY(const float& y)
{
	_y = y;
}

void Vertex::SetZ(const float& z)
{
	_z = z;
}

void Vertex::SetW(const float& w)
{
	_w = w;
}

const Vertex& Vertex::operator=(const Vertex& rhs)
{
	_x = rhs._x;
	_y = rhs._y;
	_z = rhs._z;
	_w = rhs._w;

	_depth = rhs._depth;
	_vertexData = rhs._vertexData;

	return *this;
}

const Vertex Vertex::operator+(const Vertex& rhs) const
{
	Vertex sum;
	sum._x = _x + rhs._x;
	sum._y = _y + rhs._y;
	sum._z = _z + rhs._z;
	sum._w = _w + rhs._w;

	return sum;
}

const Vertex Vertex::operator-(const Vertex& rhs) const
{
	return Vertex(_x - rhs._x, _y - rhs._y, _z - rhs._z);
}

const Vector3 Vertex::AsVector() const
{
	return Vector3(_x, _y, _z);
}

void Vertex::Dehomogenise()
{
	_depth = _w;

	_x /= _w;
	_y /= _w;
	_z /= _w;
	_w = 1; /* w/w = 1 */
}

VertexData& Vertex::GetVertexData()
{
	return _vertexData;
}

const VertexData& Vertex::GetVertexData() const
{
	return _vertexData;
}

//
// Interpolates lineraly between the to vertices.
//
Vertex Vertex::Lerp(const Vertex& lhs, const Vertex& rhs, const float& alpha)
{
	Vertex lerp;

	lerp.SetX(rhs.GetX() * alpha + lhs.GetX() * (1 - alpha));
	lerp.SetY(rhs.GetY() * alpha + lhs.GetY() * (1 - alpha));
	lerp.SetZ(rhs.GetZ() * alpha + lhs.GetZ() * (1 - alpha));
	lerp.SetW(rhs.GetW() * alpha + lhs.GetW() * (1 - alpha));

	lerp.GetVertexData().SetNormal(Vector3::Lerp(lhs.GetVertexData().GetNormal(), rhs.GetVertexData().GetNormal(), alpha));
	lerp.GetVertexData().SetColour(Colour::Lerp(lhs.GetVertexData().GetColour(), rhs.GetVertexData().GetColour(), alpha));

	return lerp;
}

//
// Calculates the average of the given vertices.
//
Vertex Vertex::GetAverage(const std::initializer_list<Vertex>& vertices)
{
	Vertex sum = std::accumulate(vertices.begin(), vertices.end(), Vertex());
	size_t size = vertices.size();

	sum._x /= size;
	sum._y /= size;
	sum._z /= size;
	sum._w /= size;

	return sum;
}
