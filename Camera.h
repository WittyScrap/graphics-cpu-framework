#pragma once
#include "Matrix.h"
#include "Vector.h"
#include "Bitmap.h"
#include "Transformable.h"

//
// Scene camera.
//
class Camera
{
public:
	Camera();
	virtual ~Camera();

	// Copy constructor
	Camera(const Camera& other);

	//
	// Matrix to convert from camera space to screen space.
	//
	const Matrix GetProjectionMatrix() const;

	//
	// Matrices to convert from world to camera space and vice versa.
	//
	const Matrix& GetWorldToCameraMatrix() const;
	const Matrix  GetCameraToWorldMatrix() const;

	//
	// Field of view accessors.
	//
	const float GetFieldOfView() const;
	void SetFieldOfView(const float& fieldOfView);

	//
	// Toggle perspective.
	//
	const bool& IsPerspective() const;
	void SetPerspective(const bool& toggle);

	//
	// Allows the camera to be placed in a certain position and rotation
	//
	void SetPosition(const Vector3& position);
	void SetRotation(const Vector3& rotation);

	const Vector3 GetPosition() const;
	const Vector3 GetRotation() const;

	//
	// Main camera
	//
	static Camera* const GetMainCamera();
	void SetMain();

private:
	//
	// Matrix to convert points from projection space to screen space.
	//
	const Matrix GetProjectionToScreenMatrix() const;

private:
	//
	// View and screen matrix
	//
	Matrix _worldToCameraMatrix;

	//
	// Individual containers
	//
	Matrix _position;
	Matrix _rotation;

	//
	// The main camera object
	//
	static Camera* _mainCamera;

	//
	// Field of view and projection parameters
	//
	float _fieldOfView = 90.f;
	bool _isPerspective = true;
};

