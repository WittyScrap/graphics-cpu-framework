#include "TriangleRasteriser.h"
#include "UnclampedColour.h"
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
	// Make copies of the vertices (not references) as we'll need to sort them.
	Vertices clipData
	{
		clipSpace.a,
		clipSpace.b,
		clipSpace.c
	};

	Vertices worldData
	{
		worldSpace.a,
		worldSpace.b,
		worldSpace.c
	};

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

	float slope0 = (v1.GetX() - v0.GetX()) / (v1.GetY() - v0.GetY());
	float slope1 = (v2.GetX() - v0.GetX()) / (v2.GetY() - v0.GetY());

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const float point0 = slope0 * ((float)y + 0.5f - v0.GetY()) + v0.GetX();
		const float point1 = slope1 * ((float)y + 0.5f - v0.GetY()) + v0.GetX();

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

	const float slope0 = (v2.GetX() - v0.GetX()) / (v2.GetY() - v0.GetY());
	const float slope1 = (v2.GetX() - v1.GetX()) / (v2.GetY() - v1.GetY());

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const float point0 = slope0 * ((float)y + 0.5f - v2.GetY()) + v2.GetX();
		const float point1 = slope1 * ((float)y + 0.5f - v2.GetY()) + v2.GetX();

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

	const UnclampedColour c0(v0.GetVertexData().GetColour());
	const UnclampedColour c1(v1.GetVertexData().GetColour());
	const UnclampedColour c2(v2.GetVertexData().GetColour());

	const float ba = (v1.GetY() - v0.GetY()); // Get the change along edge v2->v1
	const float ca = (v2.GetY() - v0.GetY()); // Get the change along edge v3->v1

	const float slopeSourceY = (v1.GetX() - v0.GetX()) / ba;
	const float slopeTargetY = (v2.GetX() - v0.GetX()) / ca;

	const UnclampedColour colourSlopeSourceY((c1 - c0) / ba);
	const UnclampedColour colourSlopeTargetY((c2 - c0) / ca);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const float slopeSourceX = slopeSourceY * ((float)y + 0.5f - v0.GetY()) + v0.GetX();
		const float slopeTargetX = slopeTargetY * ((float)y + 0.5f - v0.GetY()) + v0.GetX();

		const UnclampedColour colourSlopeSourceX(colourSlopeSourceY * ((float)y + 0.5f - v0.GetY()) + c0);
		const UnclampedColour colourSlopeTargetX(colourSlopeTargetY * ((float)y + 0.5f - v0.GetY()) + c0);
		const UnclampedColour colourSlope((colourSlopeTargetX - colourSlopeSourceX) / (slopeTargetX - slopeSourceX));

		const int sourceX = static_cast<int>(std::ceil(slopeSourceX - 0.5f));
		const int targetX = static_cast<int>(std::ceil(slopeTargetX - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const UnclampedColour colour(colourSlopeSourceX + colourSlope * (static_cast<float>(x) + 0.5f - sourceX));
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

	const UnclampedColour c0(v0.GetVertexData().GetColour());
	const UnclampedColour c1(v1.GetVertexData().GetColour());
	const UnclampedColour c2(v2.GetVertexData().GetColour());

	const float ca = (v2.GetY() - v0.GetY()); // Get the change along edge v3->v1
	const float cb = (v2.GetY() - v1.GetY()); // Get the change along edge v3->v2

	const float slopeSourceY = (v2.GetX() - v0.GetX()) / ca;
	const float slopeTargetY = (v2.GetX() - v1.GetX()) / cb;

	const UnclampedColour colourSlopeSourceY((c2 - c0) / ca);
	const UnclampedColour colourSlopeTargetY((c2 - c1) / cb);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const float slopeSourceX = slopeSourceY * ((float)y + 0.5f - v2.GetY()) + v2.GetX();
		const float slopeTargetX = slopeTargetY * ((float)y + 0.5f - v2.GetY()) + v2.GetX();

		const UnclampedColour colourSlopeSourceX(colourSlopeSourceY * ((float)y + 0.5f - v2.GetY()) + c2);
		const UnclampedColour colourSlopeTargetX(colourSlopeTargetY * ((float)y + 0.5f - v2.GetY()) + c2);
		const UnclampedColour colourSlope((colourSlopeTargetX - colourSlopeSourceX) / (slopeTargetX - slopeSourceX));

		const int sourceX = static_cast<int>(std::ceil(slopeSourceX - 0.5f));
		const int targetX = static_cast<int>(std::ceil(slopeTargetX - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const UnclampedColour colour(colourSlopeSourceX + colourSlope * (static_cast<float>(x) + 0.5f - sourceX));
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

	const Vector3 n0(w0.GetVertexData().GetNormal());
	const Vector3 n1(w1.GetVertexData().GetNormal());
	const Vector3 n2(w2.GetVertexData().GetNormal());

	const Vector3 p0(w0);
	const Vector3 p1(w1);
	const Vector3 p2(w2);

	// Here, UVs will be converted to a Vector3 structure to simplify mathematics
	const Vector3 u0(v0.GetVertexData().GetUV().X, v0.GetVertexData().GetUV().Y, 0);
	const Vector3 u1(v1.GetVertexData().GetUV().X, v1.GetVertexData().GetUV().Y, 0);
	const Vector3 u2(v2.GetVertexData().GetUV().X, v2.GetVertexData().GetUV().Y, 0);

	const float ba = (v1.GetY() - v0.GetY()); // Get the change along edge v2->v1
	const float ca = (v2.GetY() - v0.GetY()); // Get the change along edge v3->v1

	const float slopeSourceY = (v1.GetX() - v0.GetX()) / ba;
	const float slopeTargetY = (v2.GetX() - v0.GetX()) / ca;

	const Vector3 normalSlopeSourceY((n1 - n0) / ba);
	const Vector3 normalSlopeTargetY((n2 - n0) / ca);

	const Vector3 worldSlopeSourceY((p1 - p0) / ba);
	const Vector3 worldSlopeTargetY((p2 - p0) / ca);

	const Vector3 texcoordSlopeSourceY((u2 - u0) / ba);
	const Vector3 texcoordSlopeTargetY((u2 - u1) / ca);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const float slopeSourceX = slopeSourceY * ((float)y + 0.5f - v0.GetY()) + v0.GetX();
		const float slopeTargetX = slopeTargetY * ((float)y + 0.5f - v0.GetY()) + v0.GetX();

		const Vector3 normalSlopeSourceX(normalSlopeSourceY * ((float)y + 0.5f - v0.GetY()) + n0);
		const Vector3 normalSlopeTargetX(normalSlopeTargetY * ((float)y + 0.5f - v0.GetY()) + n0);
		const Vector3 normalSlope((normalSlopeTargetX - normalSlopeSourceX) / (slopeTargetX - slopeSourceX));

		const Vector3 worldSlopeSourceX(worldSlopeSourceY * ((float)y + 0.5f - v0.GetY()) + p0);
		const Vector3 worldSlopeTargetX(worldSlopeTargetY * ((float)y + 0.5f - v0.GetY()) + p0);
		const Vector3 worldSlope((worldSlopeTargetX - worldSlopeSourceX) / (slopeTargetX - slopeSourceX));
		
		const Vector3 texcoordSlopeSourceX(texcoordSlopeSourceY * ((float)y + 0.5f - v0.GetY()) + u0);
		const Vector3 texcoordSlopeTargetX(texcoordSlopeTargetY * ((float)y + 0.5f - v0.GetY()) + u0);
		const Vector3 texcoordSlope((texcoordSlopeTargetX - texcoordSlopeSourceX) / (slopeTargetX - slopeSourceX));

		const int sourceX = static_cast<int>(std::ceil(slopeSourceX - 0.5f));
		const int targetX = static_cast<int>(std::ceil(slopeTargetX - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const Vector3 normal(Vector3::NormaliseVector(normalSlopeSourceX + normalSlope * (static_cast<float>(x) + 0.5f - sourceX)));
			const Vector3 uvVec3(texcoordSlopeSourceX + texcoordSlope * (static_cast<float>(x) + 0.5f - sourceX));

			const Point<float> uv{ uvVec3.GetX(), uvVec3.GetY() };

			Vertex fragment(worldSlopeSourceX + worldSlope * (static_cast<float>(x) + 0.5f - sourceX));
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

	const Vector3 n0(w0.GetVertexData().GetNormal());
	const Vector3 n1(w1.GetVertexData().GetNormal());
	const Vector3 n2(w2.GetVertexData().GetNormal());

	const Vector3 p0(w0);
	const Vector3 p1(w1);
	const Vector3 p2(w2);

	// Here, UVs will be converted to a Vector3 structure to simplify mathematics
	const Vector3 u0(v0.GetVertexData().GetUV().X, v0.GetVertexData().GetUV().Y, 0);
	const Vector3 u1(v1.GetVertexData().GetUV().X, v1.GetVertexData().GetUV().Y, 0);
	const Vector3 u2(v2.GetVertexData().GetUV().X, v2.GetVertexData().GetUV().Y, 0);

	const float ca = (v2.GetY() - v0.GetY()); // Get the change along edge v3->v1
	const float cb = (v2.GetY() - v1.GetY()); // Get the change along edge v3->v2

	const float slopeSourceY = (v2.GetX() - v0.GetX()) / ca;
	const float slopeTargetY = (v2.GetX() - v1.GetX()) / cb;

	const Vector3 normalSlopeSourceY((n2 - n0) / ca);
	const Vector3 normalSlopeTargetY((n2 - n1) / cb);

	const Vector3 worldSlopeSourceY((p2 - p0) / ca);
	const Vector3 worldSlopeTargetY((p2 - p1) / cb);

	const Vector3 texcoordSlopeSourceY((u2 - u0) / ca);
	const Vector3 texcoordSlopeTargetY((u2 - u1) / cb);

	const int sourceY = static_cast<int>(std::ceil(v0.GetY() - 0.5f));
	const int targetY = static_cast<int>(std::ceil(v2.GetY() - 0.5f));

	for (int y = sourceY; y < targetY; ++y)
	{
		const float slopeSourceX = slopeSourceY * ((float)y + 0.5f - v2.GetY()) + v2.GetX();
		const float slopeTargetX = slopeTargetY * ((float)y + 0.5f - v2.GetY()) + v2.GetX();

		const Vector3 normalSlopeSourceX(normalSlopeSourceY * ((float)y + 0.5f - v2.GetY()) + n2);
		const Vector3 normalSlopeTargetX(normalSlopeTargetY * ((float)y + 0.5f - v2.GetY()) + n2);
		const Vector3 normalSlope((normalSlopeTargetX - normalSlopeSourceX) / (slopeTargetX - slopeSourceX));

		const Vector3 worldSlopeSourceX(worldSlopeSourceY * ((float)y + 0.5f - v2.GetY()) + p2);
		const Vector3 worldSlopeTargetX(worldSlopeTargetY * ((float)y + 0.5f - v2.GetY()) + p2);
		const Vector3 worldSlope((worldSlopeTargetX - worldSlopeSourceX) / (slopeTargetX - slopeSourceX));

		const Vector3 texcoordSlopeSourceX(texcoordSlopeSourceY * ((float)y + 0.5f - v2.GetY()) + u2);
		const Vector3 texcoordSlopeTargetX(texcoordSlopeTargetY * ((float)y + 0.5f - v2.GetY()) + u2);
		const Vector3 texcoordSlope((texcoordSlopeTargetX - texcoordSlopeSourceX) / (slopeTargetX - slopeSourceX));

		const int sourceX = static_cast<int>(std::ceil(slopeSourceX - 0.5f));
		const int targetX = static_cast<int>(std::ceil(slopeTargetX - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const Vector3 normal(Vector3::NormaliseVector(normalSlopeSourceX + normalSlope * (static_cast<float>(x) + 0.5f - sourceX)));
			const Vector3 uvVec3(texcoordSlopeSourceX + texcoordSlope * (static_cast<float>(x) + 0.5f - sourceX));

			const Point<float> uv{ uvVec3.GetX(), uvVec3.GetY() };

			Vertex fragment(worldSlopeSourceX + worldSlope * (static_cast<float>(x) + 0.5f - sourceX));
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
const void TriangleRasteriser::GetTemporaryUV(Vertex& tmp, const Vertices& clipSpace)
{
	Point<float> uv0 = clipSpace.a.GetVertexData().GetUV();
	Point<float> uv2 = clipSpace.b.GetVertexData().GetUV();

	float uvX = uv0.X + (uv2.X - uv0.X) * ((clipSpace.b.GetY() - clipSpace.a.GetY()) / (clipSpace.c.GetY() - clipSpace.a.GetY()));
	float uvY = uv0.Y + (uv2.Y - uv0.Y) * ((clipSpace.b.GetY() - clipSpace.a.GetY()) / (clipSpace.c.GetY() - clipSpace.a.GetY()));

	tmp.GetVertexData().SetUV({ uvX, uvY });
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
