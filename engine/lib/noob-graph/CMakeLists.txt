cmake_minimum_required(VERSION 2.8)
project(NoobMapTest)

set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions -fno-rtti ${CMAKE_CXX_FLAGS}")


include_directories(test/extern/rde)

list (APPEND rdestl_src
	test/extern/rde/rdestl/allocator.cpp
	test/extern/rde/rdestl/intrusive_list.cpp
	test/extern/rde/rdestl/intrusive_slist.cpp
	test/extern/rde/rdestl/list.cpp
	)

include_directories(test)

include_directories(test/extern/component)

include_directories(include)

list (APPEND test_src test/main.cpp) 

add_executable(Test  ${rdestl_src} ${test_src})
