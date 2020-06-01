#!/usr/bin/python3

SOURCES = [ "cxxptl.h", "sysspec_apple.cpp", "sysspec_bsd.cpp", "sysspec_linux.cpp", "sysspec_posix.cpp", "sysspec_win32.cpp", "cxxptl.cpp" ]
IGNORE_LINES = [ "#include \"cxxptl.h\"", "#ifndef __CXXPTL", "#endif // __CXXPTL_H__" ]

USAGE = """Usage: make_amalgamation_version.py > RESULTFILE.cpp.

The resulting file can be included as a single-file version of the whole lib
"""

def lineAllowed(s):
	for ignore in IGNORE_LINES:
		if s.startswith(ignore):
			return False
	return True

for fn in SOURCES:
	f = open(fn, "rt")
	for line in f:
		if lineAllowed(line):
			print(line.rstrip())
	f.close()
