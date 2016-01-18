// A so-called "active mesh" is one whose primary representation is a half-edge structure. It converts to a basic_mesh whenever one is needed, This allows fast application of algorithms as there is no need to convert an indexed list into a half-edge structure every time we use any algorithms. Also, as a bonus the faces on these meshes can be of any number of vertices, instead of being forced to use triangles.
// NOTE: Thjo class adds overhead on top of the PolyMesh structure backing it. This comes from validity checks that prevent the app from crashing on bad values. In order to make more cool algorithms function as fast as possible, it is recommended that you extend this class and send me the patches. :)


// TODO: As a utility, this mesh provides snapshotting and retrieval of previous generation.
// TODO: Split faces
// TODO: Add face merging (single-mesh and two meshes)

#pragma once

#include "Component.hpp"
#include "MathFuncs.hpp"
#include "BasicMesh.hpp"
#include "MeshUtils.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> PolyMesh;

namespace noob
{
	struct indexed_polymesh
	{	
		// Convenience
		std::vector<std::vector<std::array<float, 3>>> get_face_list() const;
		
		// Struct data memebers
		std::vector<std::array<float, 3>> vertices;
		std::vector<std::vector<uint32_t>> faces;
	};

	class active_mesh
	{
		public:
			struct face
			{
				std::vector<noob::vec3> verts;
				std::vector<noob::vec2> get_2d() const;
				noob::vec3 get_normal() const;
			};

			typedef noob::component<noob::active_mesh::face>::handle face_handle;
			
			// Basic functionality
			face_handle add_face(const noob::active_mesh::face&);
			face_handle add_face(const std::vector<std::array<float, 3>>&);
			
			bool exists(const noob::vec3&) const;
			bool exists(const face&) const;
			bool exists(face_handle) const;

			noob::active_mesh::face get_face_vals(face_handle) const;
			noob::active_mesh::face get_face_vals(const noob::vec3&) const;
			noob::active_mesh::face get_face_vals(uint32_t) const;
			
			// std::vector<uint32_t> get_indices(const noob::vec3&) const;
			
			noob::basic_mesh to_basic_mesh() const;
			noob::indexed_polymesh to_indexed_polymesh() const;

			std::vector<noob::active_mesh::face_handle> get_adjacent_faces(noob::active_mesh::face_handle) const;

			// Generation-related utilities (meshes change and we sometimes/often need to retrieve older copies)
			size_t get_current_generation() const;
			noob::active_mesh retrieve_by_generation(uint32_t) const;
			bool is_generation_current(uint32_t) const;
		
			// TODO: Find out if "topological split" makes proper sense in this context
			std::vector<noob::active_mesh> topological_split(const std::vector<noob::active_mesh::face_handle>&) const;

			// Destructive utiiities. Those take full advantage of the speed benefits of half-edged meshes
			void make_hole(noob::active_mesh::face_handle);
			void fill_holes();
			void cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal);
			void cut_faces(std::vector<noob::active_mesh::face_handle>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal);
			void extrude(noob::active_mesh::face_handle, const noob::vec3& normal, float magnitude);
			void connect_faces(noob::active_mesh::face_handle first_handle, noob::active_mesh::face_handle second_handle);
			void move_vertex(const noob::vec3& vertex, const noob::vec3& normal, float magnitude);
			void move_vertex(uint32_t index, const noob::vec3& normal, float magnitude);


			// This function is static because it acts on two objects. The static modifier makes it explicit.
			// TODO: Possibly move into mesh_utils
			static void join_meshes(const noob::active_mesh& first, noob::active_mesh::face_handle first_handle, const noob::active_mesh& second, noob::active_mesh::face_handle second_handle);
			
			
		protected:

			PolyMesh half_edges;

			struct inner_index_mesh
			{
				std::vector<std::tuple<std::array<float, 3>, PolyMesh::Vertex*>> vertices;
				std::vector<uint32_t> faces;
			};

			void update_index();
			noob::active_mesh::inner_index_mesh index;
			// TODO: Optimize.
			// std::map<std::array<float, 3>, std::vector<uint32_t>>  verts_to_indices;
			uint32_t current_generation;
			typedef noob::component<noob::active_mesh::face> faces_holder;
			faces_holder faces;
	};
}
