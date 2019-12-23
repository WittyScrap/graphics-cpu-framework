#include "Presentation.h"
#include "Environment.h"
#include "Rasteriser.h"
#define PI 3.14159265359f

//
// Setup phase.
//
void Presentation::OnInit()
{
	Environment::GetActive().SetBackgroundColour(Colour::White.AsColor());

	_displayText = CreateShape<TextShape>();
	_displayText->SetValue(L"Welcome to this presentation!");
	_displayText->SetColour(Colour::Black);

	_delay = 2.5f;

	Camera::GetMainCamera()->SetPosition({ 0, 0, -50 });
	Camera::GetMainCamera()->SetRotation({ 0, 0, 0 });
}

//
// Presentation cycle.
//
void Presentation::OnTick(const float& deltaTime)
{
	if (_doRotateFigurine && _figurine)
	{
		_figurine->Rotate(Vector3(0, 0.025f, 0));
	}

	if (_directional)
	{
		Vector3 lightDir = _directional->GetDirection();
		lightDir = Matrix::RotationMatrix(0.f, .1f, 0.f) * lightDir;

		_directional->SetDirection(lightDir);
	}

	if (_delay > 0)
	{
		_delay -= deltaTime;
		return;
	}

	switch (_state)
	{
	case PresentationStage::PHASE_BEGIN:
		BeginPhase(deltaTime);
		break;
	case PresentationStage::PHASE_WIREFRAME:
		WireframePhase(deltaTime);
		break;
	case PresentationStage::PHASE_MARVIN:
		MarvinPhase(deltaTime);
		break;
	case PresentationStage::PHASE_BOUNCE:
		BouncePhase(deltaTime);
		break;
	case PresentationStage::PHASE_BACKFACE:
		BackfacePhase(deltaTime);
		break;
	case PresentationStage::PHASE_POLYGON:
		PolygonPhase(deltaTime);
		break;
	case PresentationStage::PHASE_DIRECTIONAL:
		DirectionalPhase(deltaTime);
		break;
	case PresentationStage::PHASE_FLAT:
		FlatPhase(deltaTime);
		break;
	case PresentationStage::PHASE_VERTEX:
		VertexPhase(deltaTime);
		break;
	case PresentationStage::PHASE_FRAGMENT:
		FragmentPhase(deltaTime);
		break;
	case PresentationStage::PHASE_SPOT:
		SpotPhase(deltaTime);
		break;
	case PresentationStage::PHASE_POINT:
		PointPhase(deltaTime);
		break;
	case PresentationStage::PHASE_END:
		EndPhase(deltaTime);
		break;
	default:
		// How did we get here??
		return;
	}
}

//
// Will simply show some text and wait.
//
void Presentation::BeginPhase(const float& deltaTime)
{
	_displayText->SetValue(L"Let's start things off with a cube...");

	if (!_pedistal)
	{
		_pedistal = CreateShape<Mesh>();
		_pedistal->LoadFromFile("Meshes/cube.md2", "lines.pcx");
		_pedistal->SetColour(Colour::White);
		_pedistal->Mode(Mesh::DrawMode::DRAW_WIREFRAME);
		_pedistal->Shade(Mesh::ShadeMode::SHADE_FLAT); // For later...
		_pedistal->SetPosition({ 0, -43.f, 0 });
		_pedistal->SetScale({ 0, 0, 0 });
		_pedistal->Cull(false);
	}
	
	Vector3 cubeScale = _pedistal->GetTransform().GetScale();
	cubeScale += deltaTime;

	if (cubeScale.GetX() >= 1)
	{
		cubeScale = 1;
		_state = PresentationStage::PHASE_WIREFRAME;
		_delay = 2.f;
	}

	_pedistal->SetScale(cubeScale);
}

//
// Displays a simple cube wireframe.
//
void Presentation::WireframePhase(const float& deltaTime)
{
	_displayText->SetValue(L"This is a simple wireframe, and does not cull any polygons facing away from us.");

	if (_cubeRotationValue < 1.f)
	{
		_cubeRotationValue += deltaTime / 5.f;
		_pedistal->Rotate(Vector3(0, PI * deltaTime / 5.f, 0));
	}
	else
	{
		_pedistal->SetRotation(Vector3(0, 0, 0));
		_state = PresentationStage::PHASE_MARVIN;
	}
}

