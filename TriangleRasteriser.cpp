#include "TriangleRasteriser.h"
#include <cmath>
#include <Windows.h>

//
// Rasterises a triangle using the standard solid rasterisation
// technique.
//
void TriangleRasteriser::Draw(const HDC& hdc, const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace)
{
	// Make copies of the vertices (not references) as we'll need to modify them.
	Vertex a = clipSpace[polygon.GetIndex(0)];
	Vertex b = clipSpace[polygon.GetIndex(1)];
	Vertex c = clipSpace[polygon.GetIndex(2)];

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
// Ceils a floating point value and casts it into an
// integer.
//
int TriangleRasteriser::Ceil(const float& value)
{
	return static_cast<int>(ceilf(value));
}

//
// Floors a floating point value and casts it into an
// integer.
//
int TriangleRasteriser::Floor(const float& value)
{
	return static_cast<int>(floorf(value));
}
