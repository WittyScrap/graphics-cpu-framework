#pragma once
#include "Framework.h"
#include <vector>
#include <iostream>
#include <sstream>
#include "Vertex.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Camera.h"
#include "ModelLoadingException.h"

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
	virtual void Update(const Bitmap& bitmap, const float& deltaTime) override;

	const COLORREF GetBackgroundColour() const;
	const COLORREF GetForegroundColour() const;

	void Log(const char* const message) const;

protected:
	//
	// Shape generation and handling.
	// 
	virtual void CreateShape();

	// 
	// Drawing utils
	//
	void Clear(const COLORREF& colour, const Bitmap& bitmap);

	//
	// Input
	//
	bool IsKeyHeld(int keyCode);
	bool IsKeyDown(int keyCode);
	bool IsKeyUp(int keyCode);

private:
	// Square shape
	Mesh _shape;
	Camera _camera;

	// Colours
	COLORREF _background = RGB(0xFF, 0xFF, 0xFF);
	COLORREF _shapeColor = RGB(0x00, 0x00, 0xFF);

	// Time elapsed
	float _timeElapsed = 0;
};

