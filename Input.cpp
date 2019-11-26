#include "Input.h"
#include <Windows.h>

Input* Input::_instance;

//
// Default constructor.
//
Input::Input()
{
	_instance = this;
}

//
// Destructor.
//
Input::~Input()
{ }

//
// Returns true if the given key code is currently held down.
//
bool Input::IsKeyHeld(int keyCode) const
{
	SHORT keyState = GetKeyState(keyCode);
	return keyState & 0x8000;
}

//
// Returns true if the given key has just been pressed.
//
bool Input::IsKeyDown(int keyCode) const
{
	SHORT keyState = GetKeyState(keyCode);
	return keyState & (0x8000 | 1);
}

//
// Returns true if the given key has just been released.
//
bool Input::IsKeyUp(int keyCode) const
{
	SHORT keyState = GetKeyState(keyCode);
	if (!(keyState & 0x8000))
	{
		return keyState & 1;
	}
	else
	{
		return false;
	}
}

//
// Saves an axis pair.
//
void Input::SaveAxis(const int& a, const int& b, const std::string& name)
{
	_axis.insert(std::pair<std::string, InputAxis>(name, InputAxis(a, b)));
}

//
// Gets the value of an existing axis pair.
//
float Input::GetAxis(const std::string& name) const
{
	if (!_axis.contains(name))
	{
		return 0.f;
	}

	return _axis.at(name).Evaluate();
}

//
// The singleton input instance.
//
Input* const Input::Get()
{
	return _instance;
}
