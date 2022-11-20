#include "CDC.h"
#include "CBitmap.h"
#include <math.h>

void CDC::Render(const CBitmap& bitmap)
{
	BitBlt(m_hDC, 0, 0, bitmap.GetWidth(), bitmap.GetHeight(), bitmap.GetMemDC(), 0, 0, SRCCOPY);
}

void CDC::Render(const CBitmap& bitmap, long dx, long dy, float multiple)
{
	int width = bitmap.GetWidth();
	int height = bitmap.GetHeight();

	StretchBlt(m_hDC, dx, dy, width * multiple, height * multiple, bitmap.GetMemDC(), 0, 0, width, height, SRCCOPY);
}
