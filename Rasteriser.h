#pragma once
#include "Framework.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include "Camera.h"
#include "SceneObject.h"
#include "ModelLoadingException.h"

//
// Rasteriser class for handling drawing and mathematics operations.
//
class Rasteriser : public Framework
{
public:
	// Initialises the square corners.
	virtual bool Initialise() override;

	virtual void Render(const Bitmap& bitmap) override;
	virtual void Tick(const Bitmap& bitmap, const float& deltaTime) override;

	const COLORREF GetBackgroundColour() const;

	void Log(const char* const message) const;

	//
	// Input
	//
	bool IsKeyHeld(int keyCode);
	bool IsKeyDown(int keyCode);
	bool IsKeyUp(int keyCode);

	const float& GetTimeElapsed() const;

	//
	// Scene object management
	//
	template<class TSceneObj>
	const SceneObject& CreateSceneObject();
	void DeleteSceneObject(const SceneObject& object);

protected:
	//
	// Shape generation and handling.
	// 
	virtual void InitialiseComponents();

	// 
	// Drawing utils
	//
	void Clear(const COLORREF& colour, const Bitmap& bitmap);

private:
	Camera _camera;

	// Colours
	COLORREF _background = RGB(0xFF, 0xFF, 0xFF);
	float _timeElapsed = 0;

	// Every shape in the scene
	std::vector<std::unique_ptr<SceneObject>> _sceneObjects;
};

//
// Creates a new scene object, adds it to the objects list, returns it.
//
template<class TSceneObj>
const SceneObject& Rasteriser::CreateSceneObject()
{
	if constexpr (!std::is_base_of<SceneObject, TSceneObj>::value)
	{
		throw std::exception("Invalid type being created for scene object!");
	}

	_sceneObjects.push_back(std::make_unique<TSceneObj>(*this));
	TSceneObj& created = *dynamic_cast<TSceneObj*>(_sceneObjects.back().get());

	return created;
}