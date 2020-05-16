# -*- coding: utf-8 -*-
import sys
from ctypes import *

MB_YESNO = 0x00000004
MB_ICONQUESTION = 0x00000020

def print_py_version():
	import sys
	print("Python: {}".format(sys.version))

if __name__ == '__main__':
	print_py_version()
	windll.user32.MessageBoxW(
			0, 
			c_wchar_p("Yes or No, boy?"),
			c_wchar_p("Make your life better?"),
			MB_YESNO | MB_ICONQUESTION
			)