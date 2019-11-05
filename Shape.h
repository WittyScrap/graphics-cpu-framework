#pragma once
#include <vector>
#include "Vector.h"
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
	void SetPosition(const Vector3& position);
	void SetRotation(const Vector3& rotation);
	void SetScale(const Vector3& scale);

	//
	// Transformation alterations, these will multiply
	// the related current transformation matrices by
	// the supplied matrices.
	//
	void Translate(const Vector3& amount);
	void Rotate(const Vector3& amount);
	void Scale(const Vector3& amount);

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

	//
	// Transformation matrices
	//
	Matrix _position;
	Matrix _rotation;
	Matrix _scale;
};

