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
	Colour CalculateContribution(const Vertex& position, const Vector3& normal) override;
};

