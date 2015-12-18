/*#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "imgui.h"
#include "InputGeom.h"
#include "Sample.h"
*/

#include <vector>
#include "Logger.hpp"
#include "SimpleNavMesh.hpp"
#include "Recast.h"
//#include "Sample.h"
#include "RecastDebugDraw.h"
#include "RecastDump.h"
#include "InputGeom.h"
#include "DetourNode.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourStatus.h"
#include "DetourDebugDraw.h"
#include "NavMeshTesterTool.h"
#include "NavMeshPruneTool.h"
#include "OffMeshConnectionTool.h"
#include "ConvexVolumeTool.h"
#include "CrowdTool.h"
#include "MathFuncs.hpp"

// #include <btBulletDynamicsCommon.h>

noob::simple_navmesh::simple_navmesh() :
	m_keepInterResults(true),
	m_totalBuildTimeMs(0),
	m_triareas(0),
	m_solid(0),
	m_chf(0),
	m_cset(0),
	m_pmesh(0),
	m_dmesh(0),
	m_DrawMode(DRAWMODE_NAVMESH)
{
	//	setTool(new NavMeshTesterTool);
}

noob::simple_navmesh::~simple_navmesh()
{
	cleanup();
}

void noob::simple_navmesh::cleanup()
{
	delete [] m_triareas;
	m_triareas = 0;
	rcFreeHeightField(m_solid);
	m_solid = 0;
	rcFreeCompactHeightfield(m_chf);
	m_chf = 0;
	rcFreeContourSet(m_cset);
	m_cset = 0;
	rcFreePolyMesh(m_pmesh);
	m_pmesh = 0;
	rcFreePolyMeshDetail(m_dmesh);
	m_dmesh = 0;
	dtFreeNavMesh(m_navMesh);
	m_navMesh = 0;
}

void noob::simple_navmesh::handleSettings()
{
	/*
	   Sample::handleCommonSettings();

	   if (imguiCheck("Keep Itermediate Results", m_keepInterResults))
	   m_keepInterResults = !m_keepInterResults;

	   imguiSeparator();

	   char msg[64];
	   snprintf(msg, 64, "Build Time: %.1fms", m_totalBuildTimeMs);
	   imguiLabel(msg);

	   imguiSeparator();
	   */
}

void noob::simple_navmesh::handleTools()
{
	/*
	   int type = !m_tool ? TOOL_NONE : m_tool->type();

	   if (imguiCheck("Test Navmesh", type == TOOL_NAVMESH_TESTER))
	   {
	   setTool(new NavMeshTesterTool);
	   }
	   if (imguiCheck("Prune Navmesh", type == TOOL_NAVMESH_PRUNE))
	   {
	   setTool(new NavMeshPruneTool);
	   }
	   if (imguiCheck("Create Off-Mesh Connections", type == TOOL_OFFMESH_CONNECTION))
	   {
	   setTool(new OffMeshConnectionTool);
	   }
	   if (imguiCheck("Create Convex Volumes", type == TOOL_CONVEX_VOLUME))
	   {
	   setTool(new ConvexVolumeTool);
	   }
	   if (imguiCheck("Create Crowds", type == TOOL_CROWD))
	   {
	   setTool(new CrowdTool);
	   }

	   imguiSeparatorLine();

	   imguiIndent();

	   if (m_tool)
	   m_tool->handleMenu();

	   imguiUnindent();
	   */
}

