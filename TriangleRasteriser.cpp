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
		BottomFlat(hdc, a, b, c);
	}
	else if (a.GetY() == b.GetY())
	{
		TopFlat(hdc, a, b, c);
	}
	else
	{
		// general case - split the triangle in a topflat and bottom-flat one
		float middleX = a.GetX() + (static_cast<float>(b.GetY() - a.GetY()) / static_cast<float>(c.GetY() - a.GetY())) * (c.GetX() - a.GetX());
		float middleY = b.GetY();

		Vertex tempVertex(middleX, middleY, 0.f);

		BottomFlat(hdc, a, b, tempVertex);
		TopFlat(hdc, b, tempVertex, c);
	}
}

//
// Rasterises a triangle using the standard solid rasterisation
// technique, shading on a fragment-by-fragment basis.
//
void TriangleRasteriser::DrawSmooth(const HDC& hdc, const PolygonData& clipSpace, const FragmentFunction& frag)
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
		BottomSmooth(hdc, frag, data);
	}
	else if (a.GetY() == b.GetY())
	{
		TopSmooth(hdc, frag, data);
	}
	else
	{
		// general case - split the triangle in a topflat and bottom-flat one
		Vertex tempVertex = Vertex::Lerp(a, c, 0.5f);

		PolygonData topTriangle{ a, b, tempVertex };
		PolygonData bottomTriangle{ b, tempVertex, c };

		BottomSmooth(hdc, frag, topTriangle);
		TopSmooth(hdc, frag, bottomTriangle);
	}
}

//
// Fills a triangle whose bottom side is perfectly horizontal.
// Precondition is that v2 and v3 perform the flat side and 
// that v1.y < v2.y, v3.y.
//
void TriangleRasteriser::BottomFlat(const HDC& hdc, const Vertex& vertexA, const Vertex& vertexB, const Vertex& vertexC)
{
	float slope1 = (vertexB.GetX() - vertexA.GetX()) / (vertexB.GetY() - vertexA.GetY());
	float slope2 = (vertexC.GetX() - vertexA.GetX()) / (vertexC.GetY() - vertexA.GetY());

	float x1 = vertexA.GetX();
	float x2 = x1 + 0.5f;

	for (int scanlineY = static_cast<int>(vertexA.GetY()); scanlineY <= vertexB.GetY(); scanlineY++)
	{
		MoveToEx(hdc, static_cast<int>(x1), scanlineY, NULL);
		LineTo(hdc, static_cast<int>(x2), scanlineY);

		x1 += slope1;
		x2 += slope2;
	}
}

//
// Fills a triangle whose top side is perfectly horizontal.
// Precondition is that v1 and v2 perform the flat side and 
// that v3.y > v1.y, v2.y.
//
void TriangleRasteriser::TopFlat(const HDC& hdc, const Vertex& vertexA, const Vertex& vertexB, const Vertex& vertexC)
{
	float slope1 = (vertexC.GetX() - vertexA.GetX()) / (vertexC.GetY() - vertexA.GetY());
	float slope2 = (vertexC.GetX() - vertexB.GetX()) / (vertexC.GetY() - vertexB.GetY());

	float x1 = vertexC.GetX();
	float x2 = x1 + 0.5f;

	for (int scanlineY = static_cast<int>(vertexC.GetY()); scanlineY > vertexA.GetY(); scanlineY--)
	{
		MoveToEx(hdc, static_cast<int>(x1), scanlineY, NULL);
		LineTo(hdc, static_cast<int>(x2), scanlineY);

		x1 -= slope1;
		x2 -= slope2;
	}
}

