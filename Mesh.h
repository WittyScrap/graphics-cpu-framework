#pragma once
#include "Shape.h"
#include "Vertex.h"
#include "Polygon3D.h"
#include "Colour.h"
#include "TriangleRasteriser.h"
#include "Texture.h"


//
// A 3D mesh composed of multiple plygons and vertices.
//
class Mesh : public Shape
{
public:
	//
	// How to draw a mesh.
	//
	enum class DrawMode
	{
		DRAW_NONE,
		DRAW_WIREFRAME,
		DRAW_SOLID,
		DRAW_FRAGMENT
	};

	//
	// How to shade a triangle.
	//
	enum class ShadeMode
	{
		SHADE_FLAT,
		SHADE_GOURAUD,
		SHADE_PHONG
	};


	Mesh();
	~Mesh();

	// Loads the mesh data from a file.
	void LoadFromFile(const char* const fileName, const char* texture = nullptr);

	//
	// Accessors
	//
	const std::vector<Polygon3D>& GetPolygons() const;
	const size_t GetPolygonCount() const;

	//
	// Modifiers
	//
	void AddVertex(float x, float y, float z);
	void AddPolygon(int i0, int i1, int i2, int u0, int u1, int u2);
	void AddUVcoord(float u, float v);

	//
	// Draw operation
	//
	void Draw(HDC hdc);

	//
	// Drawming modes.
	//
	void Mode(const DrawMode& mode);
	void Shade(const ShadeMode& mode);
	void Cull(const bool& mode);

	//
	// Shading information.
	//
	const float& GetRoughness() const;
	void SetRoughness(const float& value);

	//
	// Specular information.
	//
	const float& GetSpecularCoefficient() const;
	void SetSpecularCoefficient(const float& value);

	//
	// Ambient coefficient
	//
	const Colour& GetAmbientCoefficient() const;
	void SetAmbientCoefficient(const Colour& value);

	//
	// Lighting tools
	//
	static Colour ComputeLighting(const Polygon3D& polygon, const std::vector<Vertex>& vertices);
	static Colour ComputeLighting(const Vertex& vertex, const Colour& ambient, const float& roughness, const float& specular);

	//
	// Texturing
	//
	const Texture& GetTexture() const;
	Texture& GetTexture();

private:
	//
	// Pen handler
	//
	void SetActiveColour(const HDC& hdc, const COLORREF& penColor, int thickness = 1);
	void ResetActiveColour(const HDC& hdc);

	//
	// Normals
	//
	void GenerateObjectNormals();
	void GenerateWorldNormals();
	void GenerateClipNormals();
	void GenerateVertexNormals();

	//
	// Optimisation tools
	//
	void CalculateBackfaceCulling(const std::vector<Vertex>& vertices);
	void CalculateDepthSorting(const std::vector<Vertex>& polygons);
	
	//
	// Drawing tools
	//
	void DrawSolidPolygon(const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace, const HDC& hdc);
	void DrawWirePolygon(const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace, const HDC& hdc);
	void DrawFragPolygon(const Polygon3D& polygon, const std::vector<Vertex>& clipSpace, const std::vector<Vertex>& worldSpace, const HDC& hdc);

	//
	// Lighting tools
	//
	void ComputeVertexLighting(); // Computes the lighting for all vertices.

private:
	std::vector<Polygon3D> _polygons;
	std::vector<Polygon3D*> _visiblePolygons;
	std::vector<Vector3> _uv;

	HPEN _previousPen;
	HBRUSH _previousBrush;

	DrawMode _drawMode;
	ShadeMode _shadeMode;

	Texture _texture;

	float _roughness;	// Alpha
	float _specular;	// Ks
	Colour _ambient;	// Ka
//	Colour _colour;		// Kd, this is included in Shape and it is essentially its functionality.

	bool _doBackfaceCulling{ true };
};

