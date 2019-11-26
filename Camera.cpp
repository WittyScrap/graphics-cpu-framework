#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

// Definition for static member
Camera* Camera::_mainCamera;

//
// Default constructor
//
Camera::Camera() : _worldToCameraMatrix(Matrix::IdentityMatrix()),
				   _position(Matrix::IdentityMatrix()),
				   _rotation(Matrix::IdentityMatrix())
{ }

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
// The projection matrix, transforms a point from camera space directly into screen
// space (merges cam2proj and proj2screen matrices into one function call).
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
// Matrix to transform coordinates from projection space to screen space.
//
const Matrix Camera::GetProjectionToClipMatrix() const
{
	Matrix projectionToScreenMatrix;

	/*
	 *	 w/2  0   0  w/2
	 *	  0 -h/2  0  h/2
	 *	  0   0  d/2 d/2
	 *	  0	  0   0   1
	 */
	float d = 1 / std::tan((_fieldOfView * static_cast<float>(M_PI) / 180.f) / 2);
	float width = static_cast<float>(Bitmap::GetActive()->GetWidth());
	float height = static_cast<float>(Bitmap::GetActive()->GetHeight());

	projectionToScreenMatrix.SetM(0, 0, width / 2);
	projectionToScreenMatrix.SetM(0, 3, width / 2);
	projectionToScreenMatrix.SetM(1, 1, -height / 2);
	projectionToScreenMatrix.SetM(1, 3, height / 2);
	projectionToScreenMatrix.SetM(2, 2, d / 2);
	projectionToScreenMatrix.SetM(2, 3, d / 2);
	projectionToScreenMatrix.SetM(3, 3, 1);

	return projectionToScreenMatrix;
}

//
// The field of view
//
const float Camera::GetFieldOfView() const
{
	return _fieldOfView;
}

//
// Sets the field of view.
//
void Camera::SetFieldOfView(const float& fieldOfView)
{
	_fieldOfView = fieldOfView;
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
// Sets the internal position and world2cam matrices
//
void Camera::SetPosition(const Vector3& position)
{
	_position = {
		1, 0, 0, -position.GetX(),
		0, 1, 0, -position.GetY(),
		0, 0, 1, -position.GetZ(),
		0, 0, 0, 1
	};

	_worldToCameraMatrix = _position * _rotation;
}

//
// Sets the internal rotation and world2cam matrices
//
void Camera::SetRotation(const Vector3& rotation)
{
	Matrix rotationX = {
		   1,		 	   0,					    0,		         0,
		   0,  std::cos(rotation.GetX()), std::sin(rotation.GetX()), 0,
		   0, -std::sin(rotation.GetX()), std::cos(rotation.GetX()), 0,
		   0,			   0,					    0,			     1
	};

	Matrix rotationY = {
		std::cos(rotation.GetY()), 0, -std::sin(rotation.GetY()), 0,
				   0,			   1,		      0,			  0,
		std::sin(rotation.GetY()), 0,  std::cos(rotation.GetY()), 0,
				   0,			   0,		      0,			  1
	};

	Matrix rotationZ = {
		 std::cos(rotation.GetZ()), std::sin(rotation.GetZ()), 0, 0,
		-std::sin(rotation.GetZ()), std::cos(rotation.GetZ()), 0, 0,
					0,							0,			   1, 0,
					0,							0,			   0, 1
	};

	_rotation = rotationX * rotationY * rotationZ;
	_worldToCameraMatrix = _position * _rotation;
}

//
// The position of this camera
//
const Vector3 Camera::GetPosition() const
{
	return -_position.GetPosition();
}

//
// The rotation of this camera
//
const Vector3 Camera::GetRotation() const
{
	return -_rotation.GetRotation();
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
