#pragma once
#include "Light.h"

//
// Constant ambient light.
//
class AmbientLight : public Light
{
public:
	// Construct light
	AmbientLight();

	//
	// Return the intensity value as a constant for all polygons.
	//
	Colour CalculateContribution(const Polygon3D& polygon, const std::vector<Vertex>& vertices) override;
};

