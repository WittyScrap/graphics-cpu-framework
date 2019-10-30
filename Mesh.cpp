#include "Mesh.h"

//
// Default constructor.
//
Mesh::Mesh() : _previousPen { 0 }
{ }

//
// Does nothing.
//
Mesh::~Mesh()
{ }

//
// Returns a constant, read-only reference to the list of polygons.
//
const std::vector<Polygon3D>& Mesh::GetPolygons() const
{
	return _polygons;
}

//
// The polygon number.
//
const size_t Mesh::GetPolygonCount() const
{
	return _polygons.size();
}

//
// Adds a new vertex.
//
void Mesh::AddVertex(float x, float y, float z)
{
	Shape::CreateVertex(Vertex(x, y, z));
}

//
// Adds a new polygon.
//
void Mesh::AddPolygon(int i0, int i1, int i2)
{
	_polygons.push_back(Polygon3D(i0, i1, i2));
}

//
// TODO: Draw mesh...
//
void Mesh::Draw(HDC hdc)
{
	SetPen(hdc, GetColour());

	for (Polygon3D polygon : _polygons)
	{
		DrawPolygon(polygon, hdc);
	}

	SelectObject(hdc, _previousPen);
}

//
// Pen handling.
//
void Mesh::SetPen(const HDC& hdc, const COLORREF& penColor, int thickness)
{
	HPEN newPen = CreatePen(PS_SOLID, thickness, penColor);
	HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, newPen));

	_previousPen = oldPen;
}

//
// Resets the pen to its previous value, releases the new pen.
//
void Mesh::ResetPen(const HDC& hdc)
{
	HPEN newPen = static_cast<HPEN>(SelectObject(hdc, _previousPen));
	DeleteObject(newPen);
}

//
// Draws a single polygon.
//
void Mesh::DrawPolygon(const Polygon3D& polygon, const HDC& hdc)
{
	std::vector<Vertex> vertices = GetShape();

	Vertex a = vertices[polygon.GetIndex(0)];
	Vertex b = vertices[polygon.GetIndex(1)];
	Vertex c = vertices[polygon.GetIndex(2)];

	// Move to first vertex in triangle
	MoveToEx(hdc, static_cast<int>(a.GetX()), static_cast<int>(a.GetY()), NULL);

	// Draw lines to b and c, then back to a
	LineTo(hdc, static_cast<int>(b.GetX()), static_cast<int>(b.GetY()));
	LineTo(hdc, static_cast<int>(c.GetX()), static_cast<int>(c.GetY()));
	LineTo(hdc, static_cast<int>(a.GetX()), static_cast<int>(a.GetY()));
}
