#include "Mesh.h"
#include "ModelLoadingException.h"
#include "MD2Loader.h"
#include <algorithm>
#include <windowsx.h>
#include <memory>
#include "Environment.h"
#include "TriangleRasteriser.h"

//
// Default constructor.
//
Mesh::Mesh() : _previousPen{ 0 }, _previousBrush{ 0 }, _drawMode{ DRAW_SOLID }
{ }

//
// Does nothing.
//
Mesh::~Mesh()
{ }

//
// Loads the mesh data from a file.
//
void Mesh::LoadFromFile(const char* const fileName)
{
	ClearVertices();
	_polygons.clear();

	if (!MD2Loader::LoadModel(fileName, *this, &Mesh::AddPolygon, &Mesh::AddVertex))
	{
		throw ModelLoadingException(fileName);
	}

	GenerateObjectNormals();
}

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
// Recalculates the normals for all polygons.
//
void Mesh::GenerateObjectNormals()
{
	const std::vector<Vertex>& objectVertices = GetVertices();

	for (Polygon3D& polygon : _polygons)
	{
		polygon.CalculateObjectNormal(objectVertices);
	}
}

//
// Recalculates the normals for all polygons in world space.
//
void Mesh::GenerateWorldNormals()
{
	const std::vector<Vertex>& worldVertices = GetWorldSpaceVertices();

	for (Polygon3D& polygon : _polygons)
	{
		polygon.CalculateWorldNormal(worldVertices);
	}
}

//
// Recalculates the normals for all polygons in clip space.
//
void Mesh::GenerateClipNormals()
{
	const std::vector<Vertex>& clipVertices = GetClipSpaceVertices();

	for (Polygon3D& polygon : _polygons)
	{
		polygon.CalculateClipNormal(clipVertices);
	}
}

//
// Recalculates the normals for the vertices of the mesh.
//
void Mesh::GenerateVertexNormals()
{

}

//
// TODO: Draw mesh...
//
void Mesh::Draw(HDC hdc)
{
	CalculateTransformations();

	// Calculate transformed vertices
	const auto& clipSpace = GetClipSpaceVertices();
	const auto& worldSpace = GetWorldSpaceVertices();

	GenerateWorldNormals();
	GenerateClipNormals();
	CalculateBackfaceCulling(clipSpace);
	CalculateDepthSorting(clipSpace);

	for (const Polygon3D* polygon : _culledPolygons)
	{
		switch (_drawMode)
		{
		case DRAW_WIREFRAME:
			DrawWirePolygon(*polygon, clipSpace, worldSpace, hdc);
			break;
		case DRAW_SOLID:
			DrawSolidPolygon(*polygon, clipSpace, worldSpace, hdc);
			break;
		case DRAW_FRAGMENT:
			DrawFragPolygon(*polygon, clipSpace, worldSpace, hdc);
			break;
		}
	}
}

//
// The way this mesh should be rendered.
//
void Mesh::DrawMode(const ::DrawMode& mode)
{
	_drawMode = mode;
}

//
// Pen handling.
//
void Mesh::SetActiveColour(const HDC& hdc, const COLORREF& color, int thickness)
{
	HPEN newPen = CreatePen(PS_SOLID, thickness, color);
	HPEN oldPen = SelectPen(hdc, newPen);

	HBRUSH newBrush = CreateSolidBrush(color);
	HBRUSH oldBrush = SelectBrush(hdc, newBrush);

	_previousPen = oldPen;
	_previousBrush = oldBrush;
}

//
// Resets the pen to its previous value, releases the new pen.
//
void Mesh::ResetActiveColour(const HDC& hdc)
{
	HPEN newPen = SelectPen(hdc, _previousPen);
	HBRUSH newBrush = SelectBrush(hdc, _previousBrush);

	DeleteObject(newPen);
	DeleteObject(newBrush);
}

