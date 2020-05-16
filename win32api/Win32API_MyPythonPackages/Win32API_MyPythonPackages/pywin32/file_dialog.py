# -*- coding: utf-8 -*-

from ctypes import windll
from ctypes import create_unicode_buffer
from ctypes import sizeof
from ctypes import c_wchar_p
from ctypes import cast
from ctypes import byref

from pywin32.structures import OPENFILENAME
from const import *

def _make_openfilename():  # todo add args
    """ Fill OPENFILENAME structure 
    according with args.
    """
    MAX_PATH = 1024
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
    return ofn


class FileDialog(object):
    """ Class holding functions that show win32 dialog
    for opening and saving files. """

    @staticmethod
    def show_save_dialog():  # todo add args
        """ Show win32 dialog to choose file for saving. """
        ofn = _make_openfilename()
        if windll.comdlg32.GetSaveFileNameW(byref(ofn)):
            return ofn.lpstrFile.value

    @staticmethod
    def show_open_dialog():  # todo add args
        """ Show win32 dialog to choose file for opening. """
        ofn = _make_openfilename()
        if windll.comdlg32.GetOpenFileNameW(byref(ofn)):
            return ofn.lpstrFile.value
