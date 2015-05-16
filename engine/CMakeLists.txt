cmake_minimum_required(VERSION 3.0)

project(NoobWerkz)

set(CMAKE_CXX_FLAGS "-std=c++11 -I/usr/local/include/GraphicsMagick -DEIGEN_MPL2_ONLY ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 -DSTB_IMAGE_STATIC ${CMAKE_C_FLAGS}")

add_subdirectory(lib/glfw)
add_subdirectory(lib/freetype)
add_subdirectory(lib/polyvox)
add_subdirectory(lib/rbdl)
add_subdirectory(lib/shapeop)
#add_subdirectory(lib/FasTC)
add_subdirectory(lib/dlib/dlib)

include_directories(lib/bx/include)
include_directories(lib/bgfx/include)
include_directories(lib/glfw/include)
include_directories(lib/bgfx/3rdparty)
include_directories(lib/bgfx/3rdparty/khronos)
include_directories(lib/bgfx/3rdparty/ocornut-imgui)
include_directories(lib/freetype/include)
include_directories(lib/polyvox/include)
include_directories(lib/lemon)
include_directories(lib/noisepp/core)
include_directories(lib/eigen/Eigen)
include_directories(lib/soil2/src/SOIL2)

include_directories(common)
include_directories(common/bgfx_related)
include_directories(common/bgfx_related/font)
include_directories(common/bgfx_related/shaderc)
include_directories(common/bfgx_related/nanovg)
include_directories(common/bgfx_related/imgui)


include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND soil2_src
	lib/soil2/src/SOIL2/etc1_utils.c
	lib/soil2/src/SOIL2/image_DXT.c
	lib/soil2/src/SOIL2/image_helper.c
	lib/soil2/src/SOIL2/SOIL2.c
	)

list(APPEND bgfx_src
	lib/bgfx/src/amalgamated.cpp
	)

list(APPEND bgfx_utils_src
	common/bgfx_related/cube_atlas.cpp
	common/bgfx_related/font/font_manager.cpp
	common/bgfx_related/font/utf8.cpp
	common/bgfx_related/font/text_metrics.cpp
	common/bgfx_related/font/text_buffer_manager.cpp
	common/bgfx_related/bounds.cpp
	common/bgfx_related/nanovg/nanovg.cpp
	common/bgfx_related/nanovg/nanovg_bgfx.cpp
	common/bgfx_related/imgui/ocornut_imgui.cpp
	common/bgfx_related/imgui/imgui.cpp
	lib/bgfx/3rdparty/ocornut-imgui/imgui.cpp
	)

list(APPEND common_src
	common/Graphics.cpp
	common/Logger.cpp
	common/Application.cpp
	common/SandboxApp.cpp
	common/MathFuncs.cpp
	common/Drawable.cpp
	common/NoobUtils.cpp
	common/EditorUtils.cpp
	common/Voxels.cpp
	common/bgfx_related/Shaders.cpp
	)

list(APPEND main_src platform/nix/EngineDesktop.cpp)

add_executable(NoobEdit ${main_src} ${common_src} ${bgfx_src}
	${bgfx_utils_src} ${soil2_src}) 

target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES}
	${OPENGL_LIBRARIES} freetype rt dl pthread assimp GraphicsMagick++
	GraphicsMagick X11 Xext z gomp png bz2 webp jpeg tiff jasper lcms2
	xml2 z wmflite m) 
project(LogServer)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

include_directories(lib/asio/asio/include)

list(APPEND logger_src
	common/LogServer.cpp
	)

add_executable(LogServer ${logger_src})

target_link_libraries(LogServer LINK_PUBLIC pthread)
