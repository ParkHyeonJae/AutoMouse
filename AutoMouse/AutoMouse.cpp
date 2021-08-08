// AutoMouse.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "AutoMouse.h"

#pragma warning(disable: 4996)

#define MAX_LOADSTRING 100
void WCharToChar(const wchar_t* pwstrSrc, char pstrDest[])
{
    int nLen = (int)wcslen(pwstrSrc);
    wcstombs(pstrDest, pwstrSrc, nLen + 1);
}

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    /*UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);*/

    g_EscapeThread = std::thread(ConsoleInputObserver);

    char dst[10];
    ZeroMemory(dst, 10);
    WCharToChar(lpCmdLine, dst);

    g_duration = atoi(dst);
    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AUTOMOUSE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AUTOMOUSE));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AUTOMOUSE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
static HWND m_hWnd = nullptr;
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 400, 100, nullptr, nullptr, hInstance, nullptr);
    
   if (!hWnd)
   {
      return FALSE;
   }
   m_hWnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   

   return TRUE;
}

void ConsoleInputObserver()
{
    while (g_auto_enable)
    {
        if (_getch()) {
            g_auto_enable = false;
            PostQuitMessage(0);
        }
    }
}

void SetActive()
{
    if (::GetForegroundWindow() != m_hWnd) {
        HWND h_active_wnd = ::GetForegroundWindow();
        if (h_active_wnd != NULL) {
            DWORD thread_id = GetWindowThreadProcessId(h_active_wnd, NULL);
            DWORD current_thread_id = GetCurrentThreadId();
            if (current_thread_id != thread_id) {
                if (AttachThreadInput(current_thread_id, thread_id, TRUE)) {
                    BringWindowToTop(m_hWnd);
                    AttachThreadInput(current_thread_id, thread_id, FALSE);
                }
            }
        }
    }
}
void TryActiveForegroundWindow()
{
    while (g_auto_enable)
    {
        for (size_t i = 0; i < 5000; i++)
        {
            if (::GetForegroundWindow() != m_hWnd) {
                SetActive();
            }
        }
        
        std::this_thread::yield();
    }
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    int x = 0, y = 0;
    bool result = false;
    INPUT Inputs[3] = { 0 };
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
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
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_SPACE:
            if (g_auto_enable)
                g_auto_enable = !g_auto_enable;
            else g_auto_enable = !g_auto_enable;

            if (g_auto_enable) {
                g_TryThread = std::thread(TryActiveForegroundWindow);
                SetTimer(m_hWnd, 0, g_duration, NULL);
            }
            else {
                KillTimer(hWnd, 0);
                if (g_TryThread.joinable())
                    g_TryThread.join();
            }
            break;
        case VK_ESCAPE:
            g_auto_enable = false;
            PostQuitMessage(0);
        default:
            break;
        }
        break;
    case WM_TIMER:
        mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        if (g_EscapeThread.joinable())
            g_EscapeThread.join();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



// Message handler for about box.
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
