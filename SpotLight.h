#pragma once
#include "Light.h"

//
// Represents a spot-type light source.
//
class SpotLight : public Light
{
public:
	SpotLight();
	SpotLight(const Vector3& position, const Vector3& direction, const float& atten, const float& inner, const float& outer);
	~SpotLight();

	//
	// Calculates the overall contribution of this light on the given vertex and normal.
	//
	Colour CalculateContribution(const Vertex& position, const Vector3& normal, const float& roughness, const float& specular) override;

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
	// Light direction
	//
	const Vector3& GetDirection() const;
	void SetDirection(const Vector3& vector);

	//
	// The angle of the inner radius for the spotlight.
	//
	const float& GetInnerAngle() const;
	void SetInnerAngle(const float& value);

	//
	// The angle of the outer radius for the spotlight.
	//
	const float& GetOuterAngle() const;
	void SetOuterAngle(const float& value);

private:
	//
	// Smoothstep implementation.
	//
	static const float Smoothstep(const float a, const float b, const float x);

private:
	Vector3 _position;
	Vector3 _direction;

	float _attenuation;
	float _innerAngle;
	float _outerAngle;
};

