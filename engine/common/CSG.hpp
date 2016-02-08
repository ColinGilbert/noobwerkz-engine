#pragma once

#include "BasicMesh.hpp"

#include <brlcad/vmath.h>
#include <brlcad/nmg.h>
#include <brlcad/rtgeom.h>
#include <brlcad/raytrace.h>


namespace noob
{
	class csg
	{
		public:
			csg() : verbose(false), NMG_debug(0), regions_tried(0), regions_converted(0), regions_written(0), total_polygons(0) {}
			
			void init();
			noob::basic_mesh to_basic_mesh() const;

		protected:
			// Members
			bool verbose;
			int NMG_debug, regions_tried, regions_converted, regions_written;
			size_t total_polygons;

			db_i *dbip;
			rt_tess_tol tesselation_tolerance;
			bn_tol calculation_tolerance;
			model *csg_model;

			db_tree_state tree_state;

			// Functions
			// 
	};
}
