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
		Vertex tempVertex(GetTemporaryVertex(a, b, c));

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
	Vertex c0(clipSpace.a);
	Vertex c1(clipSpace.b);
	Vertex c2(clipSpace.c);

	Vertex w0(worldSpace.a);
	Vertex w1(worldSpace.b);
	Vertex w2(worldSpace.c);

	// Make copies of the vertices (not references) as we'll need to sort them.
	Vertices clipData{ c0, c1, c2 };
	Vertices worldData{ w0, w1, w2 };

	SortVertices(clipData, worldData);

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
		Vertex clipTemp(GetTemporaryVertex(clipData.a, clipData.b, clipData.c));
		Vertex worldTemp(GetTemporaryWorldVertex(clipData, worldData));

		Vertices topClipTriangle{ clipData.a, clipData.b, clipTemp };
		Vertices bottomClipTriangle{ clipData.b, clipTemp, clipData.c };

		Vertices topWorldTriangle{ worldData.a, worldData.b, worldTemp };
		Vertices bottomWorldTriangle{ worldData.b, worldTemp, worldData.c };

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

		MoveToEx(hdc, sourceX, y, NULL);
		LineTo(hdc, targetX, y);
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

		MoveToEx(hdc, sourceX, y, NULL);
		LineTo(hdc, targetX, y);
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

		const int sourceX = static_cast<int>(std::ceil(lineData.sourceSlope - 0.5f));
		const int targetX = static_cast<int>(std::ceil(lineData.targetSlope - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const UnclampedColour colour(lineData.sourceColourSlope + lineData.horizontalColourSlope * (static_cast<float>(x) + 0.5f - sourceX));
			const COLORREF pixelColour = RGB(colour.GetRed() * 255, colour.GetGreen() * 255, colour.GetBlue() * 255);

			SetPixelV(hdc, x, y, pixelColour);
		}
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

		const int sourceX = static_cast<int>(std::ceil(lineData.sourceSlope - 0.5f));
		const int targetX = static_cast<int>(std::ceil(lineData.targetSlope - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const UnclampedColour colour(lineData.sourceColourSlope + lineData.horizontalColourSlope * (static_cast<float>(x) + 0.5f - sourceX));
			const COLORREF pixelColour = RGB(colour.GetRed() * 255, colour.GetGreen() * 255, colour.GetBlue() * 255);

			SetPixelV(hdc, x, y, pixelColour);
		}
	}
}

//
// Fills a triangle whose bottom side is perfectly horizontal.
// Precondition is that v2 and v3 perform the flat side and 
// that v1.y < v2.y, v3.y.
//
void TriangleRasteriser::TopPhongShaded(const HDC& hdc, const Vertices& clipSpace, const Vertices& worldSpace, const FragmentFunction& frag)
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
	const PhongShadeInput input{ sortedClip, sortedWorld };

	const PhongShadeData data = GetPhongDataTop(input);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const PhongLineData lineData = GetPhongLineTop(data, y);

		const int sourceX = static_cast<int>(std::ceil(lineData.sourceSlope - 0.5f));
		const int targetX = static_cast<int>(std::ceil(lineData.targetSlope - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const Vector3 normal(Vector3::NormaliseVector(lineData.sourceNormalSlope + lineData.horizontalNormalSlope * (static_cast<float>(x) + 0.5f - sourceX)));
			const Vector3 uv(lineData.sourceUVSlope + lineData.horizontalUVSlope * (static_cast<float>(x) + 0.5f - sourceX));

			Vertex fragment(lineData.sourceWorldSlope + lineData.horizontalWorldSlope * (static_cast<float>(x) + 0.5f - sourceX));

			fragment.GetVertexData().SetNormal(normal);
			fragment.GetVertexData().SetUV(uv);

			const Colour colour = frag(fragment);
			const COLORREF pixelColour = RGB(colour.GetRed() * 255, colour.GetGreen() * 255, colour.GetBlue() * 255);

			SetPixelV(hdc, x, y, pixelColour);
		}
	}
}

