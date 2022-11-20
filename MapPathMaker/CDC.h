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
	void RenderSprite(HWND hWnd, int x, int y, int idx);
};