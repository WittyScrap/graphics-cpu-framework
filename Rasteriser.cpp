#include "Rasteriser.h"
#include "DefaultObject.h"
#include <cmath>
#include <algorithm>

Rasteriser app;

//
// Initialises the class and the shape vertices.
//
bool Rasteriser::Initialise(const Bitmap& bitmap)
{
	bool exitCode;

	try
	{
		InitialiseComponents(bitmap);

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
	// No active bitmap = not yet fully ready to draw.
	if (!Bitmap::GetActive())
	{
		return;
	}

	COLORREF clearColour = _environment.GetBackgroundColour();
	HDC hdc = bitmap.GetDC();

	Clear(clearColour, bitmap);
	_environment.OnRender(hdc);
}

//
// Handles moving the square about
//
void Rasteriser::Tick(const Bitmap& bitmap, const float& deltaTime)
{
	_timeElapsed += deltaTime;
	_environment.OnTick(deltaTime);
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
void Rasteriser::InitialiseComponents(const Bitmap& bitmap)
{
	// Set the internal camera as main
	_camera.SetMain();

	// Add default scene object
	_start = _environment.CreateObject<DefaultObject>("Starter");
	_environment.OnStart();
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
// The time elapsed since the initialisation of this rasteriser.
//
const float& Rasteriser::GetTimeElapsed() const
{
	return _timeElapsed;
}