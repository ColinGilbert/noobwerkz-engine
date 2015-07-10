cmake_minimum_required(VERSION 3.0)
project(NoobWerkz)

set(CMAKE_CXX_FLAGS "-std=c++11 -fno-strict-aliasing -fpermissive -DASIO_STANDALONE -DOM_STATIC_BUILD -DEIGEN_MPL2_ONLY -Wall ${CMAKE_CXX_FLAGS}") #-DSQLITECPP_ENABLE_ASSERT_HANDLER -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 -fno-strict-aliasing -DSQLITE_ENABLE_COLUMN_METADATA -DSTB_IMAGE_STATIC -DSQLITE_ENABLE_FTS4 -DSQLITE_ENABLE_RTREE -Wall ${CMAKE_C_FLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

include_directories(lib/chaiscript/include)
list (APPEND chai-src
lib/chaiscript/src/main.cpp
lib/chaiscript/src/stl_extra.cpp
)

include_directories(lib/cppformat)
list (APPEND cppformat_src
	lib/cppformat/format.cc
	lib/cppformat/posix.cc
	)

#include_directories(lib/clipper/cpp)
#list(APPEND clipper_src
	#lib/clipper/cpp/clipper.cpp
	#	)

#include_directories(lib/poly2tri/poly2tri/common)
#include_directories(lib/poly2tri/poly2tri/sweep)
#include_directories(lib/poly2tri/poly2tri)

#list(APPEND poly2tri_src
	#	lib/poly2tri/poly2tri/common/shapes.cc
	#	lib/poly2tri/poly2tri/sweep/advancing_front.cc
	#	lib/poly2tri/poly2tri/sweep/cdt.cc
	#	lib/poly2tri/poly2tri/sweep/sweep_context.cc
	#	lib/poly2tri/poly2tri/sweep/sweep.cc
	#)

	#include_directories(lib/clip2tri/clip2tri)
	#include_directories(lib/clip2tri/clipper)
	#include_directories(lib/clip2tri/polytri)
	#include_directories(lib/clip2tri/common)
	#include_directories(lib/clip2tri/sweep)

list(APPEND clip2tri_src
	lib/clip2tri/clip2tri/clip2tri.cpp
	lib/clip2tri/clipper/clipper.cpp
	lib/clip2tri/poly2tri/common/shapes.cc
	lib/clip2tri/poly2tri/sweep/advancing_front.cc
	lib/clip2tri/poly2tri/sweep/cdt.cc
	lib/clip2tri/poly2tri/sweep/sweep.cc
	lib/clip2tri/poly2tri/sweep/sweep_context.cc
	)

include_directories(lib/bstrlib)
list(APPEND bstrlib_src
	lib/bstrlib/bstrlib.c
	lib/bstrlib/bstraux.c
	lib/bstrlib/bstrwrap.cpp
	#lib/bstrlib/bsafe.c
	)

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

include_directories(lib/freetype/include)
add_subdirectory(lib/freetype)

add_subdirectory(lib/polyvox)
include_directories(lib/polyvox/include)

include_directories(lib/eigen)
#include_directories(lib/eigen/Eigen)
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

list (APPEND noisepp_src
	lib/noisepp/utils/NoiseBuilders.cpp
	lib/noisepp/utils/NoiseGradientRenderer.cpp
	lib/noisepp/utils/NoiseJobQueue.cpp
	lib/noisepp/utils/NoiseOutStream.cpp
	lib/noisepp/utils/NoiseEndianUtils.cpp
	lib/noisepp/utils/NoiseInStream.cpp
	lib/noisepp/utils/NoiseSystem.cpp
	lib/noisepp/utils/NoiseWriter.cpp
	lib/noisepp/utils/NoiseColourValue.cpp
	lib/noisepp/utils/NoiseImage.cpp
	lib/noisepp/utils/NoiseModules.cpp
	lib/noisepp/utils/NoiseReader.cpp
	)

include_directories(lib/v-hacd/src/VHACD_Lib/inc)
include_directories(lib/v-hacd/src/VHACD_Lib/public)
list (APPEND vhacd_src
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

# TODO: Add when needed
include_directories(lib/cereal/include)
include_directories(lib/stb)

include_directories(common)
include_directories(common/drawing_techniques)
include_directories(common/bgfx_related)
include_directories(common/bgfx_related/font)
include_directories(common/bgfx_related/shaderc)
include_directories(common/bfgx_related/nanovg)

#include_directories(lib/clipper/cpp)
#list(APPEND clipper_src
#	lib/clipper/cpp/clipper.cpp
#	)

#include_directories(lib/cppformat)
#list(APPEND cppformat_src
#	lib/cppformat/format.cc
#	lib/cppformat/posix.cc
#	)

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
	common/bgfx_related/nanovg/nanovg_bgfx.cpp
	common/bgfx_related/nanovg/nanovg.cpp
	)

list(APPEND openmesh_src
	#lib/openmesh/src/OpenMesh/Tools/Decimater/BaseDecimaterT.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/ModQuadricT.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/DecimaterT.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/Observer.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/ArrayKernelT.cc
	)

include_directories(lib/sqlite)
list (APPEND sqlite_src
	lib/sqlite/sqlite3.c
	)

include_directories(lib/SQLiteCpp/include)
list (APPEND sqlitecpp_src
	lib/SQLiteCpp/src/Column.cpp
	lib/SQLiteCpp/src/Database.cpp
	lib/SQLiteCpp/src/Statement.cpp
	lib/SQLiteCpp/src/Transaction.cpp
	)

list(APPEND common_src
	common/Mesh.cpp
	common/Graphics.cpp
	common/Logger.cpp
	common/Application.cpp
	common/MathFuncs.cpp
	common/Drawable.cpp
	common/NoobUtils.cpp
	common/EditorUtils.cpp
	common/VoxelWorld.cpp
	common/UIFont.cpp
	common/PhysicsWorld.cpp
	common/DebugDraw.cpp
	common/GUI.cpp
	common/Scene.cpp
	common/Camera.cpp
	common/Scene.cpp
	common/drawing_techniques/TriPlanar.cpp
	common/drawing_techniques/HACDRenderer.cpp
	common/drawing_techniques/GUIRenderer.cpp
	)

include_directories(platform/nix)
list(APPEND main_src platform/nix/EngineDesktop.cpp platform/nix/NDOF.cpp)

add_executable(NoobEdit ${main_src} ${common_src} ${bgfx_src}
	${bgfx_utils_src} ${vhacd_src} ${openmesh_src} ${cppformat_src} ${noisepp_src}
	${sqlite_src} ${sqlitecpp_src})

target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES}
	${OPENGL_LIBRARIES} freetype rt dl pthread assimp m OpenMeshCore
	OpenMeshTools emon BulletDynamics BulletCollision BulletSoftBody
	LinearMath spnav)


project(LogServer)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

include_directories(lib/asio/asio/include)

list(APPEND logger_src
	common/LogServer.cpp
	)

add_executable(LogServer ${logger_src})

target_link_libraries(LogServer LINK_PUBLIC pthread)
