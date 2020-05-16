/* Timer */

#include <windows.h> 
#include <tchar.h>

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
		WinName, _T("Line"),    
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, NULL, This, NULL);								

	ShowWindow(hWnd, mode);					
	while (GetMessage(&msg, NULL, 0, 0))	
	{
		TranslateMessage(&msg);		
		DispatchMessage(&msg);
	}
	return 0;
}

const int WIDTH = 320;
const int HEIGHT = 200;
const double K = 4.0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	static HPEN hpen1;
	static HPEN black_pen;

	static HBRUSH red_solid_brush;
	static HBRUSH hatch_brush;

	static int sx, sy;
	switch (message)
	{
	case WM_SIZE:
		// При создании и изменении размеров
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		break;
	case WM_CREATE:
		// Создание пера (НУжно будет освобождать ресурсы!!!)
		hpen1 = CreatePen(
			PS_DASH,		// PS_SOLID, PS_DASH, PS_DASHDOT, PS_DASHDOTDOT, PS_DOT, PS_INSIDEFRAME, PS_NULL
			1, // width
			RGB(0, 0, 255)); // color

		black_pen = (HPEN)GetStockObject(BLACK_PEN); // Не нужно освобождать ресурсы

		red_solid_brush = CreateSolidBrush(RGB(255, 0, 0)); // Задание кисти сплошной
		hatch_brush = CreateHatchBrush(HS_CROSS, RGB(0, 128, 0)); // Нужно освобождать ресурсы
		/* HS_BDIAGONAL, HS_CROSS, HS_DIAGCROSS, HS_FDIAGONAL, HS_HORIZONTAL, HS_VERTICAL */
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// Настройка графического режима
		SetMapMode(hdc, MM_ANISOTROPIC);
		/* 1ЛЕ - 1 логическая единица
		MM_TEXT - 1ЛЕ = 1 пиксель
		MM_LOMETRIC - 1ЛЕ = 0.1 мм
		MM_HIMETRIC - 1ЛЕ = 0.01 мм
		MM_LOENGLISH - 1ЛЕ = 0.01 дюйма
		MM_HIENGLISH - 1ЛЕ = 0.001 дюйма
		MM_TWIPS - 1ЛЕ = 1/12 точки принтера = 1/1440 дюйма
		MM_ISOTROPIC - одинаковое масштабирование по осям координат
		MM_ANISOTROPIC - различное масштабирование по осям координат
		*/

		SetWindowExtEx(hdc, 2*WIDTH, -2*HEIGHT, NULL); /* Задает логические размеры окна 
										 				Лучше задавать ближе к физицческим.
										 				Ось y направлена вверх
										 				*/

		SetViewportExtEx(hdc, sx / 2, sy / 2, NULL);			/* Задает физические размеры окна (четверть окна) */

		SetViewportOrgEx(hdc, sx / 4, sy / 4, NULL);			/* Устанавливает точку отсчета */

		// Далее все отрисовывается относительно логических координат.

		// выбор пера как текущего
		SelectObject(hdc, hpen1);

		// Рисование линий
		for (int x = -WIDTH; x < WIDTH; x += 2*WIDTH / 10)
		{
			MoveToEx(hdc, x, -HEIGHT, NULL);
			LineTo(hdc, x, HEIGHT);
		}

		for (int y = -HEIGHT; y < HEIGHT; y += 2*HEIGHT / 10)
		{
			MoveToEx(hdc, -WIDTH, y, NULL);
			LineTo(hdc, WIDTH, y);
		}

		// Рисование прямоугольника
		SelectObject(hdc, red_solid_brush);
		Rectangle(hdc, - WIDTH / 2, HEIGHT / 2, WIDTH / 2, - HEIGHT / 2);

		// Использование системного пера
		SelectObject(hdc, black_pen); 
		SelectObject(hdc, hatch_brush);
		Ellipse(hdc, - WIDTH / 2, HEIGHT / 2, WIDTH / 2, - HEIGHT / 2);

		// HBRUSH sys_brush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		SelectObject(hdc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
		Ellipse(hdc, -30, 30, 30, -30);

		SetPixel(hdc, 0, 0, RGB(0, 255, 255));

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		DeleteObject(hpen1);
		DeleteObject(red_solid_brush);
		DeleteObject(hatch_brush);
		PostQuitMessage(0);
		break;	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}