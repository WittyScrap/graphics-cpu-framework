#pragma once
#include <vector>
#include "Vector.h"
#include "Vertex.h"
#include "Matrix.h"
#include "Transformable.h"
#include <Windows.h>
#include <stack>

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

	const COLORREF& GetColour() const;
	void SetColour(const COLORREF& colour);

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
	virtual const std::vector<Vertex>& CalculateTransformations();
	const Matrix GetMVP(const char& type) const;

private:
	COLORREF _shapeColour; // The colour of the shape.

	std::vector<Vertex> _shapeData;			// Where the model-space vertices will be stored.
	std::vector<Vertex> _transformedShape;	// Where the world-space vertices will be stored and updated.
};

