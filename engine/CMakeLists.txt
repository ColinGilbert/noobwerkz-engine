cmake_minimum_required(VERSION 3.0)
project(NoobWerkz)

#set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_BUILD_TYPE "RELEASE")

set(CMAKE_CXX_FLAGS "-std=c++11 -fpermissive -DASIO_STANDALONE -DOM_STATIC_BUILD -DEIGEN_MPL2_ONLY -DCHAISCRIPT_NO_THREADS -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 -fno-strict-aliasing -DSQLITE_ENABLE_COLUMN_METADATA -DSTB_IMAGE_STATIC -DSQLITE_ENABLE_FTS4 -DSQLITE_ENABLE_RTREE -Wall ${CMAKE_C_FLAGS}")

find_package(Boost COMPONENTS system filesystem REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})



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
#add_subdirectory(lib/rbdl)

include_directories(lib/openmesh/src)
add_subdirectory(lib/openmesh)

include_directories(lib/bx/include)
include_directories(lib/bgfx/include)
include_directories(lib/bgfx/3rdparty)
include_directories(lib/bgfx/3rdparty/khronos)

include_directories(lib/lemon)

#include_directories(lib/noisepp/core)
#include_directories(lib/noisepp/threadpp)
#include_directories(lib/noisepp/utils)

#list (APPEND noisepp_src
#	lib/noisepp/utils/NoiseBuilders.cpp
#	lib/noisepp/utils/NoiseGradientRenderer.cpp
#	lib/noisepp/utils/NoiseJobQueue.cpp
#	lib/noisepp/utils/NoiseOutStream.cpp
#	lib/noisepp/utils/NoiseEndianUtils.cpp
#	lib/noisepp/utils/NoiseInStream.cpp
#	lib/noisepp/utils/NoiseSystem.cpp
#	lib/noisepp/utils/NoiseWriter.cpp
#	lib/noisepp/utils/NoiseColourValue.cpp
#	lib/noisepp/utils/NoiseImage.cpp
#	lib/noisepp/utils/NoiseModules.cpp
#	lib/noisepp/utils/NoiseReader.cpp
#	)

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

#include_directories(lib/csgjs-cpp)

# TODO: Add when needed

include_directories(lib/cereal/include)
include_directories(lib/stb)

include_directories(lib/inline_variant_visitor)

include_directories(common)
include_directories(common/Physics)
include_directories(common/drawing_techniques)
include_directories(common/drawing)
include_directories(common/drawing/font)
include_directories(common/drawing/shaderc)
#include_directories(common/bfgx_related/nanovg)

#include_directories(common/libs/ozz-animation/include)

list(APPEND bgfx_src
	lib/bgfx/src/amalgamated.cpp
	)

list(APPEND bgfx_utils_src
	common/drawing/cube_atlas.cpp
	common/drawing/font/font_manager.cpp
	common/drawing/font/utf8.cpp
	common/drawing/font/text_metrics.cpp
	common/drawing/font/text_buffer_manager.cpp
	common/drawing/bounds.cpp
	#common/drawing/nanovg/nanovg_bgfx.cpp
	#common/drawing/nanovg/nanovg.cpp
	)

list(APPEND openmesh_src
	lib/openmesh/src/OpenMesh/Tools/Decimater/ModQuadricT.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/DecimaterT.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/Observer.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/ArrayKernelT.cc
	)

#include_directories(lib/sqlite)
#list (APPEND sqlite_src
#	lib/sqlite/sqlite3.c
#	)

#include_directories(lib/SQLiteCpp/include)
#list (APPEND sqlitecpp_src
#	lib/SQLiteCpp/src/Column.cpp
#	lib/SQLiteCpp/src/Database.cpp
#	lib/SQLiteCpp/src/Statement.cpp
#	lib/SQLiteCpp/src/Transaction.cpp
#	)

include_directories(lib/es)
list (APPEND es_src
	lib/es/es/storage.cpp
	)

include_directories(lib/variant)

list(APPEND common_src
	common/Mesh.cpp
	common/Graphics.cpp
	common/Logger.cpp
	common/Application.cpp
	common/MathFuncs.cpp
	common/Drawable2D.cpp
	common/Drawable3D.cpp
	common/NoobUtils.cpp
	common/EditorUtils.cpp
	common/VoxelWorld.cpp
	common/UIFont.cpp
	common/Physics/PhysicsBody.cpp
	common/Physics/PhysicsConstraintGeneric.cpp
	common/Physics/PhysicsShape.cpp
	common/Physics/PhysicsWorld.cpp
	common/Physics/CharacterController.cpp
	common/Skeleton.cpp
	common/ModelLoader.cpp
	# common/DebugDraw.cpp
	common/GUI.cpp
	common/Camera.cpp
	common/Stage.cpp
	common/Scenery.cpp
	common/Actor.cpp
	common/Prop.cpp
	common/AnimatedModel.cpp
	common/drawing/TriplanarGradientMap.cpp
	common/drawing/HACDRenderer.cpp
	common/drawing/GUIRenderer.cpp
	)

include_directories(platform/nix)

list(APPEND main_src platform/nix/EngineDesktop.cpp platform/nix/NDOF.cpp)

add_executable(NoobEdit ${main_src} ${common_src} ${bgfx_src} ${bgfx_utils_src} ${vhacd_src} ${openmesh_src} ${cppformat_src} ${es_src})

target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES}  freetype rt dl pthread assimp m OpenMeshCore OpenMeshToolsStatic emon BulletDynamics BulletCollision BulletSoftBody LinearMath spnav ozz_animation_offline_r ozz_animation_r ozz_geometry_r ozz_base_r ${Boost_LIBRARIES})

project(LogServer)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

include_directories(lib/asio/asio/include)

list(APPEND logger_src
	common/LogServer.cpp
	)

add_executable(LogServer ${logger_src})

target_link_libraries(LogServer LINK_PUBLIC pthread)
