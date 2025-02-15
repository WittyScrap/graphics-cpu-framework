#include "Texture.h"

Texture::Texture()
{
	_width = 0;
	_height = 0;
	_paletteIndices = nullptr;
	_palette = nullptr;
}

Texture::~Texture()
{
	if (_paletteIndices != nullptr)
	{
		delete[] _paletteIndices;
		_paletteIndices = nullptr;
	}
	if (_palette != nullptr)
	{
		delete[] _palette;
		_palette = nullptr;
	}
}

void Texture::SetTextureSize(size_t width, size_t height)
{
	_width = width;
	_height = height;
	if (_paletteIndices != nullptr)
	{
		delete[] _paletteIndices;
	}
	_paletteIndices = new BYTE[_width * _height];
	if (_palette != nullptr)
	{
		delete[] _palette;
	}
	_palette = new COLORREF[256];
}

COLORREF Texture::GetTextureValue(int u, int v) const
{
	if (!_paletteIndices)
	{
		return RGB(255, 255, 255); // Default to white if no texture can be sampled.
	}

	if (v < 0)
	{
		v = 0;
	}
	if (v >= _height)
	{
		v = static_cast<int>(_height - 1);
	}
	if (u < 0)
	{
		u = 0;
	}
	if (u >= _width)
	{
		u = static_cast<int>(_width - 1);
	}
	return _palette[_paletteIndices[v * static_cast<int>(_width) + u]];
}

BYTE * Texture::GetPaletteIndices()
{
	return _paletteIndices;
}

COLORREF * Texture::GetPalette()
{
	return _palette;
}

size_t Texture::GetWidth() const
{
	return _width;
}

size_t Texture::GetHeight() const
{
	return _height;
}
