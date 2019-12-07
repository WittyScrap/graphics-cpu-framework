#include "PointLight.h"
#include <algorithm>

//
// Default point light constructor.
//
PointLight::PointLight() : Light()
{
	_attenuation = .05f;
}

//
// Constructor with a custom defined light position and radius.
//
PointLight::PointLight(Vector3 position, float radius, float attenuation) : Light()
{
	_attenuation = attenuation;
	_position = position;
}

//
// Copy constructor.
//
PointLight::PointLight(const PointLight& copy) : Light()
{
	_attenuation = copy._attenuation;
	_position = copy._position;
}

//
// The position of the point light.
//
const Vector3& PointLight::GetPosition() const
{
	return _position;
}

//
// Sets the position of the point light.
//
void PointLight::SetPosition(const Vector3& value)
{
	_position = value;
}

//
// The attenuation of this light.
//
const float& PointLight::GetAttenuation() const
{
	return _attenuation;
}

//
// Sets the attenuation of the light.
//
void PointLight::SetAttenuation(const float& value)
{
	_attenuation = max(value, 0.f);
}

//
// Calculates the overall contribution of the point light.
//
Colour PointLight::CalculateContribution(const Polygon3D& polygon, const std::vector<Vertex>& vertices)
{
	constexpr float a = 0.f;
	constexpr float c = 0.f;

	const Vector3 polygonCenter = polygon.CalculateCenter(vertices);
	const Vector3 lightRay = _position - polygonCenter;
	const Vector3 normal = polygon.GetWorldNormal();

	const float distance = lightRay.GetMagnitude();
	const float attenuation = 1 / (a + _attenuation * distance + c * (distance * distance));
	const float normalDotRay = Vector3::Dot(normal, Vector3::NormaliseVector(lightRay));
	const float finalIntensity = normalDotRay * attenuation;

	return GetIntensity() * finalIntensity;
}
