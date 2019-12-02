#include "Colour.h"

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
	_red = red;
}

//
// Set the GREEN component.
//
void Colour::SetGreen(const float& green)
{
	_green = green;
}

//
// Set the BLUE component.
//
void Colour::SetBlue(const float& blue)
{
	_blue = blue;
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
	_red += rhs._red;
	_green += rhs._green;
	_blue += rhs._blue;

	return *this;
}

//
// Subtract colour operator.
//
const Colour& Colour::operator-=(const Colour& rhs)
{
	_red -= rhs._red;
	_green -= rhs._green;
	_blue -= rhs._blue;

	return *this;
}

//
// Multiply by operator.
//
const Colour& Colour::operator*=(const Colour& rhs)
{
	_red *= rhs._red;
	_green *= rhs._green;
	_blue *= rhs._blue;

	return *this;
}

//
// Divide by operator.
//
const Colour& Colour::operator/=(const Colour& rhs)
{
	_red /= rhs._red;
	_green /= rhs._green;
	_blue /= rhs._blue;

	return *this;
}

//
// Addition operator.
//
const Colour Colour::operator+(const Colour& rhs) const
{
	Colour add;

	add._red = _red + rhs._red;
	add._green = _green + rhs._green;
	add._blue = _blue + rhs._blue;

	return add;
}

//
// Subtraction operator.
//
const Colour Colour::operator-(const Colour& rhs) const
{
	Colour add;

	add._red = _red - rhs._red;
	add._green = _green - rhs._green;
	add._blue = _blue - rhs._blue;

	return add;
}

//
// Multiplication operator.
//
const Colour Colour::operator*(const Colour& rhs) const
{
	Colour add;

	add._red = _red * rhs._red;
	add._green = _green * rhs._green;
	add._blue = _blue * rhs._blue;

	return add;
}

//
// Division operator.
//
const Colour Colour::operator/(const Colour& rhs) const
{
	Colour add;

	add._red = _red / rhs._red;
	add._green = _green / rhs._green;
	add._blue = _blue / rhs._blue;

	return add;
}

//
// Multiplies a colour by a constant
//
const Colour Colour::operator*(const float& rhs) const
{
	Colour multiplied;

	multiplied._red = _red * rhs;
	multiplied._green = _green * rhs;
	multiplied._blue = _blue * rhs;

	return multiplied;
}

//
// Divides a colour by a constant
//
const Colour Colour::operator/(const float& rhs) const
{
	Colour divided;

	divided._red = _red / rhs;
	divided._green = _green / rhs;
	divided._blue = _blue / rhs;

	return divided;
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
