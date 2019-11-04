#pragma once
#include "SceneObject.h"

//
// Default entry point object.
//
class DefaultObject : public SceneObject
{
public:
	// Necessary constructor
	DefaultObject(Rasteriser& world);

	// Initialisation...
	void OnInit() override;

	// On tick...
	void OnTick(const float& deltaTime) override;

private:
	COLORREF _shapeColour = RGB(0x00, 0x00, 0xFF);
};

