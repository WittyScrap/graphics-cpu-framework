#include "UnclampedColour.h"
#include <cmath>

//
// Default constructor.
//
UnclampedColour::UnclampedColour() : _red{0}, _green{0}, _blue{0}
{ }

//
// Full constructor.
//
UnclampedColour::UnclampedColour(const float& red, const float& green, const float& blue) : _red{red}, _green{green}, _blue{blue}
{ }

//
// Copy constructor. 
//
UnclampedColour::UnclampedColour(const UnclampedColour& copy) : _red{copy._red}, _green{copy._green}, _blue{copy._blue}
{ }

//
// Standard colour copy constructor.
//
UnclampedColour::UnclampedColour(const Colour& copy) : _red{copy.GetRed()}, _green{copy.GetGreen()}, _blue{copy.GetBlue()}
{ }

//
// The red component of this colour, this could be any value but will likely remain within
// the [0, 1] range.
//
const float& UnclampedColour::GetRed() const
{
	return _red;
}

//
// The green component of this colour, this could be any value but will likely remain within
// the [0, 1] range.
//
const float& UnclampedColour::GetGreen() const
{
	return _green;
}

//
// The blue component of this colour, this could be any value but will likely remain within
// the [0, 1] range.
//
const float& UnclampedColour::GetBlue() const
{
	return _blue;
}

//
// Sets the red component, preferrably within the [0, 1] range.
//
void UnclampedColour::SetRed(const float& red)
{
	_red = red;
}

//
// Sets the green component, preferrably within the [0, 1] range.
//
void UnclampedColour::SetGreen(const float& green)
{
	_green = green;
}

//
// Sets the blue component, preferrably within the [0, 1] range.
//
void UnclampedColour::SetBlue(const float& blue)
{
	_blue = blue;
}

//
// Normalises the colour to be within the [0, 1] range.
//
void UnclampedColour::Normalise()
{
	float magnitude = std::sqrtf(_red * _red + _green * _green + _blue * _blue);

	_red /= magnitude;
	_green /= magnitude;
	_blue /= magnitude;
}

//
// Adds two colours together.
//
UnclampedColour UnclampedColour::operator+(const UnclampedColour& rhs)
{
	return UnclampedColour(_red + rhs._red, _green + rhs._green, _blue + rhs._blue);
}

//
// Substracts one colour from another.
//
UnclampedColour UnclampedColour::operator-(const UnclampedColour& rhs)
{
	return UnclampedColour(_red - rhs._red, _green - rhs._green, _blue - rhs._blue);
}

//
// Multiplies two colours together.
//
UnclampedColour UnclampedColour::operator*(const UnclampedColour& rhs)
{
	return UnclampedColour(_red * rhs._red, _green * rhs._green, _blue * rhs._blue);
}

//
// Divides a colour by another.
//
UnclampedColour UnclampedColour::operator/(const UnclampedColour& rhs)
{
	return UnclampedColour(_red / rhs._red, _green / rhs._green, _blue / rhs._blue);
}

//
// Adds a colour to the existing one.
//
const UnclampedColour& UnclampedColour::operator+=(const UnclampedColour& rhs)
{
	_red += rhs._red;
	_green += rhs._green;
	_blue += rhs._blue;

	return *this;
}

//
// Subtracts a colour from the existing colour.
//
const UnclampedColour& UnclampedColour::operator-=(const UnclampedColour& rhs)
{
	_red -= rhs._red;
	_green -= rhs._green;
	_blue -= rhs._blue;

	return *this;
}

//
// Multiplies this colour by the provided colour.
//
const UnclampedColour& UnclampedColour::operator*=(const UnclampedColour& rhs)
{
	_red *= rhs._red;
	_green *= rhs._green;
	_blue *= rhs._blue;

	return *this;
}

//
// Divides this colour by the provided colour.
//
const UnclampedColour& UnclampedColour::operator/=(const UnclampedColour& rhs)
{
	_red /= rhs._red;
	_green /= rhs._green;
	_blue /= rhs._blue;

	return *this;
}

//
// Adds a constant value to all components of a colour.
//
UnclampedColour UnclampedColour::operator+(const float& rhs)
{
	return UnclampedColour(_red + rhs, _green + rhs, _blue + rhs);
}

//
// Subtracts a constant value from all components of a colour.
//
UnclampedColour UnclampedColour::operator-(const float& rhs)
{
	return UnclampedColour(_red - rhs, _green - rhs, _blue - rhs);
}

//
// Multiplies all components of a colour by a constant value.
//
UnclampedColour UnclampedColour::operator*(const float& rhs)
{
	return UnclampedColour(_red * rhs, _green * rhs, _blue * rhs);
}

//
// Divides all components of a colour by a constant value.
//
UnclampedColour UnclampedColour::operator/(const float& rhs)
{
	return UnclampedColour(_red / rhs, _green / rhs, _blue / rhs);
}

//
// Adds a constant value to all components of this colour.
//
const UnclampedColour& UnclampedColour::operator+=(const float& rhs)
{
	_red += rhs;
	_green += rhs;
	_blue += rhs;

	return *this;
}

//
// Subtracts a constant values from all components of this colour.
//
const UnclampedColour& UnclampedColour::operator-=(const float& rhs)
{
	_red -= rhs;
	_green -= rhs;
	_blue -= rhs;

	return *this;
}

//
// Multiplies all components of this colour by a constant value.
//
const UnclampedColour& UnclampedColour::operator*=(const float& rhs)
{
	_red *= rhs;
	_green *= rhs;
	_blue *= rhs;

	return *this;
}

//
// Divides all components of this colour by a constant value.
//
const UnclampedColour& UnclampedColour::operator/=(const float& rhs)
{
	_red /= rhs;
	_green /= rhs;
	_blue /= rhs;

	return *this;
}

//
// Assigns all values from the provided colour into this colour.
//
const UnclampedColour& UnclampedColour::operator=(const UnclampedColour& rhs)
{
	if (&rhs != this)
	{
		_red = rhs._red;
		_green = rhs._green;
		_blue = rhs._blue;
	}

	return *this;
}

//
// Assigns all values from the provided colour into this colour.
//
const UnclampedColour& UnclampedColour::operator=(const Colour& rhs)
{
	_red = rhs.GetRed();
	_green = rhs.GetGreen();
	_blue = rhs.GetBlue();

	return *this;
}

//
// Assigns the provided constant value to this colour's components.
//
const UnclampedColour& UnclampedColour::operator=(const float& rhs)
{
	_red = rhs;
	_green = rhs;
	_blue = rhs;

	return *this;
}
