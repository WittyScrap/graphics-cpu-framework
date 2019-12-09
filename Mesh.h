#pragma once
#include "Shape.h"
#include "Vertex.h"
#include "Polygon3D.h"
#include "Colour.h"
#include "TriangleRasteriser.h"


//
// A 3D mesh composed of multiple plygons and vertices.
//
class Mesh : public Shape
{
public:
	//
	// How to draw a mesh.
	//
	enum class DrawMode
	{
		DRAW_WIREFRAME,
		DRAW_SOLID,
		DRAW_FRAGMENT
	};

	//
	// How to shade a triangle.
	//
	enum class ShadeMode
	{
		SHADE_FLAT,
		SHADE_SMOOTH
	};


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
	void Mode(const DrawMode& mode);
	void Shade(const ShadeMode& mode);

private:
	//
	// Pen handler
	//
	void SetActiveColour(const HDC& hdc, const COLORREF& penColor, int thickness = 1);
	void ResetActiveColour(const HDC& hdc);

	//
	// Normals
	//
	void GenerateObjectNormals();
	void GenerateWorldNormals();
	void GenerateClipNormals();
	void GenerateVertexNormals();

	//
	// Optimisation tools
	//
	void CalculateBackfaceCulling(const std::vector<Vertex>& vertices);
	void CalculateDepthSorting(const std::vector<Vertex>& polygons);
	
	//
	// Drawing tools
	//
	void DrawSolidPolygon(const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace, const HDC& hdc);
	void DrawWirePolygon(const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace, const HDC& hdc);
	void DrawFragPolygon(const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace, const HDC& hdc);

	//
	// Lighting tools
	//
	Colour ComputeLighting(const Polygon3D& polygon, const std::vector<Vertex>& vertices) const;
	Colour ComputeLighting(const Vertex& vertices) const;

	//
	// Fragment callback
	//
	Colour Frag(const Vertex& i) const;

private:
	std::vector<Polygon3D> _polygons;
	std::vector<Polygon3D*> _visiblePolygons;

	HPEN _previousPen;
	HBRUSH _previousBrush;

	DrawMode _drawMode;
	ShadeMode _shadeMode;
};

