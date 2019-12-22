#include "Rasteriser.h"

// Output a string to the bitmap at co-ordinates 10, 10
// 
// Parameters: hdc	  - A the device context handler
//             text   - A pointer to a string of wide characters
//
// For example, you might call this using:
//
//   DrawString(bitmap, L"Text to display");

void Rasteriser::DrawString(HDC hdc, LPCTSTR text, COLORREF colour, COLORREF back)
{
	HFONT hFont, hOldFont;

	// Retrieve a handle to the variable stock font.  
	hFont = hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
							   CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Myfont"));

	// Select the variable stock font into the specified device context. 
	if (hOldFont = (HFONT)SelectObject(hdc, hFont))
	{
		SetTextColor(hdc, colour);
		SetBkColor(hdc, back);

		// Display the text string.  
		TextOut(hdc, 10, 10, text, lstrlen(text));

		// Restore the original font.        
		SelectObject(hdc, hOldFont);
	}
	DeleteObject(hFont);
}
