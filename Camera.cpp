#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

// Definition for static member
const Camera* Camera::_mainCamera;

//
// Default constructor
//
Camera::Camera() : _projectionMatrix (Matrix::ZeroMatrix()), _worldToCameraMatrix(Matrix::IdentityMatrix()), _near(.1f), _far(1000.f)
{
	// Initial matrix setup
	SetupProjectionMatrix();
	SetupWorldToCameraMatrix({ 0.f, 0.f, 0.f });
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
	_projectionMatrix = other._projectionMatrix;
	_worldToCameraMatrix = other._worldToCameraMatrix;
}

//
// The projection matrix.
//
const Matrix& Camera::GetProjectionMatrix() const
{
	return _projectionMatrix;
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
// The field of view
//
const float Camera::GetFieldOfView() const
{
	float t = _projectionMatrix.GetM(1, 1);
	const float radToDegrees = 180.f / static_cast<float>(M_PI);
	return std::atan(1.f / t) * 2.f * radToDegrees;
}

//
// Sets the field of view.
//
void Camera::SetFieldOfView(const float& fieldOfView)
{
	float scale = 1.f / tan(fieldOfView * 0.5f * static_cast<float>(M_PI) / 180.f);

	_projectionMatrix.SetM(0, 0, scale); // scale the x coordinates of the projected point 
	_projectionMatrix.SetM(1, 1, scale); // scale the y coordinates of the projected point
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
	if (_isPerspective && !toggle)
	{
		_projectionMatrix = Matrix::IdentityMatrix();
	}
	
	if (!_isPerspective && toggle)
	{
		SetupProjectionMatrix();
	}

	_isPerspective = toggle;
}

//
// Sets up the projection matrix to a valid initial state.
//
void Camera::SetupProjectionMatrix()
{
	/*
	 ( x  0  a  0 )       x = 2*near/(right-left)          y = 2*near/(top-bottom)
	 ( 0  y  b  0 )       a = (right+left)/(right-left)    b = (top+bottom)/(top-bottom)
	 ( 0  0  c  d )       c = -(far+near)/(far-near)       d = -(2*far*near)/(far-near)
	 ( 0  0  e  0 )       e = 1
	*/

	float x = 2 * _near / (_right - _left);
	float y = 2 * _near / (_top - _bottom);
	float a = (_right + _left) / (_right - _left);
	float b = (_top + _bottom) / (_top - _bottom);
	float c = -(_far - _near) / (_far - _near);
	float d = -(2 * _far * _near) / (_far - _near);
	float e = -1;

	_projectionMatrix.SetM(0, 0, x);
	_projectionMatrix.SetM(1, 1, y);
	_projectionMatrix.SetM(0, 2, a);
	_projectionMatrix.SetM(1, 2, b);
	_projectionMatrix.SetM(2, 2, c);
	_projectionMatrix.SetM(3, 2, e);
}

//
// Sets the world to camera matrix to a valid initial state.
//
void Camera::SetupWorldToCameraMatrix(const Point3D<float>& initialPosition)
{
	_worldToCameraMatrix.SetM(0, 3, -initialPosition.X);
	_worldToCameraMatrix.SetM(1, 3, -initialPosition.Y);
	_worldToCameraMatrix.SetM(2, 3, -initialPosition.Z);
}

//
// Allows the camera to be placed in a certain position and rotation.
//
void Camera::Transform(const Point3D<float>& position, const Point3D<float>& rotation)
{
	_worldToCameraMatrix = Matrix::TRS(-position, -rotation, { 1, 1, 1 });
}

//
// Returns the main camera
//
const Camera* const Camera::GetMainCamera()
{
	return _mainCamera;
}

//
// Sets this camera as the main camera.
//
void Camera::SetMain() const
{
	_mainCamera = this;
}
