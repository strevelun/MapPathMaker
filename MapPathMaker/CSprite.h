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

	int GetX() const { return x; }
	int GetY() const { return y; }
	int GetWidth() const { return w; }
	int GetHeight() const { return h; }
};

