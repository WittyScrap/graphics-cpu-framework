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
// Assigns the values of the passed polygon to this one, then returns this object.
//
Polygon3D& Polygon3D::operator=(const Polygon3D& rhs)
{
	_indices[0] = rhs._indices[0];
	_indices[1] = rhs._indices[1];
	_indices[2] = rhs._indices[2];

	return *this;
}
