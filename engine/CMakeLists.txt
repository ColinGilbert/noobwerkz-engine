cmake_minimum_required(VERSION 3.0)
project(NoobWerkz)

set(CMAKE_BUILD_TYPE "DEBUG")
#set(CMAKE_BUILD_TYPE "RELEASE")

set(CMAKE_CXX_FLAGS "-std=c++1y -O2 -fno-strict-aliasing -fpermissive -DASIO_STANDALONE -DOM_STATIC_BUILD -DOM_GARBAGE_NO_STATUS_WARNING -DEIGEN_MPL2_ONLY -Wall ${CMAKE_CXX_FLAGS}") # -DCHAISCRIPT_NO_THREADS
set(CMAKE_C_FLAGS "-std=c99 -fno-strict-aliasing -DSQLITE_ENABLE_COLUMN_METADATA -DSTB_IMAGE_STATIC -DSQLITE_ENABLE_FTS4 -DSQLITE_ENABLE_RTREE -Wall ${CMAKE_C_FLAGS}")

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#include_directories(lib/tinympl)

include_directories(lib/brigand)

include_directories(lib/es)
list (APPEND es_src
	lib/es/es/storage.cpp
	)

include_directories(lib/cppformat)
list (APPEND cppformat_src
	lib/cppformat/format.cc
	lib/cppformat/posix.cc
	)

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

include_directories(lib/freetype/include)
add_subdirectory(lib/freetype)

#add_subdirectory(lib/polyvox)
include_directories(lib/polyvox/include)



