#include "DefaultObject.h"
#include "Rasteriser.h"
#include <cmath>

//
// Initialiser constructor...
//
DefaultObject::DefaultObject(Rasteriser& world) : SceneObject(world)
{ }

//
// Initialises the class.
//
void DefaultObject::OnInit()
{
	// Create a cube mesh for the pedistal and a marvin mesh for reasons.
	_pedistal = &CreateShape<Mesh>();
	_figurine = &CreateShape<Mesh>();

	// Actually load the meshes because you haven't yet
	_pedistal->LoadFromFile("Meshes/cube.md2");
	_figurine->LoadFromFile("Meshes/marvin.md2");

	// Place the cube underneath
	_pedistal->SetPosition({ 0, 0, 0 });

	Camera::GetMainCamera()->SetPosition({ 0, 0, -50 });
	
	// Register all inputs...
	RegisterInputPair('A', 'D', "Horizontal");
	RegisterInputPair('S', 'W', "Forward");
	RegisterInputPair('C', VK_SPACE, "Vertical");

	RegisterInputPair(VK_LEFT, VK_RIGHT, "X");
	RegisterInputPair(VK_UP, VK_DOWN, "Y");
}

//
// Will be called on every frame.
//
void DefaultObject::OnTick(const float& deltaTime)
{
	Vector3 angle = { 0.f, .025f, 0.f };

	// A nice spinning animation!
	_pedistal->Rotate(angle);
	_figurine->Rotate(angle);

	// Move the camera, hurray!
	Vector3 cameraMovement{ 0, 0, 0 };
	Vector3 cameraRotation{ 0, 0, 0 };

	cameraMovement.X = GetInput("Horizontal");
	cameraMovement.Y = GetInput("Vertical");
	cameraMovement.Z = GetInput("Forward");

	cameraRotation.X = GetInput("Y") * .01f;
	cameraRotation.Y = GetInput("X") * .01f;

	Camera* const mainCamera = Camera::GetMainCamera();

	Vector3 cameraCurrentPosition = mainCamera->GetPosition();
	Vector3 cameraCurrentRotation = mainCamera->GetRotation();

	cameraCurrentPosition = cameraCurrentPosition + cameraMovement;
	cameraCurrentRotation = cameraCurrentRotation + cameraRotation;

	mainCamera->SetPosition(cameraCurrentPosition);
	mainCamera->SetRotation(cameraCurrentRotation);
}

//
// Registers a new input pair to the internal map.
//
void DefaultObject::RegisterInputPair(const int& a, const int& b, const std::string& name)
{
	_inputManager.insert(std::pair<std::string, Input>(name, Input(GetWorld(), a, b)));
}

//
// Polls input data from an existing manager.
//
float DefaultObject::GetInput(const std::string& name) const
{
	return _inputManager.at(name).Evaluate();
}

//
// Initialises an input system module.
//
Input::Input(Rasteriser& world, int a, int b) : _world(world), _a(a), _b(b)
{ }

//
// Destructor
//
Input::~Input()
{ }

//
// Evaluates this input module.
//
float Input::Evaluate() const
{
	float value = 0.f;

	if (_world.IsKeyHeld(_a))
	{
		value -= 1.f;
	}

	if (_world.IsKeyHeld(_b))
	{
		value += 1.f;
	}

	return value;
}
