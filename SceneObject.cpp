#include "SceneObject.h"
#include "Rasteriser.h"
#include <algorithm>

//
// Default constructor.
//
SceneObject::SceneObject() : _name("")
{ }

//
// Constructor to define an object name.
//
SceneObject::SceneObject(const std::string& objectName) : _name(objectName)
{ }

//
// Called when the scene is initialised.
//
void SceneObject::OnStart()
{ }

//
// Called when the scene object is initialised.
//
void SceneObject::OnInit()
{ }

//
// Called on each pre-render tick.
//
void SceneObject::OnTick(const float& deltaTime)
{ }

//
// Called when the object is about to be destroyed.
//
void SceneObject::OnDelete()
{ }

//
// Draws any shape held within this object.
//
void SceneObject::Render(const HDC& hdc)
{
	for (auto& shape : _shapes)
	{
		shape->Draw(hdc);
	}
}

//
// Destroys a previously created shape.
//
void SceneObject::DestroyShape(const Shape& shape)
{
	auto searchFunction = [&shape](const std::unique_ptr<Shape>& obj) -> bool {
		return *obj == shape;
	};

	auto obj_ptr = std::find_if(_shapes.begin(), _shapes.end(), searchFunction);

	if (obj_ptr == _shapes.end())
	{
		throw std::exception("The object that is being deleted does not exist.");
	}

	_shapes.erase(obj_ptr);
}

//
// Internal equality function (by default, it will simply compare memory addresses).
//
const bool SceneObject::Equals(const SceneObject& rhs) const
{
	return this == &rhs;
}

//
// Full equality operator.
//
const bool operator==(const SceneObject& lhs, const SceneObject& rhs)
{
	return lhs.Equals(rhs);
}
