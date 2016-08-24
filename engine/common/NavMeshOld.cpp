#include <vector>

#include "Logger.hpp"
#include "SimpleNavMesh.hpp"
#include "Recast.h"
#include "RecastDebugDraw.h"
#include "RecastDump.h"
#include "InputGeom.h"
#include "DetourNode.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourStatus.h"
#include "NavMeshTesterTool.h"
#include "NavMeshPruneTool.h"
#include "OffMeshConnectionTool.h"
#include "ConvexVolumeTool.h"
#include "CrowdTool.h"
#include "MathFuncs.hpp"

#include <btBulletDynamicsCommon.h>

noob::simple_navmesh::~simple_navmesh() noexcept(true)
{
	cleanup();
}

void noob::simple_navmesh::cleanup() noexcept(true)
{
	delete [] tri_areas;
	tri_areas = 0;
	rcFreeHeightField(voxels);
	voxels = 0;
	rcFreeCompactHeightfield(compact_heightfield);
	compact_heightfield = 0;
	rcFreeContourSet(contour_set);
	contour_set = 0;
	rcFreePolyMesh(poly_mesh);
	poly_mesh = 0;
	rcFreePolyMeshDetail(detail_mesh);
	detail_mesh = 0;
	dtFreeNavMesh(nav_mesh);
	nav_mesh = 0;
}


void noob::simple_navmesh::add_mesh(const noob::basic_mesh& arg) noexcept(true)
{

}


