#include "NavMesh.hpp"

#include "Globals.hpp"


noob::navigation::~navigation() noexcept(true)
{
	clear_temporaries();

	dtFreeNavMesh(navmesh);
	navmesh = nullptr;
}

void noob::navigation::set_config(const noob::navigation::config& arg) noexcept(true)
{
	cfg = {0};

	// NOT SET HERE:
	// int width, height, tileSize, borderSize;
	// float[3] bmin, bmax;

	cfg.cs = arg.cell_size;
	cfg.ch = arg.cell_height;
	cfg.walkableSlopeAngle = arg.max_agent_slope;
	cfg.walkableHeight = static_cast<int>(std::ceil(arg.agent_height / cfg.ch));
	cfg.walkableClimb = static_cast<int>(std::floor(arg.agent_climb / cfg.ch));
	cfg.walkableRadius = static_cast<int>(std::ceil(arg.agent_radius / cfg.cs));
	cfg.maxEdgeLen = static_cast<int>(arg.max_edge_length / cfg.cs);
	cfg.maxSimplificationError = arg.max_simplification_error;
	cfg.minRegionArea = static_cast<int>(arg.region_min_size * arg.region_min_size);
	cfg.mergeRegionArea = static_cast<int>(arg.region_merge_size * arg.region_merge_size);
	cfg.maxVertsPerPoly = arg.verts_per_poly;
	cfg.detailSampleDist = arg.detail_sample_dist > 0.9f ? 0.0 : cfg.cs * arg.detail_sample_dist;
	cfg.detailSampleMaxError = arg.detail_sample_max_error * cfg.ch;
}


void noob::navigation::add_geom(const noob::basic_mesh& mesh) noexcept(true)
{
	
	logger::log("[Navigation] Adding geometry");
	for (noob::vec3 v : mesh.vertices)
	{
		input_verts.push_back(v[0]);
		input_verts.push_back(v[1]);
		input_verts.push_back(v[2]);
	}

	for (uint32_t i : mesh.indices)
	{
		input_indices.push_back(i);
	}

	assert(mesh.indices.size() % 3 == 0);

	uint32_t num_tris = mesh.indices.size() % 3;
	for (uint32_t i  = 0; i < num_tris; ++i)
	{
		input_tri_flags.push_back(static_cast<uint8_t>(noob::navigation::poly_type::GROUND));
	}

	input_bbox = update_bbox(input_bbox, mesh.bbox);
}


void noob::navigation::clear_temporaries() noexcept(true)
{
	rcFreeHeightField(heightfield);
	heightfield = nullptr;
	rcFreeCompactHeightfield(compact_heightfield);
	compact_heightfield = nullptr;
	rcFreeContourSet(contours);
	contours = nullptr;
	rcFreePolyMesh(polymesh);
	polymesh = nullptr;
	rcFreePolyMeshDetail(polymesh_detail);
	polymesh_detail = nullptr;
}


void noob::navigation::clear_geom() noexcept(true)
{
	input_verts.clear();
	input_indices.clear();
	input_tri_flags.clear();
	input_bbox.reset();

	offmesh_connections_verts = {};
	offmesh_connections_rads = {};
	offmesh_connections_dirs = {};
	offmesh_connections_areas = {};
	offmesh_connections_flags = {};
	offmesh_connections_ids = {};
	offmesh_connections_count = 0;

	convexes = {};
	convexes_count = 0;
}



