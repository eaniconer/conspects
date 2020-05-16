# -*- coding: utf-8 -*-
import sys
from ctypes import *

WM_CREATE = 0x0001
WM_DESTROY = 0x0002


CS_VREDRAW = 0x0001
CS_HREDRAW = 0x0002

IDI_APPLICATION = 32512
IDC_ARROW = 32512

WHITE_BRUSH = 0

WS_OVERLAPPED = 0x00000000
WS_CAPTION = 0x00C00000
WS_SYSMENU = 0x00080000
WS_THICKFRAME = 0x00040000
WS_MINIMIZEBOX = 0x00020000
WS_MAXIMIZEBOX = 0x00010000
WS_OVERLAPPEDWINDOW = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX

CW_USEDEFAULT = 0x80000000

SW_SHOWNORMAL = 1

MB_YESNO = 0x00000004
MB_ICONQUESTION = 0x00000020

def print_py_version():
	import sys
	print("Python: {}".format(sys.version))

WNDPROC = WINFUNCTYPE(c_long, c_int, c_uint, c_int, c_int)

class WNDCLASS(Structure):
	_fields_ = [('style', c_uint),
				('lpfnWndProc', WNDPROC),
				('cbClsExtra', c_int),
				('cbWndExtra', c_int),
				('hInstance', c_int),
				('hIcon', c_int),
				('hCursor', c_int),
				('hbrBackground', c_int),
				('lpszMenuName', c_wchar_p),
				('lpszClassName', c_wchar_p)
				]

class POINT(Structure):
	_fields_ = [('x', c_long),
				('y', c_long)]

class MSG(Structure):
	_fields_ = [('hwnd', c_int),
				('message', c_uint),
				('wParam', c_int),
				('lParam', c_int),
				('time', c_int),
				('pt', POINT)]

import ctypes.wintypes as wintypes

LPOFNHOOKPROC = c_voidp
LPCTSTR = LPTSTR = c_wchar_p
class OPENFILENAME(Structure):
    _fields_ = [("lStructSize", wintypes.DWORD),
                ("hwndOwner", wintypes.HWND),
                ("hInstance", wintypes.HINSTANCE),
                ("lpstrFilter", LPCTSTR),
                ("lpstrCustomFilter", LPTSTR),
                ("nMaxCustFilter", wintypes.DWORD),
                ("nFilterIndex", wintypes.DWORD),
                ("lpstrFile", LPTSTR),
                ("nMaxFile", wintypes.DWORD),
                ("lpstrFileTitle", LPTSTR),
                ("nMaxFileTitle", wintypes.DWORD),
                ("lpstrInitialDir", LPCTSTR),
                ("lpstrTitle", LPCTSTR),
                ("flags", wintypes.DWORD),
                ("nFileOffset", wintypes.WORD),
                ("nFileExtension", wintypes.WORD),
                ("lpstrDefExt", LPCTSTR),
                ("lCustData", wintypes.LPARAM),
                ("lpfnHook", LPOFNHOOKPROC),
                ("lpTemplateName", LPCTSTR),
                ("pvReserved", wintypes.LPVOID),
                ("dwReserved", wintypes.DWORD),
                ("flagsEx", wintypes.DWORD)]

WinName = "MainFrame"

hInst = 0


OFN_ENABLESIZING      =0x00800000
OFN_PATHMUSTEXIST     =0x00000800
OFN_OVERWRITEPROMPT   =0x00000002
OFN_NOCHANGEDIR       =0x00000008
MAX_PATH=1024

def WndProc(hWnd, message, wParam, lParam):
	""" Обработчик сообщений окна """
	# print(message)

	if message == WM_DESTROY:
		windll.user32.PostQuitMessage(0)
		return 0
	elif message == WM_CREATE:
		fileBuffer = create_unicode_buffer("", MAX_PATH)
		def_ext = "txt"
		filter_string = "Executable\0*.exe\0All files\0*.*\0"
		ofn = OPENFILENAME()
		ofn.lStructSize = sizeof(OPENFILENAME)
		ofn.lpstrTitle = c_wchar_p("Title")
		ofn.lpstrFile = cast(fileBuffer, c_wchar_p)
		ofn.nMaxFile = MAX_PATH
		ofn.lpstrDefExt = c_wchar_p(def_ext)
		ofn.lpstrFilter = c_wchar_p(filter_string)
		ofn.Flags = OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR
		res = windll.comdlg32.GetOpenFileNameW(byref(ofn))
		if res != 0:
			print(dir(fileBuffer))
			print(fileBuffer.value)

		res = windll.comdlg32.GetSaveFileNameW(byref(ofn))
		if res != 0:
			print(dir(fileBuffer))
			print(fileBuffer.value)

	return windll.user32.DefWindowProcW(c_int(hWnd), c_int(message), c_int(wParam), c_int(lParam))

def WinMain(This):
	global hInst
	hInst = This

	wc = WNDCLASS()
	wc.hInstance = This
	wc.lpszClassName = WinName
	wc.lpfnWndProc = WNDPROC(WndProc)
	wc.style = CS_HREDRAW | CS_VREDRAW
	wc.hIcon = windll.user32.LoadIconW(c_int(0), c_int(IDI_APPLICATION))
	wc.hCursor = windll.user32.LoadCursorW(c_int(0), c_int(IDC_ARROW))
	wc.lpszMenuName = None
	wc.cbClsExtra = 0
	wc.cbWndExtra = 0
	wc.hbrBackground = windll.gdi32.GetStockObject(c_int(WHITE_BRUSH))

	if not windll.user32.RegisterClassW(byref(wc)):
		raise WinError

	CreateWindowEx = windll.user32.CreateWindowExW
	CreateWindowEx.argtypes = \
		[c_int, c_wchar_p, c_wchar_p, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int]
	
	def ErrorIfZero(handle):
		if handle == 0:
			raise WinError
		else:
			return handle

	CreateWindowEx.restype = ErrorIfZero
	hWnd = CreateWindowEx(
		c_int(0),
		wc.lpszClassName,
		c_wchar_p("Python Window"),
		c_int(WS_OVERLAPPEDWINDOW),
		c_int(CW_USEDEFAULT),
		c_int(CW_USEDEFAULT),
		c_int(CW_USEDEFAULT),
		c_int(CW_USEDEFAULT),
		c_int(0),
		c_int(0),
		c_int(wc.hInstance),
		c_int(0)
		)

	windll.user32.ShowWindow(c_int(hWnd), c_int(SW_SHOWNORMAL))
	windll.user32.UpdateWindow(c_int(hWnd))
	
	msg = MSG()
	pMsg = pointer(msg)
	NULL = c_int(0)

	while windll.user32.GetMessageW(pMsg, NULL, 0, 0) != 0:
		windll.user32.TranslateMessage(pMsg)
		windll.user32.DispatchMessageW(pMsg)

	return msg.wParam


if __name__ == '__main__':
	print_py_version()
	handle = windll.kernel32.GetModuleHandleW(c_int(0))
	sys.exit(WinMain(handle))
	