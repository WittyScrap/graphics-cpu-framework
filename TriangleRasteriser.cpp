#include "TriangleRasteriser.h"
#include <cmath>
#include <Windows.h>

//
// Rasterises a triangle using the standard flat rasterisation
// technique.
//
void TriangleRasteriser::DrawFlat(const HDC& hdc, const PolygonData& clipSpace)
{
	// Make copies of the vertices (not references) as we'll need to sort them.
	Vertex a = clipSpace.a;
	Vertex b = clipSpace.b;
	Vertex c = clipSpace.c;

	SortVertices(a, b, c);

	// Construct copied structure
	PolygonData data
	{
		a,
		b,
		c
	};

	/* here we know that v1.y <= v2.y <= v3.y
	 * check for trivial case of bottom-flat triangle
	 * or that of a top-flat triangle. */
	if (b.GetY() == c.GetY())
	{
		TopFlatShaded(hdc, data);
	}
	else if (a.GetY() == b.GetY())
	{
		BottomFlatShaded(hdc, data);
	}
	else
	{
		// general case - split the triangle in a topflat and bottom-flat one
		Vertex tempVertex(a.GetX() + ((b.GetY() - a.GetY()) / (c.GetY() - a.GetY())) * (c.GetX() - a.GetX()), b.GetY(), 0);

		PolygonData topTriangle{ a, b, tempVertex };
		PolygonData bottomTriangle{ b, tempVertex, c };

		TopFlatShaded(hdc, topTriangle);
		BottomFlatShaded(hdc, bottomTriangle);
	}
}

//
// Rasterises a triangle using the standard solid rasterisation
// technique, shading on a vertex-by-vertex basis.
//
void TriangleRasteriser::DrawSmooth(const HDC& hdc, const PolygonData& clipSpace)
{
	// Make copies of the vertices (not references) as we'll need to sort them.
	Vertex a = clipSpace.a;
	Vertex b = clipSpace.b;
	Vertex c = clipSpace.c;

	SortVertices(a, b, c);

	// Construct copied structure
	PolygonData data
	{
		a,
		b,
		c
	};

	/* here we know that v1.y <= v2.y <= v3.y
	 * check for trivial case of bottom-flat triangle
	 * or that of a top-flat triangle. */
	if (b.GetY() == c.GetY())
	{
		TopSmoothShaded(hdc, data);
	}
	else if (a.GetY() == b.GetY())
	{
		BottomSmoothShaded(hdc, data);
	}
	else
	{
		// General case
		Vertex tempVertex(GetSplitVertex(a, b, c));

		PolygonData topTriangle{ a, b, tempVertex };
		PolygonData bottomTriangle{ b, tempVertex, c };

		TopSmoothShaded(hdc, topTriangle);
		BottomSmoothShaded(hdc, bottomTriangle);
	}
}

//
// Rasterises a triangle using the standard solid rasterisation
// technique, shading on a fragment-by-fragment basis.
//
void TriangleRasteriser::DrawPhong(const HDC& hdc, const PolygonData& clipSpace, const PolygonData& worldSpace, const FragmentFunction& frag)
{
	// Make copies of the vertices (not references) as we'll need to sort them.
	Vertex c0(clipSpace.a);
	Vertex c1(clipSpace.b);
	Vertex c2(clipSpace.c);

	Vertex w0(worldSpace.a);
	Vertex w1(worldSpace.b);
	Vertex w2(worldSpace.c);

	{
		SorterData clipData{ c0, c1, c2 };
		SorterData worldData{ w0, w1, w2 };

		SortVertices(clipData, worldData);
	}

	PolygonData clipData{ c0, c1, c2 };
	PolygonData worldData{ w0, w1, w2 };

	/* here we know that v1.y <= v2.y <= v3.y
	 * check for trivial case of bottom-flat triangle
	 * or that of a top-flat triangle. */
	if (clipData.b.GetY() == clipData.c.GetY())
	{
		TopPhongShaded(hdc, clipData, worldData, frag);
	}
	else if (clipData.a.GetY() == clipData.b.GetY())
	{
		BottomPhongShaded(hdc, clipData, worldData, frag);
	}
	else
	{
		// General case
		Vertex clipTemp(GetSplitVertex(clipData.a, clipData.b, clipData.c));
		Vertex worldTemp(GetSplitWorldVertex(clipData, worldData));

		const PolygonData topClipTriangle{ clipData.a, clipData.b, clipTemp };
		const PolygonData bottomClipTriangle{ clipData.b, clipTemp, clipData.c };

		const PolygonData topWorldTriangle{ worldData.a, worldData.b, worldTemp };
		const PolygonData bottomWorldTriangle{ worldData.b, worldTemp, worldData.c };

		TopPhongShaded(hdc, topClipTriangle, topWorldTriangle, frag);
		BottomPhongShaded(hdc, bottomClipTriangle, bottomWorldTriangle, frag);
	}
}

