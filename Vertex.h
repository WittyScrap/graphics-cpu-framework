#pragma once

//
// Vector class forward declaration
//
struct Vector3;

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

	void SetX(const float& x);
	void SetY(const float& x);
	void SetZ(const float& x);
	void SetW(const float& x);

	const Vertex operator+(const Vertex& rhs) const;
	const Vertex operator-(const Vertex& rhs) const;
	const Vertex operator*(const Vertex& rhs) const;
	const Vertex operator/(const Vertex& rhs) const;

	const Vertex& operator =(const Vertex& rhs);
	const Vertex& operator*=(const Vertex& rhs);
	const Vertex& operator/=(const Vertex& rhs);

	const Vertex& operator*=(const float& rhs);
	const Vertex& operator/=(const float& rhs);

	const Vertex operator+(const Vector3& rhs) const;
	const Vertex operator-(const Vector3& rhs) const;
	const Vertex operator*(const Vector3& rhs) const;
	const Vertex operator/(const Vector3& rhs) const;

	const Vector3 AsPoint() const;
	void Dehomogenise();
private:
	float _x;
	float _y;
	float _z;
	float _w;
};