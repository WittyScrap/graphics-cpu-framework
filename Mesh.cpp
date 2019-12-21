#include "Mesh.h"
#include "ModelLoadingException.h"
#include "MD2Loader.h"
#include <algorithm>
#include <windowsx.h>
#include <memory>
#include "Environment.h"

//
// Implements a basic unlit fragment function.
//
struct Unlit : public FragmentFunction
{
private:
	const Texture& _texture;

public:
	inline Unlit(const Texture& texture) : _texture(texture)
	{ }

	inline const Colour operator()(const Vertex& v) const override
	{
		COLORREF sample = _texture.GetTextureValue((int)v.GetVertexData().GetUV().X, (int)v.GetVertexData().GetUV().Y);

		return Colour(sample);
	}
};

//
// Implements a simple screen space gradient effect.
//
struct Gradient : public FragmentFunction
{
	inline const Colour operator()(const Vertex& v) const override
	{
		return Colour(v.GetY(), v.GetY(), v.GetY());
	}
};

//
// Displays the world space normals on each point on the mesh.
//
struct Normal : public FragmentFunction
{
	inline const Colour operator()(const Vertex& v) const override
	{
		return Colour(v.GetVertexData().GetNormal().GetX(), v.GetVertexData().GetNormal().GetY(), v.GetVertexData().GetNormal().GetZ());
	}
};

//
// Implementation of phong shading.
//
struct Phong : public FragmentFunction
{
private:
	float _roughness;
	const Texture& _texture;

public:
	inline Phong(const float& roughness, const Texture& texture) : _roughness{ roughness }, _texture{ texture }
	{ }

	inline const Colour operator()(const Vertex& v) const override
	{
		COLORREF sample = _texture.GetTextureValue((int)v.GetVertexData().GetUV().X, (int)v.GetVertexData().GetUV().Y);
		Colour tex(sample);

		return tex * Mesh::ComputeLighting(v, _roughness);
	}
};

//
// Default constructor.
//
Mesh::Mesh() : _previousPen{ 0 }, _previousBrush{ 0 }, _drawMode{ DrawMode::DRAW_SOLID }, _shadeMode { ShadeMode::SHADE_FLAT }, _roughness{ 0.f }
{ }

//
// Does nothing.
//
Mesh::~Mesh()
{ }

