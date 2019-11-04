#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

// Definition for static member
Camera* Camera::_mainCamera;

//
// Default constructor
//
Camera::Camera() : _worldToCameraMatrix(Matrix::IdentityMatrix())
{
	SetupMatrices(Matrix::IdentityMatrix());
}

//
// Destructor
//
Camera::~Camera()
{ }

//
// Copy constructor
//
Camera::Camera(const Camera& other)
{
	_worldToCameraMatrix = other._worldToCameraMatrix;
}

//
// The projection matrix.
//
const Matrix Camera::GetProjectionMatrix() const
{
	Matrix projectionMatrix;
	float d = 1 / std::tan((_fieldOfView * static_cast<float>(M_PI) / 180.f) / 2);

	if (!_isPerspective)
	{
		/*
		 *	1  0  0  0
		 *  0  1  0  0
		 *  0  0  0  d
		 *  0  0  0  1
		 */

		projectionMatrix = Matrix::IdentityMatrix();
		projectionMatrix.SetM(2, 2, 0);
		projectionMatrix.SetM(2, 3, d);
	}
	else // ^^^^^ Orthographic projection ^^^^^ / vvvvv Perspective projection vvvvv
	{
		/*
		 * d/a 0  0  0
		 *  0  d  0  0	Where:
		 *  0  0  d  0	  - a = width / height
		 *  0  0  1  0
		 */

		float aspect = static_cast<float>(Bitmap::GetActive()->GetWidth()) / static_cast<float>(Bitmap::GetActive()->GetHeight());
		projectionMatrix = Matrix::IdentityMatrix();
		projectionMatrix.SetM(0, 0, d / aspect);
		projectionMatrix.SetM(1, 1, d);
		projectionMatrix.SetM(2, 2, d);
		projectionMatrix.SetM(3, 3, 0);
		projectionMatrix.SetM(3, 2, 1);
	}

	return projectionMatrix;
}

//
// Matrix that converts objects from world-space to camera-space.
//
const Matrix& Camera::GetWorldToCameraMatrix() const
{
	return _worldToCameraMatrix;
}

//
// Matrix that converts objects from camera-space to world-space.
//
const Matrix Camera::GetCameraToWorldMatrix() const
{
	return _worldToCameraMatrix.Inverse();
}

//
// Matrix to transform coordinates from camera space to screen space.
//
const Matrix Camera::GetCameraToScreenMatrix() const
{
	Matrix cameraToScreenMatrix;

	/*
	 *	 w/2  0   0  w/2
	 *	  0 -h/2  0  h/2
	 *	  0   0  d/2 d/2
	 *	  0	  0   0   1
	 */
	float d = 1 / std::tan((_fieldOfView * static_cast<float>(M_PI) / 180.f) / 2);
	float width = static_cast<float>(Bitmap::GetActive()->GetWidth());
	float height = static_cast<float>(Bitmap::GetActive()->GetHeight());

	cameraToScreenMatrix.SetM(0, 0, width / 2);
	cameraToScreenMatrix.SetM(0, 3, width / 2);
	cameraToScreenMatrix.SetM(1, 1, -height / 2);
	cameraToScreenMatrix.SetM(1, 3, height / 2);
	cameraToScreenMatrix.SetM(2, 2, d / 2);
	cameraToScreenMatrix.SetM(2, 3, d / 2);
	cameraToScreenMatrix.SetM(3, 3, 1);

	return cameraToScreenMatrix;
}

//
// Matrix to transform coordinates from screen space to camera space.
//
const Matrix Camera::GetScreenToCameraMatrix() const
{
	return GetCameraToScreenMatrix().Inverse();
}

//
// The field of view
//
const float Camera::GetFieldOfView() const
{
	// To redo!
	return 360.f; // I'm a spider.
}

//
// Sets the field of view.
//
void Camera::SetFieldOfView(const float& fieldOfView)
{
	// To redo!
}

//
// Is the camera requesting a persective distortion?
//
const bool& Camera::IsPerspective() const
{
	return _isPerspective;
}

//
// Set whether or not the camera should perform perspective distortion.
//
void Camera::SetPerspective(const bool& toggle)
{
	_isPerspective = toggle;
}

//
// Sets the world to camera matrix to a valid initial state.
//
void Camera::SetupMatrices(const Matrix& initialTransform)
{
	_worldToCameraMatrix = initialTransform;
}

//
// Allows the camera to be placed in a certain position and rotation.
//
void Camera::Transform(const Vector3& position, const Vector3& rotation)
{
	_worldToCameraMatrix = Matrix::TRS(-position, -rotation, { 1, 1, 1 });
}

//
// Returns the main camera
//
Camera* const Camera::GetMainCamera()
{
	return _mainCamera;
}

//
// Sets this camera as the main camera.
//
void Camera::SetMain()
{
	_mainCamera = this;
}
