# -*- coding: utf-8 -*-
import sys
from ctypes import *
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

hInst = 0
OFN_ENABLESIZING = 0x00800000
OFN_PATHMUSTEXIST = 0x00000800
OFN_OVERWRITEPROMPT = 0x00000002
OFN_NOCHANGEDIR = 0x00000008
MAX_PATH = 1024

if __name__ == '__main__':
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

	