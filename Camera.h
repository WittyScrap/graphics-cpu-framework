#pragma once
#include "Matrix.h"
#include "Vector.h"

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
	const Matrix& GetProjectionMatrix() const;
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
	void Transform(const Vector3& position, const Vector3& rotation);

	//
	// Main camera
	//
	static const Camera* const GetMainCamera();
	void SetMain() const;

private:
	//
	// Sets up the projection matrix to a valid initial state.
	//
	void SetupProjectionMatrix();
	void SetupWorldToCameraMatrix(const Vector3& initialPosition);

private:
	//
	// Internal matrices
	//
	Matrix _projectionMatrix;
	Matrix _worldToCameraMatrix;

	//
	// The main camera object
	//
	static const Camera* _mainCamera;

	//
	// Clip planes
	//
	float _far	= 0;
	float _near = 0;

	//
	// Offset
	//
	float _left		=  .01f;
	float _right	= -.01f;
	float _top		=  .01f;
	float _bottom	= -.01f;

	//
	// Options
	//
	bool _isPerspective = true;
};

