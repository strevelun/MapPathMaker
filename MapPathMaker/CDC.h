#pragma once

#include "framework.h"

class CBitmap;
class CSprite;

class CDC
{
	CBitmap* m_bitmap;

public:
	CDC(CBitmap* bitmap);

	void Render(HWND hWnd);
	void Render(HWND hWnd, long dx, long dy, float multiple);
	void RenderSprite(HDC hdc, int x, int y, int idx);
};