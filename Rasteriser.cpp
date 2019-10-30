#include "Rasteriser.h"
#include "Point.h"
#include "MD2Loader.h"
#include <cmath>

Rasteriser app;

//
// Initialises the class and the shape vertices.
//
bool Rasteriser::Initialise()
{
	bool exitCode;

	try
	{
		CreateShape();

		// Everything went well (hopefully)...
		exitCode = true;
	}
	catch(...)
	{
		// Everything went disastrously wrong (expected behaviour).
		exitCode = false;
	}

	return exitCode;
}

//
// Renders the shape(s).
//
void Rasteriser::Render(const Bitmap& bitmap)
{
	COLORREF clearColour = GetBackgroundColour();
	HDC hdc = bitmap.GetDC();

	Clear(clearColour, bitmap);
	_shape.Draw(hdc);
}

//
// Handles moving the square about
//
void Rasteriser::Update(const Bitmap& bitmap)
{
	// Rotate about everything...
	_shape.Rotate({ .025f, .05f, .025f });

	// Wobbly scale woo...
	float scaleFactor = std::sin(_timeElapsed * .5f) * 2;
	_shape.SetScale({ scaleFactor, scaleFactor, scaleFactor * scaleFactor });

	// Move around in circles, all together now
	_shape.SetPosition({ _startingPosition.X + std::cos(_timeElapsed) * 100, _startingPosition.Y + std::sin(_timeElapsed) * 100, 0 });

	_timeElapsed += 1.f / 60.f;
}

//
// The window (bitmap)'s background colour.
//
const COLORREF Rasteriser::GetBackgroundColour() const
{
	return _background;
}

//
// The colour for the shapes wireframe.
//
const COLORREF Rasteriser::GetForegroundColour() const
{
	return _shapeColor;
}

//
// Creates a square shape (default).
//
void Rasteriser::CreateShape()
{
	if (!MD2Loader::LoadModel("cube.md2", _shape, &Mesh::AddPolygon, &Mesh::AddVertex))
	{
		throw;
	}

	_shape.SetColour(GetForegroundColour());
	_shape.SetPosition({ 300, 300, 0 });

	_startingPosition = _shape.GetTransform().GetPosition();
}

//
// Clears the screen to a colour
//
void Rasteriser::Clear(const COLORREF& colour, const Bitmap& bitmap)
{
	HDC hdc = bitmap.GetDC();
	RECT fillRect;

	int width = bitmap.GetWidth();
	int height = bitmap.GetHeight();

	fillRect.left = 0;
	fillRect.top = 0;

	fillRect.right = width;
	fillRect.bottom = height;
	
	HBRUSH brush = CreateSolidBrush(colour);
	FillRect(hdc, &fillRect, brush);
	DeleteObject(brush);
}