//
// It's marvin time!
//
void Presentation::MarvinPhase(const float& deltaTime)
{
	_displayText->SetValue(L"Let's add a little marvin to the play!");

	if (!_figurine)
	{
		_figurine = CreateShape<Mesh>();
		_figurine->LoadFromFile("Meshes/marvin.md2", "marvin.pcx");
		_figurine->SetColour(Colour::White);
		_figurine->Mode(Mesh::DrawMode::DRAW_WIREFRAME);
		_figurine->Shade(Mesh::ShadeMode::SHADE_FLAT); // For later...
		_figurine->SetPosition({ 0, 100.f, 0 });
		_figurine->Cull(false);
	}

	/* -- Drop phase -- */

	Vector3 position = _figurine->GetTransform().GetPosition();
	float positionY = position.GetY();
	positionY -= deltaTime * 100.f;

	if (positionY < 0)
	{
		positionY = 0;
		_state = PresentationStage::PHASE_BOUNCE;
	}

	position.SetY(positionY);
	_figurine->SetPosition(position);
}

//
// Make marvin bounce like those guys at Disney do (kind of).
//
void Presentation::BouncePhase(const float& deltaTime)
{
	_displayText->SetValue(L"Objects can move and scale on any axis independently!");

	if (_preSquish)
	{
		Vector3 scale = _figurine->GetTransform().GetScale();
		Vector3 position = _figurine->GetTransform().GetPosition();

		float scaleY = scale.GetY();
		float positionY = position.GetY();

		scaleY -= deltaTime * 4;
		positionY -= deltaTime * 100;

		if (scaleY < .5f)
		{
			scaleY = .5f;
			_preSquish = false;
		}

		scale.SetY(scaleY);
		position.SetY(positionY);

		_figurine->SetScale(scale);
		_figurine->SetPosition(position);
	}
	else if (_preBounce)
	{
		Vector3 scale = _figurine->GetTransform().GetScale();
		Vector3 position = _figurine->GetTransform().GetPosition();

		float scaleY = scale.GetY();
		float positionY = position.GetY();

		scaleY += deltaTime * 8;
		positionY += deltaTime * 200;

		if (scaleY > 2.f)
		{
			scaleY = 2.f;
		}

		if (positionY > 50.f)
		{
			positionY = 50.f;
			_preBounce = false;
		}

		scale.SetY(scaleY);
		position.SetY(positionY);

		_figurine->SetScale(scale);
		_figurine->SetPosition(position);
	}
	else
	{
		Vector3 scale = _figurine->GetTransform().GetScale();
		Vector3 position = _figurine->GetTransform().GetPosition();

		float scaleY = scale.GetY();
		float positionY = position.GetY();

		scaleY -= deltaTime * 8.f;
		positionY -= deltaTime * 100;

		if (scaleY < 1.f)
		{
			scaleY = 1.f;
		}

		if (positionY < 0.f)
		{
			positionY = 0.f;
			_state = PresentationStage::PHASE_BACKFACE;
			_delay = 5.f;
			_doRotateFigurine = true;
		}

		scale.SetY(scaleY);
		position.SetY(positionY);

		_figurine->SetScale(scale);
		_figurine->SetPosition(position);
	}
}

//
// Toggles culling ON for both meshes.
//
void Presentation::BackfacePhase(const float& deltaTime)
{
	_displayText->SetValue(L"Time to enable backface culling, now meshes only display polygons that are facing us. (State: Cull)");

	_figurine->Cull(true);
	_pedistal->Cull(true);

	_state = PresentationStage::PHASE_POLYGON;
	_delay = 5.f;
}

//
// Starts rendering polygons using Polygon flat shading.
//
void Presentation::PolygonPhase(const float& deltaTime)
{
	_displayText->SetValue(L"Polygons are now being filled in using Win32 Polygon calls. (State: Cull + Polygon)");

	_figurine->Mode(Mesh::DrawMode::DRAW_SOLID);
	_pedistal->Mode(Mesh::DrawMode::DRAW_SOLID);

	_state = PresentationStage::PHASE_DIRECTIONAL;
	_delay = 5.f;
}

