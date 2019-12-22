#pragma once
#include "Shape.h"

//
// Represents text that can be drawn on the screen.
//
class TextShape : public Shape
{
public:
	TextShape();
	TextShape(const wchar_t* value);
	~TextShape();

	const wchar_t* GetValue() const;
	void SetValue(const wchar_t* value);

	const Colour& GetBackground() const;
	void SetBackground(const Colour& background);

	void Draw(HDC hdc) override;

private:
	const wchar_t* _value;
	Colour _background;
};

