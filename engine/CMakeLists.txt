cmake_minimum_required(VERSION 3.0)
project(NoobWerkz)

#set(CMAKE_BUILD_TYPE "DEBUG")
#set(CMAKE_BUILD_TYPE "RELEASE")

set(CMAKE_CXX_FLAGS "-std=c++1y -O2 -fno-strict-aliasing -fpermissive -DASIO_STANDALONE -DOM_STATIC_BUILD -DOM_GARBAGE_NO_STATUS_WARNING -DEIGEN_MPL2_ONLY -Wall ${CMAKE_CXX_FLAGS}") # -DCHAISCRIPT_NO_THREADS
set(CMAKE_C_FLAGS "-std=c99 -fno-strict-aliasing -DSQLITE_ENABLE_COLUMN_METADATA -DSTB_IMAGE_STATIC -DSQLITE_ENABLE_FTS4 -DSQLITE_ENABLE_RTREE -Wall ${CMAKE_C_FLAGS}")

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include_directories(lib/tinympl)

include_directories(lib/cppformat)
list (APPEND cppformat_src
	lib/cppformat/format.cc
	lib/cppformat/posix.cc
	)

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

include_directories(lib/freetype/include)
add_subdirectory(lib/freetype)

add_subdirectory(lib/polyvox)
include_directories(lib/polyvox/include)

#include_directories(lib/eigen)
#add_subdirectory(lib/rbdl)

include_directories(lib/csgjs-cpp)
list (APPEND csgjs_src
	lib/csgjs-cpp/csgjs.cpp
	)

include_directories(lib/openmesh/src)
add_subdirectory(lib/openmesh)

include_directories(lib/bx/include)
include_directories(lib/bgfx/include)
include_directories(lib/bgfx/3rdparty)
include_directories(lib/bgfx/3rdparty/khronos)

include_directories(lib/lemon)

include_directories(lib/cereal/include)
include_directories(lib/stb)

include_directories(lib/inline_variant_visitor)

include_directories(common)
include_directories(common/Drawing)
include_directories(common/Drawing/font)
include_directories(common/Drawing/shaderc)

list(APPEND bgfx_src
	lib/bgfx/src/amalgamated.cpp
	)

list(APPEND bgfx_utils_src
	common/Drawing/cube_atlas.cpp
	common/Drawing/font/font_manager.cpp
	common/Drawing/font/utf8.cpp
	common/Drawing/font/text_metrics.cpp
	common/Drawing/font/text_buffer_manager.cpp
	common/Drawing/bounds.cpp
	)

list(APPEND openmesh_src
	lib/openmesh/src/OpenMesh/Tools/Decimater/ModQuadricT.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/DecimaterT.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/Observer.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/ArrayKernelT.cc
	)

include_directories(lib/variant)
include_directories(lib/recastnavigation/Detour/Include)
include_directories(lib/recastnavigation/Recast/Include)
include_directories(lib/recastnavigation/RecastDemo/Include)
include_directories(lib/recastnavigation/DebugUtils/Include)
include_directories(lib/recastnavigation/DetourTileCache/Include)
include_directories(lib/recastnavigation/DetourCrowd/Include)

