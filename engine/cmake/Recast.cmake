
if (DEFINED NOOB_USE_RECAST)

	include_directories(lib/recastnavigation/Detour/Include)
	include_directories(lib/recastnavigation/Recast/Include)
	include_directories(lib/recastnavigation/RecastDemo/Include)
	include_directories(lib/recastnavigation/DebugUtils/Include)
	include_directories(lib/recastnavigation/DetourTileCache/Include)
	include_directories(lib/recastnavigation/DetourCrowd/Include)

	list (APPEND main_src
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
	#########################
	#
	# Consciously excluded:
	#
	#########################
	#
	# lib/recastnavigation/DebugUtils/Source/DetourDebugDraw.cpp
	# lib/recastnavigation/DebugUtils/Source/RecastDebugDraw.cpp
	# lib/recastnavigation/DebugUtils/Source/RecastDump.cpp
	# lib/recastnavigation/DebugUtils/Source/DebugDraw.cpp
	# lib/recastnavigation/RecastDemo/Source/CrowdTool.cpp
	# lib/recastnavigation/RecastDemo/Source/Sample_TempObstacles.cpp
	# lib/recastnavigation/RecastDemo/Source/main.cpp
	# lib/recastnavigation/RecastDemo/Source/imguiRenderGL.cpp
	# lib/recastnavigation/RecastDemo/Source/Sample.cpp
	# lib/recastnavigation/RecastDemo/Source/Filelist.cpp
	# lib/recastnavigation/RecastDemo/Source/ConvexVolumeTool.cpp
	# lib/recastnavigation/RecastDemo/Source/imgui.cpp
	# lib/recastnavigation/RecastDemo/Source/Sample_Debug.cpp
	# lib/recastnavigation/RecastDemo/Source/SlideShow.cpp
	# lib/recastnavigation/RecastDemo/Source/NavMeshPruneTool.cpp
	# lib/recastnavigation/RecastDemo/Source/PerfTimer.cpp
	# lib/recastnavigation/RecastDemo/Source/InputGeom.cpp
	# lib/recastnavigation/RecastDemo/Source/NavMeshTesterTool.cpp
	# lib/recastnavigation/RecastDemo/Source/MeshLoaderObj.cpp
	# lib/recastnavigation/RecastDemo/Source/ChunkyTriMesh.cpp
	# lib/recastnavigation/RecastDemo/Source/Sample_SoloMesh.cpp
	# lib/recastnavigation/RecastDemo/Source/Sample_TileMesh.cpp
	# lib/recastnavigation/RecastDemo/Source/SampleInterfaces.cpp
	# lib/recastnavigation/RecastDemo/Source/OffMeshConnectionTool.cpp
	# lib/recastnavigation/RecastDemo/Source/TestCase.cpp
	# lib/recastnavigation/RecastDemo/Source/ValueHistory.cpp
	# lib/recastnavigation/RecastDemo/Contrib/fastlz/fastlz.c

endif()


