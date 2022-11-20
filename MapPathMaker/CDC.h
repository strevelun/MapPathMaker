#pragma once

#include "framework.h"

class CBitmap;

class CDC
{
	HDC m_hDC;

public:
	CDC(HDC hdc) : m_hDC(hdc) {}

	void Render(const CBitmap& bitmap);
	void Render(const CBitmap& bitmap, long dx, long dy, float multiple);
};