#pragma once
#include "Shape.h"

class Square : public Shape
{
public:
	void CreateModel(const Point3D<float>& location);
	void Draw(HDC hdc);

private:
	const float _squareWidth = 50;
	const float _squareHeight = 50;
};

