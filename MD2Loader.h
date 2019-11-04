#pragma once
#include "Mesh.h"

// Declare typedefs used by the MD2Loader to call the methods to add a vertex and
// add a polygon to the lists

typedef void (Mesh::*AddVertex)(float x, float y, float z);
typedef void (Mesh::*AddPolygon)(int i0, int i1, int i2);

class MD2Loader
{
	public:
		static bool LoadModel(const char* md2Filename, Mesh& model, AddPolygon addPolygon, AddVertex addVertex);
};
