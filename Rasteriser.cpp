#include "Rasteriser.h"
#include "DefaultObject.h"
#include <cmath>
#include <algorithm>

Rasteriser app;

//
// Initialises the class and the shape vertices.
//
bool Rasteriser::Initialise()
{
	bool exitCode;

	try
	{
		InitialiseComponents();

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

	for (auto& sceneObject : _sceneObjects)
	{
		sceneObject->Draw(hdc);
	}
}

//
// Handles moving the square about
//
void Rasteriser::Tick(const Bitmap& bitmap, const float& deltaTime)
{
	_timeElapsed += deltaTime;

	for (auto& sceneObject : _sceneObjects)
	{
		sceneObject->OnTick(deltaTime);
	}
}

//
// The window (bitmap)'s background colour.
//
const COLORREF Rasteriser::GetBackgroundColour() const
{
	return _background;
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
void Rasteriser::InitialiseComponents()
{
	// Add default scene object
	CreateSceneObject<DefaultObject>();

	for (auto& sceneObject : _sceneObjects)
	{
		sceneObject->OnInit();
	}

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

//
// The time elapsed since the initialisation of this rasteriser.
//
const float& Rasteriser::GetTimeElapsed() const
{
	return _timeElapsed;
}

//
// Deletes an object from the scene.
//
void Rasteriser::DeleteSceneObject(const SceneObject& object)
{
	auto searchFunction = [&object](const std::unique_ptr<SceneObject>& obj) -> bool {
		return *obj == object;
	};

	auto obj_ptr = std::find_if(_sceneObjects.begin(), _sceneObjects.end(), searchFunction);

	if (obj_ptr == _sceneObjects.end())
	{
		throw std::exception("The object that is being deleted does not exist.");
	}

	(*obj_ptr)->OnDelete();
	_sceneObjects.erase(obj_ptr);
}
