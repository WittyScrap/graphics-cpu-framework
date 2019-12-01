#include "Vertex.h"
#include "Vector.h"
#include <numeric>

Vertex::Vertex() : Vertex(0, 0, 0, 1)
{ }

Vertex::Vertex(float x, float y, float z) : Vertex(x, y, z, 1)
{ }

Vertex::Vertex(float x, float y, float z, float w) : _x(x), _y(y), _z(z), _w(w)
{ }

Vertex::Vertex(const Vertex& other) : Vertex(other._x, other._y, other._z, other._w)
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
	_x /= _w;
	_y /= _w;
	_z /= _w;
	_w = 1; /* w/w = 1 */
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
