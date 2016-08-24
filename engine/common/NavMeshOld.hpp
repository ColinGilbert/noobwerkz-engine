#pragma once

#include "BasicMesh.hpp"

#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourStatus.h"
#include "DetourTileCache.h"
#include "DetourTileCacheBuilder.h"
#include "DetourCrowd.h"
#include "InputGeom.h"

namespace noob
{
	struct navmesh_config
	{
			bool keep_intermediates;
			float cell_size;
			float cell_height;
			float agent_height;
			float agent_radius;
			float agent_max_climb;
			float agent_max_slope;
			float region_min_size;
			float region_merge_size;
			float edge_max_length;
			float edge_max_error;
			float verts_per_poly;
			// Detail samples based on distance (TODO: Readup)
			float detail_sample_dist;
			float detail_sample_max_error;
			int partition_type;
	};

struct nav_path
{
	//! The start location.
	vec3			start_location;

	//! The end location.
	vec3			end_location;

	//! (Optional) The bit mask filter to use while querying.
	dtQueryFilter	path_filter;

	//! Upon query the start reference polygon index will be filled.
	dtPolyRef		start_reference;

	//! Upon query the end reference polygon index will be filled.	
	dtPolyRef		end_reference;
	
	//! Upon query the total number of polygon that the path or ray traverse will be filled.
	unsigned int	poly_count;

	//! Upon query the polygon index information will be filled.	
	dtPolyRef		poly_array[ NAVIGATION_MAX_POLY ];

};


	class simple_navmesh 
	{
		public:
			
			simple_navmesh() noexcept(true) : keep_intermediates(true), tri_areas(0), voxels(0), compact_heightfield(0), contour_set(0), poly_mesh(0), detail_mesh(0) { }
			
			~simple_navmesh() noexcept(true);

			static constexpr uint32_t max_polys = 512;
			/// The user should specify these based on needs.
			enum poly_area_types
			{
				POLYAREA_GROUND,
				POLYAREA_WATER,
				POLYAREA_ROAD,
				POLYAREA_DOOR,
				POLYAREA_GRASS,
				POLYAREA_JUMP,
			};

			enum poly_flags
			{
				POLYFLAGS_WALK		= 0x01,		// Ability to walk (ground, grass, road)
				POLYFLAGS_SWIM		= 0x02,		// Ability to swim (water).
				POLYFLAGS_DOOR		= 0x04,		// Ability to move through doors.
				POLYFLAGS_JUMP		= 0x08,		// Ability to jump.
				POLYFLAGS_DISABLED	= 0x10,		// Disabled polygon
				POLYFLAGS_ALL		= 0xffff	// All abilities.
			};

			enum partition_method
			{
				PARTITION_WATERSHED,
				PARTITION_MONOTONE,
				PARTITION_LAYERS,
			};

			enum draw_mode
			{
				DRAWMODE_NAVMESH,
				DRAWMODE_NAVMESH_TRANS,
				DRAWMODE_NAVMESH_BVTREE,
				DRAWMODE_NAVMESH_NODES,
				DRAWMODE_NAVMESH_INVIS,
				DRAWMODE_MESH,
				DRAWMODE_VOXELS,
				DRAWMODE_VOXELS_WALKABLE,
				DRAWMODE_COMPACT,
				DRAWMODE_COMPACT_DISTANCE,
				DRAWMODE_COMPACT_REGIONS,
				DRAWMODE_REGION_CONNECTIONS,
				DRAWMODE_RAW_CONTOURS,
				DRAWMODE_BOTH_CONTOURS,
				DRAWMODE_CONTOURS,
				DRAWMODE_POLYMESH,
				DRAWMODE_POLYMESH_DETAIL,
				MAX_DRAWMODE
			};

			void add_mesh(const noob::masic_mesh&) noexcept(true);
			bool build(const noob::navmesh_config&) noexcept(true);
			void cleanup() noexcept(true);


		protected:



			
			bool keep_intermediates;
			float cell_size;
			float cell_height;
			float agent_height;
			float agent_radius;
			float agent_max_climb;
			float agent_max_slope;
			float region_min_size;
			float region_merge_size;
			float edge_max_length;
			float edge_max_error;
			float verts_per_poly;
			// Detail samples based on distance (TODO: Readup)
			float detail_sample_dist;
			float detail_sample_max_error;
			int partition_type;


			size_t num_verts;
			unsigned char* tri_areas;
			






			rcHeightfield* voxels;
			rcCompactHeightfield* compact_heightfield;
			rcContourSet* contour_set;
			rcPolyMesh* poly_mesh;
			rcConfig config;	
			rcPolyMeshDetail* detail_mesh;
			
			dtNavMesh* nav_mesh;
			InputGeom* geom;
			dtNavMeshQuery* nav_query;
			dtCrowd* crowd;

	};
}
