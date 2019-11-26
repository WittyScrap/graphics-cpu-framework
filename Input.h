#pragma once
#include <unordered_map>

//
// Input pair controller
//
class InputAxis
{
public:
	InputAxis(int a, int b);
	virtual ~InputAxis();

	float Evaluate() const;
private:
	int _a;
	int _b;
};


//
// Input handler class.
//
class Input
{
public:
	Input();
	~Input();

	bool IsKeyHeld(int keyCode) const;
	bool IsKeyDown(int keyCode) const;
	bool IsKeyUp(int keyCode) const;

	void SaveAxis(const int& a, const int& b, const std::string& name);
	float GetAxis(const std::string& name) const;

	static Input* const Get();

private:
	static Input* _instance;
	std::unordered_map<std::string, InputAxis> _axis;
};
