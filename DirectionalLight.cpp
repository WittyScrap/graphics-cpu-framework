#include "DirectionalLight.h"
#include "Camera.h"
#include <cmath>

//
// Creates a directional light with a default DOWN orientation.
//
DirectionalLight::DirectionalLight() : Light()
{ 
	_direction = Vector3(0, -1, 0);
}

//
// Creates a directional light with a custom orientation.
//
DirectionalLight::DirectionalLight(Vector3 direction) : _direction(direction)
{ }

//
// The direction this light is facing.
//
const Vector3& DirectionalLight::GetDirection() const
{
	return _direction;
}

//
// Set the direction this light is facing.
//
void DirectionalLight::SetDirection(const Vector3& vector)
{
	_direction = Vector3::NormaliseVector(vector);
}

//
// Calculates the overall contribution this light is maing on the given polygon.
//
Colour DirectionalLight::CalculateContribution(const Vertex& position, const Vector3& normal, const float& roughness)
{
	const Vector3 inverseDirection = -_direction;
	float normalDotDirection = Vector3::Dot(normal, inverseDirection);

	if (normalDotDirection < 0)
	{
		normalDotDirection = 0;
	}

	float lightValue = normalDotDirection;

	if (roughness > 0)
	{
		// Assume Ks = 1.
		Vector3 eye(Camera::GetMainCamera()->GetPosition() - position);
		Vector3 r = normal * (normalDotDirection) * 2 - inverseDirection;

		lightValue += pow(Vector3::Dot(r, eye), 1 / roughness);
	}
	
	return GetIntensity() * lightValue;
}
