#pragma once
#include <Windows.h>
#include <vector>
#include <functional>
#include "Polygon3D.h"
#include "UnclampedColour.h"
#define ARTIM_TEMP template<typename TAritm>

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

	//
	// Data to be supplied for smooth shading.
	//
	struct SmoothShadeData
	{
		float sourceSlope{ 0 };
		float targetSlope{ 0 };

		UnclampedColour sourceColourSlope;
		UnclampedColour targetColourSlope;

		const PolygonData& clipSpace;
	};

	//
	// Data to be supplied for smooth shading on a per-line basis.
	//
	struct SmoothLineData
	{
		float sourceSlope{ 0 };
		float targetSlope{ 0 };

		UnclampedColour sourceColourSlope;
		UnclampedColour targetColourSlope;
		UnclampedColour horizontalColourSlope;
	};

	//
	// Input data to be provided for the vertical gradient calculations. 
	//
	struct PhongShadeInput
	{
		const PolygonData& clipSpace;
		const PolygonData& worldSpace;
	};

	//
	// Data to be supplied on a per-line basis for rendering polygons using phong shading.
	//
	struct PhongShadeData
	{
		float sourceSlope{ 0 };
		float targetSlope{ 0 };

		Vector3 sourceNormalSlope;
		Vector3 targetNormalSlope;

		Vector3 sourceWorldSlope;
		Vector3 targetWorldSlope;

		Vector3 sourceUVSlope;
		Vector3 targetUVSlope;

		const PolygonData& clipSpace;
		const PolygonData& worldSpace;
	};

	//
	// Data to be used for rendering polygons using phong shading, this is to be used per-pixel on a single line.
	//
	struct PhongLineData
	{
		float sourceSlope{ 0 };
		float targetSlope{ 0 };

		Vector3 sourceNormalSlope;
		Vector3 targetNormalSlope;
		Vector3 horizontalNormalSlope;

		Vector3 sourceWorldSlope;
		Vector3 targetWorldSlope;
		Vector3 horizontalWorldSlope;

		Vector3 sourceUVSlope;
		Vector3 targetUVSlope;
		Vector3 horizontalUVSlope;
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
	static const Vector3 GetTemporaryUV(const Vertex& a, const Vertex& b, const Vertex& c);

	//
	// General utilities
	//
	ARTIM_TEMP inline static TAritm GetHorizontalGradient(const TAritm& slope, const int& pos, const float& source, const TAritm& pt);
	ARTIM_TEMP inline static TAritm GetVerticalGradient(const TAritm& a, const TAritm& b, const float& den);
	inline static float GetSlope(const Vertex& a, const Vertex& b);

	inline static const SmoothShadeData GetSmoothDataTop(const PolygonData& clipSpace);
	inline static const SmoothShadeData GetSmoothDataBottom(const PolygonData& clipSpace);

	inline static const SmoothLineData GetSmoothLineTop(const SmoothShadeData& slopeData, const int& pos);
	inline static const SmoothLineData GetSmoothLineBottom(const SmoothShadeData& slopeData, const int& pos);

	inline static const PhongShadeData GetPhongDataTop(const PhongShadeInput& input);
	inline static const PhongShadeData GetPhongDataBottom(const PhongShadeInput& input);

	inline static const PhongLineData GetPhongLineTop(const PhongShadeData& slopeData, const int& pos);
	inline static const PhongLineData GetPhongLineBottom(const PhongShadeData& slopeData, const int& pos);
};

//
// Calculates the slope between the two vertices.
//
inline float TriangleRasteriser::GetSlope(const Vertex& a, const Vertex& b)
{
	return (b.GetX() - a.GetX()) / (b.GetY() - a.GetY());
}

