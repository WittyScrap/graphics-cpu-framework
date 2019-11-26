#include "Environment.h"
#include <algorithm>

//
// Active environment.
//
Environment* Environment::_activeEnvironment = nullptr;

//
// Default constructor.
//
Environment::Environment()
{ 
	_activeEnvironment = this;
}

//
// Destructor.
//
Environment::~Environment()
{ }

//
// Copy constructor - will and over every object pointer from the rhs
// class into this object.
//
Environment::Environment(Environment& rhs)
{
	*this = rhs;
}

//
// Assignment operator - will hand over every object pointer from the
// rhs environment class.
//
const Environment& Environment::operator=(Environment& rhs)
{
	_sceneObjects.clear();

	for (std::unique_ptr<SceneObject>& so : rhs._sceneObjects)
	{
		_sceneObjects.push_back(std::move(so));
	}

	return *this;
}

//
// Deletes an object from the environment.
//
const bool Environment::DeleteObject(SceneObject& sceneObject)
{
	auto searchFunction = [&sceneObject](const std::unique_ptr<SceneObject>& obj) -> bool {
		return *obj == sceneObject;
	};

	auto obj_ptr = std::find_if(_sceneObjects.begin(), _sceneObjects.end(), searchFunction);

	if (obj_ptr == _sceneObjects.end())
	{
		return false;
	}

	(*obj_ptr)->OnDelete();
	_sceneObjects.erase(obj_ptr);

	return true;
}

//
// Called upon initialisation.
//
void Environment::OnStart()
{
	for (auto& sceneObject : _sceneObjects)
	{
		sceneObject->OnStart();
	}
}

//
// Called every frame.
//
void Environment::OnTick(const float& deltaTime)
{
	for (auto& sceneObject : _sceneObjects)
	{
		sceneObject->OnTick(deltaTime);
	}
}

//
// Called when rendering is requested.
//
void Environment::OnRender(const HDC& hdc)
{
	for (auto& sceneObject : _sceneObjects)
	{
		sceneObject->Render(hdc);
	}
}

//
// The active environment.
//
Environment& Environment::GetActive()
{
	if (_activeEnvironment != nullptr)
	{
		return *_activeEnvironment;
	}
	else
	{
		throw std::exception("No active environment was present, but one was requested!");
	}
}
