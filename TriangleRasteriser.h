#pragma once
#include <Windows.h>
#include <vector>
#include <functional>
#include "Polygon3D.h"

//
// Represents a fragment function handler.
//
struct FragmentFunction
{
	virtual const Colour operator()(const Vertex& v) const = 0;
};

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

	//
	// Raw modifiable polygon data structure.
	//
	struct Vertices
	{
		Vertex& a;
		Vertex& b;
		Vertex& c;
	};

public:
	//
	// Drawing handlers.
	//
	static void DrawFlat(const HDC& hdc, const PolygonData& clipSpace);
	static void DrawSmooth(const HDC& hdc, const PolygonData& clipSpace);
	static void DrawPhong(const HDC& hdc, const PolygonData& clipSpace, const PolygonData& worldSpace, const FragmentFunction& frag);

private:
	//
	// Sorts vertices by their Y value, so that A has the smallest
	// Y value, B has one that is always less than C and more than A,
	// and C has the largest Y value.
	//
	static void SortVertices(Vertex& a, Vertex& b, Vertex& c);
	static void SortVertices(Vertices& clipSpace, Vertices& worldSpace);

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
	// Phong shading callbacks.
	//
	static void TopPhongShaded(const HDC& hdc, const Vertices& clipSpace, const Vertices& worldSpace, const FragmentFunction& frag);
	static void BottomPhongShaded(const HDC& hdc, const Vertices& clipSpace, const Vertices& worldSpace, const FragmentFunction& frag);

	//
	// Splitting systems.
	//
	static const Vertex GetTemporaryVertex(const Vertex& a, const Vertex& b, const Vertex& c);
	static const Colour GetTemporaryColour(const Vertex& a, const Vertex& b, const Vertex& c);
	static const Vector3 GetTemporaryNormal(const Vertex& a, const Vertex& b, const Vertex& c);
	static const Vertex GetTemporaryWorldVertex(const Vertices& clipSpace, const Vertices& worldSpace);
	static const Point<float> GetTemporaryUV(const Vertex& a, const Vertex& b, const Vertex& c);
};

