#pragma once
#include "Shape.h"
#include "SceneObject.h"
#include <vector>
#include <string>

//
// Environment:
//	Defines the collection of all scene objects and logic
//	modules currently present in the existing window context.
//
class Environment
{
public:
	Environment();
	~Environment();

	Environment(Environment& rhs);
	const Environment& operator=(Environment& rhs);
	
	template<class TObjType>
	const TObjType& CreateObject(const std::string& objectName);
	const bool DeleteObject(SceneObject& sceneObject);

	void OnStart();
	void OnTick(const float& deltaTime);
	void OnRender(const HDC& hdc);

	static Environment& GetActive();

private:
	static Environment* _activeEnvironment;
	std::vector<std::unique_ptr<SceneObject>> _sceneObjects;
};

//
// Creates a new scene object, adds it to the objects list, returns it.
//
template<class TObjType>
inline const TObjType& Environment::CreateObject(const std::string& objectName)
{
	if constexpr (!std::is_base_of<SceneObject, TObjType>::value)
	{
		throw std::exception("Invalid type being created for scene object!");
	}

	_sceneObjects.push_back(std::make_unique<TObjType>());
	TObjType& created = *dynamic_cast<TObjType*>(_sceneObjects.back().get());

	// OnInit...
	created.OnInit();

	return created;
}
