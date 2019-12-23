#include "SpotLight.h"
#define PI 3.14159265359f

//
// Default constructor
//
SpotLight::SpotLight() : Light(), _attenuation{ 0.01f }, _innerAngle{ PI * 2 / 9 }, _outerAngle{ PI / 2 }
{ }

//
// Full constructor.
//
SpotLight::SpotLight(const Vector3& position, const Vector3& direction, const float& atten, const float& inner, const float& outer)
	: _position{position}, _direction{direction}, _attenuation{atten}, _innerAngle{inner}, _outerAngle{outer}
{ }

//
// Destructor.
//
SpotLight::~SpotLight()
{ }

//
// Calculates the overall contribution of this light on the given vertex at the given normal.
//
Colour SpotLight::CalculateContribution(const Vertex& position, const Vector3& normal, const float& roughness, const float& specular)
{
	constexpr float a = 0.f;
	constexpr float c = 0.f;

	const Vector3 lightRay = _position - position;

	const float distance = lightRay.GetMagnitude();
	const float attenuation = 1 / (a + _attenuation * distance + c * (distance * distance));
	const float normalDotRay = max(Vector3::Dot(normal, Vector3::NormaliseVector(lightRay)), 0.f);
	const float finalIntensity = normalDotRay * attenuation;
	const float outerCosine = static_cast<float>(cos(_outerAngle));
	const float innerCosine = static_cast<float>(cos(_innerAngle));
	const float normalDotLight = Vector3::Dot(normal, -_direction);
	const float spotlightValue = Smoothstep(outerCosine, innerCosine, normalDotLight);

	return GetIntensity() * finalIntensity * spotlightValue;
}

//
// The position of this spotlight.
//
const Vector3& SpotLight::GetPosition() const
{
	return _position;
}

//
// Sets the position of this spotlight.
//
void SpotLight::SetPosition(const Vector3& value)
{
	_position = value;
}

//
// The attenuation modifier for this spot light.
//
const float& SpotLight::GetAttenuation() const
{
	return _attenuation;
}

//
// Allows to change the attenuation modifier for this spot light.
//
void SpotLight::SetAttenuation(const float& value)
{
	_attenuation = value;
}

//
// The direction this spot light points at.
//
const Vector3& SpotLight::GetDirection() const
{
	return _direction;
}

//
// Allows to change which direction this spot light points at.
//
void SpotLight::SetDirection(const Vector3& vector)
{
	_direction = Vector3::NormaliseVector(vector);
}

//
// Implementation of smooth step function.
//
const float SpotLight::Smoothstep(const float a, const float b, const float x)
{
	if (x < a)
	{
		return 0;
	}

	if (x > b)
	{
		return 1;
	}

	float t = (x - a) / (b - a);
	return (3.f - 2.f * t) * (t * t);
}
