// MapPathMaker.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "MapPathMaker.h"
#include "CBitmap.h"

#define MAX_LOADSTRING 100
#define WIDTH           1000
#define HEIGHT          1000

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAPPATHMAKER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAPPATHMAKER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAPPATHMAKER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MAPPATHMAKER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU,
      CW_USEDEFAULT, 0, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

int map[HEIGHT / 64 - 1][WIDTH / 64 - 1] = { 0 };

typedef struct _tile
{
    int tileNum;
    int dirStatus;
} Tile;

Tile tileDirArr[16] = {
    {7, 1,}, //   0001   
    {11, 2}, //    0010    
    {3, 3}, //    0011    
    {13, 4}, //   0100   
    {5, 5}, //    0101    
    {9, 6}, //    0110    
    {1, 7}, //    0111    
    {14, 8}, //  1000   
    {6, 9}, //    1001    
    {10, 10}, //    1010   
    {2, 11}, //    1011    
    {12, 12}, //    1100   
    {4, 13}, //    1101    
    {8, 14}, //    1110    
    {0, 15}, //     1111  
    {-1, 16} // 10000 (16)
};

typedef struct _pos
{
    int x, y;
} Pos;

Pos checkDir[4] = {
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
    { -1, 0 }
};

void InitMap(HDC hdc, CDC cdc)
{
    for(int i=0; i<(WIDTH / 64) - 1; i++)
        for(int j=0; j<(HEIGHT / 64) - 1; j++)
            cdc.RenderSprite(hdc, i * 64, j * 64, 15);

    map[5][5] = 13+1;
    cdc.RenderSprite(hdc, 5 * 64, 5 * 64, 13);
}

void CheckAround(CDC cdc, HDC hdc, int locationX, int locationY, int j)
{
    int dirStatus;

    for (int i = 0; i < 16; i++)
        if (map[locationY][locationX] - 1 == tileDirArr[i].tileNum)
        {
            dirStatus = tileDirArr[i].dirStatus;
            break;
        }

    // 만약 연결부위에 아무 타일이 없으면 그 방향은 삭제
    if (map[locationY - 1][locationX] == 0)
        dirStatus &= 7; // 0111
    if (map[locationY][locationX + 1] == 0)
        dirStatus &= 11; // 1011
    if (map[locationY + 1][locationX] == 0)
        dirStatus &= 13; // 1101
    if (map[locationY][locationX - 1] == 0)
        dirStatus &= 14; // 1110

    if (j == 0) // 위쪽
        dirStatus |= 2; // 0010
    else if (j == 1) // 오른쪽
        dirStatus |= 1; // 0001
    else if (j == 2) // 아래
        dirStatus |= 8; // 1000
    else if (j == 3) // 왼쪽
        dirStatus |= 4; // 0100

    map[locationY][locationX] = tileDirArr[dirStatus-1].tileNum + 1;
    cdc.RenderSprite(hdc, locationX * 64, locationY * 64, tileDirArr[dirStatus-1].tileNum);
}

void PutRoad(CDC cdc, HDC hdc, int x, int y, int curTile)
{
    if (x >= WIDTH-64 || y >= HEIGHT-64)
        return;

    int locationX = x / 64;
    int locationY = y / 64;

    int checkIdx = 0;

    for (int j = 0; j < 4; j++)
    {
        if (locationY + checkDir[j].y < 0 || locationY + checkDir[j].y >= HEIGHT / 64 - 1
            || locationX + checkDir[j].x < 0 || locationX + checkDir[j].x >= WIDTH / 64 - 1)
            continue;
        
        if (map[locationY + checkDir[j].y][locationX + checkDir[j].x] != 0)
        {
            CheckAround(cdc, hdc, locationX + checkDir[j].x, locationY + checkDir[j].y, j);

            if (j == 0)
                checkIdx |= 8; // 1000
            else if (j == 1)
                checkIdx |= 4; // 0100
            else if (j == 2)
                checkIdx |= 2; // 0010
            else if (j == 3)
                checkIdx |= 1; // 0001
        }
    }

    if (checkIdx == 0) // 도로가 연결되지 않은 빈공간 클릭하면
    {
        checkIdx = 16;
        map[locationY][locationX] = tileDirArr[checkIdx - 1].tileNum + 1;
        cdc.RenderSprite(hdc, locationX * 64, locationY * 64, 15);
        return;
    }

    map[locationY][locationX] = tileDirArr[checkIdx-1].tileNum + 1;
    cdc.RenderSprite(hdc, locationX * 64, locationY * 64, tileDirArr[checkIdx-1].tileNum);
}

bool init = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static CBitmap bitmap(hWnd, L"image.bmp");
    static CDC cdc(&bitmap);
    static int curTile = 13;
    static int x = 0, y = 0;
    static bool buttonClicked = false;

    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_RBUTTONUP:
        buttonClicked = true;
        InvalidateRgn(hWnd, NULL, false);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (buttonClicked)
        {
            //cdc.RenderSprite(hdc, WIDTH - 100, 0, curTile);
            buttonClicked = false;
            break;
        }

        if (!init) {
            InitMap(hdc, cdc);
            init = true;
            break;
        }
        PutRoad(cdc, hdc, x, y, curTile);
        EndPaint(hWnd, &ps);
    }
        break;

    case WM_LBUTTONUP:
        x = GET_X_LPARAM(lParam);
        y = GET_Y_LPARAM(lParam);
        
        InvalidateRgn(hWnd, NULL, false);
        
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
