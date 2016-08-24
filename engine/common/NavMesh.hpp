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
			
			navigation() noexcept(true) : navmesh(nullptr), heightfield(nullptr), compact_heightfield(nullptr), contours(nullptr), polymesh(nullptr), polymesh_detail(nullptr) {}
			~navigation() noexcept(true);
			
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
			dtNavMesh* navmesh;


			// Temporaries that can be disposed of to reduce memory usage.
			noob::basic_mesh input;

			rcHeightfield* heightfield;

			rcCompactHeightfield* compact_heightfield;

			rcContourSet* contours;

			rcPolyMesh* polymesh;

			rcPolyMeshDetail* polymesh_detail;



	};
}

/*
struct dtNavMeshCreateParams
{

	/// @name Polygon Mesh Attributes
	/// Used to create the base navigation graph.
	/// See #rcPolyMesh for details related to these attributes.
	/// @{

	const unsigned short* verts;			///< The polygon mesh vertices. [(x, y, z) * #vertCount] [Unit: vx]
	int vertCount;							///< The number vertices in the polygon mesh. [Limit: >= 3]
	const unsigned short* polys;			///< The polygon data. [Size: #polyCount * 2 * #nvp]
	const unsigned short* polyFlags;		///< The user defined flags assigned to each polygon. [Size: #polyCount]
	const unsigned char* polyAreas;			///< The user defined area ids assigned to each polygon. [Size: #polyCount]
	int polyCount;							///< Number of polygons in the mesh. [Limit: >= 1]
	int nvp;								///< Number maximum number of vertices per polygon. [Limit: >= 3]

	/// @}
	/// @name Height Detail Attributes (Optional)
	/// See #rcPolyMeshDetail for details related to these attributes.
	/// @{

	const unsigned int* detailMeshes;		///< The height detail sub-mesh data. [Size: 4 * #polyCount]
	const float* detailVerts;				///< The detail mesh vertices. [Size: 3 * #detailVertsCount] [Unit: wu]
	int detailVertsCount;					///< The number of vertices in the detail mesh.
	const unsigned char* detailTris;		///< The detail mesh triangles. [Size: 4 * #detailTriCount]
	int detailTriCount;						///< The number of triangles in the detail mesh.

	/// @}
	/// @name Off-Mesh Connections Attributes (Optional)
	/// Used to define a custom point-to-point edge within the navigation graph, an 
	/// off-mesh connection is a user defined traversable connection made up to two vertices, 
	/// at least one of which resides within a navigation mesh polygon.
	/// @{

	/// Off-mesh connection vertices. [(ax, ay, az, bx, by, bz) * #offMeshConCount] [Unit: wu]
	const float* offMeshConVerts;
	/// Off-mesh connection radii. [Size: #offMeshConCount] [Unit: wu]
	const float* offMeshConRad;
	/// User defined flags assigned to the off-mesh connections. [Size: #offMeshConCount]
	const unsigned short* offMeshConFlags;
	/// User defined area ids assigned to the off-mesh connections. [Size: #offMeshConCount]
	const unsigned char* offMeshConAreas;
	/// The permitted travel direction of the off-mesh connections. [Size: #offMeshConCount]
	///
	/// 0 = Travel only from endpoint A to endpoint B.<br/>
	/// #DT_OFFMESH_CON_BIDIR = Bidirectional travel.
	const unsigned char* offMeshConDir;	
	/// The user defined ids of the off-mesh connection. [Size: #offMeshConCount]
	const unsigned int* offMeshConUserID;
	/// The number of off-mesh connections. [Limit: >= 0]
	int offMeshConCount;

	/// @}
	/// @name Tile Attributes
	/// @note The tile grid/layer data can be left at zero if the destination is a single tile mesh.
	/// @{

	unsigned int userId;	///< The user defined id of the tile.
	int tileX;				///< The tile's x-grid location within the multi-tile destination mesh. (Along the x-axis.)
	int tileY;				///< The tile's y-grid location within the multi-tile desitation mesh. (Along the z-axis.)
	int tileLayer;			///< The tile's layer within the layered destination mesh. [Limit: >= 0] (Along the y-axis.)
	float bmin[3];			///< The minimum bounds of the tile. [(x, y, z)] [Unit: wu]
	float bmax[3];			///< The maximum bounds of the tile. [(x, y, z)] [Unit: wu]

	/// @}
	/// @name General Configuration Attributes
	/// @{

	float walkableHeight;	///< The agent height. [Unit: wu]
	float walkableRadius;	///< The agent radius. [Unit: wu]
	float walkableClimb;	///< The agent maximum traversable ledge. (Up/Down) [Unit: wu]
	float cs;				///< The xz-plane cell size of the polygon mesh. [Limit: > 0] [Unit: wu]
	float ch;				///< The y-axis cell height of the polygon mesh. [Limit: > 0] [Unit: wu]

	/// True if a bounding volume tree should be built for the tile.
	/// @note The BVTree is not normally needed for layered navigation meshes.
	bool buildBvTree;

	/// @}
};
*/_
