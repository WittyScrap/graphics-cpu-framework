#pragma once
#include <Windows.h>
#include <vector>
#include <functional>
#include "Polygon3D.h"

//
// Fragment function alias.
//
typedef std::function<Colour(const Vertex&)> FragmentFunction;

//
// Rasterises a triangle using the standard solid rasterisation
// technique.
//
class TriangleRasteriser
{
	//
	// Raw polygon data structure.
	//
	struct PolygonData
	{
		const Vertex& a;
		const Vertex& b;
		const Vertex& c;
	};

public:
	//
	// Rasterises a triangle using the standard solid rasterisation
	// technique.
	//
	static void DrawFlat(const HDC& hdc, const PolygonData& clipSpace);

	//
	// Rasterises a triangle using the standard solid rasterisation
	// technique, shading on a fragment-by-fragment basis.
	//
	static void DrawSmooth(const HDC& hdc, const PolygonData& clipSpace);

private:
	//
	// Sorts vertices by their Y value, so that A has the smallest
	// Y value, B has one that is always less than C and more than A,
	// and C has the largest Y value.
	//
	static void SortVertices(Vertex& a, Vertex& b, Vertex& c);

	//
	// Fills a triangle whose bottom side is perfectly horizontal.
	// Precondition is that v2 and v3 perform the flat side and 
	// that v1.y < v2.y, v3.y.
	//
	static void BottomFlat(const HDC& hdc, const Vertex& a, const Vertex& b, const Vertex& c);
	
	//
	// Fills a triangle whose top side is perfectly horizontal.
	// Precondition is that v1 and v2 perform the flat side and 
	// that v3.y > v1.y, v2.y.
	//
	static void TopFlat(const HDC& hdc, const Vertex& a, const Vertex& b, const Vertex& c);

	//
	// Fills a triangle whose bottom side is perfectly horizontal.
	// Precondition is that v2 and v3 perform the flat side and 
	// that v1.y < v2.y, v3.y.
	//
	static void BottomSmooth(const HDC& hdc, const PolygonData& clipSpace);

	//
	// Fills a triangle whose top side is perfectly horizontal.
	// Precondition is that v1 and v2 perform the flat side and 
	// that v3.y > v1.y, v2.y.
	//
	static void TopSmooth(const HDC& hdc, const PolygonData& clipSpace);

};