//
// Calculates y-related smooth shade data for rasterising top triangles.
//
inline const TriangleRasteriser::SmoothShadeData TriangleRasteriser::GetSmoothDataTop(const PolygonData& clipSpace)
{
	const Vertex& v0(clipSpace.a);
	const Vertex& v1(clipSpace.b);
	const Vertex& v2(clipSpace.c);

	const UnclampedColour c0(v0.GetVertexData().GetColour());
	const UnclampedColour c1(v1.GetVertexData().GetColour());
	const UnclampedColour c2(v2.GetVertexData().GetColour());

	const float ba = (v1.GetY() - v0.GetY());
	const float ca = (v2.GetY() - v0.GetY());

	return 
	{
		GetSlope(v0, v1),
		GetSlope(v0, v2),
		GetVerticalGradient(c0, c1, ba),
		GetVerticalGradient(c0, c2, ca),
		clipSpace
	};
}

//
// Calculates y-related smooth shade data for rasterising bottom triangles.
//
inline const TriangleRasteriser::SmoothShadeData TriangleRasteriser::GetSmoothDataBottom(const PolygonData& clipSpace)
{
	const Vertex& v0(clipSpace.a);
	const Vertex& v1(clipSpace.b);
	const Vertex& v2(clipSpace.c);

	const UnclampedColour c0(v0.GetVertexData().GetColour());
	const UnclampedColour c1(v1.GetVertexData().GetColour());
	const UnclampedColour c2(v2.GetVertexData().GetColour());

	const float ca = (v2.GetY() - v0.GetY());
	const float cb = (v2.GetY() - v1.GetY());

	return
	{
		GetSlope(v0, v2),
		GetSlope(v1, v2),
		GetVerticalGradient(c0, c2, ca),
		GetVerticalGradient(c1, c2, cb),
		clipSpace
	};
}

//
// Calculates x-related smooth shade data for rasterising top triangles.
//
inline const TriangleRasteriser::SmoothLineData TriangleRasteriser::GetSmoothLineTop(const SmoothShadeData& slopeData, const int& pos)
{
	const Vertex& v0(slopeData.clipSpace.a);
	const UnclampedColour c0(v0.GetVertexData().GetColour());

	SmoothLineData data;
	
	data.sourceSlope = GetHorizontalGradient(slopeData.sourceSlope, pos, v0.GetY(), v0.GetX());
	data.targetSlope = GetHorizontalGradient(slopeData.targetSlope, pos, v0.GetY(), v0.GetX());
	data.sourceColourSlope = GetHorizontalGradient(slopeData.sourceColourSlope, pos, v0.GetY(), c0);
	data.targetColourSlope = GetHorizontalGradient(slopeData.targetColourSlope, pos, v0.GetY(), c0);
	data.horizontalColourSlope = (data.targetColourSlope - data.sourceColourSlope) / (data.targetSlope - data.sourceSlope);

	return data;
}

//
// Calculates x-related smooth shade data for rasterising bottom triangles.
//
inline const TriangleRasteriser::SmoothLineData TriangleRasteriser::GetSmoothLineBottom(const SmoothShadeData& slopeData, const int& pos)
{
	const Vertex& v2(slopeData.clipSpace.c);
	const UnclampedColour c2(v2.GetVertexData().GetColour());

	SmoothLineData data;

	data.sourceSlope = GetHorizontalGradient(slopeData.sourceSlope, pos, v2.GetY(), v2.GetX());
	data.targetSlope = GetHorizontalGradient(slopeData.targetSlope, pos, v2.GetY(), v2.GetX());
	data.sourceColourSlope = GetHorizontalGradient(slopeData.sourceColourSlope, pos, v2.GetY(), c2);
	data.targetColourSlope = GetHorizontalGradient(slopeData.targetColourSlope, pos, v2.GetY(), c2);
	data.horizontalColourSlope = (data.targetColourSlope - data.sourceColourSlope) / (data.targetSlope - data.sourceSlope);

	return data;
}