list (APPEND recast_src
	lib/recastnavigation/Recast/Source/RecastAlloc.cpp
	lib/recastnavigation/Recast/Source/RecastLayers.cpp
	lib/recastnavigation/Recast/Source/RecastContour.cpp
	lib/recastnavigation/Recast/Source/RecastArea.cpp
	lib/recastnavigation/Recast/Source/RecastMeshDetail.cpp
	lib/recastnavigation/Recast/Source/RecastRasterization.cpp
	lib/recastnavigation/Recast/Source/RecastMesh.cpp
	lib/recastnavigation/Recast/Source/RecastFilter.cpp
	lib/recastnavigation/Recast/Source/Recast.cpp
	lib/recastnavigation/Recast/Source/RecastRegion.cpp
	lib/recastnavigation/DetourTileCache/Source/DetourTileCacheBuilder.cpp
	lib/recastnavigation/DetourTileCache/Source/DetourTileCache.cpp
	#lib/recastnavigation/DebugUtils/Source/DetourDebugDraw.cpp
	#lib/recastnavigation/DebugUtils/Source/RecastDebugDraw.cpp
	#lib/recastnavigation/DebugUtils/Source/RecastDump.cpp
	#lib/recastnavigation/DebugUtils/Source/DebugDraw.cpp
	#lib/recastnavigation/RecastDemo/Source/CrowdTool.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample_TempObstacles.cpp
	#lib/recastnavigation/RecastDemo/Source/main.cpp
	#lib/recastnavigation/RecastDemo/Source/imguiRenderGL.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample.cpp
	#lib/recastnavigation/RecastDemo/Source/Filelist.cpp
	#lib/recastnavigation/RecastDemo/Source/ConvexVolumeTool.cpp
	#lib/recastnavigation/RecastDemo/Source/imgui.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample_Debug.cpp
	#lib/recastnavigation/RecastDemo/Source/SlideShow.cpp
	#lib/recastnavigation/RecastDemo/Source/NavMeshPruneTool.cpp
	#lib/recastnavigation/RecastDemo/Source/PerfTimer.cpp
	#lib/recastnavigation/RecastDemo/Source/InputGeom.cpp
	#lib/recastnavigation/RecastDemo/Source/NavMeshTesterTool.cpp
	#lib/recastnavigation/RecastDemo/Source/MeshLoaderObj.cpp
	#lib/recastnavigation/RecastDemo/Source/ChunkyTriMesh.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample_SoloMesh.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample_TileMesh.cpp
	#lib/recastnavigation/RecastDemo/Source/SampleInterfaces.cpp
	#lib/recastnavigation/RecastDemo/Source/OffMeshConnectionTool.cpp
	#lib/recastnavigation/RecastDemo/Source/TestCase.cpp
	#lib/recastnavigation/RecastDemo/Source/ValueHistory.cpp
	#lib/recastnavigation/RecastDemo/Contrib/fastlz/fastlz.c
	lib/recastnavigation/Detour/Source/DetourCommon.cpp
	lib/recastnavigation/Detour/Source/DetourNavMesh.cpp
	lib/recastnavigation/Detour/Source/DetourNavMeshBuilder.cpp
	lib/recastnavigation/Detour/Source/DetourAlloc.cpp
	lib/recastnavigation/Detour/Source/DetourNode.cpp
	lib/recastnavigation/Detour/Source/DetourNavMeshQuery.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourPathQueue.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourObstacleAvoidance.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourProximityGrid.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourPathCorridor.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourCrowd.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourLocalBoundary.cpp
	)

include_directories(lib/angelscript-sdk/angelscript/include)

