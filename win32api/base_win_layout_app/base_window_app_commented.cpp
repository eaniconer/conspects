/* ����������� ��� ������� Windows-���������� */
/* ��������� ������ */


#include <windows.h> /* ������������ �� ���� Windows-���������� 
					������ ���������:
						1.  windef.h winbase.h wingdi.h winuser.h
						2. ����� � ������������� API-�������, �������� � ��������
					*/


#include <tchar.h>   /* �������� �������. �������� _T()
					 #define _T(x) __T(x)
					 #ifndef _UNICODE
					 #define __T(x)	L ## x
					 #else
					 #define __T(x) x
					 #endif

					 ������ ������������� � �������� L (���������� ������������ ��� ����������� ������ Unicode)
					 ��� �������� L ## x. ���� x = "abc", �� L ## x ���������� L"abc"
					 */

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  /*������� ��������� ������. 
													   ��� �������������� � ��, � �� ����� ������������ ��, ����� 
													   ��������� ���������� ���������. LRESULT ������������ long ��� Win32-�������*/
TCHAR WinName[] = _T("MainFrame");		/* TCHAR ������������� � wchar_t ���� ��������� _UNITCODE ����������, ����� � char
										wchar_t ������������ ���� short � ������ ��� �������� � ��������� Unicode (16 ��� �� ������)
										��� ������ ���� ������������ �� ��� ��� �������������, ����� ���� ������������.
										*/

/*������ ����� ������ ������������: HINSTANCE, HWND, ... ��� ��� 32-��������� ����� �����.*/
// ��� �������� �������
int APIENTRY _tWinMain( /*Unicode: wWinMain, �����: WinMain*/
	HINSTANCE This,	/* ���������� �������� ����������. �� ����������� ��� ���������� ��� ��������. */
	HINSTANCE Prev, /* � ����������� �������� ������ = 0. ���������� ���������� ������������ ����������. */
	LPTSTR cmd,		/* ��������� ��������� ������, LPTSTR ��� TCHAR* */
	int mode)		/* ����� ����������� ���� */
{
	HWND hWnd;  // ���������� �������� ���� ����������
	MSG msg;	/* ��������� ��� �������� ���������, ������������� ������������ �������� 
				struct MSG
				{
					HWND hWnd;		// ���������� ����
					UINT message;	// ����� ���������
					WPARAM wParam;  // 32-��������� ����� ��������		
					LPARAM lParam;  //   ��� ��������� ���������
					DWORD time;		// ����� ������� ��������� � ��
					POINT pt;		// ���������� ������� (x, y)
				};
				struct POINT { LONG x, y; };

				WPARAM - �������� �������� ��� �������� 16���������� �������� � 16 ��������� ��
				� WIN32 ��� ����� 32-��������� ��� � LPARAM
				*/
	WNDCLASS wc;// ����� ����

	// ����������� ������ ����.
	wc.hInstance = This;		// ���������� �������� ����������
	wc.lpszClassName = WinName;	// ��� ������ ����
	wc.lpfnWndProc = WndProc;	// ��� ������� ������� ��� ��������� ���������
	wc.style = CS_HREDRAW | CS_VREDRAW;	// ����� ���� (�������������� ����������� ��� ��������� ������� �� �����������/���������)
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);	/* ������ (NULL ��������� �� ��������� �����������)
											  �����:
											  IDI_APPLICATION - ����������� ������
											  IDI_ASTERISK - ���������
											  IDI_EXCLAMATION, IDI_HAND, IDI_QUESTION, IDI_WINLOGO (������� Win)
											  */
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); /*������, NULL ��������� �� ��������� ������
											  IDC_ARROW, IDC_APPSTARTING (������ � ����), IDC_CROSS, IDC_IBEAM(���������),
											  IDC_NO(������������� ����), IDC_SIZEALL(4-����������� �������),
											  IDC_SIZENESW, IDC_SIZENWSE, IDC_SIZENS, IDC_SIZEWE, IDC_UPARROW, IDC_WAIT(����)
											  */
	wc.lpszMenuName = NULL; // ��� ����
	wc.cbClsExtra = 0;      // ��� �������������� ������ ������
	wc.cbWndExtra = 0;		// ��� �������������� ������ ����

	// ���������� ���� ����� ������
	
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); /* ���������� �����, ������� ������������ ��� ���������� ����.
												   ��������� ����� ������ ����� WHITE_BRUSH. ��������� ����� �������������� ���� HBRUSH.
												   */
	if (!RegisterClass(&wc)) return 0;		// ����������� ������ ����

	hWnd = CreateWindow(
		WinName,							// ��� ������ ����
		_T("������ Windows-����������"),    // ��������� ����
		
		WS_OVERLAPPEDWINDOW,				/* ����� ����: ��������� ����, ���������, ����� ��������� ��������, ������ �����������
											�������������, ��������*/
		CW_USEDEFAULT,						// � - ���������� ������ �������� ���� (CW_USEDEFAULT ��������, ��� ������� ���� �������� �������� ������� ����� ��� ������)
		CW_USEDEFAULT,						// y
		CW_USEDEFAULT,						// Width (� ��������)
		CW_USEDEFAULT,						// Height
		HWND_DESKTOP,						// ���������� ������������� ����
		NULL,								// ��� ����
		This,								// ���������� ����������
		NULL);								// �������������� ���������� ���

	
	ShowWindow(hWnd, mode);					// �������� ����
	while (GetMessage(&msg, NULL, 0, 0))	/* ������ �������� ��������� �� ����, ��� �������� ������������� ���������
											���� NULL �� ��������� ���� ��� ���� ���� ����������
											��� ��������� ��������� ��������� �� �������� [min, max] ���������� ���������.
											���� ��� = 0, �� ������������ ��� ���������.

											� ����� winuser.h ������� ������������� ����� ��� �������. � ���� ���� ������� WM_
											*/
	{
		TranslateMessage(&msg);		/* ������� ���������� ����� ������� �������.
									����������� ��� ������� ������� � ������������ ��������� WM_CHAR.
									wParam - ��� ������� ������� � cp1251
									lParam - ������� �����: ���������� ���������� ����� ��������� � ���������� ���������
										   - ������� �����: ������� ����� �� ����������:
													���	��������
													15	1 - ��������, 0 - ������ �������
													14  1 - ������� ���� ������ ����� �������� ���������
													13  1 - ������ Alt
													12-9 ������
													8 - 1 ���� ������ �������������� �������
													7-0  Scan-��� �������

									������� ����� ��� ��������� ��������� � ����������.
									*/
		DispatchMessage(&msg);		// ��������� ��������� ������� WndProc
	}
	return 0;
}

// ������� ������� ���������� �� � �������� ��������� �� ������� ��� ������� ����������
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		/* ���������� ����������� � ����� �������� = 0.
		������������ ��������� WM_QUIT, ������� ������� ������� GetMEssage ���������� ������� �������� � ���� �����������.
		*/

		break;	// ���������� ���������
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}