//
// Enables a directional light and an ambient light, sets background to black.
//
void Presentation::DirectionalPhase(const float& deltaTime)
{
	_displayText->SetColour(Colour::White);
	_displayText->SetBackground(Colour::Black);
	_displayText->SetValue(L"Time to turn off the lights... (State: Cull + Polygon)");

	Environment::GetActive().SetBackgroundColour(Colour::Black.AsColor());

	if (_lightsDelay > 0)
	{
		_lightsDelay -= deltaTime;
		return;
	}

	_displayText->SetValue(L"...and turn on a directional one (and an ambient one, too). (State: Cull + Polygon + Directional/Ambient)");

	if (!_ambient)
	{
		_ambient = Environment::GetActive().CreateLight<AmbientLight>().get();
		_ambient->SetIntensity(Colour(.1f, .1f, .1f));
	}

	if (!_directional)
	{
		_directional = Environment::GetActive().CreateLight<DirectionalLight>().get();
		_directional->SetDirection(Vector3(-1.f, -1.f, 1.f));
		_directional->SetIntensity(Colour(1.f, 0.f, 0.f));
	}

	_state = PresentationStage::PHASE_FLAT;
	_delay = 5.f;
}

//
// Performs proper flat rendering.
//
void Presentation::FlatPhase(const float& deltaTime)
{
	_displayText->SetValue(L"The next features will now be showcased. (State: Cull + Flat (custom) + Ambient + Directional)");

	_figurine->Mode(Mesh::DrawMode::DRAW_FRAGMENT);
	_pedistal->Mode(Mesh::DrawMode::DRAW_FRAGMENT);

	_state = PresentationStage::PHASE_VERTEX;
	_delay = 5.f;
}

//
// Enables per-vertex smooth shading.
//
void Presentation::VertexPhase(const float& deltaTime)
{
	_displayText->SetValue(L"State: Cull + Gouraud + Ambient + Directional");

	_figurine->Shade(Mesh::ShadeMode::SHADE_GOURAUD);
	_pedistal->Shade(Mesh::ShadeMode::SHADE_GOURAUD);

	_state = PresentationStage::PHASE_FRAGMENT;
	_delay = 5.f;
}

//
// Enables per-fragment smooth shading.
//
void Presentation::FragmentPhase(const float& deltaTime)
{
	_displayText->SetValue(L"State: Cull + Phong + Textures + Ambient + Directional");

	_figurine->Shade(Mesh::ShadeMode::SHADE_PHONG);
	_pedistal->Shade(Mesh::ShadeMode::SHADE_PHONG);

	_state = PresentationStage::PHASE_SPOT;
	_delay = 5.f;
}

//
// Adds a spot light to the scene.
//
void Presentation::SpotPhase(const float& deltaTime)
{
	_displayText->SetValue(L"State: Cull + Phong + Textures + Ambient + Directional + Spot light");

	// Create spot light
	_spot = Environment::GetActive().CreateLight<SpotLight>().get();
	_spot->SetPosition(Vector3(70.f, 70.f, 10.f));
	_spot->SetIntensity(Colour::White * 0.5f);
	_spot->SetDirection(Vector3(-1, -1, 0));

	_state = PresentationStage::PHASE_POINT;
	_delay = 5.f;
}

//
// Adds a point light to the scene.
//
void Presentation::PointPhase(const float& deltaTime)
{
	_displayText->SetValue(L"State: Cull + Phong + Textures + Ambient + Directional + Spot light + Point light");

	// Create point light
	_point = Environment::GetActive().CreateLight<PointLight>().get();
	_point->SetPosition(Vector3(0, 50.f, -50.f));
	_point->SetIntensity(Colour(.25f, .25f, .5f));

	_state = PresentationStage::PHASE_END;
	_delay = 5.f;
}

void Presentation::EndPhase(const float& deltaTime)
{
	_displayText->SetValue(L"This concludes the presentation! (State: Cull + Phong + Textures + Ambient + Directional + Point light + Spot light)");
}
