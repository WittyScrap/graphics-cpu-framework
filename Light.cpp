#include "Light.h"
#include <math.h>

//
// Nothing to construct.
//
Light::Light() : _intensity(1.f, 1.f, 1.f)
{ }

//
// Nothing to destruct.
//
Light::~Light()
{ }

//
// The light's intensity.
//
const Colour& Light::GetIntensity() const
{
	return _intensity;
}

//
// Sets the light's intensity.
//
void Light::SetIntensity(const Colour& value)
{
	_intensity = value;
}
