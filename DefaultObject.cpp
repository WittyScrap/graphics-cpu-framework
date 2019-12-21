#include "DefaultObject.h"
#include "Camera.h"
#include "Input.h"
#include <cmath>
#include "Environment.h"

//
// Initialises the class.
//
void DefaultObject::OnInit()
{
	// Cache input manager's pointer.
	Input* const inputManager = Input::Get();
	
	// Register all inputs...
	inputManager->SaveAxis('A', 'D', "Horizontal");
	inputManager->SaveAxis('S', 'W', "Forward");
	inputManager->SaveAxis('C', VK_SPACE, "Vertical");

	inputManager->SaveAxis(VK_LEFT, VK_RIGHT, "X");
	inputManager->SaveAxis(VK_UP, VK_DOWN, "Y");

	// Start presentation/demo
#if DEMO_TYPE == MODE_SIMPLE
	_demo = Environment::GetActive().CreateObject<SimpleDemo>("Presentation").get();
#elif DEMO_TYPE == MODE_PRESENTATION
	_demo = Environment::GetActive().CreateObject<Presentation>("Presentation").get();
#endif
}

//
// Initialises an input system module.
//
InputAxis::InputAxis(int a, int b) : _a(a), _b(b)
{ }

//
// Destructor
//
InputAxis::~InputAxis()
{ }

//
// Evaluates this input module.
//
float InputAxis::Evaluate() const
{
	float value = 0.f;

	if (Input::Get()->IsKeyHeld(_a))
	{
		value -= 1.f;
	}

	if (Input::Get()->IsKeyHeld(_b))
	{
		value += 1.f;
	}

	return value;
}
