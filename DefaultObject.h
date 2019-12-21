#pragma once
#include "SceneObject.h"
#include "DirectionalLight.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "Mesh.h"
#include <string>

#define MODE_SIMPLE 0xFF
#define MODE_PRESENTATION 0xFE
#define DEMO_TYPE MODE_SIMPLE

#if DEMO_TYPE == MODE_SIMPLE
#include "SimpleDemo.h"
#elif DEMO_TYPE == MODE_PRESENTATION
#include "Presentation.h"
#endif

//
// Default entry point object.
//
class DefaultObject : public SceneObject
{
public:
	// Initialisation...
	void OnInit() override;

private:
	// Demo/presentation objects.
#if DEMO_TYPE == MODE_SIMPLE
	SimpleDemo* _demo;
#elif DEMO_TYPE == MODE_PRESENTATION
	Presentation* _demo;
#endif
};