//
// Loads the mesh data from a file.
//
void Mesh::LoadFromFile(const char* const fileName, const char* texture)
{
	ClearVertices();
	_polygons.clear();

	if (!MD2Loader::LoadModel(fileName, texture, *this, &Mesh::AddPolygon, &Mesh::AddVertex, &Mesh::AddUVcoord))
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
void Mesh::AddPolygon(int i0, int i1, int i2, int u0, int u1, int u2)
{
	_polygons.push_back(Polygon3D(i0, i1, i2, u0, u1, u2));
}

//
// Adds a new set of UV coordinates.
//
void Mesh::AddUVcoord(float u, float v)
{
	_uv.push_back({ u, v });
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

	for (Vertex& vertex : worldVertices)
	{
		vertex.GetVertexData().SetNormal({ 0, 0, 0 });
		vertex.GetVertexData().SetContribution(0);
	}

	for (const Polygon3D& polygon : _polygons)
	{
		Vertex& a = worldVertices[polygon.GetVertex(0)];
		Vertex& b = worldVertices[polygon.GetVertex(1)];
		Vertex& c = worldVertices[polygon.GetVertex(2)];

		const Vector3& normal = polygon.GetWorldNormal();

		a.GetVertexData().AddNormal(normal);
		a.GetVertexData().AddContribution(1);

		b.GetVertexData().AddNormal(normal);
		b.GetVertexData().AddContribution(1);

		c.GetVertexData().AddNormal(normal);
		c.GetVertexData().AddContribution(1);
	}

	for (Vertex& vertex : worldVertices)
	{
		vertex.GetVertexData().DivideNormalByContribution();
	}
}

//
// TODO: Draw mesh...
//
void Mesh::Draw(HDC hdc)
{
	if (_drawMode == DrawMode::DRAW_NONE)
	{
		return;
	}

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

	if (_drawMode == DrawMode::DRAW_FRAGMENT && _shadeMode == ShadeMode::SHADE_GOURAUD)
	{
		ComputeVertexLighting();
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
// How rough the material is, higher values will result in a more spread out specular reflection.
//
const float& Mesh::GetRoughness() const
{
	return _roughness;
}

//
// Sets how rough the material is, higher values will result in a more spread out specular reflection.
//
void Mesh::SetRoughness(const float& value)
{
	_roughness = value;
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
	Vertex a = clipSpace[polygon.GetVertex(0)];
	Vertex b = clipSpace[polygon.GetVertex(1)];
	Vertex c = clipSpace[polygon.GetVertex(2)];

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
	Vertex a = clipSpace[polygon.GetVertex(0)];
	Vertex b = clipSpace[polygon.GetVertex(1)];
	Vertex c = clipSpace[polygon.GetVertex(2)];

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
	const int& posIndex0 = polygon.GetVertex(0);
	const int& posIndex1 = polygon.GetVertex(1);
	const int& posIndex2 = polygon.GetVertex(2);

	const int& uvsIndex0 = polygon.GetUVCoord(0);
	const int& uvsIndex1 = polygon.GetUVCoord(1);
	const int& uvsIndex2 = polygon.GetUVCoord(2);

	Vertex clipA(clipSpace[posIndex0]);
	Vertex clipB(clipSpace[posIndex1]);
	Vertex clipC(clipSpace[posIndex2]);

	const Vertex& worldA = worldSpace[posIndex0];
	const Vertex& worldB = worldSpace[posIndex1];
	const Vertex& worldC = worldSpace[posIndex2];

	clipA.GetVertexData().SetUV(_uv[uvsIndex0]);
	clipB.GetVertexData().SetUV(_uv[uvsIndex1]);
	clipC.GetVertexData().SetUV(_uv[uvsIndex2]);

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
	case ShadeMode::SHADE_GOURAUD:
		// Lighting per-vertex was calculated before this function was called.
		TriangleRasteriser::DrawSmooth(hdc, { clipA, clipB, clipC });
		break;

	case ShadeMode::SHADE_PHONG:
	{
		Phong frag(_roughness, _texture);

		// Lighting will be calculated per-fragment, so we do not need to compute the lighting here.
		TriangleRasteriser::DrawPhong(hdc, { clipA, clipB, clipC }, { worldA, worldB, worldC }, frag);
	}
	default:
		// Invalid operation.
		return;
	}
}

//
// Computes all lighting to be applied to the polygon.
//
Colour Mesh::ComputeLighting(const Polygon3D& polygon, const std::vector<Vertex>& vertices)
{
	const Vertex position = polygon.CalculateCenter(vertices);
	const Vector3& normal = polygon.GetWorldNormal();

	const std::vector<LightPtr>& sceneLights = Environment::GetActive().GetSceneLights();
	Colour totalLightContributions;

	for (const LightPtr& light : sceneLights)
	{
		totalLightContributions += light->CalculateContribution(position, normal, 0.f); // Flat shading
	}

	return totalLightContributions;
}

//
// Computes the lighting for a single vertex.
//
Colour Mesh::ComputeLighting(const Vertex& vertex, const float& roughness)
{
	const Vector3& normal = vertex.GetVertexData().GetNormal();

	const std::vector<LightPtr>& sceneLights = Environment::GetActive().GetSceneLights();
	Colour totalLightContributions;

	for (const LightPtr& light : sceneLights)
	{
		totalLightContributions += light->CalculateContribution(vertex, normal, roughness);
	}

	return totalLightContributions;
}

//
// The texture of this mesh.
//
const Texture& Mesh::GetTexture() const
{
	return _texture;
}

//
// The modifiable reference to the texture for this mesh.
//
Texture& Mesh::GetTexture()
{
	return _texture;
}

//
// Computes lighting on a per-vertex basis on every vertex of this object.
//
void Mesh::ComputeVertexLighting()
{
	const std::vector<Vertex>& worldVertices = GetWorldSpaceVertices();
	std::vector<Colour> vertexColours;

	for (const Vertex& vertex : worldVertices)
	{
		vertexColours.push_back(GetColour() * ComputeLighting(vertex, _roughness));
	}

	// Apply vertex colours to clip-space vertices.
	std::vector<Vertex>& clipVertices = GetClipSpaceVertices();

	for (size_t i = 0; i < clipVertices.size(); ++i)
	{
		clipVertices[i].GetVertexData().SetColour(vertexColours[i]);
	}
}
