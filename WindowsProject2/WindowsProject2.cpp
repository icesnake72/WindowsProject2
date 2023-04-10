// WindowsProject2.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WindowsProject2.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define WM_FILECOPY     WM_USER+100

#define MAX_LOADSTRING  100

#define BUFFER_SIZE		4096

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

//
POINT pt;
WCHAR wszCopyInfo[512] = { 0 };

Image *pMyImage = NULL;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL OpenAndCopyFile(HWND hWnd);
void copyFile(HWND hWnd, LPCWSTR srcFileName, LPCWSTR destFileName);

BOOL OpenImageFile(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
           
    // Initialize GDI+.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT2));

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

    GdiplusShutdown(gdiplusToken);

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT2);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ZeroMemory(&pt, sizeof(POINT));

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{    

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_OPEN_SRCFILE:
                OpenAndCopyFile(hWnd);
                // ::MessageBoxW(hWnd, L"Hi", L"메시지", IDOK);
                break;

            case IDM_OPEN_IMAGE:
                OpenImageFile(hWnd);
                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_LBUTTONDOWN:
        ::SetCapture(hWnd);
        break;
        
    case WM_LBUTTONUP:
        ::ReleaseCapture();        
        break;


    case WM_MOUSEMOVE:
        if ((wParam & MK_LBUTTON))
        {
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);            
            ::InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

            //::TextOutW(hdc, 0, 0,  L"Windows Programming", lstrlenW(L"Windows Programming"));
            // wsprintfW(wszCopyInfo, L"x: %d, y: %d", pt.x, pt.y);
            ::TextOutW(hdc, 0, 0, wszCopyInfo, lstrlenW(wszCopyInfo));

            Graphics graphics(hdc);

            if ( pMyImage )
                graphics.DrawImage(pMyImage, 40, 40);
            
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        if (pMyImage)
        {
            delete[] pMyImage;
            pMyImage = NULL;
        }

        PostQuitMessage(0);
        break;

    case WM_FILECOPY:        
        ::InvalidateRect(hWnd, NULL, TRUE);
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

BOOL OpenAndCopyFile(HWND hWnd)
{
    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szSrcFile[512] = { 0 };

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szSrcFile;
    ofn.nMaxFile = sizeof(szSrcFile);
    ofn.lpstrFilter = L"All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        // use ofn.lpstrFile
        OPENFILENAME sfn;
        ZeroMemory(&sfn, sizeof(sfn));

        TCHAR szDestFile[512] = { 0 };

        sfn.lStructSize = sizeof(OPENFILENAME);
        sfn.hwndOwner = hWnd;
        sfn.lpstrFilter = L"All\0*.*\0";
        sfn.lpstrFile = szDestFile;
        sfn.nMaxFile = sizeof(szDestFile);
        sfn.lpstrFileTitle = NULL;
        sfn.nMaxFileTitle = 0;
        sfn.lpstrInitialDir = NULL;
        sfn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;       
        if (GetSaveFileName(&sfn) == TRUE)
        {
            copyFile(hWnd, (LPCWSTR)ofn.lpstrFile, (LPCWSTR)sfn.lpstrFile);
            ::MessageBox(hWnd, L"Done!!!", L"Message", IDOK);
            return TRUE;
        }
    }

    return FALSE;
}

void copyFile(HWND hWnd, LPCWSTR srcFileName, LPCWSTR destFileName)
{
    char* buf = NULL;
    // WCHAR *wszInfo = NULL;
    size_t readBytes = 0;
    size_t writtenBytes = 0;

    FILE* srcFile = NULL;
    FILE* destFile = NULL;

    errno_t error = NO_ERROR;

    // wszInfo = (WCHAR*)malloc(sizeof(WCHAR) * 100);

    buf = (char*)malloc(BUFFER_SIZE);
    if (buf == NULL)
    {
        // printf("메모리 할당 실패!!!\n");
        return;
    }

    error = _wfopen_s(&srcFile, srcFileName, L"r+b");
    if (error || srcFile==NULL)
    {
        // printf("원본파일을 찾을수 없습니다.\n");
        goto Release_Area;
    }

    error = _wfopen_s(&destFile, destFileName, L"w+b");	// destination file을 append모드로 열었을때는 파일이 있는지 체크를 해야함
    if (error || destFile==NULL)
    {
        // printf("대상 파일을 열 수 없습니다.\n");
        goto Release_Area;
    }

    if ( srcFile )
        fseek(srcFile, 0, SEEK_SET);	// 파일을 처음부터 순차적으로 읽어야 할때는 필수!!!

    while (!feof(srcFile))
    {
        readBytes = fread(buf, sizeof(char), BUFFER_SIZE, srcFile);
        if (readBytes)
        {
            // pfn((ulonglong)readBytes, READ_DATA);
            writtenBytes = fwrite(buf, sizeof(char), readBytes, destFile);
            wsprintfW(wszCopyInfo, L"%d Bytes Written...", writtenBytes);
            ::SendMessage(hWnd, WM_FILECOPY, 0, 0);
            // pfn((ulonglong)readBytes, WRITE_DATA);
        }
    }

    // printf("파일 복사가 완료되었습니다.\n");

Release_Area:

    if (buf)
    {
        free(buf);
        buf = NULL;
    }

    if (srcFile)
    {
        fclose(srcFile);
        srcFile = NULL;
    }

    if (destFile)
    {
        fclose(destFile);
        destFile = NULL;
    }

    return;
}

BOOL OpenImageFile(HWND hWnd)
{
    if (pMyImage)
    {
        delete[] pMyImage;
        pMyImage = NULL;
    }

    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szSrcFile[512] = { 0 };

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szSrcFile;
    ofn.nMaxFile = sizeof(szSrcFile);
    ofn.lpstrFilter = L"JPEG ImAGE\0*.jpg\0All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        pMyImage = new Image(ofn.lpstrFile);
        if (pMyImage)
        {
            return TRUE;
            ::InvalidateRect(hWnd, NULL, TRUE);
        }
            
    }
    
    return FALSE;
}