#pragma once
#include "Transformable.h"
#include "Shape.h"
#include <string>
#include <memory>

// Forward declare rasteriser
class Rasteriser;

//
// Interface containing the necessary functions
// to handle a scene object.
//
class SceneObject
{
public:
	// Constructors
	SceneObject();
	SceneObject(const std::string& objectName);

	//
	// Events
	//
	virtual void OnStart();								// Called when the scene initialises
	virtual void OnInit();								// Called when the object is created
	virtual void OnTick(const float& deltaTime);		// Called every frame
	virtual void OnDelete();							// Called when the object is deleted

	//
	// Handles drawing any shape this object may contain
	//
	void Render(const HDC& hdc);

	//
	// Full equality operator.
	//
	friend const bool operator==(const SceneObject& lhs, const SceneObject& rhs);

	//
	// Internal shape management
	//
	template <class TShapeType>
	TShapeType* const CreateShape();
	void DestroyShape(const Shape& shape);

protected:
	// Equality operator internal function
	virtual const bool Equals(const SceneObject& rhs) const;

private:
	std::vector<std::unique_ptr<Shape>> _shapes;
	std::string _name;
};

//
// Creates a new shape object
//
template<class TShapeType>
inline TShapeType* const SceneObject::CreateShape()
{
	if constexpr (!std::is_base_of<Shape, TShapeType>::value)
	{
		throw std::exception("Invalid type being created for shape object!");
	}

	_shapes.push_back(std::make_unique<TShapeType>());
	TShapeType* const createdShape = dynamic_cast<TShapeType*>(_shapes.back().get());

	return createdShape;
}
