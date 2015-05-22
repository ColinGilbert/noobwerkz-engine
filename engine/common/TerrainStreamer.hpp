// This class provides a dead-simple terrain streaming solution. Drawable gets extracted from mesh region on-demand.
// Allows fast changes that don't cause rebuilds or anything stupid. 
// However, the time it takes to iterate over the mesh our cost (basically just an array, so it might work out.)

#pragma once

#include "Drawable.hpp"
#include "Mesh.hpp"
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

namespace noob
{
	class terrain
	{
		public:
			// When looking down from the top, y becomes the equivalent of z (from front-view.)
			struct terrain_patch
			{
				float bottom_left_x;
				float bottom_left_z;
				float top_right_x;
				float top_right_z;
			};

			void load_terrain_mesh(const std::string& filename);

			noob::drawable* get_terrain(const terrain::terrain_patch& dims);

			// void update_terrain(const terrain::terrain_patch& area);

		protected:
			OpenMesh::TriMesh_ArrayKernelT<> world_mesh;
	};
}
