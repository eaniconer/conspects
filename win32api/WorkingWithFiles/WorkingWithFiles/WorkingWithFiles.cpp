// WorkingWithFiles.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WorkingWithFiles.h"
#include <commdlg.h>
#include <commctrl.h>  // Нужно еще подлкючить comctl32.lib в проекте (Toolbar)
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);  

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_WORKINGWITHFILES, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WORKINGWITHFILES));

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
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
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WORKINGWITHFILES));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WORKINGWITHFILES);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle,
      WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// GLOBAL VARIABLES

const int LINE_HEIGHT = 16;

TBBUTTON buttons[] = {
    { STD_FILENEW, ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0 },
    { STD_FILEOPEN, ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0 },
    { STD_FILESAVE, ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0 },
    // {Индекс изображения, Индекс команды, начальное состояние, стиль кнопки, ... }
    // Нажатия на кнопки генерируют сообщения, которые можно достать при WM_COMMAND
    // у LOWORD(wParam)
};

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    const int N_MAX_SIZE = 256;
    static OPENFILENAME file;
    static TCHAR name[N_MAX_SIZE] = _T("");
    
    static std::vector<std::string> v;
    static int sx, sy;
    static SIZE size = { 8, 16 }; // Height and width of character/
    static int iVscrollPos(0);
    static int iHscrollPos(0);
    static int COUNT(0);
    static int MAX_WIDTH(0);
    static int length(0);
    static int toolbarHeight(0);

    static LOGFONT logFont;        // Хранение информации о выбранном шрифте
    static CHOOSEFONT chooseFont;  // Взаимодействие с функцией диалога
    static HFONT hFont;            // 

    static HWND hwndToolBar; // Дескриптор панели инструментов
    switch (message)
    {   
    case WM_CREATE:
        file.lStructSize = sizeof(OPENFILENAME);
        file.hwndOwner = hWnd;
        file.hInstance = hInst;
        file.lpstrFilter = _T("Text\0*.txt\0");
        file.lpstrFile = name;
        file.nMaxFile = N_MAX_SIZE;
        file.lpstrInitialDir = _T(".\\");
        file.lpstrDefExt = _T("txt");
        
        /* Подключить comctl32.lib */
        hwndToolBar = CreateToolbarEx(
            hWnd,   // Родительское окно
            WS_CHILD | WS_VISIBLE, // Флаги
            1, // Идентификатор дочернего окна панели инструментов
            0, // Число изображений в битовом образе 
            HINST_COMMCTRL, // Дескриптор ресурса битового образа для загрузки
            IDB_STD_SMALL_COLOR, // Идентификатор ресурса битового образа (16x16 кнопки)
            buttons,    // Указатель на массив структур TBUTTON
            3, // Число кнопок на панели
            0, // Ширина кнопок
            0, // Высота кнопок
            0, // Ширина кнопки битового образа в пикселях
            0, // Высота кнопки битового образа в пикселях 
            sizeof(TBBUTTON) // Размер структуры
            );
        
        /* Выбор шрифта */
        chooseFont.lStructSize = sizeof(CHOOSEFONT);
        chooseFont.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
        // CF_EFFECTS - позволяет определять зачеркнутый, подчеркнутый шрифт. Цвет текста.
        // CF_INITTOLOGFONTSTRUCT - использование структуры LOGFONT
        // CF_SCREENFONTS - список только экранных шрифтов, поддерживаемых системой
        chooseFont.hwndOwner = hWnd;
        chooseFont.lpLogFont = &logFont;

        break;
    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        {
            int k = v.size() - sy / size.cy;
            if (k > 0)
            {
                COUNT = k;
            }
            else
            {
                COUNT = iVscrollPos = 0;
            }
            SetScrollRange(hWnd, SB_VERT, 0, COUNT, FALSE);
            SetScrollPos(hWnd, SB_VERT, iVscrollPos, TRUE);

            k = length - sx / size.cx;
            if (k > 0)
            {   
                MAX_WIDTH = k;
            }
            else
            {
                MAX_WIDTH = iHscrollPos = 0;
            }
            SetScrollRange(hWnd, SB_HORZ, 0, MAX_WIDTH, FALSE);
            SetScrollPos(hWnd, SB_HORZ, iHscrollPos, TRUE);
        }

        SendMessage(hwndToolBar, TB_AUTOSIZE, 0, 0); // Корректировка размеров.
        {
            RECT rt;
            GetWindowRect(hwndToolBar, &rt);
            toolbarHeight = rt.bottom - rt.top;
        }
        break;
    case WM_VSCROLL:
        switch (LOWORD(wParam))
        {
        case SB_LINEUP: iVscrollPos--; break;
        case SB_LINEDOWN: iVscrollPos++; break;
        case SB_PAGEUP: iVscrollPos -= sy / size.cy; break;
        case SB_PAGEDOWN: iVscrollPos += sy / size.cy; break;
        case SB_THUMBPOSITION: iVscrollPos = HIWORD(wParam); break;
        }
        iVscrollPos = max(0, min(iVscrollPos, COUNT));
        if (iVscrollPos != GetScrollPos(hWnd, SB_VERT))
        {
            SetScrollPos(hWnd, SB_VERT, iVscrollPos, TRUE);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    case WM_HSCROLL:
        switch (LOWORD(wParam))
        {
        case SB_LINEUP: iHscrollPos--; break;
        case SB_LINEDOWN: iHscrollPos++; break;
        case SB_PAGEUP: iHscrollPos -= 8; break;
        case SB_PAGEDOWN: iHscrollPos += 8; break;
        case SB_THUMBPOSITION: iHscrollPos = HIWORD(wParam); break;
        }
        iHscrollPos = max(0, min(iHscrollPos, MAX_WIDTH));
        if (iHscrollPos != GetScrollPos(hWnd, SB_HORZ))
        {
            SetScrollPos(hWnd, SB_HORZ, iHscrollPos, TRUE);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case ID_FILE_NEW:
            if (!v.empty()) std::vector<std::string>().swap(v);
            length = 0;
            SendMessage(hWnd, WM_SIZE, 0, sy << 16 | sx);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case ID_FILE_OPEN:
            file.lpstrTitle = _T("Открыть файл для чтения");
            file.Flags = OFN_HIDEREADONLY;
            if (!GetOpenFileName(&file))
            {
                return 1;
            }
            else
            {
                std::ifstream in(name);
                std::string st;
                while (std::getline(in, st))
                {
                    v.push_back(st);
                    length = max(st.length(), length);
                }
                SendMessage(hWnd, WM_SIZE, 0, sy << 16 | sx);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        case ID_FILE_SAVE:
            file.lpstrTitle = _T("Сохранить как...");
            file.Flags = OFN_NOTESTFILECREATE;
            if (!GetSaveFileName(&file))
            {
                return 1;
            }
            else
            {
                std::ofstream out(name);
                for (const auto& s : v)
                {
                    out << s << '\n';
                }
            }
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
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code here...
        {
            int y = toolbarHeight;
            for (auto it = v.begin() + iVscrollPos; it != v.end() && y < sy; ++it, y += size.cy)
            {
                if (iHscrollPos < it->length())
                {
                    TabbedTextOutA(hdc, 0, y, it->data() + iHscrollPos, it->length() - iHscrollPos, 0, NULL, 0);
                }
            }
        }
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
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
