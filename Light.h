#pragma once
#include "Polygon3D.h"
#include "Colour.h"

//
// Abstract implementation of a light structure.
//
class Light
{
public:
	Light();
	virtual ~Light();

	const Colour& GetIntensity() const;
	void SetIntensity(const Colour& value);

	//
	// Contribution calculator.
	//
	virtual Colour CalculateContribution(const Polygon3D& polygon, const std::vector<Vertex>& vertices) = 0;

private:
	Colour _intensity;
};

