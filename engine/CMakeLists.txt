include (ExternalProject)
cmake_minimum_required(VERSION 3.0)
project(NoobWerkz)

set(CMAKE_CXX_FLAGS "-std=c++11 -fno-strict-aliasing -DASIO_STANDALONE -DEIGEN_MPL2_ONLY -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 -fno-strict-aliasing -DSTB_IMAGE_STATIC -Wall ${CMAKE_C_FLAGS}")

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

include_directories(lib/freetype/include)
add_subdirectory(lib/freetype)

add_subdirectory(lib/polyvox)
include_directories(lib/polyvox/include)

add_subdirectory(lib/rbdl)

# TODO: Replace simplicial Cholesky solver prior to use (for licensing reasons.)
# add_subdirectory(lib/shapeop)

# TODO: Add these when needed.
# add_subdirectory(lib/dlib/dlib)
# add_subdirectory(lib/armadillo)
# add_subdirectory(lib/openNL)
# include_directories(lib/ulmBLAS)

include_directories(lib/openmesh/src)
add_subdirectory(lib/openmesh)

include_directories(lib/bx/include)
include_directories(lib/bgfx/include)
include_directories(lib/bgfx/3rdparty)
include_directories(lib/bgfx/3rdparty/khronos)

include_directories(lib/lemon)

include_directories(lib/noisepp/core)
include_directories(lib/noisepp/threadpp)
include_directories(lib/noisepp/utils)

include_directories(lib/v-hacd/src/VHACD_Lib/inc)
include_directories(lib/v-hacd/src/VHACD_Lib/public)
list (APPEND vhacd-src
	lib/v-hacd/src/VHACD_Lib/src/btAlignedAllocator.cpp
	lib/v-hacd/src/VHACD_Lib/src/btConvexHullComputer.cpp
	lib/v-hacd/src/VHACD_Lib/src/VHACD.cpp
	lib/v-hacd/src/VHACD_Lib/src/vhacdICHull.cpp
	lib/v-hacd/src/VHACD_Lib/src/vhacdManifoldMesh.cpp
	lib/v-hacd/src/VHACD_Lib/src/vhacdMesh.cpp
	lib/v-hacd/src/VHACD_Lib/src/vhacdVolume.cpp
	)

#add_subdirectory(lib/pcl)

include_directories(lib/csgjs-cpp)

include_directories(lib/eigen/Eigen)
include_directories(lib/soil2/src/SOIL2)
list(APPEND soil2_src
	lib/soil2/src/SOIL2/etc1_utils.c
	lib/soil2/src/SOIL2/image_DXT.c
	lib/soil2/src/SOIL2/image_helper.c
	lib/soil2/src/SOIL2/SOIL2.c
	)

# TODO: Add when needed
include_directories(lib/cereal/include)

include_directories(common)
include_directories(common/bgfx_related)
include_directories(common/bgfx_related/font)
include_directories(common/bgfx_related/shaderc)
include_directories(common/bfgx_related/nanovg)
#include_directories(common/bgfx_related/imgui)

include_directories(lib/cppformat)

list(APPEND cppformat_src
	lib/cppformat/format.cc
	lib/cppformat/posix.cc
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
	#common/bgfx_related/nanovg/nanovg.cpp
	#common/bgfx_related/nanovg/nanovg_bgfx.cpp
	#common/bgfx_related/imgui/ocornut_imgui.cpp
	#common/bgfx_related/imgui/imgui.cpp
	#lib/bgfx/3rdparty/ocornut-imgui/imgui.cpp
	)

list(APPEND common_src
	common/Mesh.cpp
	common/Graphics.cpp
	common/Logger.cpp
	common/Application.cpp
	common/SandboxApp.cpp
	common/MathFuncs.cpp
	common/Drawable.cpp
	common/NoobUtils.cpp
	common/EditorUtils.cpp
	common/Voxels.cpp
	common/Font.cpp
	#common/bgfx_related/Shaders.cpp
	)

list(APPEND main_src platform/nix/EngineDesktop.cpp)

add_executable(NoobEdit ${soil2_src} ${main_src} ${common_src} ${bgfx_src} ${bgfx_utils_src} ${vhacd-src}) 

target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} freetype rt dl pthread assimp m OpenMeshCore)


project(LogServer)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

include_directories(lib/asio/asio/include)

list(APPEND logger_src
	common/LogServer.cpp
	)

add_executable(LogServer ${logger_src})

target_link_libraries(LogServer LINK_PUBLIC pthread)
