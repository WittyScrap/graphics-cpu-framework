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
	Colour CalculateContribution(const Vertex& position, const Vector3& normal, const Colour& ambient, const float& roughness, const float& specular) override;

private:
	Vector3 _position;
	float _attenuation;
};

