cmake_minimum_required(VERSION 2.8)
project(NoobMapTest)

set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions -fno-rtti ${CMAKE_CXX_FLAGS}")

include_directories(include)

include_directories(test/extern)
list (APPEND rdestl_src
	test/extern/rdestl/allocator.cpp
	test/extern/rdestl/intrusive_list.cpp
	test/extern/rdestl/intrusive_slist.cpp
	test/extern/rdestl/list.cpp
	)


list (APPEND test_src test/main.cpp) 

add_executable(Test ${test_src} ${rdestl_src})
