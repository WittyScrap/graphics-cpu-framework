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
	// Drawing handlers.
	//
	static void DrawFlat(const HDC& hdc, const PolygonData& clipSpace);
	static void DrawSmooth(const HDC& hdc, const PolygonData& clipSpace);

private:
	//
	// Sorts vertices by their Y value, so that A has the smallest
	// Y value, B has one that is always less than C and more than A,
	// and C has the largest Y value.
	//
	static void SortVertices(Vertex& a, Vertex& b, Vertex& c);

	//
	// Flat shading callbacks.
	//
	static void TopFlatShaded(const HDC& hdc, const PolygonData& clipSpace);
	static void BottomFlatShaded(const HDC& hdc, const PolygonData& clipSpace);

	//
	// Smooth shading callbacks.
	//
	static void TopSmoothShaded(const HDC& hdc, const PolygonData& clipSpace);
	static void BottomSmoothShaded(const HDC& hdc, const PolygonData& clipSpace);

	//
	// Splitting systems.
	//
	static Vertex GetTemporaryVertex(const Vertex& a, const Vertex& b, const Vertex& c);
	static Colour GetTemporaryColour(const Vertex& a, const Vertex& b, const Vertex& c);
};

