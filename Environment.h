#pragma once
#include "Shape.h"
#include "Light.h"
#include "SceneObject.h"
#include <vector>
#include <string>
#include <type_traits>

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
	
	template<typename TObjType>
	std::shared_ptr<TObjType> CreateObject(const std::string& objectName);
	const bool DeleteObject(std::shared_ptr<SceneObject> sceneObject);

	template<class TLightType>
	std::shared_ptr<TLightType> CreateLight();
	const bool DeleteLight(std::shared_ptr<Light> sceneLight);
	const std::vector<std::shared_ptr<Light>>& GetSceneLights() const;

	void OnStart();
	void OnTick(const float& deltaTime);
	void OnRender(const HDC& hdc);

	static Environment& GetActive();

private:
	static Environment* _activeEnvironment;

	std::vector<std::shared_ptr<SceneObject>> _sceneObjects;
	std::vector<std::shared_ptr<Light>> _sceneLights;
};

//
// Creates a new scene object, adds it to the objects list, returns it.
//
template<typename TObjType>
inline std::shared_ptr<TObjType> Environment::CreateObject(const std::string& objectName)
{
	if constexpr (!std::is_base_of<SceneObject, TObjType>::value)
	{
		throw std::exception("Invalid type being created for scene object!");
	}

	_sceneObjects.push_back(std::make_shared<TObjType>());
	std::shared_ptr<TObjType> created = std::dynamic_pointer_cast<TObjType>(_sceneObjects.back());

	// OnInit...
	created->OnInit();

	return created;
}

//
// Generates a new light source of a given type.
//
template<class TLightType>
inline std::shared_ptr<TLightType> Environment::CreateLight()
{
	if constexpr (!std::is_base_of<Light, TLightType>::value)
	{
		throw std::exception("Invalid type being created for scene object!");
	}

	_sceneLights.push_back(std::make_shared<TLightType>());
	return std::dynamic_pointer_cast<TLightType>(_sceneLights.back());
}
