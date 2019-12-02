#pragma once
#include "Framework.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include "Camera.h"
#include "Environment.h"
#include "ModelLoadingException.h"
#include "Input.h"

//
// Rasteriser class for handling drawing and mathematics operations.
//
class Rasteriser : public Framework
{
public:
	// Initialises the square corners.
	virtual bool Initialise(const Bitmap& bitmap) override;

	virtual void Render(const Bitmap& bitmap) override;
	virtual void Tick(const Bitmap& bitmap, const float& deltaTime) override;

	const COLORREF GetBackgroundColour() const;
	void Log(const char* const message) const;

	const float& GetTimeElapsed() const;

protected:
	//
	// Shape generation and handling.
	// 
	virtual void InitialiseComponents(const Bitmap& bitmap);

	// 
	// Drawing utils
	//
	void Clear(const COLORREF& colour, const Bitmap& bitmap);

private:
	Camera _camera;

	// Colours
	COLORREF _background = RGB(0x75, 0x75, 0x75);
	float _timeElapsed = 0;

	// Scene objects and render objects
	Environment _environment;
	Input _inputManager;
};
