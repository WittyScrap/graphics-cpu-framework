#include "AmbientLight.h"

//
// Call superconstructor.
//
AmbientLight::AmbientLight() : Light()
{ }

//
// Returns a constant value related to its intensity.
//
Colour AmbientLight::CalculateContribution(const Polygon3D& polygon, const std::vector<Vertex>& vertices)
{
	return GetIntensity();
}
