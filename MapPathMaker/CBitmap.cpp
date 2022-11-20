#include "CBitmap.h"
#include "CSprite.h"
#include "framework.h"

CBitmap::CBitmap(HWND hWnd, LPCWSTR fileName)
{
    m_hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hFile);
        return;
    }

    m_sprites = new CSprite[16];

    HDC hdc = GetDC(hWnd);
    BITMAPFILEHEADER    bmFileHeader;
    DWORD               dwReadBytes;

    ReadFile(m_hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwReadBytes, NULL);

    int iBitmapInfoSize = bmFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
    BITMAPINFO* pBitmapInfo = (BITMAPINFO*) new BYTE[iBitmapInfoSize];
    ReadFile(m_hFile, (LPVOID)pBitmapInfo, iBitmapInfoSize, &dwReadBytes, NULL);

    m_width = pBitmapInfo->bmiHeader.biWidth;
    m_height = pBitmapInfo->bmiHeader.biHeight;

    m_bitmap = CreateDIBSection(hdc, pBitmapInfo, DIB_RGB_COLORS, (void**)&m_pixels, NULL, 0);
    ReadFile(m_hFile, m_pixels, pBitmapInfo->bmiHeader.biSizeImage, &dwReadBytes, NULL);

    m_hMemDC = CreateCompatibleDC(hdc);
    SelectObject(m_hMemDC, m_bitmap);

    int x = 10, y = 12;

    // 10, 12, width/height:63
    // 10, 90, 170, 250 => АЃАн 80 (width)
    // 12, 92, 172, 252
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m_sprites[(i * 4) + j] = CSprite(x + (j * 80), y + (i * 80), 64, 64);

    ReleaseDC(hWnd, hdc);
    CloseHandle(m_hFile);
}

CSprite CBitmap::GetSprites(int idx)
{
    return m_sprites[idx];
}
