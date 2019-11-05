#pragma once
#include "SceneObject.h"
#include "Mesh.h"
#include <string>
#include <unordered_map>

//
// Input pair controller
//
class Input
{
public:
	Input(Rasteriser& world, int a, int b);
	virtual ~Input();

	float Evaluate() const;
private:
	int _a;
	int _b;

	Rasteriser& _world;
};

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
	// Saves an input combination
	void RegisterInputPair(const int& a, const int& b, const std::string& name);
	float GetInput(const std::string& name) const;

private:
	// The colour of the shape's wireframe.
	COLORREF _shapeColour = RGB(0x00, 0x00, 0xFF);

	// Pedistal and figurine (don't shame me).
	Mesh* _pedistal;
	Mesh* _figurine;

	// Input map
	std::unordered_map<std::string, Input> _inputManager;
};

