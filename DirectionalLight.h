#pragma once
#include "Light.h"
#include "Vector.h"

//
// Represents a directional light with no positions.
//
class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(Vector3 direction);
	
	const Vector3& GetDirection() const;
	void SetDirection(const Vector3& vector);

	Colour CalculateContribution(const Vertex& position, const Vector3& normal, const float& roughness, const float& specular) override;

private:
	Vector3 _direction;
};

