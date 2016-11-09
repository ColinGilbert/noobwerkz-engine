include_directories(lib/libogg/include)

list (APPEND main_src
	lib/libogg/src/framing.c
	lib/libogg/src/bitwise.c
	)
