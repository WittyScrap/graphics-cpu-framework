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
		Vertex tempVertex = Vertex::Lerp(a, c, 0.5f);

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
		// general case - split the triangle in a topflat and bottom-flat one
		Vertex tempVertex = Vertex::Lerp(a, c, 0.5f);

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
	const Vertex& v1 = clipSpace.a;
	const Vertex& v2 = clipSpace.b;
	const Vertex& v3 = clipSpace.c;

	float slope1 = (v2.GetX() - v1.GetX()) / (v2.GetY() - v1.GetY());
	float slope2 = (v3.GetX() - v1.GetX()) / (v3.GetY() - v1.GetY());

	float x1 = v1.GetX();
	float x2 = x1 + 0.5f;

	for (float scanlineY = v1.GetY(); scanlineY <= v2.GetY(); scanlineY++)
	{
		MoveToEx(hdc, static_cast<int>(std::floorf(x1)), static_cast<int>(std::ceilf(scanlineY)), NULL);
		LineTo(hdc, static_cast<int>(std::ceilf(x2)), static_cast<int>(std::ceilf(scanlineY)));

		x1 += slope1;
		x2 += slope2;
	}
}

//
// Fills a triangle whose top side is perfectly horizontal.
// Precondition is that v1 and v2 perform the flat side and 
// that v3.y > v1.y, v2.y.
//
void TriangleRasteriser::BottomFlatShaded(const HDC& hdc, const PolygonData& clipSpace)
{
	const Vertex& v1 = clipSpace.a;
	const Vertex& v2 = clipSpace.b;
	const Vertex& v3 = clipSpace.c;

	float slope1 = (v3.GetX() - v1.GetX()) / (v3.GetY() - v1.GetY());
	float slope2 = (v3.GetX() - v2.GetX()) / (v3.GetY() - v2.GetY());

	float x1 = v3.GetX();
	float x2 = x1 + 0.5f;

	for (float scanlineY = v3.GetY(); scanlineY >= v1.GetY(); scanlineY--)
	{
		MoveToEx(hdc, static_cast<int>(std::floorf(x1)), static_cast<int>(std::floorf(scanlineY)), NULL);
		LineTo(hdc, static_cast<int>(std::ceilf(x2)), static_cast<int>(std::floorf(scanlineY)));

		x1 -= slope1;
		x2 -= slope2;
	}
}

//
// Fills a triangle whose bottom side is perfectly horizontal.
// Precondition is that v2 and v3 perform the flat side and 
// that v1.y < v2.y, v3.y.
//
void TriangleRasteriser::TopSmoothShaded(const HDC& hdc, const PolygonData& clipSpace)
{
	const Vertex& v1 = clipSpace.a;
	const Vertex& v2 = clipSpace.b;
	const Vertex& v3 = clipSpace.c;

	const Colour& c1 = v1.GetVertexData().GetColour();
	const Colour& c2 = v2.GetVertexData().GetColour();
	const Colour& c3 = v3.GetVertexData().GetColour();

	float slope1 = (v2.GetX() - v1.GetX()) / (v2.GetY() - v1.GetY());
	float slope2 = (v3.GetX() - v1.GetX()) / (v3.GetY() - v1.GetY());

	float x1 = v1.GetX();
	float x2 = x1 + 0.5f;

	float ba = (v2.GetY() - v1.GetY()); // Get the change of colour components along edge v2->v1
	float ca = (v3.GetY() - v1.GetY()); // Get the change of colour components along edge v3->v1

	UnclampedColour sourceSlope((c2.GetRed() - c1.GetRed()) / ba, (c2.GetGreen() - c1.GetGreen()) / ba, (c2.GetBlue() - c1.GetBlue()) / ba);
	UnclampedColour targetSlope((c3.GetRed() - c1.GetRed()) / ca, (c3.GetGreen() - c1.GetGreen()) / ca, (c3.GetBlue() - c1.GetBlue()) / ca);

	// Get starting values
	UnclampedColour source(c1);
	UnclampedColour target(source);

	// Interpolator
	UnclampedColour interpolator;

	/* As we will do not fill in a complete line using MoveToEx/LineTo but instead
	 * we will loop over all pixels of a horizontal line, we need a predefined
	 * direction -> choose left to right. This means that x1 must be the smaller
	 * compared to x2 so slope1 must be smaller than slope2. If they are not we will
	 * swap them here. Of course we have also to swap the colors of both line endpoints.
	 */
	if (slope2 < slope1)
	{
		std::swap(slope1, slope2);
		std::swap(sourceSlope, targetSlope);
	}

	for (float scanlineY = v1.GetY(); scanlineY <= v2.GetY(); scanlineY++)
	{
		// Loop over each pixel in the horizontal line.
		for (float xPos = std::floorf(x1); xPos <= std::ceilf(x2); xPos++)
		{
			float t = (xPos - x1) / (x2 - x1);
			interpolator = source * (1 - t) + target * t;

			COLORREF pixelColour = RGB(interpolator.GetRed() * 255, interpolator.GetGreen() * 255, interpolator.GetBlue() * 255);

			SetPixelV(hdc, static_cast<int>(std::floorf(xPos)), static_cast<int>(std::ceilf(scanlineY)), pixelColour);
		}

		// Get new x-coordinate of endpoints of horizontal line.
		x1 += slope1;
		x2 += slope2;

		source += sourceSlope;
		target += targetSlope;
	}
}

