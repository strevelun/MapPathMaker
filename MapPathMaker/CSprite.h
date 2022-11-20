#pragma once

#include "CBitmap.h"

class CSprite
{
private:
	int x, y, w, h;
	int px, py;

public:
	CSprite() {}
	CSprite(int _x, int _y, int _w, int _h) :
		x(_x), y(_y), w(_w), h(_h), px(0), py(0)
	{}

	void Render(HDC dc, const CBitmap& bitmap) 
	{
		BitBlt(dc, 0, 0, w, h, bitmap.GetMemDC(), x, y, SRCCOPY);
	}
};