void noob::simple_navmesh::handle_debug_mode()
{
	// Check which modes are valid.
	/*	bool valid[MAX_DRAWMODE];
		for (int i = 0; i < MAX_DRAWMODE; ++i)
		valid[i] = false;

		if (m_geom)
		{
		valid[DRAWMODE_NAVMESH] = m_navMesh != 0;
		valid[DRAWMODE_NAVMESH_TRANS] = m_navMesh != 0;
		valid[DRAWMODE_NAVMESH_BVTREE] = m_navMesh != 0;
		valid[DRAWMODE_NAVMESH_NODES] = m_navQuery != 0;
		valid[DRAWMODE_NAVMESH_INVIS] = m_navMesh != 0;
		valid[DRAWMODE_MESH] = true;
		valid[DRAWMODE_VOXELS] = m_solid != 0;
		valid[DRAWMODE_VOXELS_WALKABLE] = m_solid != 0;
		valid[DRAWMODE_COMPACT] = m_chf != 0;
		valid[DRAWMODE_COMPACT_DISTANCE] = m_chf != 0;
		valid[DRAWMODE_COMPACT_REGIONS] = m_chf != 0;
		valid[DRAWMODE_REGION_CONNECTIONS] = m_cset != 0;
		valid[DRAWMODE_RAW_CONTOURS] = m_cset != 0;
		valid[DRAWMODE_BOTH_CONTOURS] = m_cset != 0;
		valid[DRAWMODE_CONTOURS] = m_cset != 0;
		valid[DRAWMODE_POLYMESH] = m_pmesh != 0;
		valid[DRAWMODE_POLYMESH_DETAIL] = m_dmesh != 0;
		}

		int unavail = 0;
		for (int i = 0; i < MAX_DRAWMODE; ++i)
		if (!valid[i]) unavail++;

		if (unavail == MAX_DRAWMODE)
		return;

		imguiLabel("Draw");
		if (imguiCheck("Input Mesh", draw_mode == DRAWMODE_MESH, valid[DRAWMODE_MESH]))
		draw_mode = DRAWMODE_MESH;
		if (imguiCheck("Navmesh", draw_mode == DRAWMODE_NAVMESH, valid[DRAWMODE_NAVMESH]))
		draw_mode = DRAWMODE_NAVMESH;
		if (imguiCheck("Navmesh Invis", draw_mode == DRAWMODE_NAVMESH_INVIS, valid[DRAWMODE_NAVMESH_INVIS]))
		draw_mode = DRAWMODE_NAVMESH_INVIS;
		if (imguiCheck("Navmesh Trans", draw_mode == DRAWMODE_NAVMESH_TRANS, valid[DRAWMODE_NAVMESH_TRANS]))
		draw_mode = DRAWMODE_NAVMESH_TRANS;
		if (imguiCheck("Navmesh BVTree", draw_mode == DRAWMODE_NAVMESH_BVTREE, valid[DRAWMODE_NAVMESH_BVTREE]))
		draw_mode = DRAWMODE_NAVMESH_BVTREE;
		if (imguiCheck("Navmesh Nodes", draw_mode == DRAWMODE_NAVMESH_NODES, valid[DRAWMODE_NAVMESH_NODES]))
		draw_mode = DRAWMODE_NAVMESH_NODES;
		if (imguiCheck("Voxels", draw_mode == DRAWMODE_VOXELS, valid[DRAWMODE_VOXELS]))
		draw_mode = DRAWMODE_VOXELS;
		if (imguiCheck("Walkable Voxels", draw_mode == DRAWMODE_VOXELS_WALKABLE, valid[DRAWMODE_VOXELS_WALKABLE]))
		draw_mode = DRAWMODE_VOXELS_WALKABLE;
		if (imguiCheck("Compact", draw_mode == DRAWMODE_COMPACT, valid[DRAWMODE_COMPACT]))
		draw_mode = DRAWMODE_COMPACT;
		if (imguiCheck("Compact Distance", draw_mode == DRAWMODE_COMPACT_DISTANCE, valid[DRAWMODE_COMPACT_DISTANCE]))
		draw_mode = DRAWMODE_COMPACT_DISTANCE;
		if (imguiCheck("Compact Regions", draw_mode == DRAWMODE_COMPACT_REGIONS, valid[DRAWMODE_COMPACT_REGIONS]))
		draw_mode = DRAWMODE_COMPACT_REGIONS;
		if (imguiCheck("Region Connections", draw_mode == DRAWMODE_REGION_CONNECTIONS, valid[DRAWMODE_REGION_CONNECTIONS]))
		draw_mode = DRAWMODE_REGION_CONNECTIONS;
		if (imguiCheck("Raw Contours", draw_mode == DRAWMODE_RAW_CONTOURS, valid[DRAWMODE_RAW_CONTOURS]))
		draw_mode = DRAWMODE_RAW_CONTOURS;
		if (imguiCheck("Both Contours", draw_mode == DRAWMODE_BOTH_CONTOURS, valid[DRAWMODE_BOTH_CONTOURS]))
		draw_mode = DRAWMODE_BOTH_CONTOURS;
		if (imguiCheck("Contours", draw_mode == DRAWMODE_CONTOURS, valid[DRAWMODE_CONTOURS]))
		draw_mode = DRAWMODE_CONTOURS;
		if (imguiCheck("Poly Mesh", draw_mode == DRAWMODE_POLYMESH, valid[DRAWMODE_POLYMESH]))
		draw_mode = DRAWMODE_POLYMESH;
		if (imguiCheck("Poly Mesh Detail", draw_mode == DRAWMODE_POLYMESH_DETAIL, valid[DRAWMODE_POLYMESH_DETAIL]))
		draw_mode = DRAWMODE_POLYMESH_DETAIL;

		if (unavail)
		{
		imguiValue("Tick 'Keep Itermediate Results'");
	imguiValue("to see more debug mode options.");
}
*/
}

