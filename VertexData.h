#pragma once
#include "Colour.h"
#include "Vector.h"

//
// Provides per-vertex data.
//
class VertexData
{
public:
	VertexData();
	VertexData(const VertexData& copy);

	inline const Vector3& GetNormal() const;
	inline void SetNormal(const Vector3& value);
	inline void AddNormal(const Vector3& value);

	inline const Colour& GetColour() const;
	inline void SetColour(const Colour& value);

	inline const Point<float>& GetUV() const;
	inline void SetUV(const Point<float>& uv);

	inline const float& GetContribution() const;
	inline void SetContribution(const float& value);
	inline void AddContribution(const float& value);

	inline void DivideNormalByContribution();

	const VertexData& operator=(const VertexData& rhs);

private:
	Vector3 _normal;
	Colour _colour;
	Point<float> _uv;

	float _contribution;
};

//
// Empty constructor.
//
inline VertexData::VertexData() : _normal{ 0, 0, 0 }, _colour{ 0, 0, 0 }, _uv{ 0, 0 }, _contribution{ 0 }
{ }

//
// Copy constructor.
//
inline VertexData::VertexData(const VertexData& copy) : _normal(copy.GetNormal()), _colour(copy.GetColour()), _uv(copy.GetUV()), _contribution(copy.GetContribution())
{ }

//
// The vertex normal.
//
inline const Vector3& VertexData::GetNormal() const
{
	return _normal;
}

//
// Updates the vertex normal.
//
inline void VertexData::SetNormal(const Vector3& value)
{
	_normal = value;
}

//
// Adds a vector to the stored normal value.
//
inline void VertexData::AddNormal(const Vector3& value)
{
	_normal += value;
}

//
// The vertex colour.
//
inline const Colour& VertexData::GetColour() const
{
	return _colour;
}

//
// Updates the vertex colour.
//
inline void VertexData::SetColour(const Colour& value)
{
	_colour = value;
}

//
// Returns the UV coordinates for this vertex.
//
inline const Point<float>& VertexData::GetUV() const
{
	return _uv;
}

//
// Sets the UV coordinates for this vertex.
//
inline void VertexData::SetUV(const Point<float>& uv)
{
	_uv = uv;
}

//
// The total contributions from all polygon normals.
//
inline const float& VertexData::GetContribution() const
{
	return _contribution;
}

//
// Updates the contribution from all polygon normals.
//
inline void VertexData::SetContribution(const float& value)
{
	_contribution = value;
}

//
// Adds a value to the stored contribution amount.
//
inline void VertexData::AddContribution(const float& value)
{
	_contribution += value;
}

//
// Divides the gathered normal value by the contribution amount.
//
inline void VertexData::DivideNormalByContribution()
{
	_normal /= _contribution;
}

//
// Assignment operator.
//
inline const VertexData& VertexData::operator=(const VertexData& rhs)
{
	if (&rhs != this)
	{
		_normal = rhs._normal;
		_colour = rhs._colour;
		_contribution = rhs._contribution;
	}

	return *this;
}
