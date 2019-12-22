#pragma once
#include <Windows.h>
#include <vector>
#include <functional>
#include "Polygon3D.h"
#include "UnclampedColour.h"

// Template shorthand definitions
#define ARITM_TEMP(t_name) template<typename t_name>
#define ARTIM_TEMP ARITM_TEMP(TAritm)

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
	// Structure to be passed exclusively as reference for vertex sorter.
	//
	struct SorterData
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

		Vector3 uv0;
		Vector3 uv1;
		Vector3 uv2;

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
	static void SortVertices(SorterData& clipSpace, SorterData& worldSpace);

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
	static void TopPhongShaded(const HDC& hdc, const PolygonData& clipSpace, const PolygonData& worldSpace, const FragmentFunction& frag);
	static void BottomPhongShaded(const HDC& hdc, const PolygonData& clipSpace, const PolygonData& worldSpace, const FragmentFunction& frag);

	//
	// Splitting systems.
	//
	static const Vertex		GetSplitVertex(const Vertex& a, const Vertex& b, const Vertex& c);
	static const Colour		GetSplitColour(const Vertex& a, const Vertex& b, const Vertex& c);
	static const Vector3	GetSplitNormal(const Vertex& a, const Vertex& b, const Vertex& c);
	static const Vertex		GetSplitWorldVertex(const PolygonData& clipSpace, const PolygonData& worldSpace);
	static const Vector3	GetSplitUV(const Vertex& a, const Vertex& b, const Vertex& c);

	//
	// General utilities
	//
	ARTIM_TEMP inline static TAritm GetHorizontalGradient(const TAritm& slope, const int& pos, const float& source, const TAritm& pt);
	ARTIM_TEMP inline static TAritm GetVerticalGradient(const TAritm& a, const TAritm& b, const float& den);

	static float GetSlope(const Vertex& a, const Vertex& b);

	static const SmoothShadeData GetSmoothDataTop(const PolygonData& clipSpace);
	static const SmoothShadeData GetSmoothDataBottom(const PolygonData& clipSpace);

	static const SmoothLineData GetSmoothLineTop(const SmoothShadeData& slopeData, const int& pos);
	static const SmoothLineData GetSmoothLineBottom(const SmoothShadeData& slopeData, const int& pos);

	static const PhongShadeData GetPhongDataTop(const PhongShadeInput& input);
	static const PhongShadeData GetPhongDataBottom(const PhongShadeInput& input);

	static const PhongLineData GetPhongLineTop(const PhongShadeData& slopeData, const int& pos);
	static const PhongLineData GetPhongLineBottom(const PhongShadeData& slopeData, const int& pos);

	static void AdjustUV(Vector3& uv, const float& z);
	static void ApplyUV(Vector3& uv);

	static void RenderFlat(const HDC& hdc, const int& start, const int& end, const int& pos);
	static void RenderSmooth(const HDC& hdc, const SmoothLineData& lineData, const int& pos);
	static void RenderPhong(const HDC& hdc, const PhongLineData& lineData, const FragmentFunction& frag, const int& pos);
};

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