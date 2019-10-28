#include "Rasteriser.h"
#include "Point.h"

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
	DrawShape(_shape, hdc);
}

//
// Handles moving the square about
//
void Rasteriser::Update(const Bitmap& bitmap)
{
	Transform(Matrix::TranslationMatrix(1, 1));
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
// Apply a translation matrix to all vertices
//
void Rasteriser::Transform(const Matrix& transformMatrix)
{
	for (Shape::iterator vertex_it = _shape.begin(); vertex_it != _shape.end(); vertex_it++)
	{
		*vertex_it = transformMatrix * *vertex_it;
	}
}

//
// Creates a square shape (default).
//
void Rasteriser::CreateShape()
{
	Point<float> initialPosition{ 200, 200 };
	Point<float> initialSize{ 50, 50 };

	_shape.resize(4); // Avoids unnecessarily resizing the buffer each time.

	_shape[0] = Vertex(initialPosition.X, initialPosition.Y, 0);
	_shape[1] = Vertex(initialPosition.X + initialSize.X, initialPosition.Y, 0);
	_shape[2] = Vertex(initialPosition.X + initialSize.X, initialPosition.Y + initialSize.Y, 0);
	_shape[3] = Vertex(initialPosition.X, initialPosition.Y + initialSize.Y, 0);
}

//
// Draws the square
//
void Rasteriser::DrawShape(const Shape& shape, const HDC& hdc)
{
	// Windows event calls could happen before Rasteriser::Initialise() is called.
	if (shape.size() == 0)
	{
		return;
	}

	HPEN pen = CreatePen(PS_SOLID, 1, GetForegroundColour());
	HPEN old = static_cast<HPEN>(SelectObject(hdc, pen));

	// Iterator to vertices
	Shape::const_iterator it = shape.begin();
	MoveToEx(hdc, static_cast<int>(it->GetX()), static_cast<int>(it->GetY()), NULL);

	// Draw lines to all vertices.
	while (++it != shape.end())
	{
		LineTo(hdc, static_cast<int>(it->GetX()), static_cast<int>(it->GetY()));
	}

	// Draw last connection...
	it = shape.begin();
	LineTo(hdc, static_cast<int>(it->GetX()), static_cast<int>(it->GetY()));

	// Restore old pen, free new pen
	SelectObject(hdc, old);
	DeleteObject(pen);
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
