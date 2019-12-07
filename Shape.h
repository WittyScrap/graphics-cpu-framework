#pragma once
#include <vector>
#include "Vector.h"
#include "Vertex.h"
#include "Matrix.h"
#include "Transformable.h"
#include "Colour.h"
#include <Windows.h>
#include <stack>

#define M	0b00000001
#define V	0b00000010
#define P	0b00000100

#define MV	M | V
#define MP	M | P
#define VP	V | P
#define MVP M | V | P


//
// Base shape class (abstract).
//
class Shape : public Transformable
{
public:
	Shape();
	virtual ~Shape();

	//
	// Draws the shape (abstract).
	//
	virtual void Draw(HDC hdc) = 0;

	//
	// Model-space vertices (read-only).
	//
	const std::vector<Vertex>& GetVertices() const;
	const size_t GetVerticesCount() const;

	const Colour GetColour() const;
	void SetColour(const Colour& colour);

	//
	// Full equality operator.
	//
	friend const bool operator==(const Shape& lhs, const Shape& rhs);

protected:
	void CreateVertex(const Vertex& vertex);
	void CreateVertices(const std::vector<Vertex>& vertices);
	void ClearVertices();

	//
	// World-space vertices (read-only).
	//
	void CalculateTransformations();

	//
	// Transformations
	//
	const Matrix GetMVP(const char& type) const;
	const Matrix GetP2C() const;

	//
	// Final application of all necessary transformations.
	//
	const Vertex ObjectToClipSpace(const Matrix& mvp, const Matrix& p2c, const Vertex& vertex);

	//
	// The world-space vertices without any projection/clip space transformation applied to them.
	//
	const std::vector<Vertex>& GetClipSpaceVertices() const;
	const std::vector<Vertex>& GetWorldSpaceVertices() const;

private:
	COLORREF _shapeColour; // The colour of the shape.

	std::vector<Vertex> _shapeData;			// Where the model-space vertices will be stored.
	std::vector<Vertex> _clipSpaceData;		// Where the clip-space vertices will be stored and updated.
	std::vector<Vertex> _worldSpaceData;	// WHere the world-space vertices will be stored and updated.
};