//
// Fills a triangle whose bottom side is perfectly horizontal.
// Precondition is that v2 and v3 perform the flat side and 
// that v1.y < v2.y, v3.y.
//
void TriangleRasteriser::TopFlatShaded(const HDC& hdc, const PolygonData& clipSpace)
{
	const Vertex& v0 = clipSpace.a;
	const Vertex& v1 = clipSpace.b;
	const Vertex& v2 = clipSpace.c;

	float slope0 = GetSlope(v0, v1);
	float slope1 = GetSlope(v0, v2);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const float point0 = GetHorizontalGradient(slope0, y, v0.GetY(), v0.GetX());
		const float point1 = GetHorizontalGradient(slope1, y, v0.GetY(), v0.GetX());

		int sourceX = static_cast<int>(std::ceil(point0 - 0.5f));
		int targetX = static_cast<int>(std::ceil(point1 - 0.5f));

		if (sourceX > targetX)
		{
			std::swap(sourceX, targetX);
		}

		RenderFlat(hdc, sourceX, targetX, y);
	}
}

//
// Fills a triangle whose top side is perfectly horizontal.
// Precondition is that v1 and v2 perform the flat side and 
// that v3.y > v1.y, v2.y.
//
void TriangleRasteriser::BottomFlatShaded(const HDC& hdc, const PolygonData& clipSpace)
{
	const Vertex& v0 = clipSpace.a;
	const Vertex& v1 = clipSpace.b;
	const Vertex& v2 = clipSpace.c;

	const float slope0 = GetSlope(v0, v2);
	const float slope1 = GetSlope(v1, v2);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const float point0 = GetHorizontalGradient(slope0, y, v2.GetY(), v2.GetX());
		const float point1 = GetHorizontalGradient(slope1, y, v2.GetY(), v2.GetX());

		int sourceX = static_cast<int>(std::ceil(point0 - 0.5f));
		int targetX = static_cast<int>(std::ceil(point1 - 0.5f));

		if (sourceX > targetX)
		{
			std::swap(sourceX, targetX);
		}

		RenderFlat(hdc, sourceX, targetX, y);
	}
}

//
// Fills a triangle whose bottom side is perfectly horizontal.
// Precondition is that v2 and v3 perform the flat side and 
// that v1.y < v2.y, v3.y.
//
void TriangleRasteriser::TopSmoothShaded(const HDC& hdc, const PolygonData& clipSpace)
{
	Vertex v0(clipSpace.a);
	Vertex v1(clipSpace.b);
	Vertex v2(clipSpace.c);

	if (v1.GetX() > v2.GetX())
	{
		std::swap(v1, v2);
	}

	const PolygonData sortedClipSpace{ v0, v1, v2 };
	const SmoothShadeData shadeData = GetSmoothDataTop(sortedClipSpace);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const SmoothLineData lineData = GetSmoothLineTop(shadeData, y);

		RenderSmooth(hdc, lineData, y);
	}
}

