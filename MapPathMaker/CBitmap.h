 #pragma once

#include "framework.h"

class CSprite;

class CBitmap
{
	HBITMAP			m_bitmap;
	HANDLE			m_hFile;
	LPDWORD			m_pixels = NULL;
	HDC				m_hMemDC;

	long			m_width = 0, m_height = 0;

	CSprite* m_sprites;

public:
	CBitmap() {}
	CBitmap(HWND hWnd, LPCWSTR fileName);

	void RenderBit(HDC hdc);
	void RenderStretch(HDC hdc, long dx, long dy, float multiple);
	void RenderSprite(HDC hdc, int x, int y, int idx);
};