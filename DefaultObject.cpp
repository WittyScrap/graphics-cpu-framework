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
	// Create a cube mesh for the pedistal and a marvin mesh for reasons.
	_pedistal = CreateShape<Mesh>();
	_figurine = CreateShape<Mesh>();

	// Actually load the meshes because you haven't yet
	_pedistal->LoadFromFile("Meshes/cube.md2");
	_figurine->LoadFromFile("Meshes/marvin.md2");

	// Set colour
	_pedistal->SetColour(Colour(.5f, .5f, 1.f));
	_figurine->SetColour(Colour(1.f, .5f, .5f));

	// Set draw modes
	_pedistal->DrawMode(DRAW_FRAGMENT);
	_figurine->DrawMode(DRAW_FRAGMENT);

	// Place the cube underneath
	_pedistal->SetPosition({ 0, -43.f, 0 });

	// Create light
	_directional = Environment::GetActive().CreateLight<DirectionalLight>().get();
	_directional->SetDirection(Vector3(-1.f, -1.f, 1.f));
	_directional->SetIntensity(Colour(1.f, 0.f, 0.f));

	// Create ambient light
	_ambient = Environment::GetActive().CreateLight<AmbientLight>().get();
	_ambient->SetIntensity(Colour(.1f, .1f, .1f));

	// Create point light
	_point = Environment::GetActive().CreateLight<PointLight>().get();
	_point->SetPosition(Vector3(0, 50, -50.f));
	_point->SetIntensity(Colour(.5f, .5f, 1.f));

	Camera::GetMainCamera()->SetPosition({ 0, 0, -50 });
	Camera::GetMainCamera()->SetRotation({ 0, 0, 0 });

	// Cache input manager's pointer.
	Input* const inputManager = Input::Get();
	
	// Register all inputs...
	inputManager->SaveAxis('A', 'D', "Horizontal");
	inputManager->SaveAxis('S', 'W', "Forward");
	inputManager->SaveAxis('C', VK_SPACE, "Vertical");

	inputManager->SaveAxis(VK_LEFT, VK_RIGHT, "X");
	inputManager->SaveAxis(VK_UP, VK_DOWN, "Y");
}

//
// Will be called on every frame.
//
void DefaultObject::OnTick(const float& deltaTime)
{
	Vector3 angle = { 0.f, .025f, 0.f };

	// A nice spinning animation!
	_figurine->Rotate(angle);

	// Move the camera, hurray!
	Vector3 cameraMovement{ 0, 0, 0 };
	Vector3 cameraRotation{ 0, 0, 0 };

	// Cache input manager's pointer.
	Input* const inputManager = Input::Get();

	cameraMovement.SetX(inputManager->GetAxis("Horizontal"));
	cameraMovement.SetY(inputManager->GetAxis("Vertical"));
	cameraMovement.SetZ(inputManager->GetAxis("Forward"));

	cameraRotation.SetX(inputManager->GetAxis("Y") * .01f);
	cameraRotation.SetY(inputManager->GetAxis("X") * .01f);

	Camera* const mainCamera = Camera::GetMainCamera();

	Vector3 cameraCurrentPosition = mainCamera->GetPosition();
	Vector3 cameraCurrentRotation = mainCamera->GetRotation();

	cameraCurrentPosition = cameraCurrentPosition + cameraMovement;
	cameraCurrentRotation = cameraCurrentRotation + cameraRotation;

	mainCamera->SetPosition(cameraCurrentPosition);
	mainCamera->SetRotation(cameraCurrentRotation);

	Vector3 lightDir = _directional->GetDirection();
	lightDir = Matrix::RotationMatrix(0.f, .1f, 0.f) * lightDir;

	_directional->SetDirection(lightDir);
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