//
// Calculates which polygons should be backface culled and sorts all others in
// a list.
//
void Mesh::CalculateBackfaceCulling(const std::vector<Vertex>& vertices)
{
	Matrix transform = GetMVP(MVP);

	_culledPolygons.clear();
	for (Polygon3D& polygon : _polygons)
	{
		Vector3 normal = polygon.GetClipNormal();
		Vector3 view = polygon.CalculateCenter(vertices).AsVector();

		normal.Normalise();
		view.Normalise();

		float normalDotView = Vector3::Dot(normal, view);
		if (normalDotView > 0)
		{
			_culledPolygons.push_back(&polygon);
		}
	}
}

//
// Sorts polygons from furthest away to closest.
//
void Mesh::CalculateDepthSorting(const std::vector<Vertex>& vertices)
{
	for (Polygon3D* polygon : _culledPolygons)
	{
		polygon->CalculateDepth(vertices);
	}

	std::sort(_culledPolygons.begin(), _culledPolygons.end(), DepthTest());
}

//
// Draws a single polygon.
//
void Mesh::DrawSolidPolygon(const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace, const HDC& hdc)
{
	Vertex a = clipSpace[polygon.GetIndex(0)];
	Vertex b = clipSpace[polygon.GetIndex(1)];
	Vertex c = clipSpace[polygon.GetIndex(2)];

	POINT points[3]
	{ 
		{ static_cast<int>(a.GetX()), static_cast<int>(a.GetY()) },
		{ static_cast<int>(b.GetX()), static_cast<int>(b.GetY()) },
		{ static_cast<int>(c.GetX()), static_cast<int>(c.GetY()) }
	};

	// Compute final colour
	Colour lighting = ComputeLighting(polygon, worldSpace);
	Colour finalColour = GetColour() * lighting;

	SetActiveColour(hdc, finalColour.AsColor());
	Polygon(hdc, points, 3);
	ResetActiveColour(hdc);
}

//
// Draws a polygon as a wireframe.
//
void Mesh::DrawWirePolygon(const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace, const HDC& hdc)
{
	Vertex a = clipSpace[polygon.GetIndex(0)];
	Vertex b = clipSpace[polygon.GetIndex(1)];
	Vertex c = clipSpace[polygon.GetIndex(2)];

	// Compute final colour
	Colour lighting = ComputeLighting(polygon, worldSpace);
	Colour finalColour = GetColour() * lighting;

	SetActiveColour(hdc, finalColour.AsColor());

	MoveToEx(hdc, static_cast<int>(a.GetX()), static_cast<int>(a.GetY()), NULL);

	LineTo(hdc, static_cast<int>(b.GetX()), static_cast<int>(b.GetY()));
	LineTo(hdc, static_cast<int>(c.GetX()), static_cast<int>(c.GetY()));
	LineTo(hdc, static_cast<int>(a.GetX()), static_cast<int>(a.GetY()));

	ResetActiveColour(hdc);
}

//
// Draws a polygon fragment by fragment.
//
void Mesh::DrawFragPolygon(const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace, const HDC& hdc)
{
	// Compute final colour
	Colour lighting = ComputeLighting(polygon, worldSpace);
	Colour finalColour = GetColour() * lighting;

	SetActiveColour(hdc, finalColour.AsColor());

	// Draw using custom rasterizing system.
	TriangleRasteriser::Draw(hdc, polygon, clipSpace, worldSpace);

	ResetActiveColour(hdc);
}

//
// Computes all lighting to be applied to the polygon.
//
Colour Mesh::ComputeLighting(const Polygon3D& polygon, const std::vector<Vertex>& vertices)
{
	const std::vector<std::shared_ptr<Light>> sceneLights = Environment::GetActive().GetSceneLights();
	Colour totalLightContributions;

	for (std::shared_ptr<Light> light : sceneLights)
	{
		totalLightContributions += light->CalculateContribution(polygon, vertices);
	}

	return totalLightContributions;
}
