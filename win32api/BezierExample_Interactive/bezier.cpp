/* Bezier Interactive Builder */

#include <windows.h> 
#include <tchar.h>

#include <fstream>
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

const int SCALE = 1000;
const int MARK = 4;
static int sx, sy;

void DcInLp(POINT& point)
{
	point.x = point.x * SCALE / sx;
	point.y = SCALE - point.y * SCALE / sy;
}

void transform(HDC& hdc)
{
	SetMapMode(hdc, MM_ANISOTROPIC);
	SetWindowExtEx(hdc, SCALE, -SCALE, NULL);
	SetViewportExtEx(hdc, sx, sy, NULL);
	SetViewportOrgEx(hdc, 0, sy, NULL);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static HPEN hDash, hBezier;
	static HBRUSH hRect, hSel;
	static POINT pt[20];
	static POINT point;
	RECT rt;

	static int count, index;
	static bool capture = false;
	int i;
	std::ifstream in;
	std::ofstream out;


	switch (message)
	{
	case WM_SIZE:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		break;
	case WM_CREATE:
		in.open("dat.txt");
		if (in.fail()) 
		{
			MessageBox(hWnd, _T("File dat.txt is not found!"), _T("Opening file..."), MB_OK | MB_ICONEXCLAMATION);
			PostQuitMessage(0);
			return 1;
		}

		count = 0;
		while (in >> pt[count].x) 
		{
			in >> pt[count].y; 
			count++;
			std::cout << pt[count - 1].x << " " << pt[count - 1].y << std::endl;
		}
		std::cout << "Count: " << count << std::endl;
		in.close();
		hDash = CreatePen(PS_DASH, 1, 0);
		hBezier = CreatePen(PS_SOLID, 4, RGB(0, 0, 255));
		hRect = CreateSolidBrush(RGB(128, 0, 128));
		hSel = CreateSolidBrush(RGB(255, 0, 0));
		break;
	case WM_LBUTTONDOWN:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);

		// Преобразование экранных координат в логические
		DcInLp(point);
		for (int i = 0; i < count; ++i)
		{
			SetRect(&rt, pt[i].x - MARK, pt[i].y - MARK, pt[i].x + MARK, pt[i].y + MARK);
			if (PtInRect(&rt, point))
			{
				index = i;
				capture = true;
				hdc = GetDC(hWnd);
				transform(hdc);
				FillRect(hdc, &rt, hSel);
				ReleaseDC(hWnd, hdc);
				SetCapture(hWnd); // Захват мыши
				return 0;
			}
		}
		break;
	case WM_LBUTTONUP:
		if (capture)
		{
			ReleaseCapture(); // Освобождение мыши
			capture = false;
		}
		break;
	case WM_MOUSEMOVE:
		if (capture)
		{
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			DcInLp(point);
			pt[index] = point;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		transform(hdc);

		SelectObject(hdc,  hDash);
		Polyline(hdc, pt, count); // Строим ломаную линию
		
		SelectObject(hdc, hBezier);
		PolyBezier(hdc, pt, count); // Кривая Безье

		for (int i = 0; i < count; ++i)
		{
			SetRect(&rt, pt[i].x - MARK, pt[i].y - MARK, pt[i].x + MARK, pt[i].y + MARK);
			FillRect(hdc, &rt, hRect);
		}

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		DeleteObject(hDash);
		DeleteObject(hBezier);
		DeleteObject(hRect);
		DeleteObject(hSel);
		out.open("dat_r.txt");
		for (int i = 0; i < count; ++i)
		{
			out << pt[i].x << " " << pt[i].y << " ";
		}
		out.close();
		PostQuitMessage(0);
		break;	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}