include_directories(lib/d-collide/d-collide)
list (APPEND d_collide_src
    lib/d-collide/d-collide/debug.cpp

    lib/d-collide/d-collide/broadphase/broadphase.cpp
    lib/d-collide/d-collide/broadphase/broadphasefactory.cpp
    lib/d-collide/d-collide/broadphase/broadphaseproxydata.cpp
    lib/d-collide/d-collide/broadphase/broadphasejob.cpp
    lib/d-collide/d-collide/broadphase/broadphasecollisions.cpp

    # Broadphase Octree Files
    lib/d-collide/d-collide/roadphase/broadphasehierarchicalgrid.cpp
    lib/d-collide/d-collide/roadphase/broadphasehierarchicalgridproxydata.cpp
    lib/d-collide/d-collide/roadphase/broadphasehierarchicalgridjob.cpp
    lib/d-collide/d-collide/broadphase/broadphasehierarchicalgridjob2.cpp
    lib/d-collide/d-collide/roadphase/broadphasehierarchicalgridjobcollection.cpp
    lib/d-collide/d-collide/roadphase/hierarchicalgrid.cpp

    # BroadPhase Brute Force Files
    lib/d-collide/d-collide/roadphase/broadphasebruteforce.cpp
    lib/d-collide/d-collide/roadphase/broadphasebruteforcejob.cpp

    lib/d-collide/d-collide/math/matrix.cpp
    lib/d-collide/d-collide/math/quaternion.cpp
    lib/d-collide/d-collide/math/vector.cpp
    lib/d-collide/d-collide/math/plane.cpp
    lib/d-collide/d-collide/orld.cpp
    lib/d-collide/d-collide/ipeline.cpp
    lib/d-collide/d-collide/orldparameters.cpp
    lib/d-collide/d-collide/orldcollisions.cpp
    lib/d-collide/d-collide/roxy.cpp
    lib/d-collide/d-collide/roxyfactory.cpp
    lib/d-collide/d-collide/hread/thread.cpp
    lib/d-collide/d-collide/hread/jobpool.cpp
    lib/d-collide/d-collide/hread/jobpoolcollection.cpp
    lib/d-collide/d-collide/hread/threadpool.cpp
    lib/d-collide/d-collide/hread/threadjobcollection.cpp
    lib/d-collide/d-collide/ebuglog.cpp
    lib/d-collide/d-collide/ebugstream.cpp
    lib/d-collide/d-collide/ebugstreamconfiguration_initialize.cpp

    lib/d-collide/d-collide/etectordeform/detectordeformmanager.cpp
    lib/d-collide/d-collide/etectordeform/detectordeformalgorithm.cpp
    lib/d-collide/d-collide/detectordeform/detectordeformjob.cpp
    lib/d-collide/d-collide/detectordeform/detectordeformproxydata.cpp
    lib/d-collide/d-collide/detectordeform/spatialhash/intvector3.cpp
    lib/d-collide/d-collide/detectordeform/spatialhash/simplehash.cpp
    lib/d-collide/d-collide/detectordeform/spatialhash/spatialhash.cpp
    lib/d-collide/d-collide/detectordeform/spatialhash/spatialgrid.cpp
    lib/d-collide/d-collide/detectordeform/spatialhash/spatialhashalgorithm.cpp
    lib/d-collide/d-collide/detectordeform/spatialhash/spatialhashjobcollection.cpp
    lib/d-collide/d-collide/detectordeform/surfacehierarchy/surfacehierarchy.cpp
    lib/d-collide/d-collide/detectordeform/surfacehierarchy/normalcone.cpp
    lib/d-collide/d-collide/detectordeform/surfacehierarchy/deformablebvhnode.cpp
    lib/d-collide/d-collide/detectordeform/surfacehierarchy/surfacehierarchyalgorithm.cpp
    lib/d-collide/d-collide/detectordeform/surfacehierarchy/surfacehierarchyjob.cpp
    lib/d-collide/d-collide/detectordeform/surfacehierarchy/surfacehierarchyselfjob.cpp
    lib/d-collide/d-collide/detectordeform/surfacehierarchy/surfacehierarchyproxydata.cpp
    lib/d-collide/d-collide/detectordeform/bvhierarchy/bvhierarchyalgorithm.cpp
    lib/d-collide/d-collide/detectordeform/bvhierarchy/bvhierarchybvhnode.cpp
    lib/d-collide/d-collide/detectordeform/bvhierarchy/bvhierarchyjob.cpp
    lib/d-collide/d-collide/detectordeform/bvhierarchy/bvhierarchyproxydata.cpp

    lib/d-collide/d-collide/bvhnode.cpp
    lib/d-collide/d-collide/bvhnodedefault.cpp
    lib/d-collide/d-collide/meshsplitter.cpp

    lib/d-collide/d-collide/boundingvolumes/aabb.cpp
    lib/d-collide/d-collide/boundingvolumes/obb.cpp
    lib/d-collide/d-collide/boundingvolumes/boundingvolume.cpp
    lib/d-collide/d-collide/boundingvolumes/boundingsphere.cpp
    lib/d-collide/d-collide/boundingvolumes/kdop.cpp

    lib/d-collide/d-collide/shapes/box.cpp
    lib/d-collide/d-collide/shapes/cone.cpp
    lib/d-collide/d-collide/shapes/cylinder.cpp
    lib/d-collide/d-collide/shapes/mesh.cpp
    lib/d-collide/d-collide/shapes/shape.cpp
    lib/d-collide/d-collide/shapes/sphere.cpp
    lib/d-collide/d-collide/shapes/wedge.cpp
    lib/d-collide/d-collide/shapes/mesh/triangle.cpp
    lib/d-collide/d-collide/shapes/mesh/vertex.cpp
    lib/d-collide/d-collide/shapes/mesh/line.cpp
    lib/d-collide/d-collide/shapes/mesh/meshfactory.cpp

    lib/d-collide/d-collide/exceptions/exception.cpp


    lib/d-collide/d-collide/detectorrigid/bvhtraverse.cpp
    lib/d-collide/d-collide/detectorrigid/middlephaserigidjob.cpp
    lib/d-collide/d-collide/detectorrigid/middlephaserigidjobcreator.cpp

    lib/d-collide/d-collide/narrowphase/narrowphase.cpp
    lib/d-collide/d-collide/narrowphase/triangleintersector.cpp
    lib/d-collide/d-collide/narrowphase/boxsphereintersector.cpp
    lib/d-collide/d-collide/narrowphase/boxboxintersector.cpp
    lib/d-collide/d-collide/narrowphase/wedgeintersector.cpp
    lib/d-collide/d-collide/narrowphase/intersectionhelpers.cpp
    lib/d-collide/d-collide/narrowphase/cylinderintersector.cpp
    lib/d-collide/d-collide/narrowphase/coneintersector.cpp
    lib/d-collide/d-collide/narrowphase/spheresphereintersector.cpp
    lib/d-collide/d-collide/narrowphase/meshmeshintersector.cpp
    lib/d-collide/d-collide/narrowphase/narrowphasejob.cpp
    lib/d-collide/d-collide/narrowphase/spheretriangleintersector.cpp
    
    lib/d-collide/d-collide/collisioncache.cpp
    lib/d-collide/d-collide/collisionpair.cpp

	)


#include_directories(lib/eigen)
#add_subdirectory(lib/rbdl)

include_directories(lib/csgjs-cpp)
list (APPEND csgjs_src
	lib/csgjs-cpp/csgjs.cpp
	)

include_directories(lib/openmesh/src)
#add_subdirectory(lib/openmesh)

include_directories(lib/bx/include)
include_directories(lib/bgfx/include)
include_directories(lib/bgfx/3rdparty)
include_directories(lib/bgfx/3rdparty/khronos)

include_directories(lib/lemon)

include_directories(lib/cereal/include)

include_directories(lib/stb)

include_directories(lib/inline_variant_visitor)

include_directories(common)
include_directories(common/drawing)
include_directories(common/drawing/font)
include_directories(common/drawing/shaderc)

list(APPEND bgfx_src
	lib/bgfx/src/amalgamated.cpp
	)

