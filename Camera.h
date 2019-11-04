#pragma once
#include "Matrix.h"
#include "Vector.h"
#include "Bitmap.h"

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
	// Transformation matrices
	//
	const Matrix  GetProjectionMatrix() const;
	const Matrix& GetWorldToCameraMatrix() const;
	const Matrix  GetCameraToWorldMatrix() const;
	const Matrix  GetCameraToScreenMatrix() const;
	const Matrix  GetScreenToCameraMatrix() const;

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
	void Transform(const Vector3& position, const Vector3& rotation);

	//
	// Main camera
	//
	static Camera* const GetMainCamera();
	void SetMain();

private:
	//
	// Sets up the view matrix to a valid initial state.
	//
	void SetupMatrices(const Matrix& initialTransform);

private:
	//
	// View and screen matrix
	//
	Matrix _worldToCameraMatrix;

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

