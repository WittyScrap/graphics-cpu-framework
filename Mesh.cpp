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
Mesh::Mesh() : _previousPen{ 0 }, _previousBrush{ 0 }, _drawMode{ DrawMode::DRAW_SOLID }, _shadeMode { ShadeMode::SHADE_FLAT }
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
	std::vector<Vertex>& worldVertices = GetWorldSpaceVertices();
	std::vector<Vertex>& clipVertices = GetClipSpaceVertices();

	for (Vertex& vertex : worldVertices)
	{
		vertex.GetVertexData().SetNormal({ 0, 0, 0 });
		vertex.GetVertexData().SetContribution(0);
	}

	for (const Polygon3D& polygon : _polygons)
	{
		Vertex& a = worldVertices[polygon.GetIndex(0)];
		Vertex& b = worldVertices[polygon.GetIndex(1)];
		Vertex& c = worldVertices[polygon.GetIndex(2)];

		const Vector3& normal = polygon.GetWorldNormal();

		a.GetVertexData().AddNormal(normal);
		a.GetVertexData().AddContribution(1);

		b.GetVertexData().AddNormal(normal);
		b.GetVertexData().AddContribution(1);

		c.GetVertexData().AddNormal(normal);
		c.GetVertexData().AddContribution(1);
	}

	for (size_t i = 0; i < worldVertices.size(); ++i)
	{
		worldVertices[i].GetVertexData().DivideNormalByContribution();
		clipVertices[i].GetVertexData().SetNormal(worldVertices[i].GetVertexData().GetNormal());
	}
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

	if (_drawMode == DrawMode::DRAW_FRAGMENT)
	{
		GenerateVertexNormals();
	}

	for (const Polygon3D* polygon : _visiblePolygons)
	{
		switch (_drawMode)
		{
		case DrawMode::DRAW_WIREFRAME:
			DrawWirePolygon(*polygon, clipSpace, worldSpace, hdc);
			break;
		case DrawMode::DRAW_SOLID:
			DrawSolidPolygon(*polygon, clipSpace, worldSpace, hdc);
			break;
		case DrawMode::DRAW_FRAGMENT:
			DrawFragPolygon(*polygon, clipSpace, worldSpace, hdc);
			break;
		}
	}
}

//
// The way this mesh should be rendered.
//
void Mesh::Mode(const DrawMode& mode)
{
	_drawMode = mode;
}

//
// The way this mesh should shade its triangles.
//
void Mesh::Shade(const ShadeMode& mode)
{
	_shadeMode = mode;
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

	_visiblePolygons.clear();
	for (Polygon3D& polygon : _polygons)
	{
		Vector3 normal = polygon.GetClipNormal();
		Vector3 view = polygon.CalculateCenter(vertices).AsVector();

		normal.Normalise();
		view.Normalise();

		float normalDotView = Vector3::Dot(normal, view);
		if (normalDotView > 0)
		{
			_visiblePolygons.push_back(&polygon);
		}
	}
}

//
// Sorts polygons from furthest away to closest.
//
void Mesh::CalculateDepthSorting(const std::vector<Vertex>& vertices)
{
	for (Polygon3D* polygon : _visiblePolygons)
	{
		polygon->CalculateDepth(vertices);
	}

	std::sort(_visiblePolygons.begin(), _visiblePolygons.end(), DepthTest());
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
	const int& index0 = polygon.GetIndex(0);
	const int& index1 = polygon.GetIndex(1);
	const int& index2 = polygon.GetIndex(2);

	const Vertex& clipA = clipSpace[index0];
	const Vertex& clipB = clipSpace[index1];
	const Vertex& clipC = clipSpace[index2];

	FragmentFunction frag = std::bind(&Mesh::Frag, this, std::placeholders::_1);

	// Draw using custom rasterizing system.
	switch (_shadeMode)
	{
	case ShadeMode::SHADE_FLAT:
	{
		Colour lighting = ComputeLighting(polygon, worldSpace);
		Colour finalColour = GetColour() * lighting;

		SetActiveColour(hdc, finalColour.AsColor());
		TriangleRasteriser::DrawFlat(hdc, { clipA, clipB, clipC });
		ResetActiveColour(hdc);

		break;
	}
	case ShadeMode::SHADE_SMOOTH:
		// Lighting will be calculated per-fragment, so we do not need to compute the lighting here.
		TriangleRasteriser::DrawSmooth(hdc, { clipA, clipB, clipC }, frag);
		break;
	}
}

//
// Computes all lighting to be applied to the polygon.
//
Colour Mesh::ComputeLighting(const Polygon3D& polygon, const std::vector<Vertex>& vertices) const
{
	const Vertex position = polygon.CalculateCenter(vertices);
	const Vector3& normal = polygon.GetWorldNormal();

	const std::vector<LightPtr> sceneLights = Environment::GetActive().GetSceneLights();
	Colour totalLightContributions;

	for (const LightPtr& light : sceneLights)
	{
		totalLightContributions += light->CalculateContribution(position, normal);
	}

	return totalLightContributions;
}

//
// Computers all lighting to be applied to this vertex.
//
Colour Mesh::ComputeLighting(const Vertex& vertex) const
{
	const Vector3& normal = vertex.GetVertexData().GetNormal();

	const std::vector<LightPtr> sceneLights = Environment::GetActive().GetSceneLights();
	Colour totalLightContributions;

	for (const LightPtr& light : sceneLights)
	{
		totalLightContributions += light->CalculateContribution(vertex, normal);
	}

	return totalLightContributions;
}

//
// Fragment function.
//
Colour Mesh::Frag(const Vertex& i) const
{
	const Colour light = ComputeLighting(i);
	const Colour& base = GetColour();

	return base;
}
