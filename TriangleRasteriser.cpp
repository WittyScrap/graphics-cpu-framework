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

	/* here we know that v1.y <= v2.y <= v3.y
	 * check for trivial case of bottom-flat triangle
	 * or that of a top-flat triangle. */
	if (b.GetY() == c.GetY())
	{
		TopFlatShaded(hdc, a, b, c);
	}
	else if (a.GetY() == b.GetY())
	{
		BottomFlatShaded(hdc, a, b, c);
	}
	else
	{
		// general case - split the triangle in a topflat and bottom-flat one
		Vertex tempVertex = Vertex::Lerp(a, c, 0.5f);

		TopFlatShaded(hdc, a, b, tempVertex);
		BottomFlatShaded(hdc, b, tempVertex, c);
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
void TriangleRasteriser::TopFlatShaded(const HDC& hdc, const Vertex& vertexA, const Vertex& vertexB, const Vertex& vertexC)
{
	float slope1 = (vertexB.GetX() - vertexA.GetX()) / (vertexB.GetY() - vertexA.GetY());
	float slope2 = (vertexC.GetX() - vertexA.GetX()) / (vertexC.GetY() - vertexA.GetY());

	float x1 = vertexA.GetX();
	float x2 = x1 + 0.5f;

	for (float scanlineY = vertexA.GetY(); scanlineY <= vertexB.GetY(); scanlineY++)
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
void TriangleRasteriser::BottomFlatShaded(const HDC& hdc, const Vertex& vertexA, const Vertex& vertexB, const Vertex& vertexC)
{
	float slope1 = (vertexC.GetX() - vertexA.GetX()) / (vertexC.GetY() - vertexA.GetY());
	float slope2 = (vertexC.GetX() - vertexB.GetX()) / (vertexC.GetY() - vertexB.GetY());

	float x1 = vertexC.GetX();
	float x2 = x1 + 0.5f;

	for (float scanlineY = vertexC.GetY(); scanlineY >= vertexA.GetY(); scanlineY--)
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

	// Get the change of colour components along edge v2->v1
	float baDifference = (v2.GetY() - v1.GetY());

	// Colour
	float colourSlopeRed1 = (c2.GetRed() - c1.GetRed()) / baDifference;
	float colourSlopeGreen1 = (c2.GetGreen() - c1.GetGreen()) / baDifference;
	float colourSlopeBlue1 = (c2.GetBlue() - c1.GetBlue()) / baDifference;

	// Get the change of colour components along edge v3->v1
	float caDifference = (v3.GetY() - v1.GetY());

	// Colour
	float colourSlopeRed2 = (c3.GetRed() - c1.GetRed()) / caDifference;
	float colourSlopeGreen2 = (c3.GetGreen() - c1.GetGreen()) / caDifference;
	float colourSlopeBlue2 = (c3.GetBlue() - c1.GetBlue()) / caDifference;

	// Get starting values
	float colourRed1 = c1.GetRed();
	float colourGreen1 = c1.GetGreen();
	float colourBlue1 = c1.GetBlue();

	float colourRed2 = colourRed1;
	float colourGreen2 = colourGreen1;
	float colourBlue2 = colourBlue1;

	/* As we will do not fill in a complete line using MoveToEx/LineTo but instead
	 * we will loop over all pixels of a horizontal line, we need a predefined
	 * direction -> choose left to right. This means that x1 must be the smaller
	 * compared to x2 so slope1 must be smaller than slope2. If they are not we will
	 * swap them here. Of course we have also to swap the colors of both line endpoints.
	 */
	if (slope2 < slope1)
	{
		std::swap(slope1, slope2);
		std::swap(colourSlopeRed1, colourSlopeRed2);
		std::swap(colourSlopeGreen1, colourSlopeGreen2);
		std::swap(colourSlopeBlue1, colourSlopeBlue2);
	}

	for (float scanlineY = v1.GetY(); scanlineY <= v2.GetY(); scanlineY++)
	{
		// Loop over each pixel in the horizontal line.
		for (float xPos = std::floorf(x1); xPos <= std::ceilf(x2); xPos++)
		{
			float t = (xPos - x1) / (x2 - x1);

			float red = (1 - t) * colourRed1 + t * colourRed2;
			float green = (1 - t) * colourGreen1 + t * colourGreen2;
			float blue = (1 - t) * colourBlue1 + t * colourBlue2;

			SetPixelV(hdc, static_cast<int>(std::floorf(xPos)), static_cast<int>(std::ceilf(scanlineY)), RGB(red * 255, green * 255, blue * 255));
		}

		// Get new x-coordinate of endpoints of horizontal line.
		x1 += slope1;
		x2 += slope2;

		// Get new colour of left endpoint of horizontal line.
		colourRed1 += colourSlopeRed1;
		colourGreen1 += colourSlopeGreen1;
		colourBlue1 += colourSlopeBlue1;

		// Get new colour of right endpoint of horizontal line.
		colourRed2 += colourSlopeRed2;
		colourGreen2 += colourSlopeGreen2;
		colourBlue2 += colourSlopeBlue2;
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

	// Get the change of colour components along edge v3->v1
	float caDifference = (v3.GetY() - v1.GetY());

	// Colour
	float colourSlopeRed1 = (c3.GetRed() - c1.GetRed()) / caDifference;
	float colourSlopeGreen1 = (c3.GetGreen() - c1.GetGreen()) / caDifference;
	float colourSlopeBlue1 = (c3.GetBlue() - c1.GetBlue()) / caDifference;

	// Get the change of colour components along edge v3->v2
	float cbDifference = (v3.GetY() - v2.GetY());

	// Colour
	float colourSlopeRed2 = (c3.GetRed() - c2.GetRed()) / cbDifference;
	float colourSlopeGreen2 = (c3.GetGreen() - c2.GetGreen()) / cbDifference;
	float colourSlopeBlue2 = (c3.GetBlue() - c2.GetBlue()) / cbDifference;

	// Get starting values
	float colourRed1 = c3.GetRed();
	float colourGreen1 = c3.GetGreen();
	float colourBlue1 = c3.GetBlue();

	float colourRed2 = colourRed1;
	float colourGreen2 = colourGreen1;
	float colourBlue2 = colourBlue1;

	/* As we will do not fill in a complete line using MoveToEx/LineTo but instead
	 * we will loop over all pixels of a horizontal line, we need a predefined
	 * direction -> choose left to right. This means that x1 must be the smaller
	 * compared to x2 so slope1 must be smaller than slope2. If they are not we will
	 * swap them here. Of course we have also to swap the colors of both line endpoints.
	 */
	if (slope1 < slope2)
	{
		std::swap(slope1, slope2);
		std::swap(colourSlopeRed1, colourSlopeRed2);
		std::swap(colourSlopeGreen1, colourSlopeGreen2);
		std::swap(colourSlopeBlue1, colourSlopeBlue2);
	}

	for (float scanlineY = v3.GetY(); scanlineY >= v1.GetY(); scanlineY--)
	{
		// Loop over each pixel in the horizontal line.
		for (float xPos = std::floorf(x1); xPos <= std::ceilf(x2); xPos++)
		{
			float t = (xPos - x1) / (x2 - x1);

			float red = (1 - t) * colourRed1 + t * colourRed2;
			float green = (1 - t) * colourGreen1 + t * colourGreen2;
			float blue = (1 - t) * colourBlue1 + t * colourBlue2;

			SetPixelV(hdc, static_cast<int>(std::ceilf(xPos)), static_cast<int>(std::floorf(scanlineY)), RGB(red * 255, green * 255, blue * 255));
		}

		// Get new x-coordinate of endpoints of horizontal line.
		x1 -= slope1;
		x2 -= slope2;

		// Get new colour of left endpoint of horizontal line.
		colourRed1 -= colourSlopeRed1;
		colourGreen1 -= colourSlopeGreen1;
		colourBlue1 -= colourSlopeBlue1;

		// Get new colour of right endpoint of horizontal line.
		colourRed2 -= colourSlopeRed2;
		colourGreen2 -= colourSlopeGreen2;
		colourBlue2 -= colourSlopeBlue2;
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