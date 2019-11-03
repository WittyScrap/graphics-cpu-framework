#include "Rasteriser.h"
#include "Point.h"
#include "MD2Loader.h"
#include <cmath>

Rasteriser app;

//
// Initialises the class and the shape vertices.
//
bool Rasteriser::Initialise()
{
	bool exitCode;

	try
	{
		CreateShape();

		// Everything went well (hopefully)...
		exitCode = true;
	}
	catch (ModelLoadingException& e)
	{
		Log(e.what());

		// Everything went disastrously wrong (expected behaviour).
		exitCode = false;
	}

	return exitCode;
}

//
// Renders the shape(s).
//
void Rasteriser::Render(const Bitmap& bitmap)
{
	COLORREF clearColour = GetBackgroundColour();
	HDC hdc = bitmap.GetDC();

	Clear(clearColour, bitmap);
	_shape.Draw(hdc);
}

//
// Handles moving the square about
//
void Rasteriser::Update(const Bitmap& bitmap, const float& deltaTime)
{
	// Rotate about everything...
	_shape.Rotate({ .025f, .05f, .025f });

	// Wobbly scale woo...
	float scaleFactor = std::sin(_timeElapsed * .5f) * 2;
	_shape.SetScale({ scaleFactor, scaleFactor, scaleFactor * scaleFactor });

	// Move around in circles, all together now
	_shape.SetPosition({ std::cos(_timeElapsed) * 100, std::sin(_timeElapsed * 2) * 100, 0 });

	_timeElapsed += deltaTime;

	// Move the camera, hurray!
	Vector3 cameraMovement{ 0, 0, 0 };

	if (IsKeyHeld(VK_LEFT))
	{
		cameraMovement.X -= 10.f;
	}

	if (IsKeyHeld(VK_RIGHT))
	{
		cameraMovement.X += 10.f;
	}

	if (IsKeyHeld(VK_UP))
	{
		cameraMovement.Y -= 10.f;
	}

	if (IsKeyHeld(VK_DOWN))
	{
		cameraMovement.Y += 10.f;
	}

	Vector3 cameraPosition = _camera.GetCameraToWorldMatrix().GetPosition();
	cameraPosition = cameraPosition + cameraMovement;

	_camera.Transform(cameraPosition, { 0, 0, 0 });
}

//
// The window (bitmap)'s background colour.
//
const COLORREF Rasteriser::GetBackgroundColour() const
{
	return _background;
}

//
// The colour for the shapes wireframe.
//
const COLORREF Rasteriser::GetForegroundColour() const
{
	return _shapeColor;
}

//
// Logs a message to the output console (Visual Studio only).
//
void Rasteriser::Log(const char* const message) const
{
	std::ostringstream ss;
	ss << message;
	ss << '\n';

	OutputDebugStringA(ss.str().c_str());
}

//
// Creates a square shape (default).
//
void Rasteriser::CreateShape()
{
	if (!MD2Loader::LoadModel("cube.md2", _shape, &Mesh::AddPolygon, &Mesh::AddVertex))
	{
		throw ModelLoadingException("cube.md2");
	}

	_shape.SetColour(GetForegroundColour());

	// Set the internal camera as main
	_camera.SetMain();
	_camera.Transform({ -400, -300, 0 }, { 0 });
	
	// Disable projection until it actually works
	_camera.SetPerspective(false);
}

//
// Clears the screen to a colour
//
void Rasteriser::Clear(const COLORREF& colour, const Bitmap& bitmap)
{
	HDC hdc = bitmap.GetDC();
	RECT fillRect;

	int width = bitmap.GetWidth();
	int height = bitmap.GetHeight();

	fillRect.left = 0;
	fillRect.top = 0;

	fillRect.right = width;
	fillRect.bottom = height;
	
	HBRUSH brush = CreateSolidBrush(colour);
	FillRect(hdc, &fillRect, brush);
	DeleteObject(brush);
}

//
// Returns true if the given key code is currently held down.
//
bool Rasteriser::IsKeyHeld(int keyCode)
{
	SHORT keyState = GetKeyState(keyCode);
	return keyState & 0x8000;
}

//
// Returns true if the given key has just been pressed.
//
bool Rasteriser::IsKeyDown(int keyCode)
{
	SHORT keyState = GetKeyState(keyCode);
	return keyState & (0x8000 | 1);
}

bool Rasteriser::IsKeyUp(int keyCode)
{
	SHORT keyState = GetKeyState(keyCode);
	if (!(keyState & 0x8000))
	{
		return keyState & 1;
	}
	else
	{
		return false;
	}
}
