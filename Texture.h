#pragma once
#include "windows.h"

class Texture
{
public:
	Texture();
	~Texture();

	void		SetTextureSize(int width, int height);
	COLORREF	GetTextureValue(int u, int v) const;
	BYTE *		GetPaletteIndices();
	COLORREF *	GetPalette();
	int			GetWidth() const;
	int			GetHeight() const;

private:
	BYTE	 * _paletteIndices;
	COLORREF * _palette;
	int		   _width;
	int		   _height;
};