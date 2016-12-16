include_directories(lib/bstrlib)
list (APPEND main_src
	lib/bstrlib/bstraux.c
	lib/bstrlib/bstrlib.c
	#lib/bstrlib/bsafe.c
	)
#lib/bstrlib/bstest.c
#lib/bstrlib/testaux.c
#lib/bstrlib/test.cpp
# lib/bstrlib/bstrwrap.cpp // It was disgusting, so I got rid of it.

