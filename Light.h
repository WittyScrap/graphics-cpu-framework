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
	virtual Colour CalculateContribution(const Vertex& position, const Vector3& normal) = 0;

private:
	Colour _intensity;
};

