#pragma once
#include "SceneObject.h"
#include "DirectionalLight.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "Mesh.h"
#include <string>

//
// Presents a simple static demo scene.
//
class SimpleDemo : public SceneObject
{
public:
	// Initialisation...
	void OnInit() override;

	// On tick...
	void OnTick(const float& deltaTime) override;

private:
	// Pedistal and figurine (don't shame me).
	Mesh* _pedistal = nullptr;
	Mesh* _figurine = nullptr;

	// Light
	DirectionalLight* _directional = nullptr;
	AmbientLight* _ambient = nullptr;
	PointLight* _point = nullptr;
};

