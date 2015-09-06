#pragma once

#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourStatus.h"
#include "DetourNavMeshBuilder.h"
#include "DetourTileCache.h"
#include "DetourTileCacheBuilder.h"
#include "DetourCrowd.h"
#include "InputGeom.h"
#include "SampleInterfaces.h"


namespace noob
{
	class simple_navmesh 
	{
		public:
			simple_navmesh();
			~simple_navmesh();
			/*enum SampleToolType
			{
				TOOL_NONE = 0,
				TOOL_TILE_EDIT,
				TOOL_TILE_HIGHLIGHT,
				TOOL_TEMP_OBSTACLE,
				TOOL_NAVMESH_TESTER,
				TOOL_NAVMESH_PRUNE,
				TOOL_OFFMESH_CONNECTION,
				TOOL_CONVEX_VOLUME,
				TOOL_CROWD,
				MAX_TOOLS
			};*/

			/// These are just sample areas to use consistent values across the samples.
			/// The use should specify these base on his needs.
			enum SamplePolyAreas
			{
				SAMPLE_POLYAREA_GROUND,
				SAMPLE_POLYAREA_WATER,
				SAMPLE_POLYAREA_ROAD,
				SAMPLE_POLYAREA_DOOR,
				SAMPLE_POLYAREA_GRASS,
				SAMPLE_POLYAREA_JUMP,
			};
			enum SamplePolyFlags
			{
				SAMPLE_POLYFLAGS_WALK		= 0x01,		// Ability to walk (ground, grass, road)
				SAMPLE_POLYFLAGS_SWIM		= 0x02,		// Ability to swim (water).
				SAMPLE_POLYFLAGS_DOOR		= 0x04,		// Ability to move through doors.
				SAMPLE_POLYFLAGS_JUMP		= 0x08,		// Ability to jump.
				SAMPLE_POLYFLAGS_DISABLED	= 0x10,		// Disabled polygon
				SAMPLE_POLYFLAGS_ALL		= 0xffff	// All abilities.
			};

			enum SamplePartitionType
			{
				SAMPLE_PARTITION_WATERSHED,
				SAMPLE_PARTITION_MONOTONE,
				SAMPLE_PARTITION_LAYERS,
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



			bool handleBuild();
			void handleRender();
			void cleanup();
			void handleSettings();
			void handleTools();
			void handle_debug_mode();

		protected:
			draw_mode m_DrawMode;
			bool m_keepInterResults;
			float m_totalBuildTimeMs;
			size_t nverts;
			rcConfig config;
			unsigned char* m_triareas;
			rcHeightfield* m_solid;
			rcCompactHeightfield* m_chf;
			rcContourSet* m_cset;
			rcPolyMesh* m_pmesh;
			rcConfig m_cfg;	
			rcPolyMeshDetail* m_dmesh;
			dtNavMesh* m_navMesh;
			
			InputGeom* m_geom;
			dtNavMeshQuery* m_navQuery;
			dtCrowd* m_crowd;

			unsigned char m_navMeshDrawFlags;

			float m_cellSize;
			float m_cellHeight;
			float m_agentHeight;
			float m_agentRadius;
			float m_agentMaxClimb;
			float m_agentMaxSlope;
			float m_regionMinSize;
			float m_regionMergeSize;
			float m_edgeMaxLen;
			float m_edgeMaxError;
			float m_vertsPerPoly;
			float m_detailSampleDist;
			float m_detailSampleMaxError;
			int m_partitionType;

			// SampleTool* m_tool;
			//mpleToolState* m_toolStates[MAX_TOOLS];

			BuildContext* m_ctx;

	};
}
