#include "CDC.h"
#include "CBitmap.h"


CDC::CDC(CBitmap* bitmap)
{
	m_bitmap = bitmap;
}

void CDC::Render(HDC hdc)
{
	m_bitmap->RenderBit(hdc);
}

void CDC::Render(HDC hdc, long dx, long dy, float multiple)
{
	m_bitmap->RenderStretch(hdc, dx, dy, multiple);
}

void CDC::RenderSprite(HDC hdc, int x, int y, int idx)
{
	m_bitmap->RenderSprite(hdc, x, y, idx);
}