//
// Calculates y-related phong shade data for rasterising top triangles.
//
inline const TriangleRasteriser::PhongShadeData TriangleRasteriser::GetPhongDataTop(const PhongShadeInput& input)
{
	const Vertex& v0(input.clipSpace.a);
	const Vertex& v1(input.clipSpace.b);
	const Vertex& v2(input.clipSpace.c);

	const Vertex& w0(input.worldSpace.a);
	const Vertex& w1(input.worldSpace.b);
	const Vertex& w2(input.worldSpace.c);

	const Vector3 n0(w0.GetVertexData().GetNormal());
	const Vector3 n1(w1.GetVertexData().GetNormal());
	const Vector3 n2(w2.GetVertexData().GetNormal());

	const Vector3 p0(w0);
	const Vector3 p1(w1);
	const Vector3 p2(w2);

	// Here, UVs will be converted to a Vector3 structure to simplify mathematics
	Vector3 u0(v0.GetVertexData().GetUV());
	Vector3 u1(v1.GetVertexData().GetUV());
	Vector3 u2(v2.GetVertexData().GetUV());

	const float ba = (v1.GetY() - v0.GetY()); // Get the change along edge v2->v1
	const float ca = (v2.GetY() - v0.GetY()); // Get the change along edge v3->v1

	return
	{
		GetSlope(v0, v1),
		GetSlope(v0, v2),
		GetVerticalGradient(n0, n1, ba),
		GetVerticalGradient(n0, n2, ca),
		GetVerticalGradient(p0, p1, ba),
		GetVerticalGradient(p0, p2, ca),
		GetVerticalGradient(u0, u1, ba),
		GetVerticalGradient(u0, u2, ca),
		input.clipSpace,
		input.worldSpace
	};
}

//
// Calculates y-related phong shade data for rasterising bottom triangles.
//
inline const TriangleRasteriser::PhongShadeData TriangleRasteriser::GetPhongDataBottom(const PhongShadeInput& input)
{
	const Vertex& v0(input.clipSpace.a);
	const Vertex& v1(input.clipSpace.b);
	const Vertex& v2(input.clipSpace.c);

	const Vertex& w0(input.worldSpace.a);
	const Vertex& w1(input.worldSpace.b);
	const Vertex& w2(input.worldSpace.c);

	const Vector3 n0(w0.GetVertexData().GetNormal());
	const Vector3 n1(w1.GetVertexData().GetNormal());
	const Vector3 n2(w2.GetVertexData().GetNormal());

	const Vector3 p0(w0);
	const Vector3 p1(w1);
	const Vector3 p2(w2);

	Vector3 u0(v0.GetVertexData().GetUV());
	Vector3 u1(v1.GetVertexData().GetUV());
	Vector3 u2(v2.GetVertexData().GetUV());

	const float ca = (v2.GetY() - v0.GetY()); // Get the change along edge v2->v1
	const float cb = (v2.GetY() - v1.GetY()); // Get the change along edge v3->v1

	return
	{
		GetSlope(v0, v2),
		GetSlope(v1, v2),
		GetVerticalGradient(n0, n2, ca),
		GetVerticalGradient(n1, n2, cb),
		GetVerticalGradient(p0, p2, ca),
		GetVerticalGradient(p1, p2, cb),
		GetVerticalGradient(u0, u2, ca),
		GetVerticalGradient(u1, u2, cb),
		input.clipSpace,
		input.worldSpace
	};
}

