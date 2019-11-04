#include "Square.h"
#include <Windows.h>

//
// Creates a square and immediately transforms it to the position.
//
void Square::CreateModel(const Vector3& position)
{
	// Create four vertices...
	CreateVertex(Vertex(-_squareWidth, -_squareHeight, 0));
	CreateVertex(Vertex( _squareWidth, -_squareHeight, 0));
	CreateVertex(Vertex( _squareWidth,  _squareHeight, 0));
	CreateVertex(Vertex(-_squareWidth,  _squareHeight, 0));

	// Transform them by the position
	SetPosition({ position.X, position.Y, 0 });
}

//
// Draws the square
//
void Square::Draw(HDC hdc)
{
	std::vector<Vertex> shape = GetVertices();

	// Ensure the shape exists.
	if (shape.size() == 0)
	{
		return;
	}

	HPEN pen = CreatePen(PS_SOLID, 1, GetColour());
	HPEN old = static_cast<HPEN>(SelectObject(hdc, pen));

	// Iterator to vertices
	std::vector<Vertex>::const_iterator it = shape.begin();
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
