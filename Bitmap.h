#pragma once
#include "windows.h"

class Bitmap
{
public:
	Bitmap();
	~Bitmap();

	bool			Create(HWND hWnd, unsigned int width, unsigned int height);
	HDC				GetDC() const;
	unsigned int	GetWidth() const;
	unsigned int	GetHeight() const;
	void			Clear(HBRUSH hBrush) const;
	void			Clear(COLORREF colour) const;

private:
	HBITMAP			_hBitmap{ 0 };
	HBITMAP			_hOldBitmap{ 0 };
	HDC				_hMemDC{ 0 };
	unsigned int	_width{ 0 };
	unsigned int	_height{ 0 };

	void DeleteBitmap();
};

