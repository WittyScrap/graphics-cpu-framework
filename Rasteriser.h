#pragma once
#include "Framework.h"
#include <vector>
#include "Vertex.h"
#include "Matrix.h"

//
// Simple shape alias
//
using Shape = std::vector<Vertex>;

//
// Rasteriser class for handling drawing and mathematics operations.
//
class Rasteriser : public Framework
{
public:
	// Initialises the square corners.
	virtual bool Initialise() override;

	virtual void Render(const Bitmap& bitmap) override;
	virtual void Update(const Bitmap& bitmap) override;

	const COLORREF GetBackgroundColour() const;
	const COLORREF GetForegroundColour() const;

	//
	// Matrix transformations
	//
	void Transform(const Matrix& transformMatrix);

protected:
	//
	// Shape generation and handling.
	// 
	virtual void CreateShape();
	virtual void DrawShape(const Shape& shape, const HDC& hdc);

	// 
	// Drawing utils
	//
	void Clear(const COLORREF& colour, const Bitmap& bitmap);

private:
	// Dynamic sized shape.
	Shape _shape;

	// Colours
	COLORREF _background = RGB(0xFF, 0xFF, 0xFF);
	COLORREF _shapeColor = RGB(0x00, 0x00, 0xFF);
};

