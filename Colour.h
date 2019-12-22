#pragma once
#include <Windows.h>

//
// Colour structure that contains a RED, GREEN, and BLUE component.
//
class Colour
{
public:
	Colour();
	Colour(const float& red, const float& green, const float& blue);
	Colour(const Colour& copy);
	Colour(const COLORREF& copy);

	const float& GetRed() const;
	const float& GetGreen() const;
	const float& GetBlue() const;

	void SetRed(const float& red);
	void SetGreen(const float& green);
	void SetBlue(const float& blue);

	const Colour& operator=(const Colour& rhs);
	const Colour& operator+=(const Colour& rhs);
	const Colour& operator-=(const Colour& rhs);
	const Colour& operator*=(const Colour& rhs);
	const Colour& operator/=(const Colour& rhs);

	const Colour operator+(const Colour& rhs) const;
	const Colour operator-(const Colour& rhs) const;
	const Colour operator*(const Colour& rhs) const;
	const Colour operator/(const Colour& rhs) const;

	const Colour operator*(const float& rhs) const;
	const Colour operator/(const float& rhs) const;

	const bool operator==(const Colour& rhs) const;

	static Colour FromColor(const COLORREF& rhs);
	const COLORREF AsColor() const;

	static Colour Lerp(const Colour& lhs, const Colour& rhs, const float& alpha);

	// Simple colour presets
	static const Colour Black;
	static const Colour White;
	static const Colour Red;
	static const Colour Green;
	static const Colour Blue;
	static const Colour Magenta;

protected:
	static float Clamp(const float& value, const float& a, const float& b);

private:
	float _red;
	float _green;
	float _blue;
};

