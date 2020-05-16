/* Таймер */

#include <windows.h> 
#include <tchar.h>
#include <iostream>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  
TCHAR WinName[] = _T("MainFrame");		

HINSTANCE hInst;

int APIENTRY _tWinMain(HINSTANCE This,	HINSTANCE Prev,	LPTSTR cmd, int mode)		
{
	hInst = This;
	HWND hWnd;  
	MSG msg;	
	WNDCLASS wc;

	wc.hInstance = This;		
	wc.lpszClassName = WinName;	
	wc.lpfnWndProc = WndProc;	
	wc.style = CS_HREDRAW | CS_VREDRAW;	
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);	
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wc.lpszMenuName = NULL; 
	wc.cbClsExtra = 0;      
	wc.cbWndExtra = 0;		
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); 
	if (!RegisterClass(&wc)) return 0;		

	hWnd = CreateWindow(
		WinName, _T("Каркас Windows-приложения"),    	
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, NULL, This, NULL);								

	ShowWindow(hWnd, mode);					
	while (GetMessage(&msg, NULL, 0, 0))	
	{
		TranslateMessage(&msg);		
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{

			// Пример выбора файла для открытия
			
			TCHAR name[256] = _T("\0\0\0\0");
			OPENFILENAME file;
			file.lStructSize = sizeof(OPENFILENAME);
			file.hwndOwner = hWnd;
			file.hInstance = hInst;
			file.lpstrFilter = _T("Executable\0*.exe\0All files\0*.*\0");
			file.lpstrFile = name;
			file.nMaxFile = 256;
			file.lpstrInitialDir = _T(".\\");
			file.lpstrDefExt = _T("txt");
			
			file.lpstrTitle = _T("Открыть файл для чтения.");
			file.Flags = OFN_HIDEREADONLY;
			auto res = GetOpenFileName(&file);
			std::cout << "You'd like to open file: " << name << std::endl;


			// Сохранение:
			file.lpstrTitle = _T("Открыть файл для записи.");
			file.Flags = OFN_NOTESTFILECREATE;
			res = GetSaveFileName(&file);
			std::cout << "You'd like to save file: " << name << std::endl;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}