//
// Fills a triangle whose top side is perfectly horizontal.
// Precondition is that v1 and v2 perform the flat side and 
// that v3.y > v1.y, v2.y.
//
void TriangleRasteriser::BottomSmoothShaded(const HDC& hdc, const PolygonData& clipSpace)
{
	const Vertex& v1 = clipSpace.a;
	const Vertex& v2 = clipSpace.b;
	const Vertex& v3 = clipSpace.c;

	const Colour& c1 = v1.GetVertexData().GetColour();
	const Colour& c2 = v2.GetVertexData().GetColour();
	const Colour& c3 = v3.GetVertexData().GetColour();

	float slope1 = (v3.GetX() - v1.GetX()) / (v3.GetY() - v1.GetY());
	float slope2 = (v3.GetX() - v2.GetX()) / (v3.GetY() - v2.GetY());

	float x1 = v3.GetX();
	float x2 = x1 + 0.5f;

	float ca = (v3.GetY() - v1.GetY()); // Get the change of colour components along edge v2->v1
	float cb = (v3.GetY() - v2.GetY()); // Get the change of colour components along edge v3->v1

	UnclampedColour sourceSlope((c3.GetRed() - c1.GetRed()) / ca, (c3.GetGreen() - c1.GetGreen()) / ca, (c3.GetBlue() - c1.GetBlue()) / ca);
	UnclampedColour targetSlope((c3.GetRed() - c2.GetRed()) / cb, (c3.GetGreen() - c2.GetGreen()) / cb, (c3.GetBlue() - c2.GetBlue()) / cb);

	// Get starting values
	UnclampedColour source(c3);
	UnclampedColour target(source);

	// Interpolator
	UnclampedColour interpolator;

	/* As we will do not fill in a complete line using MoveToEx/LineTo but instead
	 * we will loop over all pixels of a horizontal line, we need a predefined
	 * direction -> choose left to right. This means that x1 must be the smaller
	 * compared to x2 so slope1 must be smaller than slope2. If they are not we will
	 * swap them here. Of course we have also to swap the colors of both line endpoints.
	 */
	if (slope1 < slope2)
	{
		std::swap(slope1, slope2);
		std::swap(sourceSlope, targetSlope);
	}

	for (float scanlineY = v3.GetY(); scanlineY >= v1.GetY(); scanlineY--)
	{
		// Loop over each pixel in the horizontal line.
		for (float xPos = std::floorf(x1); xPos <= std::ceilf(x2); xPos++)
		{
			float t = (xPos - x1) / (x2 - x1);
			interpolator = source * (1 - t) + target * t;

			COLORREF pixelColour = RGB(interpolator.GetRed() * 255, interpolator.GetGreen() * 255, interpolator.GetBlue() * 255);

			SetPixelV(hdc, static_cast<int>(std::ceilf(xPos)), static_cast<int>(std::floorf(scanlineY)), pixelColour);
		}

		// Get new x-coordinate of endpoints of horizontal line.
		x1 -= slope1;
		x2 -= slope2;

		source -= sourceSlope;
		target -= targetSlope;
	}
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