void noob::navigation::add_offmesh_link(const noob::vec3& start_pos, const noob::vec3& end_pos, float radius, bool bidir, noob::navigation::poly_type area, noob::navigation::poly_flag flag) noexcept(true)
{
	if (offmesh_connections_count >= max_offmesh_connections) return;

	offmesh_connections_rads[offmesh_connections_count] = radius;
	offmesh_connections_dirs[offmesh_connections_count] = static_cast<uint8_t>(bidir);
	offmesh_connections_areas[offmesh_connections_count] = static_cast<uint8_t>(area);
	offmesh_connections_flags[offmesh_connections_count] = static_cast<uint16_t>(flag);
	offmesh_connections_ids[offmesh_connections_count] = 1000 + offmesh_connections_count;

	uint32_t index = offmesh_connections_count * 6;

	for (uint32_t i = 0; i < 3; ++i)
	{
		offmesh_connections_verts[index + i] = start_pos[i];
	}

	index += 3;

	for (uint32_t i = 0; i < 3; ++i)
	{
		offmesh_connections_verts[index + i] = end_pos[i];
	}

	++offmesh_connections_count;
}


bool noob::navigation::add_obstacle(const noob::vec3& scales, const noob::vec3& pos, const noob::versor& orient, float height_min, float height_max, noob::navigation::poly_type area) noexcept(true)
{
	if (convexes_count < max_convexes)
	{
		++convexes_count;
		noob::navigation::convex_volume* vol = &convexes[convexes_count];

		*vol = {};

		noob::basic_mesh m = noob::mesh_utils::box(scales[0], scales[1], scales[2]);
		uint32_t num_verts = m.vertices.size();
		for (uint32_t i = 0; i < num_verts; ++i)
		{
			noob::mat4 mat = noob::identity_mat4();
			mat = noob::rotate(mat, orient);
			mat = noob::translate(mat, pos);

			noob::vec4 temp_v = mat * noob::vec4(m.vertices[i], 1.0);
			vol->verts[i * 3] = temp_v[0];
			vol->verts[(i * 3) + 1] = temp_v[1];
			vol->verts[(i * 3) + 2] = temp_v[2];
		}

		vol->hmin = height_min;
		vol->hmax = height_max;
		vol->area = static_cast<uint8_t>(area);

		return true;	
	}

	return false;
}


