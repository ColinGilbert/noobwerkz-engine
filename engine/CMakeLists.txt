cmake_minimum_required(VERSION 3.0)

project(NoobWerkz)

set(CMAKE_CC "clang++")
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

add_subdirectory(glfw)
include_directories(bx/include)
include_directories(bgfx/include)
include_directories(glfw/include)
include_directories(bgfx/3rdparty/khronos)

list(APPEND bgfx_src
	bgfx/src/amalgamated.cpp
	)

list(APPEND main_src main.cpp)

add_executable(NoobEdit ${main_src} ${bgfx_src} ) 
target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} rt dl pthread)