//
// Fills a triangle whose top side is perfectly horizontal.
// Precondition is that v1 and v2 perform the flat side and 
// that v3.y > v1.y, v2.y.
//
void TriangleRasteriser::BottomSmoothShaded(const HDC& hdc, const PolygonData& clipSpace)
{
	Vertex v0(clipSpace.a);
	Vertex v1(clipSpace.b);
	Vertex v2(clipSpace.c);

	if (v0.GetX() > v1.GetX())
	{
		std::swap(v0, v1);
	}

	const PolygonData sortedClipSpace{ v0, v1, v2 };
	const SmoothShadeData shadeData = GetSmoothDataBottom(sortedClipSpace);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const SmoothLineData lineData = GetSmoothLineBottom(shadeData, y);

		RenderSmooth(hdc, lineData, y);
	}
}

//
// Fills a triangle whose bottom side is perfectly horizontal.
// Precondition is that v2 and v3 perform the flat side and 
// that v1.y < v2.y, v3.y.
//
void TriangleRasteriser::TopPhongShaded(const HDC& hdc, const PolygonData& clipSpace, const PolygonData& worldSpace, const FragmentFunction& frag)
{
	Vertex v0(clipSpace.a);
	Vertex v1(clipSpace.b);
	Vertex v2(clipSpace.c);

	Vertex w0(worldSpace.a);
	Vertex w1(worldSpace.b);
	Vertex w2(worldSpace.c);

	if (v1.GetX() > v2.GetX())
	{
		std::swap(v1, v2);
		std::swap(w1, w2);
	}

	const PolygonData sortedClip{ v0, v1, v2 };
	const PolygonData sortedWorld{ w0, w1, w2 };

	const PhongShadeData data = GetPhongDataTop({ sortedClip, sortedWorld });

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const PhongLineData lineData = GetPhongLineTop(data, y);

		RenderPhong(hdc, lineData, frag, y);
	}
}

//
// Fills a triangle whose top side is perfectly horizontal.
// Precondition is that v1 and v2 perform the flat side and 
// that v3.y > v1.y, v2.y.
//
void TriangleRasteriser::BottomPhongShaded(const HDC& hdc, const PolygonData& clipSpace, const PolygonData& worldSpace, const FragmentFunction& frag)
{
	Vertex v0(clipSpace.a);
	Vertex v1(clipSpace.b);
	Vertex v2(clipSpace.c);

	Vertex w0(worldSpace.a);
	Vertex w1(worldSpace.b);
	Vertex w2(worldSpace.c);

	if (v0.GetX() > v1.GetX())
	{
		std::swap(v0, v1);
		std::swap(w0, w1);
	}

	Vertex fragment;

	const PolygonData sortedClip{ v0, v1, v2 };
	const PolygonData sortedWorld{ w0, w1, w2 };

	const PhongShadeData data = GetPhongDataBottom({ sortedClip, sortedWorld });

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const PhongLineData lineData = GetPhongLineBottom(data, y);

		RenderPhong(hdc, lineData, frag, y);
	}
}

//
// Generates a temporary vertex to be placed between the A/B-C diagonal.
// This method also generates the temporary normal and colour
// to be used for lighting/smooth shading calculations.
//
const Vertex TriangleRasteriser::GetSplitVertex(const Vertex& a, const Vertex& b, const Vertex& c)
{
	// Split the triangle in a topflat and bottomflat one
	Vertex tempVertex(a.GetX() + ((b.GetY() - a.GetY()) / (c.GetY() - a.GetY())) * (c.GetX() - a.GetX()), b.GetY(), 0);

	tempVertex.GetVertexData().SetColour(GetSplitColour(a, b, c));
	tempVertex.GetVertexData().SetNormal(GetSplitNormal(a, b, c));
	tempVertex.GetVertexData().SetUV(GetSplitUV(a, b, c));

	return tempVertex;
}

