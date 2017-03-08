cmake_minimum_required(VERSION 2.8)
project(noob-tests)

set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_CXX_FLAGS "-std=c++11  ${CMAKE_CXX_FLAGS}")

include_directories(rde)
list (APPEND rdestl_src
	rde/rdestl/allocator.cpp
	rde/rdestl/intrusive_list.cpp
	rde/rdestl/intrusive_slist.cpp
	rde/rdestl/list.cpp
	)

include_directories(include)
list (APPEND modp_src
	include/noob/strings/modp_numtoa.c)

list (APPEND fast_hashtable_src
	include/noob/fast_hashtable/fast_hashtable.cpp
	)

list (APPEND test_src
	tests/main.cpp
	)

add_executable (t ${test_src} ${fast_hashtable_src} ${rdestl_src} ${modp_src})