list(APPEND angelscript_src
	lib/angelscript-sdk/angelscript/source/as_array.h
	lib/angelscript-sdk/angelscript/source/as_atomic.cpp
	lib/angelscript-sdk/angelscript/source/as_atomic.h
	lib/angelscript-sdk/angelscript/source/as_builder.cpp
	lib/angelscript-sdk/angelscript/source/as_builder.h
	lib/angelscript-sdk/angelscript/source/as_bytecode.cpp
	lib/angelscript-sdk/angelscript/source/as_bytecode.h
	lib/angelscript-sdk/angelscript/source/as_callfunc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc.h
	lib/angelscript-sdk/angelscript/source/as_callfunc_arm.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_arm_gcc.S
	lib/angelscript-sdk/angelscript/source/as_callfunc_arm_msvc.asm
	lib/angelscript-sdk/angelscript/source/as_callfunc_arm_vita.S
	lib/angelscript-sdk/angelscript/source/as_callfunc_arm_xcode.S
	lib/angelscript-sdk/angelscript/source/as_callfunc_mips.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_ppc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_ppc_64.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_sh4.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_gcc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_mingw.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_msvc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_msvc_asm.asm
	lib/angelscript-sdk/angelscript/source/as_callfunc_x86.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_xenon.cpp
	lib/angelscript-sdk/angelscript/source/as_compiler.cpp
	lib/angelscript-sdk/angelscript/source/as_compiler.h
	lib/angelscript-sdk/angelscript/source/as_config.h
	lib/angelscript-sdk/angelscript/source/as_configgroup.cpp
	lib/angelscript-sdk/angelscript/source/as_configgroup.h
	lib/angelscript-sdk/angelscript/source/as_context.cpp
	lib/angelscript-sdk/angelscript/source/as_context.h
	lib/angelscript-sdk/angelscript/source/as_criticalsection.h
	lib/angelscript-sdk/angelscript/source/as_datatype.cpp
	lib/angelscript-sdk/angelscript/source/as_datatype.h
	lib/angelscript-sdk/angelscript/source/as_debug.h
	lib/angelscript-sdk/angelscript/source/as_gc.cpp
	lib/angelscript-sdk/angelscript/source/as_gc.h
	lib/angelscript-sdk/angelscript/source/as_generic.cpp
	lib/angelscript-sdk/angelscript/source/as_generic.h
	lib/angelscript-sdk/angelscript/source/as_globalproperty.cpp
	lib/angelscript-sdk/angelscript/source/as_map.h
	lib/angelscript-sdk/angelscript/source/as_memory.cpp
	lib/angelscript-sdk/angelscript/source/as_memory.h
	lib/angelscript-sdk/angelscript/source/as_module.cpp
	lib/angelscript-sdk/angelscript/source/as_module.h
	lib/angelscript-sdk/angelscript/source/as_namespace.h
	lib/angelscript-sdk/angelscript/source/as_objecttype.cpp
	lib/angelscript-sdk/angelscript/source/as_objecttype.h
	lib/angelscript-sdk/angelscript/source/as_outputbuffer.cpp
	lib/angelscript-sdk/angelscript/source/as_outputbuffer.h
	lib/angelscript-sdk/angelscript/source/as_parser.cpp
	lib/angelscript-sdk/angelscript/source/as_parser.h
	lib/angelscript-sdk/angelscript/source/as_property.h
	lib/angelscript-sdk/angelscript/source/as_restore.cpp
	lib/angelscript-sdk/angelscript/source/as_restore.h
	lib/angelscript-sdk/angelscript/source/as_scriptcode.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptcode.h
	lib/angelscript-sdk/angelscript/source/as_scriptengine.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptengine.h
	lib/angelscript-sdk/angelscript/source/as_scriptfunction.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptfunction.h
	lib/angelscript-sdk/angelscript/source/as_scriptnode.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptnode.h
	lib/angelscript-sdk/angelscript/source/as_scriptobject.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptobject.h
	lib/angelscript-sdk/angelscript/source/as_string.cpp
	lib/angelscript-sdk/angelscript/source/as_string.h
	lib/angelscript-sdk/angelscript/source/as_string_util.cpp
	lib/angelscript-sdk/angelscript/source/as_string_util.h
	lib/angelscript-sdk/angelscript/source/as_symboltable.h
	lib/angelscript-sdk/angelscript/source/as_texts.h
	lib/angelscript-sdk/angelscript/source/as_thread.cpp
	lib/angelscript-sdk/angelscript/source/as_thread.h
	lib/angelscript-sdk/angelscript/source/as_tokendef.h
	lib/angelscript-sdk/angelscript/source/as_tokenizer.cpp
	lib/angelscript-sdk/angelscript/source/as_tokenizer.h
	lib/angelscript-sdk/angelscript/source/as_typeinfo.cpp
	lib/angelscript-sdk/angelscript/source/as_typeinfo.h
	lib/angelscript-sdk/angelscript/source/as_variablescope.cpp
	lib/angelscript-sdk/angelscript/source/as_variablescope.h

	)

list(APPEND common_src
	common/BasicMesh.cpp
	common/Graphics.cpp
	common/Logger.cpp
	common/Application.cpp
	common/MathFuncs.cpp
	common/NoobUtils.cpp
	common/EditorUtils.cpp
	common/VoxelWorld.cpp
	common/UIFont.cpp
	common/CharacterController.cpp
	common/GUI.cpp
	common/Camera.cpp
	common/Stage.cpp
	#common/Scenery.cpp
	#common/Actor.cpp
	#common/Prop.cpp
	common/Body.cpp
	common/Shape.cpp
	common/MeshUtils.cpp
	common/BasicModel.cpp
	common/AnimatedModel.cpp
	common/SkeletalAnim.cpp
	common/ModelLoader.cpp
	common/Drawing/TriplanarGradientMap.cpp
	common/Drawing/HACDRenderer.cpp
	common/Drawing/GUIRenderer.cpp
	common/Drawing/BasicRenderer.cpp
	common/SimpleNavMesh.cpp
	)

list(APPEND user_src
	../sandbox/UserApp.cpp
	)

include_directories(platform/nix)

list(APPEND main_src platform/nix/EngineDesktop.cpp platform/nix/NDOF.cpp)

add_executable(NoobEdit ${main_src} ${common_src} ${bgfx_src} ${csgjs_src}
	${bgfx_utils_src} ${vhacd_src} ${openmesh_src} ${cppformat_src}
	${es_src} ${user_src} ${recast_src} ${angelscript_src}) 

target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES}
	${OPENGL_LIBRARIES} freetype rt dl pthread assimp m emon OpenMeshCore
	OpenMeshToolsStatic spnav ozz_animation_offline_r ozz_animation_r
	ozz_geometry_r ozz_base_r BulletDynamics BulletCollision
	BulletSoftBody LinearMath ${Boost_LIBRARIES})

project(LogServer)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

include_directories(lib/asio/asio/include)

list(APPEND logger_src
	common/LogServer.cpp
	)

add_executable(LogServer ${logger_src})

target_link_libraries(LogServer LINK_PUBLIC pthread)
