#include "Colour.h"
#include <algorithm>

//
// Colour samples.
//
const Colour Colour::Black(0, 0, 0);
const Colour Colour::White(1, 1, 1);
const Colour Colour::Red(1, 0, 0);
const Colour Colour::Green(0, 1, 0);
const Colour Colour::Blue(0, 0, 1);
const Colour Colour::Magenta(1, 0, 1);

//
// Colour default constructor.
//
Colour::Colour() : _red{ 0 }, _green{ 0 }, _blue{ 0 }
{ }

//
// Colour full constructor.
//
Colour::Colour(const float& red, const float& green, const float& blue) : _red{ red }, _green{ green }, _blue{ blue }
{ }

//
// Colour copy constructor.
//
Colour::Colour(const Colour& copy) : _red{ copy._red }, _green{ copy._green }, _blue{ copy._blue }
{ }

//
// Creates colour from COLORREF struct.
//
Colour::Colour(const COLORREF& copy)
{
	_red = GetRValue(copy) / 255.f;
	_green = GetGValue(copy) / 255.f;
	_blue = GetBValue(copy) / 255.f;
}

//
// Access the RED component.
//
const float& Colour::GetRed() const
{
	return _red;
}

//
// Access the GREEN component.
//
const float& Colour::GetGreen() const
{
	return _green;
}

//
// Access the BLUE component.
//
const float& Colour::GetBlue() const
{
	return _blue;
}

//
// Set the RED component.
//
void Colour::SetRed(const float& red)
{
	_red = Clamp(red, 0.f, 1.f);
}

//
// Set the GREEN component.
//
void Colour::SetGreen(const float& green)
{
	_green = Clamp(green, 0.f, 1.f);
}

//
// Set the BLUE component.
//
void Colour::SetBlue(const float& blue)
{
	_blue = Clamp(blue, 0.f, 1.f);
}

//
// Assignment operator.
//
const Colour& Colour::operator=(const Colour& rhs)
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
// Add colour operator.
//
const Colour& Colour::operator+=(const Colour& rhs)
{
	_red = Clamp(rhs._red + _red, 0.f, 1.f);
	_green = Clamp(rhs._green + _green, 0.f, 1.f);
	_blue = Clamp(rhs._blue + _blue, 0.f, 1.f);

	return *this;
}

//
// Subtract colour operator.
//
const Colour& Colour::operator-=(const Colour& rhs)
{
	_red = Clamp(rhs._red - _red, 0.f, 1.f);
	_green = Clamp(rhs._green - _green, 0.f, 1.f);
	_blue = Clamp(rhs._blue - _blue, 0.f, 1.f);

	return *this;
}

//
// Multiply by operator.
//
const Colour& Colour::operator*=(const Colour& rhs)
{
	_red = Clamp(rhs._red * _red, 0.f, 1.f);
	_green = Clamp(rhs._green * _green, 0.f, 1.f);
	_blue = Clamp(rhs._blue * _blue, 0.f, 1.f);

	return *this;
}

//
// Divide by operator.
//
const Colour& Colour::operator/=(const Colour& rhs)
{
	_red = Clamp(rhs._red / _red, 0.f, 1.f);
	_green = Clamp(rhs._green / _green, 0.f, 1.f);
	_blue = Clamp(rhs._blue / _blue, 0.f, 1.f);

	return *this;
}

//
// Addition operator.
//
const Colour Colour::operator+(const Colour& rhs) const
{
	Colour add;

	add._red = Clamp(rhs._red + _red, 0.f, 1.f);
	add._green = Clamp(rhs._green + _green, 0.f, 1.f);
	add._blue = Clamp(rhs._blue + _blue, 0.f, 1.f);

	return add;
}

//
// Subtraction operator.
//
const Colour Colour::operator-(const Colour& rhs) const
{
	Colour subtract;

	subtract._red = Clamp(_red - rhs._red, 0.f, 1.f);
	subtract._green = Clamp(_green - rhs._green, 0.f, 1.f);
	subtract._blue = Clamp(_blue - rhs._blue, 0.f, 1.f);

	return subtract;
}

//
// Multiplication operator.
//
const Colour Colour::operator*(const Colour& rhs) const
{
	Colour multiply;

	multiply._red = Clamp(rhs._red * _red, 0.f, 1.f);
	multiply._green = Clamp(rhs._green * _green, 0.f, 1.f);
	multiply._blue = Clamp(rhs._blue * _blue, 0.f, 1.f);

	return multiply;
}

//
// Division operator.
//
const Colour Colour::operator/(const Colour& rhs) const
{
	Colour divide;

	divide._red = Clamp(_red / rhs._red, 0.f, 1.f);
	divide._green = Clamp(_green / rhs._green, 0.f, 1.f);
	divide._blue = Clamp(_blue / rhs._blue, 0.f, 1.f);

	return divide;
}

//
// Multiplies a colour by a constant
//
const Colour Colour::operator*(const float& rhs) const
{
	Colour multiplied;

	multiplied._red = Clamp(_red * rhs, 0.f, 1.f);
	multiplied._green = Clamp(_green * rhs, 0.f, 1.f);
	multiplied._blue = Clamp(_blue * rhs, 0.f, 1.f);

	return multiplied;
}

//
// Divides a colour by a constant
//
const Colour Colour::operator/(const float& rhs) const
{
	Colour divided;

	divided._red = Clamp(_red / rhs, 0.f, 1.f);
	divided._green = Clamp(_green / rhs, 0.f, 1.f);
	divided._blue = Clamp(_blue / rhs, 0.f, 1.f);

	return divided;
}

//
// Checks that two colours are the same.
//
const bool Colour::operator==(const Colour& rhs) const
{
	return _red == rhs._red && _green == rhs._green && _blue == rhs._blue;
}

//
// Assigns this colour's properties from the given color ref.
//
Colour Colour::FromColor(const COLORREF& rhs)
{
	return Colour(GetRValue(rhs) / 255.f, GetGValue(rhs) / 255.f, GetBValue(rhs) / 255.f);
}

//
// Converts this colour to a COLORREF struct instance.
//
const COLORREF Colour::AsColor() const
{
	return RGB(static_cast<int>(_red * 255), static_cast<int>(_green * 255), static_cast<int>(_blue * 255));
}

//
// Linearly interpolates the value of two colours.
//
Colour Colour::Lerp(const Colour& lhs, const Colour& rhs, const float& alpha)
{
	return rhs * alpha + lhs * (1 - alpha);
}

//
// Clamps a value between a and b.
// Note: This implementation is needed since std::clamp seems to use a module 
// operation to clamp its values, which results in undesired values outside of
// the a-b range.
//
float Colour::Clamp(const float& value, const float& a, const float& b)
{
	if (value < a)
	{
		return a;
	}

	if (value > b)
	{
		return b;
	}

	return value;
}