//
// Calculates x-related phong shade data for rasterising top triangles.
//
inline const TriangleRasteriser::PhongLineData TriangleRasteriser::GetPhongLineTop(const PhongShadeData& slopeData, const int& pos)
{
	const Vertex& v0(slopeData.clipSpace.a);
	const Vertex& w0(slopeData.worldSpace.a);
	const Vector3& n0(w0.GetVertexData().GetNormal());
	const Vector3& u0(v0.GetVertexData().GetUV());

	PhongLineData data;

	data.sourceSlope = GetHorizontalGradient(slopeData.sourceSlope, pos, v0.GetY(), v0.GetX());
	data.targetSlope = GetHorizontalGradient(slopeData.targetSlope, pos, v0.GetY(), v0.GetX());

	data.sourceNormalSlope = GetHorizontalGradient(slopeData.sourceNormalSlope, pos, v0.GetY(), n0);
	data.targetNormalSlope = GetHorizontalGradient(slopeData.targetNormalSlope, pos, v0.GetY(), n0);
	data.horizontalNormalSlope = (data.targetNormalSlope - data.sourceNormalSlope) / (data.targetSlope - data.sourceSlope);

	data.sourceWorldSlope = GetHorizontalGradient<Vector3>(slopeData.sourceWorldSlope, pos, v0.GetY(), w0);
	data.targetWorldSlope = GetHorizontalGradient<Vector3>(slopeData.targetWorldSlope, pos, v0.GetY(), w0);
	data.horizontalWorldSlope = (data.targetWorldSlope - data.sourceWorldSlope) / (data.targetSlope - data.sourceSlope);

	data.sourceUVSlope = GetHorizontalGradient(slopeData.sourceUVSlope, pos, v0.GetY(), u0);
	data.targetUVSlope = GetHorizontalGradient(slopeData.targetUVSlope, pos, v0.GetY(), u0);
	data.horizontalUVSlope = (data.targetUVSlope - data.sourceUVSlope) / (data.targetSlope - data.sourceSlope);

	return data;
}

//
// Calculates x-related phong shade data for rasterising bottom triangles.
//s
inline const TriangleRasteriser::PhongLineData TriangleRasteriser::GetPhongLineBottom(const PhongShadeData& slopeData, const int& pos)
{
	const Vertex& v2(slopeData.clipSpace.c);
	const Vertex& w2(slopeData.worldSpace.c);
	const Vector3& n2(w2.GetVertexData().GetNormal());
	const Vector3& u2(v2.GetVertexData().GetUV());

	PhongLineData data;

	data.sourceSlope = GetHorizontalGradient(slopeData.sourceSlope, pos, v2.GetY(), v2.GetX());
	data.targetSlope = GetHorizontalGradient(slopeData.targetSlope, pos, v2.GetY(), v2.GetX());

	data.sourceNormalSlope = GetHorizontalGradient(slopeData.sourceNormalSlope, pos, v2.GetY(), n2);
	data.targetNormalSlope = GetHorizontalGradient(slopeData.targetNormalSlope, pos, v2.GetY(), n2);
	data.horizontalNormalSlope = (data.targetNormalSlope - data.sourceNormalSlope) / (data.targetSlope - data.sourceSlope);

	data.sourceWorldSlope = GetHorizontalGradient<Vector3>(slopeData.sourceWorldSlope, pos, v2.GetY(), w2);
	data.targetWorldSlope = GetHorizontalGradient<Vector3>(slopeData.targetWorldSlope, pos, v2.GetY(), w2);
	data.horizontalWorldSlope = (data.targetWorldSlope - data.sourceWorldSlope) / (data.targetSlope - data.sourceSlope);

	data.sourceUVSlope = GetHorizontalGradient(slopeData.sourceUVSlope, pos, v2.GetY(), u2);
	data.targetUVSlope = GetHorizontalGradient(slopeData.targetUVSlope, pos, v2.GetY(), u2);
	data.horizontalUVSlope = (data.targetUVSlope - data.sourceUVSlope) / (data.targetSlope - data.sourceSlope);

	return data;
}

//
// Calculates a horizontal gradient from any arithmetic type.
//
ARTIM_TEMP inline TAritm TriangleRasteriser::GetHorizontalGradient(const TAritm& slope, const int& pos, const float& source, const TAritm& pt)
{
	return slope * (static_cast<float>(pos) + .5f - source) + pt;
}

//
// Calculates a vertical gradient from any arithmetic type.
//
ARTIM_TEMP inline TAritm TriangleRasteriser::GetVerticalGradient(const TAritm& a, const TAritm& b, const float& den)
{
	return (b - a) / den;
}