//
// Given a set of three vertices, generates the temporary colour for the
// vertex that is generated between the A/B-C diagonal. The source colours
// are taken from the vertex data.
//
const Colour TriangleRasteriser::GetSplitColour(const Vertex& a, const Vertex& b, const Vertex& c)
{
	const UnclampedColour c0(a.GetVertexData().GetColour());
	const UnclampedColour c2(c.GetVertexData().GetColour());

	return (c0 + (c2 - c0) * ((b.GetY() - a.GetY()) / (c.GetY() - a.GetY()))).AsColour();
}

//
// Given a set of three vertices, generates a temporary normal to interpolate
// between the three vertices' normals stored within their vertex data.
//
const Vector3 TriangleRasteriser::GetSplitNormal(const Vertex& a, const Vertex& b, const Vertex& c)
{
	const Vector3& n0(a.GetVertexData().GetNormal());
	const Vector3& n2(c.GetVertexData().GetNormal());

	return n0 + (n2 - n0) * ((b.GetY() - a.GetY()) / (c.GetY() - a.GetY()));
}

//
// Generates a temporary vertex to be placed between the A/B-C diagonal.
// This method also generates the temporary normal and colour
// to be used for lighting/smooth shading calculations.
//
const Vertex TriangleRasteriser::GetSplitWorldVertex(const PolygonData& clipSpace, const PolygonData& worldSpace)
{
	const Vertex& v0 = worldSpace.a;
	const Vertex& v2 = worldSpace.c;

	const Vector3& n0(v0.GetVertexData().GetNormal());
	const Vector3& n2(v2.GetVertexData().GetNormal());

	Vertex temp(Vector3(v0) + Vector3(v2 - v0) * ((clipSpace.b.GetY() - clipSpace.a.GetY()) / (clipSpace.c.GetY() - clipSpace.a.GetY())));
	temp.GetVertexData().SetNormal(n0 + (n2 - n0) * ((clipSpace.b.GetY() - clipSpace.a.GetY()) / (clipSpace.c.GetY() - clipSpace.a.GetY())));

	return temp;
}

//
// Generates a set of temporary interpolated UV coordinates to be supplied to
// the provided temporary vertex, which sits between the A/B - C slopes of the
// triangle to rasterize.
//
const Vector3 TriangleRasteriser::GetSplitUV(const Vertex& a, const Vertex& b, const Vertex& c)
{
	const Vector3& uv0 = a.GetVertexData().GetUV();
	const Vector3& uv2 = c.GetVertexData().GetUV();

	return uv0 + (uv2 - uv0) * ((b.GetY() - a.GetY()) / (c.GetY() - a.GetY()));
}

//
// Sorts vertices by their Y value, so that A has the smallest
// Y value, B has one that is always less than C and more than A,
// and C has the largest Y value.
//
void TriangleRasteriser::SortVertices(Vertex& a, Vertex& b, Vertex& c)
{
	if (a.GetY() > b.GetY())
	{
		std::swap(a, b);
	}

	if (b.GetY() > c.GetY())
	{
		std::swap(b, c);
	}

	if (a.GetY() > b.GetY())
	{
		std::swap(a, b);
	}
}

//
// Sorts vertices by their Y value, so that A has the smallest
// Y value, B has one that is always less than C and more than A,
// and C has the largest Y value.
//
void TriangleRasteriser::SortVertices(SorterData& clipSpace, SorterData& worldSpace)
{
	if (clipSpace.a.GetY() > clipSpace.b.GetY())
	{
		std::swap(clipSpace.a, clipSpace.b);
		std::swap(worldSpace.a, worldSpace.b);
	}

	if (clipSpace.b.GetY() > clipSpace.c.GetY())
	{
		std::swap(clipSpace.b, clipSpace.c);
		std::swap(worldSpace.b, worldSpace.c);
	}

	if (clipSpace.a.GetY() > clipSpace.b.GetY())
	{
		std::swap(clipSpace.a, clipSpace.b);
		std::swap(worldSpace.a, worldSpace.b);
	}
}

