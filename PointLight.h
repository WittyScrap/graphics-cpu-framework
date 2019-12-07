#pragma once
#include "Light.h"

//
// Represents a point light with a position, radius and no direction.
//
class PointLight : public Light
{
public:
	PointLight();
	PointLight(Vector3 position, float radius, float attenuation);
	PointLight(const PointLight& copy);

	//
	// Light position
	//
	const Vector3& GetPosition() const;
	void SetPosition(const Vector3& value);

	//
	// Light attenuation
	//
	const float& GetAttenuation() const;
	void SetAttenuation(const float& value);

	//
	// Point light formula here.
	//
	Colour CalculateContribution(const Polygon3D& polygon, const std::vector<Vertex>& vertices) override;

private:
	Vector3 _position;
	float _attenuation;
};