//
// Fills a triangle whose top side is perfectly horizontal.
// Precondition is that v1 and v2 perform the flat side and 
// that v3.y > v1.y, v2.y.
//
void TriangleRasteriser::BottomPhongShaded(const HDC& hdc, const Vertices& clipSpace, const Vertices& worldSpace, const FragmentFunction& frag)
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

	const PolygonData sortedClip{ v0, v1, v2 };
	const PolygonData sortedWorld{ w0, w1, w2 };
	const PhongShadeInput input{ sortedClip, sortedWorld };

	const PhongShadeData data = GetPhongDataBottom(input);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const PhongLineData lineData = GetPhongLineBottom(data, y);

		const int sourceX = static_cast<int>(std::ceil(lineData.sourceSlope - 0.5f));
		const int targetX = static_cast<int>(std::ceil(lineData.targetSlope - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const Vector3 normal(Vector3::NormaliseVector(lineData.sourceNormalSlope + lineData.horizontalNormalSlope * (static_cast<float>(x) + 0.5f - sourceX)));
			const Vector3 uv(lineData.sourceUVSlope + lineData.horizontalUVSlope * (static_cast<float>(x) + 0.5f - sourceX));

			Vertex fragment(lineData.sourceWorldSlope + lineData.horizontalWorldSlope * (static_cast<float>(x) + 0.5f - sourceX));

			fragment.GetVertexData().SetNormal(normal);
			fragment.GetVertexData().SetUV(uv);

			const Colour colour = frag(fragment);
			const COLORREF pixelColour = RGB(colour.GetRed() * 255, colour.GetGreen() * 255, colour.GetBlue() * 255);

			SetPixelV(hdc, x, y, pixelColour);
		}
	}
}

//
// Generates a temporary vertex to be placed between the A/B-C diagonal.
// This method also generates the temporary normal and colour
// to be used for lighting/smooth shading calculations.
//
const Vertex TriangleRasteriser::GetTemporaryVertex(const Vertex& a, const Vertex& b, const Vertex& c)
{
	// Split the triangle in a topflat and bottom - flat one
	Vertex tempVertex(a.GetX() + ((b.GetY() - a.GetY()) / (c.GetY() - a.GetY())) * (c.GetX() - a.GetX()), b.GetY(), 0);

	tempVertex.GetVertexData().SetColour(GetTemporaryColour(a, b, c));
	tempVertex.GetVertexData().SetNormal(GetTemporaryNormal(a, b, c));
	tempVertex.GetVertexData().SetUV(GetTemporaryUV(a, b, c));

	return tempVertex;
}

//
// Given a set of three vertices, generates the temporary colour for the
// vertex that is generated between the A/B-C diagonal. The source colours
// are taken from the vertex data.
//
const Colour TriangleRasteriser::GetTemporaryColour(const Vertex& a, const Vertex& b, const Vertex& c)
{
	const UnclampedColour c0(a.GetVertexData().GetColour());
	const UnclampedColour c2(c.GetVertexData().GetColour());

	return (c0 + (c2 - c0) * ((b.GetY() - a.GetY()) / (c.GetY() - a.GetY()))).AsColour();
}

//
// Given a set of three vertices, generates a temporary normal to interpolate
// between the three vertices' normals stored within their vertex data.
//
const Vector3 TriangleRasteriser::GetTemporaryNormal(const Vertex& a, const Vertex& b, const Vertex& c)
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
const Vertex TriangleRasteriser::GetTemporaryWorldVertex(const Vertices& clipSpace, const Vertices& worldSpace)
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
const Vector3 TriangleRasteriser::GetTemporaryUV(const Vertex& a, const Vertex& b, const Vertex& c)
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
void TriangleRasteriser::SortVertices(Vertices& clipSpace, Vertices& worldSpace)
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
