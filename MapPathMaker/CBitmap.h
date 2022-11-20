 #pragma once

#include "CDC.h"

class CBitmap
{
    HBITMAP			m_bitmap;
    HANDLE			m_hFile;
	LPDWORD			m_pixels = NULL;
	HDC				m_hMemDC;

	long			m_width = 0, m_height = 0;

public:
	CBitmap(HWND hWnd, LPCWSTR fileName);

	HDC GetMemDC() const { return m_hMemDC; }
	long GetWidth() const { return m_width; }
	long GetHeight() const { return m_height; }
};

