#include "TextShape.h"
#include "Rasteriser.h"

//
// Default constructor, the default value for this string is NULL.
//
TextShape::TextShape() : _value{ nullptr }, _background(1, 1, 1)
{ 
	SetColour(Colour::Black);
}

//
// Value constructor.
//
TextShape::TextShape(const wchar_t* value) : _value{ value }, _background(1, 1, 1)
{
	SetColour(Colour::Black); 
}

//
// Destructor.
//
TextShape::~TextShape()
{
	_value = nullptr;
}

//
// The current value of this text object.
//
const wchar_t* TextShape::GetValue() const
{
	return _value;
}

//
// Allows to set the text for this object.
//
void TextShape::SetValue(const wchar_t* value)
{
	_value = value;
}

//
// The background colour for this text.
//
const Colour& TextShape::GetBackground() const
{
	return _background;
}

//
// Allows to set the background colour for this text.
//
void TextShape::SetBackground(const Colour& background)
{
	_background = background;
}

//
// Draws the text.
//
void TextShape::Draw(HDC hdc)
{
	Rasteriser::DrawString(hdc, _value, GetColour().AsColor(), _background.AsColor());
}
