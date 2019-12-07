#pragma once
#include <Windows.h>
#include <vector>
#include "Polygon3D.h"

//
// Rasterises a triangle using the standard solid rasterisation
// technique.
//
class TriangleRasteriser
{
public:
	//
	// Rasterises a triangle using the standard solid rasterisation
	// technique.
	//
	static void Draw(const HDC& hdc, const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace);

private:
	//
	// Fills a triangle whose bottom side is perfectly horizontal.
	// Precondition is that v2 and v3 perform the flat side and 
	// that v1.y < v2.y, v3.y.
	//
	static void BottomFlat(const HDC& hdc, const Vertex& vertexA, const Vertex& vertexB, const Vertex& vertexC);
	
	//
	// Fills a triangle whose top side is perfectly horizontal.
	// Precondition is that v1 and v2 perform the flat side and 
	// that v3.y > v1.y, v2.y.
	//
	static void TopFlat(const HDC& hdc, const Vertex& vertexA, const Vertex& vertexB, const Vertex& vertexC);

	//
	// Sorts vertices by their Y value, so that A has the smallest
	// Y value, B has one that is always less than C and more than A,
	// and C has the largest Y value.
	//
	static void SortVertices(Vertex& a, Vertex& b, Vertex& c);

	//
	// Ceils a floating point value and casts it into an
	// integer.
	//
	static int Ceil(const float& value);

	//
	// Floors a floating point value and casts it into an
	// integer.
	//
	static int Floor(const float& value);
};