//
// Calculates the slope between the two vertices.
//
float TriangleRasteriser::GetSlope(const Vertex& a, const Vertex& b)
{
	return (b.GetX() - a.GetX()) / (b.GetY() - a.GetY());
}

//
// Calculates y-related smooth shade data for rasterising top triangles.
//
const TriangleRasteriser::SmoothShadeData TriangleRasteriser::GetSmoothDataTop(const PolygonData& clipSpace)
{
	const Vertex& v0(clipSpace.a);
	const Vertex& v1(clipSpace.b);
	const Vertex& v2(clipSpace.c);

	const UnclampedColour c0(v0.GetVertexData().GetColour());
	const UnclampedColour c1(v1.GetVertexData().GetColour());
	const UnclampedColour c2(v2.GetVertexData().GetColour());

	const float ba = (v1.GetY() - v0.GetY());
	const float ca = (v2.GetY() - v0.GetY());

	return
	{
		GetSlope(v0, v1),
		GetSlope(v0, v2),
		GetVerticalGradient(c0, c1, ba),
		GetVerticalGradient(c0, c2, ca),
		clipSpace
	};
}

//
// Calculates y-related smooth shade data for rasterising bottom triangles.
//
const TriangleRasteriser::SmoothShadeData TriangleRasteriser::GetSmoothDataBottom(const PolygonData& clipSpace)
{
	const Vertex& v0(clipSpace.a);
	const Vertex& v1(clipSpace.b);
	const Vertex& v2(clipSpace.c);

	const UnclampedColour c0(v0.GetVertexData().GetColour());
	const UnclampedColour c1(v1.GetVertexData().GetColour());
	const UnclampedColour c2(v2.GetVertexData().GetColour());

	const float ca = (v2.GetY() - v0.GetY());
	const float cb = (v2.GetY() - v1.GetY());

	return
	{
		GetSlope(v0, v2),
		GetSlope(v1, v2),
		GetVerticalGradient(c0, c2, ca),
		GetVerticalGradient(c1, c2, cb),
		clipSpace
	};
}

//
// Calculates x-related smooth shade data for rasterising top triangles.
//
const TriangleRasteriser::SmoothLineData TriangleRasteriser::GetSmoothLineTop(const SmoothShadeData& slopeData, const int& pos)
{
	const Vertex& v0(slopeData.clipSpace.a);
	const UnclampedColour c0(v0.GetVertexData().GetColour());

	SmoothLineData data;

	data.sourceSlope = GetHorizontalGradient(slopeData.sourceSlope, pos, v0.GetY(), v0.GetX());
	data.targetSlope = GetHorizontalGradient(slopeData.targetSlope, pos, v0.GetY(), v0.GetX());
	data.sourceColourSlope = GetHorizontalGradient(slopeData.sourceColourSlope, pos, v0.GetY(), c0);
	data.targetColourSlope = GetHorizontalGradient(slopeData.targetColourSlope, pos, v0.GetY(), c0);
	data.horizontalColourSlope = (data.targetColourSlope - data.sourceColourSlope) / (data.targetSlope - data.sourceSlope);

	return data;
}

