#pragma once
#include "SceneObject.h"
#include "Mesh.h"
#include <string>
#include <unordered_map>

//
// Default entry point object.
//
class DefaultObject : public SceneObject
{
public:
	// Initialisation...
	void OnInit() override;

	// On tick...
	void OnTick(const float& deltaTime) override;

private:
	// The colour of the shape's wireframe.
	COLORREF _shapeColour = RGB(0x00, 0x00, 0xFF);

	// Pedistal and figurine (don't shame me).
	Mesh* _pedistal;
	Mesh* _figurine;
};

