cmake_minimum_required(VERSION 3.0)

project(NoobWerkz)

set(CMAKE_CC "clang++")
set(CMAKE_CXX_FLAGS "-std=c++11 -fpermissive ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

add_subdirectory(glfw)
add_subdirectory(freetype)

include_directories(bx/include)
include_directories(bgfx/include)
include_directories(glfw/include)
include_directories(bgfx/3rdparty/khronos)
include_directories(bgfx/3rdparty)
include_directories(freetype/include)
include_directories(common)
include_directories(common/bgfx_related)
include_directories(common/bgfx_related/font)


list(APPEND bgfx_src
	bgfx/src/amalgamated.cpp
	)

list(APPEND bgfx_utils_src
	common/bgfx_related/cube_atlas.cpp
	common/bgfx_related/font/font_manager.cpp
	common/bgfx_related/font/utf8.cpp
	common/bgfx_related/font/text_metrics.cpp
	common/bgfx_related/font/text_buffer_manager.cpp
	common/bgfx_related/bounds.cpp
	)

list(APPEND common_src
	common/Graphics.cpp
	common/Logger.cpp
	common/Application.cpp
	common/Camera.cpp
	common/SandboxApp.cpp
	common/CommonIncludes.cpp
	common/MathFuncs.cpp
	common/Font.cpp
	common/Drawable.cpp
	common/NoobUtils.cpp
	)

list(APPEND main_src platform/nix/EngineDesktop.cpp)

add_executable(NoobEdit ${main_src} ${common_src} ${bgfx_src} ${bgfx_utils_src} ) 
target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} freetype rt dl pthread assimp)

project(LogServer)

set(CMAKE_CC "clang++")
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

include_directories(asio/asio/include)

list(APPEND logger_src
	common/LogServer.cpp
	)

add_executable(LogServer ${logger_src})

target_link_libraries(LogServer LINK_PUBLIC pthread)
