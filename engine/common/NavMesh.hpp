#pragma once

#include <rdestl/fixed_array.h>

#include "BasicMesh.hpp"

#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourStatus.h"
#include "DetourTileCache.h"
#include "DetourTileCacheBuilder.h"
#include "DetourCrowd.h"

namespace noob
{

	class navigation
	{
		public:

			//! The maximum amount of node for the navigation mesh.
			static constexpr uint32_t max_nodes = 1024;

			//! The maximum amount of poly that can be used as query.
			static constexpr uint32_t max_query_polys = 512;

			//! The maximum amount of poly that can be used with a path.
			static constexpr uint32_t max_path_polys = 512;


			//! Structure representing the different configuration parameters when building a navigation mesh.
			struct config
			{
				//! The size of each cell (in GL units).
				float cell_size;

				//! The height of the cell (in GL units).
				float cell_height;

				//! The size of the player or moving entity (in GL units).
				float agent_height;

				//! The radius of the player or moving entity (in GL units).
				float agent_radius;

				//! The maximum height that a player can climb (in GL units).
				float agent_max_climb;

				//! The maximum slope that the player can walk on (in degree).
				float agent_max_slope;

				//! The minimum region size to include in the navigation mesh (in GL units).
				float region_min_size;

				//! The size to use for mergin region (in GL units).
				float region_merge_size;

				//! The maximum lenght of an edge (in GL units).
				float edge_max_len;

				//! The maximum amount of "tolerable" errors to use for the edges.
				float edge_max_error;

				//! The number of vertex per polygons.
				float vert_per_poly;

				//! The number of details samples based on the distance.
				float detail_sample_dst;

				//! The maximum amount of "tolerable" error on the sample calculation.
				float detail_sample_max_error;

			};


			//! Structure to use to query the navigation mesh.
			struct query
			{
				//! The start location.
				noob::vec3 start_location;

				//! The end location.
				noob::vec3 end_location;

				//! (Optional) The bit mask filter to use while querying.
				dtQueryFilter	path_filter;

				//! Upon query the start reference polygon index will be filled.
				dtPolyRef start_reference;

				//! Upon query the end reference polygon index will be filled.	
				dtPolyRef end_reference;

				//! Upon query the total number of polygon that the path or ray traverse will be filled.
				uint32_t poly_count;

				//! Upon query the polygon index information will be filled.	
				rde::fixed_array<dtPolyRef, noob::navigation::max_query_polys> polys;

			};


			//! If a navigation path query is successful the result will be store in this structure.
			struct path_data
			{
				//! The number of point that form the navigation path.
				uint32_t path_point_count;

				//! The flags for each polygon.
				rde::fixed_array<uint8_t, noob::navigation::max_path_polys> path_flags;

				//! The XYZ position is world coordinates for each navigation points.
				rde::fixed_array<noob::vec3, noob::navigation::max_path_polys> path_points;

				//! The polygon index for each point.
				rde::fixed_array<dtPolyRef, noob::navigation::max_path_polys> path_polys;

			};


			void add_geom(const noob::basic_mesh&) noexcept(true);
			void cleanup_temporaries() noexcept(true);
			void clear_geom() noexcept(true);
			bool build() noexcept(true);



		protected:

			// The pointer we must hold onto:
			dtNavMesh* nav_mesh;


			// Temporaries that can be disposed of to reduce memory usage.
			noob::basic_mesh input;

			rcHeightfield* heightfield;

			rcCompactHeightfield* compact_heightfield;

			rcContourSet* contours;

			rcPolyMesh* polymesh;

			rcPolyMeshDetail *polymesh_detail;



	};
}
