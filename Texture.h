#pragma once
#include "windows.h"

/*
*
*	NOTE:
*	Texture loading seems to have some issues with compiler optimisations.
*	Since I do not have the time to delve deeper into what could be causing this,
*	all compiler optimisations have been disabled when building this executable.
*
*	This issue obviously does not apply in Debug mode.
*
*/
class Texture
{
public:
	Texture();
	~Texture();

	void		SetTextureSize(size_t width, size_t height);
	COLORREF	GetTextureValue(int u, int v) const;
	BYTE *		GetPaletteIndices();
	COLORREF *	GetPalette();
	size_t		GetWidth() const;
	size_t		GetHeight() const;

private:
	BYTE	 * _paletteIndices;
	COLORREF * _palette;
	size_t	 _width;
	size_t	 _height;
};