//
// Calculates x-related smooth shade data for rasterising bottom triangles.
//
const TriangleRasteriser::SmoothLineData TriangleRasteriser::GetSmoothLineBottom(const SmoothShadeData& slopeData, const int& pos)
{
	const Vertex& v2(slopeData.clipSpace.c);
	const UnclampedColour c2(v2.GetVertexData().GetColour());

	SmoothLineData data;

	data.sourceSlope = GetHorizontalGradient(slopeData.sourceSlope, pos, v2.GetY(), v2.GetX());
	data.targetSlope = GetHorizontalGradient(slopeData.targetSlope, pos, v2.GetY(), v2.GetX());
	data.sourceColourSlope = GetHorizontalGradient(slopeData.sourceColourSlope, pos, v2.GetY(), c2);
	data.targetColourSlope = GetHorizontalGradient(slopeData.targetColourSlope, pos, v2.GetY(), c2);
	data.horizontalColourSlope = (data.targetColourSlope - data.sourceColourSlope) / (data.targetSlope - data.sourceSlope);

	return data;
}

//
// Calculates y-related phong shade data for rasterising top triangles.
//
const TriangleRasteriser::PhongShadeData TriangleRasteriser::GetPhongDataTop(const PhongShadeInput& input)
{
	const Vertex& v0(input.clipSpace.a);
	const Vertex& v1(input.clipSpace.b);
	const Vertex& v2(input.clipSpace.c);

	const Vertex& w0(input.worldSpace.a);
	const Vertex& w1(input.worldSpace.b);
	const Vertex& w2(input.worldSpace.c);

	const Vector3& n0(w0.GetVertexData().GetNormal());
	const Vector3& n1(w1.GetVertexData().GetNormal());
	const Vector3& n2(w2.GetVertexData().GetNormal());

	const Vector3 p0(w0);
	const Vector3 p1(w1);
	const Vector3 p2(w2);

	// Here, UVs will be converted to a Vector3 structure to simplify mathematics
	Vector3 u0(v0.GetVertexData().GetUV());
	Vector3 u1(v1.GetVertexData().GetUV());
	Vector3 u2(v2.GetVertexData().GetUV());

	AdjustUV(u0, w0.GetZ());
	AdjustUV(u1, w1.GetZ());
	AdjustUV(u2, w2.GetZ());

	const float ba = (v1.GetY() - v0.GetY()); // Get the change along edge v2->v1
	const float ca = (v2.GetY() - v0.GetY()); // Get the change along edge v3->v1

	return
	{
		GetSlope(v0, v1),
		GetSlope(v0, v2),
		GetVerticalGradient(n0, n1, ba),
		GetVerticalGradient(n0, n2, ca),
		GetVerticalGradient(p0, p1, ba),
		GetVerticalGradient(p0, p2, ca),
		GetVerticalGradient(u0, u1, ba),
		GetVerticalGradient(u0, u2, ca),
		u0,
		u1,
		u2,
		input.clipSpace,
		input.worldSpace
	};
}

//
// Calculates y-related phong shade data for rasterising bottom triangles.
//
const TriangleRasteriser::PhongShadeData TriangleRasteriser::GetPhongDataBottom(const PhongShadeInput& input)
{
	const Vertex& v0(input.clipSpace.a);
	const Vertex& v1(input.clipSpace.b);
	const Vertex& v2(input.clipSpace.c);

	const Vertex& w0(input.worldSpace.a);
	const Vertex& w1(input.worldSpace.b);
	const Vertex& w2(input.worldSpace.c);

	const Vector3& n0(w0.GetVertexData().GetNormal());
	const Vector3& n1(w1.GetVertexData().GetNormal());
	const Vector3& n2(w2.GetVertexData().GetNormal());

	const Vector3 p0(w0);
	const Vector3 p1(w1);
	const Vector3 p2(w2);

	Vector3 u0(v0.GetVertexData().GetUV());
	Vector3 u1(v1.GetVertexData().GetUV());
	Vector3 u2(v2.GetVertexData().GetUV());

	AdjustUV(u0, w0.GetZ());
	AdjustUV(u1, w1.GetZ());
	AdjustUV(u2, w2.GetZ());

	const float ca = (v2.GetY() - v0.GetY()); // Get the change along edge v2->v1
	const float cb = (v2.GetY() - v1.GetY()); // Get the change along edge v3->v1

	return
	{
		GetSlope(v0, v2),
		GetSlope(v1, v2),
		GetVerticalGradient(n0, n2, ca),
		GetVerticalGradient(n1, n2, cb),
		GetVerticalGradient(p0, p2, ca),
		GetVerticalGradient(p1, p2, cb),
		GetVerticalGradient(u0, u2, ca),
		GetVerticalGradient(u1, u2, cb),
		u0,
		u1,
		u2,
		input.clipSpace,
		input.worldSpace
	};
}

