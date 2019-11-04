#pragma once
#include "Mesh.h"

// Forward declare rasteriser
class Rasteriser;

//
// Interface containing the necessary functions
// to handle a scene object.
//
class SceneObject : public Mesh
{
public:
	// Constructors
	SceneObject();
	SceneObject(Rasteriser& world);

	//
	// Events
	//
	virtual void OnInit();								// Initialisation
	virtual void OnTick(const float& deltaTime);		// Tick
	virtual void OnDelete();							// Object deleted

	//
	// Full equality operator.
	//
	friend const bool operator==(const SceneObject& lhs, const SceneObject& rhs);

protected:
	// Equality operator internal function
	virtual const bool Equals(const SceneObject& rhs) const;

	// The rasteriser this object exists in.
	Rasteriser& GetWorld() const;

private:
	// Reference to the world (rasteriser).
	Rasteriser* _world;
};

