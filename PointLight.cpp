#include "PointLight.h"
#include "Camera.h"
#include <algorithm>

//
// Default point light constructor.
//
PointLight::PointLight() : Light()
{
	_attenuation = .01f;
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
Colour PointLight::CalculateContribution(const Vertex& position, const Vector3& normal, const Colour& ambient, const float& roughness, const float& specular)
{
	constexpr float a = 0.f;
	constexpr float c = 0.f;

	const Vector3 lightRay = _position - position;
	const Vector3 viewRay = Camera::GetMainCamera()->GetPosition() - position;

	const float distance = lightRay.GetMagnitude();
	const float attenuation = 1 / (a + _attenuation * distance + c * (distance * distance));
	const float normalDotRay = max(Vector3::Dot(normal, Vector3::NormaliseVector(lightRay)), 0.f);
	const float finalIntensity = normalDotRay * attenuation;

	const Vector3 h((lightRay + viewRay) / (lightRay + viewRay).GetMagnitude());
	const float phongHighlights = specular * static_cast<float>(pow(Vector3::Dot(normal, h), roughness));

	return GetIntensity() * (finalIntensity + phongHighlights);
}