//
// Calculates x-related phong shade data for rasterising top triangles.
//
const TriangleRasteriser::PhongLineData TriangleRasteriser::GetPhongLineTop(const PhongShadeData& slopeData, const int& pos)
{
	const Vertex& vertx(slopeData.clipSpace.a);
	const Vertex& world(slopeData.worldSpace.a);
	const Vector3& norml(world.GetVertexData().GetNormal());
	const Vector3& uv(slopeData.uv0);

	PhongLineData data;

	data.sourceSlope = GetHorizontalGradient(slopeData.sourceSlope, pos, vertx.GetY(), vertx.GetX());
	data.targetSlope = GetHorizontalGradient(slopeData.targetSlope, pos, vertx.GetY(), vertx.GetX());

	data.sourceNormalSlope = GetHorizontalGradient(slopeData.sourceNormalSlope, pos, vertx.GetY(), norml);
	data.targetNormalSlope = GetHorizontalGradient(slopeData.targetNormalSlope, pos, vertx.GetY(), norml);
	data.horizontalNormalSlope = (data.targetNormalSlope - data.sourceNormalSlope) / (data.targetSlope - data.sourceSlope);

	data.sourceWorldSlope = GetHorizontalGradient<Vector3>(slopeData.sourceWorldSlope, pos, vertx.GetY(), world);
	data.targetWorldSlope = GetHorizontalGradient<Vector3>(slopeData.targetWorldSlope, pos, vertx.GetY(), world);
	data.horizontalWorldSlope = (data.targetWorldSlope - data.sourceWorldSlope) / (data.targetSlope - data.sourceSlope);

	data.sourceUVSlope = GetHorizontalGradient(slopeData.sourceUVSlope, pos, vertx.GetY(), uv);
	data.targetUVSlope = GetHorizontalGradient(slopeData.targetUVSlope, pos, vertx.GetY(), uv);
	data.horizontalUVSlope = (data.targetUVSlope - data.sourceUVSlope) / (data.targetSlope - data.sourceSlope);

	return data;
}

//
// Calculates x-related phong shade data for rasterising bottom triangles.
//s
const TriangleRasteriser::PhongLineData TriangleRasteriser::GetPhongLineBottom(const PhongShadeData& slopeData, const int& pos)
{
	const Vertex& vertx(slopeData.clipSpace.c);
	const Vertex& world(slopeData.worldSpace.c);
	const Vector3& norml(world.GetVertexData().GetNormal());
	const Vector3& uv(slopeData.uv2);

	PhongLineData data;

	data.sourceSlope = GetHorizontalGradient(slopeData.sourceSlope, pos, vertx.GetY(), vertx.GetX());
	data.targetSlope = GetHorizontalGradient(slopeData.targetSlope, pos, vertx.GetY(), vertx.GetX());

	data.sourceNormalSlope = GetHorizontalGradient(slopeData.sourceNormalSlope, pos, vertx.GetY(), norml);
	data.targetNormalSlope = GetHorizontalGradient(slopeData.targetNormalSlope, pos, vertx.GetY(), norml);
	data.horizontalNormalSlope = (data.targetNormalSlope - data.sourceNormalSlope) / (data.targetSlope - data.sourceSlope);

	data.sourceWorldSlope = GetHorizontalGradient<Vector3>(slopeData.sourceWorldSlope, pos, vertx.GetY(), world);
	data.targetWorldSlope = GetHorizontalGradient<Vector3>(slopeData.targetWorldSlope, pos, vertx.GetY(), world);
	data.horizontalWorldSlope = (data.targetWorldSlope - data.sourceWorldSlope) / (data.targetSlope - data.sourceSlope);

	data.sourceUVSlope = GetHorizontalGradient(slopeData.sourceUVSlope, pos, vertx.GetY(), uv);
	data.targetUVSlope = GetHorizontalGradient(slopeData.targetUVSlope, pos, vertx.GetY(), uv);
	data.horizontalUVSlope = (data.targetUVSlope - data.sourceUVSlope) / (data.targetSlope - data.sourceSlope);

	return data;
}

