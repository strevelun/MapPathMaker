#pragma once

#include "framework.h"

class CBitmap;
class CSprite;

class CDC
{
	CBitmap* m_bitmap;

public:
	CDC() {}
	CDC(CBitmap* bitmap);

	void Render(HDC hdc);
	void Render(HWND hWnd, long dx, long dy, float multiple);
	void RenderSprite(HDC hdc, int x, int y, int idx);
};