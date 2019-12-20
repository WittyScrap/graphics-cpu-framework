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
// technique, shading on a fragment-by-fragment basis.
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

		const int sourceX = static_cast<int>(std::ceil(slopeSourceX - 0.5f));
		const int targetX = static_cast<int>(std::ceil(slopeTargetX - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const float t = (x - static_cast<float>(sourceX)) / (targetX - sourceX);
			const UnclampedColour colour(colourSlopeSourceX * (1 - t) + colourSlopeTargetX * t);
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

		const int sourceX = static_cast<int>(std::ceil(slopeSourceX - 0.5f));
		const int targetX = static_cast<int>(std::ceil(slopeTargetX - 0.5f));

		for (int x = sourceX; x < targetX; ++x)
		{
			const float t = (x - static_cast<float>(sourceX)) / (targetX - sourceX);
			const UnclampedColour colour(colourSlopeSourceX * (1 - t) + colourSlopeTargetX * t);
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
Vertex TriangleRasteriser::GetTemporaryVertex(const Vertex& a, const Vertex& b, const Vertex& c)
{
	// Split the triangle in a topflat and bottom - flat one
	Vertex tempVertex(a.GetX() + ((b.GetY() - a.GetY()) / (c.GetY() - a.GetY())) * (c.GetX() - a.GetX()), b.GetY(), 0);

	tempVertex.GetVertexData().SetColour(GetTemporaryColour(a, b, c));
//	tempVertex.GetVertexData().SetNormal(GetTemporaryNormal(a, b, c));

	return tempVertex;
}

//
// Given a set of three vertices, generates the temporary colour for the
// vertex that is generated between the A/B-C diagonal. The source colours
// are taken from the vertex data.
//
Colour TriangleRasteriser::GetTemporaryColour(const Vertex& a, const Vertex& b, const Vertex& c)
{
	const Colour& c0 = a.GetVertexData().GetColour();
	const Colour& c1 = b.GetVertexData().GetColour();
	const Colour& c2 = c.GetVertexData().GetColour();

	return c0 + (c2 - c0) * ((b.GetY() - a.GetY()) / (c.GetY() - a.GetY()));
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