//
// Fills a triangle whose bottom side is perfectly horizontal.
// Precondition is that v2 and v3 perform the flat side and 
// that v1.y < v2.y, v3.y.
//
void TriangleRasteriser::BottomSmooth(const HDC& hdc, const FragmentFunction& frag, const PolygonData& clipSpace)
{
	const Vertex& v1 = clipSpace.a;
	const Vertex& v2 = clipSpace.b;
	const Vertex& v3 = clipSpace.c;

	const Vector3& n1 = v1.GetVertexData().GetNormal();
	const Vector3& n2 = v2.GetVertexData().GetNormal();
	const Vector3& n3 = v3.GetVertexData().GetNormal();

	const Colour& c1 = v1.GetVertexData().GetColour();
	const Colour& c2 = v2.GetVertexData().GetColour();
	const Colour& c3 = v3.GetVertexData().GetColour();

	Vertex raster;

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

	// Normal
	float normalSlopeX1 = (n2.GetX() - n1.GetX()) / baDifference;
	float normalSlopeY1 = (n2.GetY() - n1.GetY()) / baDifference;
	float normalSlopeZ1 = (n2.GetZ() - n1.GetZ()) / baDifference;

	// Get the change of colour components along edge v3->v1
	float caDifference = (v3.GetY() - v1.GetY());

	// Colour
	float colourSlopeRed2 = (c3.GetRed() - c1.GetRed()) / caDifference;
	float colourSlopeGreen2 = (c3.GetGreen() - c1.GetGreen()) / caDifference;
	float colourSlopeBlue2 = (c3.GetBlue() - c1.GetBlue()) / caDifference;

	// Normal
	float normalSlopeX2 = (n3.GetX() - n1.GetX()) / caDifference;
	float normalSlopeY2 = (n3.GetY() - n1.GetY()) / caDifference;
	float normalSlopeZ2 = (n3.GetZ() - n1.GetZ()) / caDifference;

	// Get starting values
	float colourRed1 = c1.GetRed();
	float colourGreen1 = c1.GetGreen();
	float colourBlue1 = c1.GetBlue();

	float colourRed2 = colourRed1;
	float colourGreen2 = colourGreen1;
	float colourBlue2 = colourBlue1;

	float normalX1 = n1.GetX();
	float normalY1 = n1.GetY();
	float normalZ1 = n1.GetZ();

	float normalX2 = normalX1;
	float normalY2 = normalY1;
	float normalZ2 = normalZ1;

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

		std::swap(normalSlopeX1, normalSlopeX2);
		std::swap(normalSlopeY1, normalSlopeY2);
		std::swap(normalSlopeZ1, normalSlopeZ2);
	}

	for (int scanlineY = static_cast<int>(v1.GetY()); scanlineY <= v2.GetY(); scanlineY++)
	{
		// Loop over each pixel in the horizontal line.
		for (int xPos = static_cast<int>(std::ceilf(x1)); xPos < static_cast<int>(x2); xPos++)
		{
			float t = (xPos - x1) / (x2 - x1);

			float red = (1 - t) * colourRed1 + t * colourRed2;
			float green = (1 - t) * colourGreen1 + t * colourGreen2;
			float blue = (1 - t) * colourBlue1 + t * colourBlue2;

			float normalX = (1 - t) * normalX1 + t * normalX2;
			float normalY = (1 - t) * normalY1 + t * normalY2;
			float normalZ = (1 - t) * normalZ1 + t * normalZ2;

			raster.SetX(static_cast<float>(xPos));
			raster.SetY(static_cast<float>(scanlineY));
			raster.SetZ(0);
			raster.SetW(1);

			raster.GetVertexData().SetNormal({ normalX, normalY, normalZ });
			raster.GetVertexData().SetColour({ red, green, blue });

			Colour output = frag(raster);

			SetPixel(hdc, static_cast<int>(xPos), scanlineY, output.AsColor());
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

		// Get new normal of left endpoint of horizontal line.
		normalX1 += normalSlopeX1;
		normalY1 += normalSlopeY1;
		normalZ1 += normalSlopeZ1;

		// Get new normal of right endpoint of horizontal line.
		normalX2 += normalSlopeX2;
		normalY2 += normalSlopeY2;
		normalZ2 += normalSlopeZ2;
	}
}

//
// Fills a triangle whose top side is perfectly horizontal.
// Precondition is that v1 and v2 perform the flat side and 
// that v3.y > v1.y, v2.y.
//
void TriangleRasteriser::TopSmooth(const HDC& hdc, const FragmentFunction& frag, const PolygonData& clipSpace)
{
	const Vertex& v1 = clipSpace.a;
	const Vertex& v2 = clipSpace.b;
	const Vertex& v3 = clipSpace.c;

	const Vector3& n1 = v1.GetVertexData().GetNormal();
	const Vector3& n2 = v2.GetVertexData().GetNormal();
	const Vector3& n3 = v3.GetVertexData().GetNormal();

	const Colour& c1 = v1.GetVertexData().GetColour();
	const Colour& c2 = v2.GetVertexData().GetColour();
	const Colour& c3 = v3.GetVertexData().GetColour();

	Vertex raster;
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