void noob::simple_navmesh::handleRender()
{
	/*	if (!m_geom || !m_geom->getMesh())
		return;

	//	DebugDrawGL dd;

	//	glEnable(GL_FOG);
	//	glDepthMask(GL_TRUE);

	const float texScale = 1.0f / (m_cellSize * 10.0f);

	// if (draw_mode != DRAWMODE_NAVMESH_TRANS)
	{
	// Draw mesh
	//	duDebugDrawTriMeshSlope(&dd, m_geom->getMesh()->getVerts(), m_geom->getMesh()->getVertCount(), m_geom->getMesh()->getTris(), m_geom->getMesh()->getNormals(), m_geom->getMesh()->getTriCount(),	m_agentMaxSlope, texScale);
	//	m_geom->drawOffMeshConnections(&dd);
	}

	//	glDisable(GL_FOG);
	//	glDepthMask(GL_FALSE);

	// Draw bounds
	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	//	duDebugDrawBoxWire(&dd, bmin[0],bmin[1],bmin[2], bmax[0],bmax[1],bmax[2], duRGBA(255,255,255,128), 1.0f);
	//	dd.begin(DU_DRAW_POINTS, 5.0f);
	//	dd.vertex(bmin[0],bmin[1],bmin[2],duRGBA(255,255,255,128));
	//	dd.end();

	//	if (m_navMesh && m_navQuery && (draw_mode == DRAWMODE_NAVMESH || draw_mode == DRAWMODE_NAVMESH_TRANS || draw_mode == DRAWMODE_NAVMESH_BVTREE ||  draw_mode == DRAWMODE_NAVMESH_NODES || draw_mode == DRAWMODE_NAVMESH_INVIS))
	{
	//	if (draw_mode != DRAWMODE_NAVMESH_INVIS)
	//	duDebugDrawNavMeshWithClosedList(&dd, *m_navMesh, *m_navQuery, m_navMeshDrawFlags);
	//	if (draw_mode == DRAWMODE_NAVMESH_BVTREE)
	//	duDebugDrawNavMeshBVTree(&dd, *m_navMesh);
	//	if (draw_mode == DRAWMODE_NAVMESH_NODES)
	//	duDebugDrawNavMeshNodes(&dd, *m_navQuery);
	//	duDebugDrawNavMeshPolysWithFlags(&dd, *m_navMesh, SAMPLE_POLYFLAGS_DISABLED, duRGBA(0,0,0,128));
	}

	//	glDepthMask(GL_TRUE);

	if (m_chf && draw_mode == DRAWMODE_COMPACT)
	//	duDebugDrawCompactHeightfieldSolid(&dd, *m_chf);

	if (m_chf && draw_mode == DRAWMODE_COMPACT_DISTANCE)
	//	duDebugDrawCompactHeightfieldDistance(&dd, *m_chf);
	if (m_chf && draw_mode == DRAWMODE_COMPACT_REGIONS)
	//	duDebugDrawCompactHeightfieldRegions(&dd, *m_chf);
	if (m_solid && draw_mode == DRAWMODE_VOXELS)
	{
	//	glEnable(GL_FOG);
	//	duDebugDrawHeightfieldSolid(&dd, *m_solid);
	//	glDisable(GL_FOG);
	}
	if (m_solid && draw_mode == DRAWMODE_VOXELS_WALKABLE)
	{
	//	glEnable(GL_FOG);
	//	duDebugDrawHeightfieldWalkable(&dd, *m_solid);
	//	glDisable(GL_FOG);
	}
	if (m_cset && draw_mode == DRAWMODE_RAW_CONTOURS)
	{
	//	glDepthMask(GL_FALSE);
	//	duDebugDrawRawContours(&dd, *m_cset);
	//	glDepthMask(GL_TRUE);
	}
	if (m_cset && draw_mode == DRAWMODE_BOTH_CONTOURS)
	{
	//	glDepthMask(GL_FALSE);
	//	duDebugDrawRawContours(&dd, *m_cset, 0.5f);
	//	duDebugDrawContours(&dd, *m_cset);
	//	glDepthMask(GL_TRUE);
}
if (m_cset && draw_mode == DRAWMODE_CONTOURS)
{
	//	glDepthMask(GL_FALSE);
	//	duDebugDrawContours(&dd, *m_cset);
	//	glDepthMask(GL_TRUE);
}
if (m_chf && m_cset && draw_mode == DRAWMODE_REGION_CONNECTIONS)
{
	//	duDebugDrawCompactHeightfieldRegions(&dd, *m_chf);

	//	glDepthMask(GL_FALSE);
	//	duDebugDrawRegionConnections(&dd, *m_cset);
	//	glDepthMask(GL_TRUE);
}
if (m_pmesh && draw_mode == DRAWMODE_POLYMESH)
{
	//	glDepthMask(GL_FALSE);
	//	duDebugDrawPolyMesh(&dd, *m_pmesh);
	//	glDepthMask(GL_TRUE);
}
if (m_dmesh && draw_mode == DRAWMODE_POLYMESH_DETAIL)
{
	//	glDepthMask(GL_FALSE);
	//	duDebugDrawPolyMeshDetail(&dd, *m_dmesh);
	//	glDepthMask(GL_TRUE);
}

//	m_geom->drawConvexVolumes(&dd);

//	if (m_tool)
//		m_tool->handleRender();
//	renderToolStates();

//	glDepthMask(GL_TRUE);
}

void noob::simple_navmesh::handleRenderOverlay(double* proj, double* model, int* view)
{
	//	if (m_tool)
	//		m_tool->handleRenderOverlay(proj, model, view);
	//	renderOverlayToolStates(proj, model, view);
}

void noob::simple_navmesh::handleMeshChanged(class InputGeom* geom)
{
	//	Sample::handleMeshChanged(geom);

	dtFreeNavMesh(m_navMesh);
	m_navMesh = 0;

	if (m_tool)
	{
		m_tool->reset();
		m_tool->init(this);
	}
	resetToolStates();
	initToolStates(this);
	*/
}


