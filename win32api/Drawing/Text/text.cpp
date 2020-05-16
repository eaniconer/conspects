/* Работа с текстом */

#include <cstdio>
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
		WinName, _T("Text"),    	
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
	PAINTSTRUCT ps;
	HDC hdc;

	static int sx, sy;

	switch (message)
	{
	case WM_SIZE:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		/* Обе функции возвращают предыдущий цвет 
		либо CLR_INVALID в случае ошибки */
		SetBkColor(hdc, RGB(0, 128, 128)); // Background
		SetTextColor(hdc, RGB(128, 0, 0)); // Text color

		/* Функция управления режимом отображения фона 
		OPAQUE - определяется SetBkColor
		TRANSPARENT - цвет фона прозрачный
		*/
		SetBkMode(hdc, OPAQUE);

		TextOut(hdc, 0, 0, _T("Hi"), 2);

		{
			SetBkMode(hdc, TRANSPARENT);
			/* Выбор шрифта 
			ANSI_FIXED_FONT - фиксированный размер символов
			ANSI_VAR_FONT - переменный размер символов
			DEVICE_DEFAULT_FONT
			DEFAULT_GUI_FONT
			OEM_FIXED_FONT
			SYSTEM_FONT
			SYSTEM_FIXED_FONT
			*/
			SelectObject(hdc, GetStockObject(ANSI_VAR_FONT));
			TEXTMETRIC tm;
			GetTextMetrics(hdc, &tm);
			TCHAR str[256];
			_stprintf(str,
				_T("tmHeight = %d\ntmInternalLeading = %d\ntmExternalLeading=%d\ntmAscent=%d\ntmDescent=%d\n"),
				tm.tmHeight, tm.tmInternalLeading, tm.tmExternalLeading,
				tm.tmAscent, tm.tmDescent
				);

			RECT rt;
			SetRect(&rt, 0, 20, 150, 100);

			/* Отрисовка текста в заданный прямоугольник, учитывая \n
			формат выравнивания LD_LEFT
			*/
			DrawText(hdc, str, _tcslen(str), &rt, DT_LEFT);

			/* Определение длины строки в логических единицах */
			TCHAR *text = _T("Some text for display");
			SIZE size;
			GetTextExtentPoint32(hdc, text, _tcslen(text), &size);
			_stprintf(str,
				_T("Row width: %d\nRow height: %d"),
				size.cx, size.cy
				);
			SetRect(&rt, 0, 100, 150, 150);
			DrawText(hdc, str, _tcslen(str), &rt, DT_RIGHT);
		}
		{
			for (int escapement = 0; escapement < 3600; escapement += 200)
			{
				// часто делают так: шрифты создают при WM_CREATE
				// А удалять при WM_DESTROY 

				HFONT newFont = CreateFont(
					20,	// Height
					0,	// Width
					escapement, // Угол наклона в десятых долях градуса
					0,	// Orientation - угол наклона каждого символа в десятых долях градуса (только для расширенного графич режима)
					700,	// Жирность текста [0, 1000]
					1,		// Ital - ненулевое значение создает наклонный шрифт
					0,		// Underline
					1,		// StrikeGhru - перечеркнутый
					DEFAULT_CHARSET, // МНожество символов шрифта
					OUT_DEFAULT_PRECIS, // Точность отображения шрифта
					CLIP_DEFAULT_PRECIS, // Как будут отсекатся символы
					PROOF_QUALITY, // Качество шрифта DEFAULT_QUALITY, DRAFT_QUALITY, PROOF_QUIALITY
					DEFAULT_PITCH | FF_DONTCARE, // Тип и семейство шрифтов. Типы: DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH. 
												 // Семейство: FF_DECORATIVE, FF_DONTCARE, FF_MODERN, FF_ROMAN, FF_SCRIPT, FF_SWISS
					_T("Arial")); // FontName
				HFONT oldFont = (HFONT)SelectObject(hdc, newFont);
				TCHAR text[] = _T("Meow meow meow");
				TextOut(hdc, sx / 2, sy / 2, text, _tcsclen(text));
				SelectObject(hdc, oldFont);
				DeleteObject(newFont);
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