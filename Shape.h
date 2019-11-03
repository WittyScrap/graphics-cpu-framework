#pragma once
#include <vector>
#include "Point3D.h"
#include "Vertex.h"
#include "Matrix.h"
#include <Windows.h>
#include <stack>

//
// Base shape class (abstract).
//
class Shape
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
	// Gets a TRS (Translation Rotation Scale) matrix from
	// the shape's current location, rotation, and scale
	// matrices.
	//
	const Matrix GetTransform() const;

	//
	// Transformation overrides, these will overwrite
	// the related current transformation matrices.
	//
	void SetPosition(const Point3D<float>& position);
	void SetRotation(const Point3D<float>& rotation);
	void SetScale(const Point3D<float>& scale);

	//
	// Transformation alterations, these will multiply
	// the related current transformation matrices by
	// the supplied matrices.
	//
	void Translate(const Point3D<float>& amount);
	void Rotate(const Point3D<float>& amount);
	void Scale(const Point3D<float>& amount);

private:
	// Ensures the _shapeData and _transform vectors have the same size.
	void Realign();

protected:
	void CreateVertex(const Vertex& vertex);
	void CreateVertices(const std::vector<Vertex>& vertices);
	void ClearVertices();

	//
	// World-space vertices (read-only).
	//
	virtual const std::vector<Vertex>& GetShape();
	const Matrix GetMVP() const;

private:
	COLORREF _shapeColor; // The colour of the shape.

	std::vector<Vertex> _shapeData;			// Where the model-space vertices will be stored.
	std::vector<Vertex> _transformedShape;	// Where the world-space vertices will be stored and updated.

	//
	// Transformation matrices
	//
	Matrix _position;
	Matrix _rotation;
	Matrix _scale;
};

