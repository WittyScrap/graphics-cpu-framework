#include "Mesh.h"
#include "ModelLoadingException.h"
#include "MD2Loader.h"
#include <algorithm>
#include <windowsx.h>
#include <memory>
#include "Environment.h"

//
// Default constructor.
//
Mesh::Mesh() : _previousPen { 0 }, _previousBrush { 0 }, _kd(1, 1, 1), _ka(1, 1, 1), _ks(1, 1, 1)
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
void Mesh::RecalculateNormals(const std::vector<Vertex>& vertices)
{
	for (Polygon3D& polygon : _polygons)
	{
		const Vertex& a = vertices[polygon.GetIndex(0)];
		const Vertex& b = vertices[polygon.GetIndex(1)];
		const Vertex& c = vertices[polygon.GetIndex(2)];

		polygon.CalculateNormal(a, b, c);
	}
}

//
// TODO: Draw mesh...
//
void Mesh::Draw(HDC hdc)
{
	// Calculate transformed vertices
	const auto& vertices = CalculateTransformations();

	RecalculateNormals(vertices);
	CalculateBackfaceCulling(vertices);
	CalculateDepthSorting(vertices);

	for (const Polygon3D* polygon : _culledPolygons)
	{
		DrawPolygon(*polygon, vertices, hdc);
	}
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
		Vector3 normal = polygon.GetNormal();
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
void Mesh::DrawPolygon(const Polygon3D& polygon, const std::vector<Vertex>& vertices, const HDC& hdc)
{
	Vertex a = vertices[polygon.GetIndex(0)];
	Vertex b = vertices[polygon.GetIndex(1)];
	Vertex c = vertices[polygon.GetIndex(2)];

	// Convert to points
	POINT pointA{ static_cast<int>(a.GetX()), static_cast<int>(a.GetY()) };
	POINT pointB{ static_cast<int>(b.GetX()), static_cast<int>(b.GetY()) };
	POINT pointC{ static_cast<int>(c.GetX()), static_cast<int>(c.GetY()) };
	POINT points[3]{ pointA, pointB, pointC };

	// Compute final colour
	Colour lighting = ComputeLighting(polygon, GetWorldSpaceVertices());
	Colour finalColour = Colour::FromColor(GetColour()) * lighting;

	SetActiveColour(hdc, finalColour.AsColor());
	Polygon(hdc, points, 3);
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
