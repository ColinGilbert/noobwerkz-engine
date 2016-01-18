// A so-called "active mesh" is one whose primary representation is a half-edge structure. It converts to a basic_mesh whenever one is needed, This allows fast application of algorithms as there is no need to convert an indexed list into a half-edge structure every time we use any algorithms. Also, as a bonus the faces on these meshes can be of any number of vertices, instead of being forced to use triangles.
// NOTE: Thjo class adds overhead on top of the PolyMesh structure backing it. This comes from validity checks that prevent the app from crashing on bad values. In order to make more cool algorithms function as fast as possible, it is recommended that you extend this class and send me the patches. :)

// TODO:
// TODO: Split faces
// TODO: Add face merging (single-mesh and two meshes)

#pragma once
#include "Component.hpp"
#include "MathFuncs.hpp"
#include "BasicMesh.hpp"
#include "MeshUtils.hpp"


namespace noob
{
	class active_mesh
	{
			
		public:
			struct face
			{
				std::vector<std::array<float, 3>> verts;
			};

			typedef noob::component<noob::active_mesh::face>::handle face_handle;

			
			noob::basic_mesh basic_mesh() const;
			face_handle add_face(const noob::active_mesh::face&);
			face_handle add_face(const std::vector<std::array<float, 3>>&);
			
			bool exists(const noob::vec3&) const;
			bool exists(const face&) const;
			bool exists(face_handle) const;

			std::vector<uint32_t> get_indices(const noob::vec3&) const;

		protected:
			// TODO: Optimize.
			std::map<std::array<float, 3>, std::vector<uint32_t>>  verts_to_indices;
			typedef noob::component<noob::active_mesh::face> faces_holder;
			faces_holder faces;
			
	};
}