bool noob::navigation::build() noexcept(true)
{
	rcVcopy(cfg.bmin, &input_bbox.min.v[0]);
	rcVcopy(cfg.bmax, &input_bbox.max.v[0]);
	rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

	// TODO: Insert timing code

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Rasterize input polygon soup.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	uint32_t num_verts = input_verts.size() / 3;

	assert(input_verts.size() % 3 == 0);

	uint32_t num_tris = input_indices.size() / 3;

	assert(input_indices.size() % 3 == 0);

	input_tri_flags.resize(num_tris);

	fmt::MemoryWriter prebuild_msg;
	prebuild_msg << "[Navigation] About to build navmesh - " << cfg.width << " x " << cfg.height << " = " << cfg.width * cfg.height << " cells, " << static_cast<float>(num_verts)/1000.0f << "k verts and " << static_cast<float>(num_tris)/1000.0f << "k tris.";
	logger::log(prebuild_msg.str());

	// Our blank BuildContext
	rcContext build_ctx(false);

	heightfield = rcAllocHeightfield();

	if (!heightfield)
	{
		logger::log("[Navigation] Could not allocate enough memory for heightfield.");
		return false;
	}
	if (!rcCreateHeightfield(&build_ctx, *heightfield, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch))
	{
		logger::log("[Navigation] Could not create heightfield.");
		return false;
	}


	rcMarkWalkableTriangles(&build_ctx, cfg.walkableSlopeAngle, &input_verts[0], num_verts, reinterpret_cast<const int*>(&input_indices[0]), num_tris, &input_tri_flags[0]);
	if (!rcRasterizeTriangles(&build_ctx, &input_verts[0], num_verts, reinterpret_cast<const int*>(&input_indices[0]), &input_tri_flags[0], num_tris, *heightfield, cfg.walkableClimb))
	{
		logger::log("[Navigation] Could not rasterize triangles.");
		return false;
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 3. Filter walkables surfaces.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(&build_ctx, cfg.walkableClimb, *heightfield);
	rcFilterLedgeSpans(&build_ctx, cfg.walkableHeight, cfg.walkableClimb, *heightfield);
	rcFilterWalkableLowHeightSpans(&build_ctx, cfg.walkableHeight, *heightfield);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 4. Partition walkable surface to simple regions.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	compact_heightfield = rcAllocCompactHeightfield();
	if (!compact_heightfield)
	{
		logger::log("[Navigation] Out of memory for compact heightfield.");
		return false;
	}
	if (!rcBuildCompactHeightfield(&build_ctx, cfg.walkableHeight, cfg.walkableClimb, *heightfield, *compact_heightfield))
	{
		logger::log("[Navigation] Could not build compact heightfield data.");
		return false;
	}

	if (!keep_temporaries)
	{
		rcFreeHeightField(heightfield);
		heightfield = nullptr;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(&build_ctx, cfg.walkableRadius, *compact_heightfield))
	{
		logger::log("[Navigation] Could not erode.");
		return false;
	}

	// (Optional) Mark areas.
	for (uint32_t i  = 0; i < convexes_count; ++i)
	{
		rcMarkConvexPolyArea(&build_ctx, &(convexes[i].verts[0]), convexes[i].nverts, convexes[i].hmin, convexes[i].hmax, static_cast<unsigned char>(convexes[i].area), *compact_heightfield);
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

	if (partitioning == partition_type::WATERSHED)
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(&build_ctx, *compact_heightfield))
		{
			logger::log("[Navigation] Could not build distance field.");
			return false;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(&build_ctx, *compact_heightfield, 0, cfg.minRegionArea, cfg.mergeRegionArea))
		{
			logger::log("[Navigation] Could not build watershed regions.");
			return false;
		}
	}
	else if (partitioning == partition_type::MONOTONE)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(&build_ctx, *compact_heightfield, 0, cfg.minRegionArea, cfg.mergeRegionArea))
		{
			logger::log("[Navigation] Could not build monotone regions.");
			return false;
		}
	}
	else // PARTITION_LAYERS
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildLayerRegions(&build_ctx, *compact_heightfield, 0, cfg.minRegionArea))
		{
			logger::log("[Navigation] Could not build layer regions.");
			return false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 5. Trace and simplify region contours.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Create contours.
	contours = rcAllocContourSet();
	if (!contours)
	{
		logger::log("[Navigation] Out of memory for contour set.");
		return false;
	}
	if (!rcBuildContours(&build_ctx, *compact_heightfield, cfg.maxSimplificationError, cfg.maxEdgeLen, *contours))
	{
		logger::log("[Navigation] Could not create contours.");
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 6. Build polygons mesh from contours.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Build polygon simple_navmesh from the contours.
	polymesh = rcAllocPolyMesh();
	if (!polymesh)
	{
		logger::log("[Navigation] Out of memory for polymesh.");
		return false;
	}
	if (!rcBuildPolyMesh(&build_ctx, *contours, cfg.maxVertsPerPoly, *polymesh))
	{
		logger::log("[Navigation] Could not build polymesh.");
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 7. Create detail mesh which records approximate height on each polygon.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	polymesh_detail = rcAllocPolyMeshDetail();
	if (!polymesh_detail)
	{
		logger::log("[Navigation] Out of memory for detail mesh..");
		return false;
	}

	if (!rcBuildPolyMeshDetail(&build_ctx, *polymesh, *compact_heightfield, cfg.detailSampleDist, cfg.detailSampleMaxError, *polymesh_detail))
	{
		logger::log("[Navigation] Could not build detail mesh.");
		return false;
	}

	if (!keep_temporaries)
	{
		rcFreeCompactHeightfield(compact_heightfield);
		compact_heightfield = nullptr;
		rcFreeContourSet(contours);
		contours = nullptr;
	}


	// COOL NOTE: At this point, the navigation mesh data should be ready.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access it.


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Only build the detour simple_navmesh if we do not exceed the max poly limit.
	if (cfg.maxVertsPerPoly > DT_VERTS_PER_POLYGON)
	{
		return false;
	}

	uint8_t* nav_data;
	int nav_data_size = 0;

	// Update poly flags from areas.
	for (uint32_t i = 0; i < polymesh->npolys; ++i)
	{
		if (polymesh->areas[i] == RC_WALKABLE_AREA)
		{
			polymesh->areas[i] = static_cast<uint8_t>(poly_type::GROUND);
		}

		if (polymesh->areas[i] == static_cast<uint8_t>(poly_type::GROUND) || polymesh->areas[i] == static_cast<uint8_t>(poly_type::GRASS) || polymesh->areas[i] == static_cast<uint8_t>(poly_type::ROAD))
		{
			polymesh->flags[i] = static_cast<uint16_t>(poly_flag::WALK);
		}
		else if (polymesh->areas[i] == static_cast<uint8_t>(poly_type::WATER))
		{
			polymesh->flags[i] = static_cast<uint16_t>(poly_flag::SWIM);
		}
		else if (polymesh->areas[i] == static_cast<uint8_t>(poly_type::DOOR))
		{
			polymesh->flags[i] = static_cast<uint16_t>(poly_flag::WALK) | static_cast<uint16_t>(poly_flag::DOOR);
		}
	}

	dtNavMeshCreateParams params = {};

	params.verts = polymesh->verts;
	params.vertCount = polymesh->nverts;
	params.polys = polymesh->polys;
	params.polyAreas = polymesh->areas;
	params.polyFlags = polymesh->flags;
	params.polyCount = polymesh->npolys;
	params.nvp = polymesh->nvp;
	params.detailMeshes = polymesh_detail->meshes;
	params.detailVerts = polymesh_detail->verts;
	params.detailVertsCount = polymesh_detail->nverts;
	params.detailTris = polymesh_detail->tris;
	params.detailTriCount = polymesh_detail->ntris;
	//params.offMeshConVerts = &offmesh_connections_verts[0];
	//params.offMeshConRad = &offmesh_connections_rads[0];
	//params.offMeshConDir = &offmesh_connections_dirs[0];
	//params.offMeshConAreas = &offmesh_connections_areas[0];
	//params.offMeshConFlags = &offmesh_connections_flags[0];
	//params.offMeshConUserID = &offmesh_connections_ids[0];
	//params.offMeshConCount = offmesh_connections_count;
	params.walkableHeight = cfg.walkableHeight;
	params.walkableRadius = cfg.walkableRadius;
	params.walkableClimb = cfg.walkableClimb;
	rcVcopy(polymesh->bmin, params.bmin);//, polymesh->bmin);
	rcVcopy(polymesh->bmax, params.bmax);//, polymesh->bmax);
	params.cs = cfg.cs;
	params.ch = cfg.ch;
	params.buildBvTree = true;

	if (!dtCreateNavMeshData(&params, &nav_data, &nav_data_size))
	{
		logger::log("[Navigation] Could not build Detour navmesh data.");
		return false;
	}

	navmesh = dtAllocNavMesh();
	if (!navmesh)
	{
		dtFree(nav_data);
		logger::log("[Navigation] Could not build Detour navmesh");
		return false;
	}

	dtStatus status;

	status = navmesh->init(nav_data, nav_data_size, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(nav_data);
		logger::log("[Navigation] Could not init Detour navmesh");
		return false;
	}

	status = nav_query->init(navmesh, 2048);
	if (dtStatusFailed(status))
	{
		logger::log("[Navigation] Could not init Detour query");
		return false;
	}

	fmt::MemoryWriter post_build_msg;
	post_build_msg << " Creation success! " << polymesh->nverts << ", verts and " << polymesh->npolys << " polys.";
	logger::log(post_build_msg.str());

	return true;
}