bool noob::simple_navmesh::handleBuild()
{
	if (!m_geom || !m_geom->getMesh())
	{
		logger::log("buildNavigation: Input mesh is not specified.");
		return false;
	}

	cleanup();

	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	const float* verts = m_geom->getMesh()->getVerts();
	nverts = m_geom->getMesh()->getVertCount();
	const int*  tris = m_geom->getMesh()->getTris();
	const int ntris = m_geom->getMesh()->getTriCount();

	//
	// Step 1. Initialize build config.
	//

	// Init build configuration from GUI
	memset(&config, 0, sizeof(config));

	//noob::simple_navmesh::config config;
	config.cs = m_cellSize;
	config.ch = m_cellHeight;
	config.walkableSlopeAngle = m_agentMaxSlope;
	config.walkableHeight = (int)ceilf(m_agentHeight / config.ch);
	config.walkableClimb = (int)floorf(m_agentMaxClimb / config.ch);
	config.walkableRadius = (int)ceilf(m_agentRadius / config.cs);
	config.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
	config.maxSimplificationError = m_edgeMaxError;
	config.minRegionArea = (int)rcSqr(m_regionMinSize);		// Note: area = size*size
	config.mergeRegionArea = (int)rcSqr(m_regionMergeSize);	// Note: area = size*size
	config.maxVertsPerPoly = (int)m_vertsPerPoly;
	config.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
	config.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcVcopy(config.bmin, bmin);
	rcVcopy(config.bmax, bmax);
	rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

	// Reset build times gathering.
	m_ctx->resetTimers();

	// Start the build process.	
	m_ctx->startTimer(RC_TIMER_TOTAL);

	logger::log(fmt::format("Building navigation: - {0} x {1} cells - {2}K verts, {3}K tris", config.width, config.height, nverts/1000.0f, ntris/1000.0f));

	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.
	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		logger::log("buildNavigation: Out of memory 'solid'.");
		return false;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, config.width, config.height, config.bmin, config.bmax, config.cs, config.ch))
	{
		logger::log("buildNavigation: Could not create solid heightfield.");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[ntris];
	if (!m_triareas)
	{
		logger::log(fmt::format("buildNavigation: Out of memory 'm_triareas' ({0}).", ntris));
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(m_triareas, 0, ntris*sizeof(unsigned char));
	rcMarkWalkableTriangles(m_ctx, config.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
	rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, config.walkableClimb);

	if (!m_keepInterResults)
	{
		delete [] m_triareas;
		m_triareas = 0;
	}

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(m_ctx, config.walkableClimb, *m_solid);
	rcFilterLedgeSpans(m_ctx, config.walkableHeight, config.walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(m_ctx, config.walkableHeight, *m_solid);


	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		logger::log("buildNavigation: Out of memory 'chf'.");
		return false;
	}
	if (!rcBuildCompactHeightfield(m_ctx, config.walkableHeight, config.walkableClimb, *m_solid, *m_chf))
	{
		logger::log("buildNavigation: Could not build compact data.");
		return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, config.walkableRadius, *m_chf))
	{
		logger::log("buildNavigation: Could not erode.");
		return false;
	}

	// (Optional) Mark areas.
	const ConvexVolume* vols = m_geom->getConvexVolumes();
	for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
		rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);


	// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
	// There are 3 martitioning methods, each with some pros and cons:
	// 1) Watershed partitioning
	//   - the classic Recast partitioning
	//   - creates the nicest tessellation
	//   - usually slowest
	//   - partitions the heightfield into nice regions without holes or overlaps
	//   - the are some corner cases where this method creates produces holes and overlaps
	//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
	//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
	//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
	// 2) Monotone partioning
	//   - fastest
	//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
	//   - creates long thin polygons, which sometimes causes paths with detours
	//   * use this if you want fast simple_navmesh generation
	// 3) Layer partitoining
	//   - quite fast
	//   - partitions the heighfield into non-overlapping regions
	//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
	//   - produces better triangles than monotone partitioning
	//   - does not have the corner cases of watershed partitioning
	//   - can be slow and create a bit ugly tessellation (still better than monotone)
	//     if you have large open areas with small obstacles (not a problem if you use tiles)
	//   * good choice to use for tiled simple_navmesh with medium and small sized tiles

	if (m_partitionType == SAMPLE_PARTITION_WATERSHED)
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(m_ctx, *m_chf))
		{
			logger::log("buildNavigation: Could not build distance field.");
			return false;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(m_ctx, *m_chf, 0, config.minRegionArea, config.mergeRegionArea))
		{
			logger::log("buildNavigation: Could not build watershed regions.");
			return false;
		}
	}
	else if (m_partitionType == SAMPLE_PARTITION_MONOTONE)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, config.minRegionArea, config.mergeRegionArea))
		{
			logger::log("buildNavigation: Could not build monotone regions.");
			return false;
		}
	}
	else // SAMPLE_PARTITION_LAYERS
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildLayerRegions(m_ctx, *m_chf, 0, config.minRegionArea))
		{
			logger::log("buildNavigation: Could not build layer regions.");
			return false;
		}
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		logger::log("buildNavigation: Out of memory 'cset'.");
		return false;
	}
	if (!rcBuildContours(m_ctx, *m_chf, config.maxSimplificationError, config.maxEdgeLen, *m_cset))
	{
		logger::log("buildNavigation: Could not create contours.");
		return false;
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon simple_navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		logger::log("buildNavigation: Out of memory 'pmesh'.");
		return false;
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, config.maxVertsPerPoly, *m_pmesh))
	{
		logger::log("buildNavigation: Could not triangulate contours.");
		return false;
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		logger::log("buildNavigation: Out of memory 'pmdtl'.");
		return false;
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, config.detailSampleDist, config.detailSampleMaxError, *m_dmesh))
	{
		logger::log("buildNavigation: Could not build detail mesh.");
		return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}

	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//

	// The GUI may allow more max points per polygon than Detour can handle.
	// Only build the detour simple_navmesh if we do not exceed the limit.
	/*
	   if (config.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	   {
	   */
	unsigned char* navData = 0;
	int navDataSize = 0;

	// Update poly flags from areas.
	for (int i = 0; i < m_pmesh->npolys; ++i)
	{
		if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
			m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

		if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
		{
			m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
		}
		else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
		{
			m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
		}
		else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
		{
			m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
		}
	}

	// 
	// 
	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof(params));
	params.verts = m_pmesh->verts;
	params.vertCount = m_pmesh->nverts;
	params.polys = m_pmesh->polys;
	params.polyAreas = m_pmesh->areas;
	params.polyFlags = m_pmesh->flags;
	params.polyCount = m_pmesh->npolys;
	params.nvp = m_pmesh->nvp;
	params.detailMeshes = m_dmesh->meshes;
	params.detailVerts = m_dmesh->verts;
	params.detailVertsCount = m_dmesh->nverts;
	params.detailTris = m_dmesh->tris;
	params.detailTriCount = m_dmesh->ntris;
	params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
	params.offMeshConRad = m_geom->getOffMeshConnectionRads();
	params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
	params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
	params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
	params.offMeshConUserID = m_geom->getOffMeshConnectionId();
	params.offMeshConCount = m_geom->getOffMeshConnectionCount();
	params.walkableHeight = m_agentHeight;
	params.walkableRadius = m_agentRadius;
	params.walkableClimb = m_agentMaxClimb;
	rcVcopy(params.bmin, m_pmesh->bmin);
	rcVcopy(params.bmax, m_pmesh->bmax);
	params.cs = config.cs;
	params.ch = config.ch;
	params.buildBvTree = true;

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
	{
		logger::log("Could not build Detour simple_navmesh.");
		return false;
	}

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		dtFree(navData);
		logger::log("Could not create Detour simple_navmesh");
		return false;
	}

	dtStatus status;

	status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navData);
		logger::log("Could not init Detour simple_navmesh");
		return false;
	}

	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{
		logger::log("Could not init Detour simple_navmesh query");
		return false;
	}


m_ctx->stopTimer(RC_TIMER_TOTAL);

// Show performance stats.
//duLogBuildTimes(*m_ctx, m_ctx->getAccumulatedTime(RC_TIMER_TOTAL));
logger::log(fmt::format("Polymesh: {0} vertices  {1} polygons", m_pmesh->nverts, m_pmesh->npolys));

//	m_totalBuildTimeMs = m_ctx->getAccumulatedTime(RC_TIMER_TOTAL)/1000.0f;

//if (m_tool)
//	m_tool->init(this);
//initToolStates(this);

//	return true;
}
