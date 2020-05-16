# -*- coding: utf-8 -*-

from ctypes import Structure
from ctypes import c_wchar_p
from ctypes import c_voidp

from ctypes.wintypes import DWORD
from ctypes.wintypes import HWND
from ctypes.wintypes import HINSTANCE
from ctypes.wintypes import WORD
from ctypes.wintypes import LPARAM
from ctypes.wintypes import LPVOID

LPOFNHOOKPROC = c_voidp
LPCTSTR = LPTSTR = c_wchar_p

class OPENFILENAME(Structure):
    """ Structure for working with GetOpenFileName and GetSaveFileName. """
    _fields_ = [("lStructSize", DWORD),
                ("hwndOwner", HWND),
                ("hInstance", HINSTANCE),
                ("lpstrFilter", LPCTSTR),
                ("lpstrCustomFilter", LPTSTR),
                ("nMaxCustFilter", DWORD),
                ("nFilterIndex", DWORD),
                ("lpstrFile", LPTSTR),
                ("nMaxFile", DWORD),
                ("lpstrFileTitle", LPTSTR),
                ("nMaxFileTitle", DWORD),
                ("lpstrInitialDir", LPCTSTR),
                ("lpstrTitle", LPCTSTR),
                ("flags", DWORD),
                ("nFileOffset", WORD),
                ("nFileExtension", WORD),
                ("lpstrDefExt", LPCTSTR),
                ("lCustData", LPARAM),
                ("lpfnHook", LPOFNHOOKPROC),
                ("lpTemplateName", LPCTSTR),
                ("pvReserved", LPVOID),
                ("dwReserved", DWORD),
                ("flagsEx", DWORD)]