bool noob::simple_navmesh::build(const noob::navmesh_config& arg) noexcept(true)
{
	if (!geom || !geom->getMesh())
	{
		logger::log("[SimpleNavMesh] Input mesh is not specified.");
		return false;
	}

	cleanup();

	const float* bmin = geom->getMeshBoundsMin();
	const float* bmax = geom->getMeshBoundsMax();
	const float* verts = geom->getMesh()->getVerts();
	num_verts = geom->getMesh()->getVertCount();
	const int*  tris = geom->getMesh()->getTris();
	const int ntris = geom->getMesh()->getTriCount();

	keep_intermediates = arg.keep_intermediates;
	cell_size = arg.cell_size;
	cell_height = arg.cell_height;
	agent_height = arg.agent_height;
	agent_radius = arg.agent_radius;
	agent_max_climb = arg.agent_max_climb;
	agent_max_slope = arg.agent_max_slope;
	region_min_size = arg.region_min_size;
	region_merge_size = arg.region_merge_size;
	edge_max_length = arg.edge_max_length;
	edge_max_error = arg.edge_max_error;
	verts_per_poly = arg.verts_per_poly;
	// Detail samples based on distance (TODO: Readup)
	detail_sample_dist = arg.detail_sample_dist;
	detail_sample_max_error = arg.detail_sample_max_error;
	partition_type = arg.partition_type;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 1. Initialize build config.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Init build configuration from GUI
	memset(&config, 0, sizeof(config));

	//noob::simple_navmesh::config config;
	config.cs = cell_size;
	config.ch = cell_height;
	config.walkableSlopeAngle = agent_max_slope;
	config.walkableHeight = static_cast<int>(ceilf(agent_height / config.ch));
	config.walkableClimb = static_cast<int>(floorf(agent_max_climb / config.ch));
	config.walkableRadius = static_cast<int>(ceilf(agent_radius / config.cs));
	config.maxEdgeLen = static_cast<int>((edge_max_length / cell_size));
	config.maxSimplificationError = edge_max_error;
	config.minRegionArea = static_cast<int>(rcSqr(region_min_size));		// Note: area = size*size
	config.mergeRegionArea = static_cast<int>(rcSqr(region_merge_size));	// Note: area = size*size
	config.maxVertsPerPoly = static_cast<int>(verts_per_poly);
	config.detailSampleDist = detail_sample_dist < 0.9f ? 0 : cell_size * detail_sample_dist;
	config.detailSampleMaxError = cell_height * detail_sample_max_error;

	// Calculate the area where the navigation will be built.
	// Here, the bounds of the input mesh are used but the area could be specified by a user-defined box, etc.
	rcVcopy(config.bmin, bmin);
	rcVcopy(config.bmax, bmax);
	rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

	// Our blank BuildContext
	rcContext build_ctx(false);

	// TODO: Insert timing code

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 2. Rasterize input polygon soup.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	fmt::MemoryWriter prebuild_msg;
	prebuild_msg << "About to build navmesh - " << config.width << " x " << config.height << " = " << config.width * config.height << " cells, " << static_cast<float>(num_verts)/1000.0f << "k verts and " << static_cast<float>(ntris)/1000.0f << "k tris.";
	logger::log(prebuild_msg.str());

	// Allocate voxel heightfield where we rasterize our input data to.
	voxels = rcAllocHeightfield();
	if (!voxels)
	{
		logger::log("[SimpleNavMesh] Could not allocate enough memory for voxels.");
		return false;
	}
	if (!rcCreateHeightfield(&build_ctx, *voxels, config.width, config.height, config.bmin, config.bmax, config.cs, config.ch))
	{
		logger::log("[SimpleNavMesh] Could not create voxels.");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	// TODO: Replace with vector
	tri_areas = new unsigned char[ntris];
	if (!tri_areas)
	{
		logger::log("[SimpleNavMesh] Could not allocate memory for triangles");
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	// TODO: Replace with fill_n
	memset(tri_areas, 0, ntris*sizeof(unsigned char));
	rcMarkWalkableTriangles(&build_ctx, config.walkableSlopeAngle, verts, num_verts, tris, ntris, tri_areas);
	rcRasterizeTriangles(&build_ctx, verts, num_verts, tris, tri_areas, ntris, *voxels, config.walkableClimb);

	if (!keep_intermediates)
	{
		delete [] tri_areas;
		tri_areas = 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 3. Filter walkables surfaces.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(&build_ctx, config.walkableClimb, *voxels);
	rcFilterLedgeSpans(&build_ctx, config.walkableHeight, config.walkableClimb, *voxels);
	rcFilterWalkableLowHeightSpans(&build_ctx, config.walkableHeight, *voxels);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 4. Partition walkable surface to simple regions.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	compact_heightfield = rcAllocCompactHeightfield();
	if (!compact_heightfield)
	{
		logger::log("[SimpleNavMesh] Out of memory for compact heightfield.");
		return false;
	}
	if (!rcBuildCompactHeightfield(&build_ctx, config.walkableHeight, config.walkableClimb, *voxels, *compact_heightfield))
	{
		logger::log("[SimpleNavMesh] Could not build compact heightfield data.");
		return false;
	}

	if (!keep_intermediates)
	{
		rcFreeHeightField(voxels);
		voxels = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(&build_ctx, config.walkableRadius, *compact_heightfield))
	{
		logger::log("[SimpleNavMesh] Could not erode.");
		return false;
	}

	// (Optional) Mark areas.
	const ConvexVolume* vols = geom->getConvexVolumes();
	for (int i  = 0; i < geom->getConvexVolumeCount(); ++i)
	{
		rcMarkConvexPolyArea(&build_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, static_cast<unsigned char>(vols[i].area), *compact_heightfield);
	}


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

	if (partition_type == PARTITION_WATERSHED)
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(&build_ctx, *compact_heightfield))
		{
			logger::log("[SimpleNavMesh] Could not build distance field.");
			return false;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(&build_ctx, *compact_heightfield, 0, config.minRegionArea, config.mergeRegionArea))
		{
			logger::log("[SimpleNavMesh] Could not build watershed regions.");
			return false;
		}
	}
	else if (partition_type == PARTITION_MONOTONE)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(&build_ctx, *compact_heightfield, 0, config.minRegionArea, config.mergeRegionArea))
		{
			logger::log("[SimpleNavMesh] Could not build monotone regions.");
			return false;
		}
	}
	else // PARTITION_LAYERS
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildLayerRegions(&build_ctx, *compact_heightfield, 0, config.minRegionArea))
		{
			logger::log("[SimpleNavMesh] Could not build layer regions.");
			return false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 5. Trace and simplify region contours.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Create contours.
	contour_set = rcAllocContourSet();
	if (!contour_set)
	{
		logger::log("[SimpleNavMesh] Out of memory for contour set.");
		return false;
	}
	if (!rcBuildContours(&build_ctx, *compact_heightfield, config.maxSimplificationError, config.maxEdgeLen, *contour_set))
	{
		logger::log("[SimpleNavMesh] Could not create contours.");
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 6. Build polygons mesh from contours.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Build polygon simple_navmesh from the contours.
	poly_mesh = rcAllocPolyMesh();
	if (!poly_mesh)
	{
		logger::log("[SimpleNavMesh] Out of memory for polymesh.");
		return false;
	}
	if (!rcBuildPolyMesh(&build_ctx, *contour_set, config.maxVertsPerPoly, *poly_mesh))
	{
		logger::log("[SimpleNavMesh] Could not build polymesh.");
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	detail_mesh = rcAllocPolyMeshDetail();
	if (!detail_mesh)
	{
		logger::log("[SimpleNavMesh] Out of memory for detail mesh..");
		return false;
	}

	if (!rcBuildPolyMeshDetail(&build_ctx, *poly_mesh, *compact_heightfield, config.detailSampleDist, config.detailSampleMaxError, *detail_mesh))
	{
		logger::log("[SimpleNavMesh] Could not build detail mesh.");
		return false;
	}

	if (!keep_intermediates)
	{
		rcFreeCompactHeightfield(compact_heightfield);
		compact_heightfield = 0;
		rcFreeContourSet(contour_set);
		contour_set = 0;
	}

	// At this point the navigation mesh data is ready, you can access it from poly_mesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//

	// The GUI may allow more max points per polygon than Detour can handle.
	// Only build the detour simple_navmesh if we do not exceed the limit.

	if (config.maxVertsPerPoly > DT_VERTS_PER_POLYGON)
	{
		return false;
	}
	unsigned char* nav_data = 0;
	int nav_data_size = 0;

	// Update poly flags from areas.
	for (int i = 0; i < poly_mesh->npolys; ++i)
	{
		if (poly_mesh->areas[i] == RC_WALKABLE_AREA)
		{
			poly_mesh->areas[i] = POLYAREA_GROUND;
		}

		if (poly_mesh->areas[i] == POLYAREA_GROUND || poly_mesh->areas[i] == POLYAREA_GRASS || poly_mesh->areas[i] == POLYAREA_ROAD)
		{
			poly_mesh->flags[i] = POLYFLAGS_WALK;
		}
		else if (poly_mesh->areas[i] == POLYAREA_WATER)
		{
			poly_mesh->flags[i] = POLYFLAGS_SWIM;
		}
		else if (poly_mesh->areas[i] == POLYAREA_DOOR)
		{
			poly_mesh->flags[i] = POLYFLAGS_WALK | POLYFLAGS_DOOR;
		}
	}

	dtNavMeshCreateParams params;
	// TODO: See if its either wise or necessary to do so.
	memset(&params, 0, sizeof(params));
	params.verts = poly_mesh->verts;
	params.vertCount = poly_mesh->nverts;
	params.polys = poly_mesh->polys;
	params.polyAreas = poly_mesh->areas;
	params.polyFlags = poly_mesh->flags;
	params.polyCount = poly_mesh->npolys;
	params.nvp = poly_mesh->nvp;
	params.detailMeshes = detail_mesh->meshes;
	params.detailVerts = detail_mesh->verts;
	params.detailVertsCount = detail_mesh->nverts;
	params.detailTris = detail_mesh->tris;
	params.detailTriCount = detail_mesh->ntris;
	params.offMeshConVerts = geom->getOffMeshConnectionVerts();
	params.offMeshConRad = geom->getOffMeshConnectionRads();
	params.offMeshConDir = geom->getOffMeshConnectionDirs();
	params.offMeshConAreas = geom->getOffMeshConnectionAreas();
	params.offMeshConFlags = geom->getOffMeshConnectionFlags();
	params.offMeshConUserID = geom->getOffMeshConnectionId();
	params.offMeshConCount = geom->getOffMeshConnectionCount();
	params.walkableHeight = agent_height;
	params.walkableRadius = agent_radius;
	params.walkableClimb = agent_max_climb;
	rcVcopy(params.bmin, poly_mesh->bmin);
	rcVcopy(params.bmax, poly_mesh->bmax);
	params.cs = config.cs;
	params.ch = config.ch;
	params.buildBvTree = true;

	if (!dtCreateNavMeshData(&params, &nav_data, &nav_data_size))
	{
		logger::log("[NavMesh] Could not build Detour navmesh data.");
		return false;
	}

	nav_mesh = dtAllocNavMesh();
	if (!nav_mesh)
	{
		dtFree(nav_data);
		logger::log("[NavMesh] Could not build Detour navmesh");
		return false;
	}

	dtStatus status;

	status = nav_mesh->init(nav_data, nav_data_size, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(nav_data);
		logger::log("[NavMesh] Could not init Detour navmesh");
		return false;
	}

	status = nav_query->init(nav_mesh, 2048);
	if (dtStatusFailed(status))
	{
		logger::log("[NavMesh] Could not init Detour query");
		return false;
	}

	fmt::MemoryWriter post_build_msg;
	post_build_msg << " Creation success! " << poly_mesh->nverts << ", verts and " << poly_mesh->npolys << " polys.";
	logger::log(post_build_msg.str());
	
	return true;
}
