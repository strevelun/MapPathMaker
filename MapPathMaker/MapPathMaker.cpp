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



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
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

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
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

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

int map[HEIGHT / 64 - 1][WIDTH / 64 - 1] = { 0 };

typedef struct _tile
{
    bool up, down, left, right;
    int canChangeTo[3] = { -1, -1, -1 };
} Tile;


Tile tileDirArr[15] = {
    {true, true, true, true, {0,-1,-1}},
    {false, true, true, true, {0,-1,-1}},
    {true, true, true, false, {0,-1,-1}},
    {false, true, true, false, {1,2,-1}},

    {true, false, true, true, {0,-1,-1}},
    {false, false, true, true, {1,4,-1}},
    {true, false, true, false, {2,4,-1}},
    {false, false, true, false, {3,5,6}},

    {true, true, false, true, {0,-1,-1}},
    {false, true, false, true, {1,8,-1}},
    {true, true, false, false, {2,8,-1}},
    {false, true, false, false, {3,9,10}},

    {true, false, false, true, {4,8,-1}},
    {false, false, false, true, {5,9,12}},
    {true, false, false, false, {6,10,12}},
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

    map[5][5] = 5+1;
    cdc.RenderSprite(hdc, 5 * 64, 5 * 64, 5);
}

void CheckAround(CDC cdc, HDC hdc, int locationX, int locationY)
{
    Tile curTileCheck = tileDirArr[map[locationY][locationX] - 1];


    for (int j = 0; j < 4; j++)
    {
        if (locationY + checkDir[j].y < 0 || locationY + checkDir[j].y >= HEIGHT / 64 - 1
            || locationX + checkDir[j].x < 0 || locationX + checkDir[j].x >= WIDTH / 64 - 1)
            continue;

        if (map[locationY + checkDir[j].y][locationX + checkDir[j].x] - 1 != -1) // 빈공간이 아닌 경우
        {
            Tile check = tileDirArr[map[locationY + checkDir[j].y][locationX + checkDir[j].x] - 1];

            if (j == 0)
            {
                if (check.down == false && curTileCheck.up == true)
                {
                    for (int i = 0; i < 3 && check.canChangeTo[i] != -1; i++)
                    {
                        if (tileDirArr[check.canChangeTo[i]].down == true)
                        {
                            map[locationY + checkDir[j].y][locationX + checkDir[j].x] = check.canChangeTo[i] + 1;
                            cdc.RenderSprite(hdc, (locationX + checkDir[j].x) * 64, (locationY + checkDir[j].y) * 64, check.canChangeTo[i]);

                        }
                    }
                }
                if (check.down == true && curTileCheck.up == false)
                {
                    for (int i = 0; i < 3 && curTileCheck.canChangeTo[i] != -1; i++)
                    {
                        if (tileDirArr[curTileCheck.canChangeTo[i]].up == true)
                        {
                            map[locationY][locationX] = curTileCheck.canChangeTo[i] + 1;
                            cdc.RenderSprite(hdc, locationX * 64, locationY * 64, curTileCheck.canChangeTo[i]);
                        }
                    }
                }
            }
            else if (j == 1)
            {
                if (check.left == false && curTileCheck.right == true)
                {
                    for (int i = 0; i < 3 && check.canChangeTo[i] != -1; i++)
                    {
                        if (tileDirArr[check.canChangeTo[i]].left == true)
                        {
                            map[locationY + checkDir[j].y][locationX + checkDir[j].x] = check.canChangeTo[i] + 1;
                            cdc.RenderSprite(hdc, (locationX + checkDir[j].x) * 64, (locationY + checkDir[j].y) * 64, check.canChangeTo[i]);

                        }
                    }
                }
                if (check.left == true && curTileCheck.right == false)
                {
                    for (int i = 0; i < 3 && curTileCheck.canChangeTo[i] != -1; i++)
                    {
                        if (tileDirArr[curTileCheck.canChangeTo[i]].right == true)
                        {
                            map[locationY][locationX] = curTileCheck.canChangeTo[i] + 1;
                            cdc.RenderSprite(hdc, locationX * 64, locationY * 64, curTileCheck.canChangeTo[i]);
                        }
                    }
                }
            }
            else if (j == 2)
            {
                if (check.up == false && curTileCheck.down == true)
                {
                    for (int i = 0; i < 3 && check.canChangeTo[i] != -1; i++)
                    {
                        if (tileDirArr[check.canChangeTo[i]].up == true)
                        {
                            map[locationY + checkDir[j].y][locationX + checkDir[j].x] = check.canChangeTo[i] + 1;
                            cdc.RenderSprite(hdc, (locationX + checkDir[j].x) * 64, (locationY + checkDir[j].y) * 64, check.canChangeTo[i]);

                        }
                    }
                }
                if (check.up == true && curTileCheck.down == false)
                {
                    for (int i = 0; i < 3 && curTileCheck.canChangeTo[i] != -1; i++)
                    {
                        if (tileDirArr[curTileCheck.canChangeTo[i]].down == true)
                        {
                            map[locationY][locationX] = curTileCheck.canChangeTo[i] + 1;
                            cdc.RenderSprite(hdc, locationX * 64, locationY * 64, curTileCheck.canChangeTo[i]);
                        }
                    }
                }
            }
            else if (j == 3)
            {
                if (check.right == false && curTileCheck.left == true)
                {
                    for (int i = 0; i < 3 && check.canChangeTo[i] != -1; i++)
                    {
                        if (tileDirArr[check.canChangeTo[i]].right == true)
                        {
                            map[locationY + checkDir[j].y][locationX + checkDir[j].x] = check.canChangeTo[i] + 1;
                            cdc.RenderSprite(hdc, (locationX + checkDir[j].x) * 64, (locationY + checkDir[j].y) * 64, check.canChangeTo[i]);

                        }
                    }
                }
                if (check.right == true && curTileCheck.left == false)
                {
                    for (int i = 0; i < 3 && curTileCheck.canChangeTo[i] != -1; i++)
                    {
                        if (tileDirArr[curTileCheck.canChangeTo[i]].left == true)
                        {
                            map[locationY][locationX] = curTileCheck.canChangeTo[i] + 1;
                            cdc.RenderSprite(hdc, locationX * 64, locationY * 64, curTileCheck.canChangeTo[i]);
                        }
                    }
                }
            }
        }
    }
}

