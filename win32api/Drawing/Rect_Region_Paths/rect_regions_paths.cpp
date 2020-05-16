/* Таймер */

#include <windows.h> 
#include <tchar.h>

#include <iostream>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  
TCHAR WinName[] = _T("MainFrame");		

int APIENTRY _tWinMain(HINSTANCE This,	HINSTANCE Prev,	LPTSTR cmd, int mode)		
{
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
		WinName, _T("RectRegionsAndPaths"),    	
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, NULL, This, NULL);								

	ShowWindow(hWnd, mode);					
	while (GetMessage(&msg, NULL, 0, 0))	
	{
		TranslateMessage(&msg);		
		DispatchMessage(&msg);
	}
	return 0;
}


RECT pRect = {-100, -100, 100, 100};
RECT pEllips = {-120, -80, 120, 80};
RECT pSm = {-60, -40, 60, 40 };
const int WIDTH = 400;
POINT pt[5] = { {0, 100}, {-59, -81}, {95, 31}, {-95, 31}, {59, -81} };


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static int sx, sy;
	HRGN hRgnEllispe; // Регионы
	HRGN hRgn; 
	static HBRUSH hBrush;

	switch (message)
	{
	case WM_SIZE:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		break;
	case WM_CREATE:
	 	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	 	break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, WIDTH, -WIDTH, NULL);
		SetViewportExtEx(hdc, sx, sy, NULL);
		SetViewportOrgEx(hdc, sx / 2, sy / 2, NULL);

		if (false) {
		hRgn = CreateRectRgnIndirect(&pRect);
		hRgnEllispe = CreateEllipticRgnIndirect(&pEllips);

		CombineRgn(hRgn, hRgn, hRgnEllispe, RGN_DIFF);
		DeleteObject(hRgnEllispe);

		hRgnEllispe = CreateEllipticRgnIndirect(&pSm);
		CombineRgn(hRgn, hRgn, hRgnEllispe, RGN_OR);
		DeleteObject(hRgnEllispe);
		FillRgn(hdc, hRgn, hBrush);
		DeleteObject(hRgn);
		}
		// Работа с путями
		/*Открывает путь, теперь графические функции
		ничего не выводят, а строят путь*/
		BeginPath(hdc);

		Polyline(hdc, pt, 5);
		/*Закрывает открытую фигуру в пути.
		 Замыкает первую и последнюю точки.*/
		CloseFigure(hdc);
		/*Закрывает путь и помещает его в контекст устройства.*/
		EndPath(hdc);

		SelectObject(hdc, hBrush);
		/* По умолчанию строит ALTERNATE режим. 
		Если его оставить то окрасятся только лепестки звезды */
		SetPolyFillMode(hdc, WINDING);
		/* Закрашивает путь текущей кистью.
		StrokePath - обводит пером. */
		FillPath(hdc);



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