list(APPEND bgfx_utils_src
	common/drawing/font/font_manager.cpp
	common/drawing/font/utf8.cpp
	common/drawing/font/text_metrics.cpp
	common/drawing/font/text_buffer_manager.cpp
	common/drawing/font/cube_atlas.cpp
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
include_directories(lib/angelscript-sdk/add_on/scriptbuilder)
include_directories(lib/angelscript-sdk/add_on/scriptstdstring)

list(APPEND angelscript_src
	lib/angelscript-sdk/angelscript/source/as_atomic.cpp
	lib/angelscript-sdk/angelscript/source/as_builder.cpp
	lib/angelscript-sdk/angelscript/source/as_bytecode.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_arm.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_mips.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_ppc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_ppc_64.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_sh4.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_gcc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_mingw.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_msvc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x86.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_xenon.cpp
	lib/angelscript-sdk/angelscript/source/as_compiler.cpp
	lib/angelscript-sdk/angelscript/source/as_configgroup.cpp
	lib/angelscript-sdk/angelscript/source/as_context.cpp
	lib/angelscript-sdk/angelscript/source/as_datatype.cpp
	lib/angelscript-sdk/angelscript/source/as_gc.cpp
	lib/angelscript-sdk/angelscript/source/as_generic.cpp
	lib/angelscript-sdk/angelscript/source/as_globalproperty.cpp
	lib/angelscript-sdk/angelscript/source/as_memory.cpp
	lib/angelscript-sdk/angelscript/source/as_module.cpp
	lib/angelscript-sdk/angelscript/source/as_objecttype.cpp
	lib/angelscript-sdk/angelscript/source/as_outputbuffer.cpp
	lib/angelscript-sdk/angelscript/source/as_parser.cpp
	lib/angelscript-sdk/angelscript/source/as_restore.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptcode.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptengine.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptfunction.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptnode.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptobject.cpp
	lib/angelscript-sdk/angelscript/source/as_string.cpp
	lib/angelscript-sdk/angelscript/source/as_string_util.cpp
	lib/angelscript-sdk/angelscript/source/as_thread.cpp
	lib/angelscript-sdk/angelscript/source/as_tokenizer.cpp
	lib/angelscript-sdk/angelscript/source/as_typeinfo.cpp
	lib/angelscript-sdk/angelscript/source/as_variablescope.cpp
	lib/angelscript-sdk/add_on/scriptany/scriptany.cpp
	lib/angelscript-sdk/add_on/scriptdictionary/scriptdictionary.cpp
	lib/angelscript-sdk/add_on/scriptmath/scriptmathcomplex.cpp
	lib/angelscript-sdk/add_on/scriptmath/scriptmath.cpp
	lib/angelscript-sdk/add_on/debugger/debugger.cpp
	lib/angelscript-sdk/add_on/contextmgr/contextmgr.cpp
	#lib/angelscript-sdk/add_on/autowrapper/generator/generateheader.cpp
	lib/angelscript-sdk/add_on/serializer/serializer.cpp
	lib/angelscript-sdk/add_on/weakref/weakref.cpp
	lib/angelscript-sdk/add_on/scriptbuilder/scriptbuilder.cpp
	lib/angelscript-sdk/add_on/scriptarray/scriptarray.cpp
	lib/angelscript-sdk/add_on/scriptgrid/scriptgrid.cpp
	#lib/angelscript-sdk/add_on/scriptfile/scriptfilesystem.cpp
	#lib/angelscript-sdk/add_on/scriptfile/scriptfile.cpp
	lib/angelscript-sdk/add_on/scripthelper/scripthelper.cpp
	lib/angelscript-sdk/add_on/scripthandle/scripthandle.cpp
	lib/angelscript-sdk/add_on/scriptstdstring/scriptstdstring.cpp
	lib/angelscript-sdk/add_on/scriptstdstring/scriptstdstring_utils.cpp
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
	common/GUI.cpp
	common/Camera.cpp
	common/Stage.cpp
	common/BodyController.cpp
	common/Shape.cpp
	common/MeshUtils.cpp
	common/BasicModel.cpp
	common/AnimatedModel.cpp
	common/SkeletalAnim.cpp
	common/ModelLoader.cpp
	common/drawing/TriplanarGradientMap.cpp
	common/drawing/HACDRenderer.cpp
	common/drawing/OverlayRenderer.cpp
	common/drawing/BasicRenderer.cpp
	common/SimpleNavMesh.cpp
	common/FileSystem.cpp
	)

list(APPEND user_src
	../sandbox/UserApp.cpp
	)

include_directories(platform/nix)

list(APPEND main_src platform/nix/EngineDesktop.cpp platform/nix/NDOF.cpp)

add_executable(NoobEdit ${main_src} ${common_src} ${bgfx_src} ${csgjs_src}
	${bgfx_utils_src} ${vhacd_src} ${openmesh_src} ${cppformat_src}
	${es_src} ${user_src} ${recast_src} ${angelscript_src} ${es_src} ) 

target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES}
	${OPENGL_LIBRARIES} freetype rt dl pthread assimp m emon OpenMeshCore
	spnav ozz_animation_offline_r ozz_animation_r
	ozz_geometry_r ozz_base_r BulletDynamics BulletCollision
	BulletSoftBody LinearMath ${Boost_LIBRARIES})
