#include "Framework.h"

const unsigned int DEFAULT_FRAMERATE = 30;

// Reference to ourselves - primarily used to access the message handler correctly
// This is initialised in the constructor

Framework *	_thisFramework = NULL;

// Forward declaration of our window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_	   HINSTANCE hInstance,
				  	  _In_opt_ HINSTANCE hPrevInstance,
					  _In_	   LPWSTR    lpCmdLine,
					  _In_	   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// We can only run if an instance of a class that inherits from Framework
	// has been created
	if (_thisFramework)
	{
		return _thisFramework->Run(hInstance, nCmdShow);
	}
	return -1;
}

Framework::Framework() : Framework(800, 600)
{
}

Framework::Framework(unsigned int width, unsigned int height)
	: _hInstance(0), _hWnd(0), _width(width), _height(height)
{
	_thisFramework = this;
}

Framework::~Framework()
{
}

int Framework::Run(HINSTANCE hInstance, int nCmdShow)
{
	int returnValue;

	_hInstance = hInstance;
	if (!InitialiseMainWindow(nCmdShow))
	{
		return -1;
	}
	if (!Initialise())
	{
		return -1;
	}
	returnValue = MainLoop();
	Shutdown();
	return returnValue;
}

// Main program loop.  

int Framework::MainLoop()
{
	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(_hInstance, MAKEINTRESOURCE(IDC_RASTERISER));
	LARGE_INTEGER counterFrequency;
	LARGE_INTEGER nextTime;
	LARGE_INTEGER currentTime;
	LARGE_INTEGER lastTime;
	bool updateFlag = true;

	// Initialise timer
	QueryPerformanceFrequency(&counterFrequency);
	DWORD msPerFrame = static_cast<DWORD>(counterFrequency.QuadPart / DEFAULT_FRAMERATE);
	double timeFactor = 1.0 / counterFrequency.QuadPart;
	QueryPerformanceCounter(&nextTime);
	lastTime = nextTime;

	// Main message loop:
	msg.message = WM_NULL;
	while (msg.message != WM_QUIT)
	{
		if (updateFlag)
		{
			QueryPerformanceCounter(&currentTime);
			_timeSpan = (currentTime.QuadPart - lastTime.QuadPart) * timeFactor;
			lastTime = currentTime;
			Update(_bitmap);
			updateFlag = false;
		}
		QueryPerformanceCounter(&currentTime);
		// Is it time to render the frame?
		if (currentTime.QuadPart > nextTime.QuadPart)
		{
			Render(_bitmap);
			// Make sure that the window gets repainted
			InvalidateRect(_hWnd, NULL, FALSE);
			// Set time for next frame
			nextTime.QuadPart += msPerFrame;
			// If we get more than a frame ahead, allow one to be dropped
			// Otherwise, we will never catch up if we let the error accumulate
			// and message handling will suffer
			if (nextTime.QuadPart < currentTime.QuadPart)
			{
				nextTime.QuadPart = currentTime.QuadPart + msPerFrame;
			}
			updateFlag = true;
		}
		// Each time we go through this loop, we look to see if there is a Windows message
		// that needs to be processed
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return static_cast<int>(msg.wParam);
}

// Initialise the application.  Called after the window and bitmap has been
// created, but before the main loop starts
//
// Return false if the application cannot be initialised.

bool Framework::Initialise()
{
	return true;
}

// Perform any updates to the structures that will be used
// to render the window (i.e. transformation matrices, etc).
//
// This should be overridden

void Framework::Update(const Bitmap &bitmap)
{
	// Default update method does nothing
}

// Render the window. This should be overridden

void Framework::Render(const Bitmap &bitmap)
{
	// Default render method just sets the background to the default window colour
	bitmap.Clear(reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
}

// Perform any application shutdown that is needed

void Framework::Shutdown()
{
}

// Register the  window class, create the window and
// create the bitmap that we will use for rendering

bool Framework::InitialiseMainWindow(int nCmdShow)
{
	#define MAX_LOADSTRING 100

	WCHAR windowTitle[MAX_LOADSTRING];          
	WCHAR windowClass[MAX_LOADSTRING];            
	
	LoadStringW(_hInstance, IDS_APP_TITLE, windowTitle, MAX_LOADSTRING);
	LoadStringW(_hInstance, IDC_RASTERISER, windowClass, MAX_LOADSTRING);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(_hInstance, MAKEINTRESOURCE(IDI_RASTERISER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = windowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!RegisterClassExW(&wcex))
	{
		return false;
	}

	// Now work out how large the window needs to be for our required client window size
	RECT windowRect = { 0, 0, static_cast<LONG>(_width), static_cast<LONG>(_height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	_width = windowRect.right - windowRect.left;
	_height = windowRect.bottom - windowRect.top;

	_hWnd = CreateWindowW(windowClass, 
						  windowTitle, 
					      WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, CW_USEDEFAULT, _width, _height,
					      nullptr, nullptr, _hInstance, nullptr);
	if (!_hWnd)
	{
		return false;
	}
	ShowWindow(_hWnd, nCmdShow);
	UpdateWindow(_hWnd);

	// Create a bitmap of the same size as the client area of the window.  This is what we
	// will be drawing on
	RECT clientArea;
	GetClientRect(_hWnd, &clientArea);
	_bitmap.Create(_hWnd, clientArea.right - clientArea.left, clientArea.bottom - clientArea.top);
	return true;
}

// The WndProc for the current window.  This cannot be a method, but we can
// redirect all messages to a method.

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (_thisFramework != NULL)
	{
		// If framework is started, then we can call our own message proc
		return _thisFramework->MsgProc(hWnd, message, wParam, lParam);
	}
	else
	{
		// otherwise, we just pass control to the default message proc
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

// Our main WndProc

LRESULT Framework::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			{
				// Copy the contents of the bitmap to the window
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				BitBlt(hdc, 0, 0, _bitmap.GetWidth(), _bitmap.GetHeight(), _bitmap.GetDC(), 0, 0, SRCCOPY);
				EndPaint(hWnd, &ps);
			}
			break;

		case WM_SIZE:
			// Delete any existing bitmap and create a new one of the required size.
			_bitmap.Create(hWnd, LOWORD(lParam), HIWORD(lParam));
			// Now render to the resized bitmap
			Update(_bitmap);
			Render(_bitmap);
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

