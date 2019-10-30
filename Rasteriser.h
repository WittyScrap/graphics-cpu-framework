#pragma once
#include "Framework.h"
#include <vector>
#include "Vertex.h"
#include "Matrix.h"
#include "Mesh.h"

using Vertices = std::vector<Vertex>;

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

protected:
	//
	// Shape generation and handling.
	// 
	virtual void CreateShape();

	// 
	// Drawing utils
	//
	void Clear(const COLORREF& colour, const Bitmap& bitmap);

private:
	// Square shape
	Mesh _shape;

	// Colours
	COLORREF _background = RGB(0xFF, 0xFF, 0xFF);
	COLORREF _shapeColor = RGB(0x00, 0x00, 0xFF);

	// Starting position
	Point3D<float> _startingPosition = { 0, 0 };

	// Time elapsed
	float _timeElapsed = 0;
};

