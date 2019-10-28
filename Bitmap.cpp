#include "Bitmap.h"

Bitmap::Bitmap()
{
}

Bitmap::~Bitmap()
{
	// Ensure we clean up any existing bitmap
	DeleteBitmap();
}

// Create a new bitmap of the specified width and height, deleting any existing bitmap
//
// Returns value of false if bitmap cannot be created.

bool Bitmap::Create(HWND hWnd, unsigned int width, unsigned int height)
{
	bool status = false;
	HDC hDc;

	// Delete any existing bitmap
	DeleteBitmap();

	// Create a device context compatible with the window device context
	hDc = ::GetDC(hWnd);
	_width = width;
	_height = height;
	_hMemDC = CreateCompatibleDC(hDc);
	if (_hMemDC != 0)
	{
		// Create a bitmap compatible with the window
		_hBitmap = CreateCompatibleBitmap(hDc, _width, _height);
		if (_hBitmap != 0)
		{
			// Select the bitmap into the new device context, saving any old bitmap handle
			_hOldBitmap = static_cast<HBITMAP>(SelectObject(_hMemDC, _hBitmap));
			status = true;
		}
	}
	// Release the device context for the window
	ReleaseDC(hWnd, hDc);
	return status;
}

// Return device context of bitmap

HDC Bitmap::GetDC() const
{
	return _hMemDC;
}

// Return width of bitmap

unsigned int Bitmap::GetWidth() const
{
	return _width;
}

// Return height of bitmap

unsigned int Bitmap::GetHeight() const
{
	return _height;
}

// Delete any existing bitmap

void Bitmap::DeleteBitmap()
{
	// Select any default bitmap that existed for the device context
	if (_hOldBitmap != 0 && _hMemDC != 0)
	{
		SelectObject(_hMemDC, _hOldBitmap);
		_hOldBitmap = 0;
	}
	// Delete any existing bitmap
	if (_hBitmap != 0)
	{
		DeleteObject(_hBitmap);
		_hBitmap = 0;
	}
	// Delete any existing bitmap device context
	if (_hMemDC != 0)
	{
		DeleteDC(_hMemDC);
		_hMemDC = 0;
	}
}

// Clear bitmap using the specified brush

void Bitmap::Clear(HBRUSH hBrush) const
{
	RECT rect;

	rect.left = 0;
	rect.right = _width;
	rect.top = 0;
	rect.bottom = _height;
	FillRect(_hMemDC, &rect, hBrush);
}

// Clear bitmap using the specified colour

void Bitmap::Clear(COLORREF colour) const
{
	HBRUSH brush = CreateSolidBrush(colour);
	Clear(brush);
	DeleteObject(brush);
}
