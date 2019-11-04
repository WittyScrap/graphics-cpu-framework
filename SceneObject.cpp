#include "SceneObject.h"
#include "Rasteriser.h"

//
// Default constructor.
//
SceneObject::SceneObject() : _world(NULL)
{ }

//
// Constructor that takes in the world it's been created in into consideration.
//
SceneObject::SceneObject(Rasteriser& world) : _world(&world)
{ }

//
// Called when the scene object is initialised.
//
void SceneObject::OnInit()
{
}

//
// Called on each pre-render tick.
//
void SceneObject::OnTick(const float& deltaTime)
{
}

//
// Called when the object is about to be destroyed.
//
void SceneObject::OnDelete()
{
}

//
// Internal equality function (by default, it will simply compare memory addresses).
//
const bool SceneObject::Equals(const SceneObject& rhs) const
{
	return this == &rhs;
}

//
// The rasteriser this object exists in.
//
Rasteriser& SceneObject::GetWorld() const
{
	return *_world;
}

//
// Full equality operator.
//
const bool operator==(const SceneObject& lhs, const SceneObject& rhs)
{
	return lhs.Equals(rhs);
}
