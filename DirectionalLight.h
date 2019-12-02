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

	Colour CalculateContribution(const Polygon3D& polygon, const std::vector<Vertex>& vertices) override;

private:
	Vector3 _direction;
};