//
// Prepares the UV coordinates for projection adjustments.
//
void TriangleRasteriser::AdjustUV(Vector3& uv, const float& z)
{
	const float u = uv.GetX() / z;
	const float v = uv.GetY() / z;
	const float d = 1 / z;

	uv.SetX(u);
	uv.SetY(v);
	uv.SetZ(d);
}

//
// Applies perspective correction on a set of UV coordinates.
//
void TriangleRasteriser::ApplyUV(Vector3& uv)
{
	const float u = uv.GetX() / uv.GetZ();
	const float v = uv.GetY() / uv.GetZ();
	constexpr float z = 0;

	uv.SetX(u);
	uv.SetY(v);
	uv.SetZ(z);
}

//
// Renders a generif flat shaded triangle line.
//
void TriangleRasteriser::RenderFlat(const HDC& hdc, const int& start, const int& end, const int& pos)
{
	MoveToEx(hdc, start, pos, NULL);
	LineTo(hdc, end, pos);
}

//
// Renders a generic smooth shaded tirangle line.
//
void TriangleRasteriser::RenderSmooth(const HDC& hdc, const SmoothLineData& lineData, const int& pos)
{
	const int sourceX = static_cast<int>(std::ceil(lineData.sourceSlope - 0.5f));
	const int targetX = static_cast<int>(std::ceil(lineData.targetSlope - 0.5f));

	for (int x = sourceX; x < targetX; ++x)
	{
		const UnclampedColour colour(lineData.sourceColourSlope + lineData.horizontalColourSlope * (static_cast<float>(x) + 0.5f - sourceX));
		const COLORREF pixelColour = RGB(colour.GetRed() * 255, colour.GetGreen() * 255, colour.GetBlue() * 255);

		SetPixelV(hdc, x, pos, pixelColour);
	}
}

//
// Renders a generic phong shaded triangle line.
//
void TriangleRasteriser::RenderPhong(const HDC& hdc, const PhongLineData& lineData, const FragmentFunction& frag, const int& pos)
{
	const int sourceX = static_cast<int>(std::ceil(lineData.sourceSlope - 0.5f));
	const int targetX = static_cast<int>(std::ceil(lineData.targetSlope - 0.5f));

	for (int x = sourceX; x < targetX; ++x)
	{
		const Vector3 normal(Vector3::NormaliseVector(lineData.sourceNormalSlope + lineData.horizontalNormalSlope * (static_cast<float>(x) + 0.5f - sourceX)));
		Vector3 uv(lineData.sourceUVSlope + lineData.horizontalUVSlope * (static_cast<float>(x) + 0.5f - sourceX));
		ApplyUV(uv);

		Vertex fragment = lineData.sourceWorldSlope + lineData.horizontalWorldSlope * (static_cast<float>(x) + 0.5f - sourceX);

		fragment.GetVertexData().SetNormal(normal);
		fragment.GetVertexData().SetUV(uv);

		const Colour colour = frag(fragment);
		const COLORREF pixelColour = RGB(colour.GetRed() * 255, colour.GetGreen() * 255, colour.GetBlue() * 255);

		SetPixelV(hdc, x, pos, pixelColour);
	}
}