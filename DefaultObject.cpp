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
	// Load mesh data...
	Mesh::LoadFromFile("cube.md2");
	SetColour(_shapeColour);
}

//
// Will be called on every frame.
//
void DefaultObject::OnTick(const float& deltaTime)
{
	// Deal with this later...
	return;

	// Rotate about everything...
	Rotate({ .025f, .05f, .025f });

	// Wobbly scale woo...
	float scaleFactor = std::sin(GetWorld().GetTimeElapsed() * .5f) * 2;
	SetScale({ scaleFactor, scaleFactor, scaleFactor * scaleFactor });

	// Move around in circles, all together now
	SetPosition({ std::cos(GetWorld().GetTimeElapsed()) * 100, std::sin(GetWorld().GetTimeElapsed() * 2) * 100, 0 });

	// Move the camera, hurray!
	Vector3 cameraMovement{ 0, 0, 0 };

	if (GetWorld().IsKeyHeld(VK_LEFT))
	{
		cameraMovement.X -= 10.f;
	}

	if (GetWorld().IsKeyHeld(VK_RIGHT))
	{
		cameraMovement.X += 10.f;
	}

	if (GetWorld().IsKeyHeld(VK_UP))
	{
		cameraMovement.Y -= 10.f;
	}

	if (GetWorld().IsKeyHeld(VK_DOWN))
	{
		cameraMovement.Y += 10.f;
	}

	Camera* const mainCamera = Camera::GetMainCamera();

	Vector3 cameraPosition = mainCamera->GetCameraToWorldMatrix().GetPosition();
	cameraPosition = cameraPosition + cameraMovement;

	mainCamera->Transform(cameraPosition, { 0, 0, 0 });
}
