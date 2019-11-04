#pragma once
#include "Shape.h"
#include "Vertex.h"
#include "Polygon3D.h"

//
// A 3D mesh composed of multiple plygons and vertices.
//
class Mesh : public Shape
{
public:
	Mesh();
	~Mesh();

	// Loads the mesh data from a file.
	void LoadFromFile(const char* const fileName);

	//
	// Accessors
	//
	const std::vector<Polygon3D>& GetPolygons() const;
	const size_t GetPolygonCount() const;

	//
	// Modifiers
	//
	void AddVertex(float x, float y, float z);
	void AddPolygon(int i0, int i1, int i2);

	//
	// Draw operation
	//
	void Draw(HDC hdc);

private:
	//
	// Pen handler
	//
	void SetPen(const HDC& hdc, const COLORREF& penColor, int thickness = 1);
	void ResetPen(const HDC& hdc);

	//
	// Drawing tools
	//
	void DrawPolygon(const Polygon3D& polygon, const HDC& hdc);

private:
	std::vector<Polygon3D> _polygons;
	HPEN _previousPen;
};

