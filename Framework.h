#pragma once
#include <windows.h>
#include "Resource.h"
#include "Bitmap.h"

using namespace std;

class Framework
{
public:
	Framework();
	Framework(unsigned int width, unsigned int height);
	virtual ~Framework();

	int Run(HINSTANCE hInstance, int nCmdShow);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual bool Initialise();
	virtual void Update(const Bitmap &bitmap);
	virtual void Render(const Bitmap &bitmap);
	virtual void Shutdown();

private:
	HINSTANCE		_hInstance;
	HWND			_hWnd;
	Bitmap			_bitmap;
	unsigned int	_width;
	unsigned int	_height;

	// Used in timing loop
	double			_timeSpan{ 0 };

	bool InitialiseMainWindow(int nCmdShow);
	int MainLoop();
};

