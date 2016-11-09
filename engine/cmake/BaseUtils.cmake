include_directories(lib/base-utils/include)
include_directories(lib/base-utils/rde)

list (APPEND main_src
	lib/base-utils/rde/rdestl/allocator.cpp
	lib/base-utils/rde/rdestl/intrusive_list.cpp
	lib/base-utils/rde/rdestl/intrusive_slist.cpp
	lib/base-utils/rde/rdestl/list.cpp
	lib/base-utils/include/noob/strings/modp_numtoa.c
	lib/base-utils/include/noob/fast_hashtable/fast_hashtable.cpp
	)