void PutRoad(CDC cdc, HDC hdc, int x, int y, int curTile)
{
    // 마우스 누른 위치 좌표에 해당하는 칸에 스프라이트 넣기
    // x와 y가 64~127 사이라면 
    // 클릭할 때마다 idx 올라감

    if (x >= WIDTH-64 || y >= HEIGHT-64)
        return;

    int locationX = x / 64;
    int locationY = y / 64;

    map[locationY][locationX] = curTile + 1; // 15번이 0으로 저장됨

    // 먼저 상하좌우 체크. 타일만 바뀌는 경우
    cdc.RenderSprite(hdc, locationX * 64, locationY * 64, curTile);
    CheckAround(cdc, hdc, locationX, locationY);
   
    // 상에 있는 타일의 down이 false이고 자신의 타일의 up이 true인 경우 상에 있는 타일 변경하는데 변경될 후보 중 down이 true인 것으로 변경
    
    /*
    if (tileDirArr[map[locationY][locationX + 1] - 1].left == true) // 오른쪽 체크
    {

        curTile = tileDirArr[curTile].canChangeTo[0];
        map[locationY][locationX] = curTile+1;
    }
    */

    // 주변 확인하고 내가 놓을 타일이 올바른 모양인지 체크

    // 스프라이트를 놓았으면 상하좌우를 확인
    // 만약 3번 위에 7번을 놓았으면 하단을 체크할 때 제일 먼저 3번 입장에서 7번이 무시할 놈인지 판단.
    // 각 타일마다 고유의 상하좌우 flag. 이 flag는 각 타일이 어느 방향에서 놓으면 바뀔 수 있는지 알려줌.
    // 예를들어 1번은 up=true이기 때문에 1번의 위쪽에 놓은 타일의 down=true가 되어야 한다.
    // 만약 7번이 있고 7번 위에 3번을 놓았으면 7번이 바뀔 수 있는 후보를 살핀다. 3번과 5번은 up이 false이기 때문에 탈락.

    // 1. 1번 타일 위에 0번 타일을 놓는다.
    // 2. 아래를 살피는데, 1번이기 때문에 tileDirArr[1].up을 본다. 1번 타일의 up은 false이고, 0번 타일의 down은 true이기 때문에 

    // 3번 왼쪽에 2번을 놓으면 0이 되어야 한다.

}

bool init = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static CBitmap bitmap(hWnd, L"image.bmp");
    static CDC cdc(&bitmap);
    static int curTile = 0;
    static int x = 0, y = 0;
    
    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_RBUTTONUP:
        curTile = ++curTile % 16;
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (!init) {
            InitMap(hdc, cdc);
            init = true;
            break;
        }
        PutRoad(cdc, hdc, x, y, curTile);
        //PutRoad(cdc, hdc, x, y, 8);
        EndPaint(hWnd, &ps);
    }
        break;

    case WM_LBUTTONUP:
        x = GET_X_LPARAM(lParam);
        y = GET_Y_LPARAM(lParam);
        
        InvalidateRgn(hWnd, NULL, false);
        
        /*
        WCHAR text[100];
        wsprintf(text, L" x : %d \t y : %d", x, y);
        TextOutW(hdc, x + 5, y, text, lstrlen(text));
        */
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
