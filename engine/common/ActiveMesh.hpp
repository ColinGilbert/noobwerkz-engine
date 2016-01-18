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
		std::vector<std::vector<noob::vec3>> get_face_list() const;
		
		// Struct data memebers
		std::vector<noob::vec3> vertices;
		std::vector<std::vector<size_t>> faces;
	};

	class active_mesh
	{
		public:
			struct face
			{
				std::vector<noob::vec3> verts;
				noob::vec3 normal;
				std::vector<noob::vec2> get_2d() const;
			};

			typedef noob::component<noob::active_mesh::face>::handle face_handle;
		
			void init(const noob::basic_mesh&);
			void init(const noob::indexed_polymesh&);

			// Basic functionality
			face_handle add_face(const noob::active_mesh::face&);
			face_handle add_face(const std::vector<std::array<float, 3>>&);
			
			bool exists(const noob::vec3&) const;
			bool exists(const face&) const;
			bool exists(face_handle) const;
			
			noob::vec3 get_vertex(uint32_t index) const;
			std::vector<uint32_t> get_face_indices(face_handle) const;
			noob::active_mesh::face get_face_values(face_handle) const;
					
			noob::basic_mesh to_basic_mesh() const;
			noob::indexed_polymesh to_indexed_polymesh() const;

			std::vector<noob::active_mesh::face_handle> get_adjacent_faces(noob::active_mesh::face_handle) const;

			// Generation-related utilities (meshes change and we sometimes/often need to retrieve older copies)
			size_t get_current_generation() const;
			noob::active_mesh retrieve_by_generation(size_t) const;
			bool is_generation_current(size_t) const;
		
			// TODO: Find out if "topological split" makes proper sense in this context
			std::vector<noob::active_mesh> topological_split(const std::vector<noob::active_mesh::face_handle>&) const;

			// Destructive utiiities. Those take full advantage of the speed benefits of half-edged meshes
			void make_hole(noob::active_mesh::face_handle);
			void fill_holes();
			void cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal);
			void cut_faces(std::vector<noob::active_mesh::face_handle>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal);
			void extrude(noob::active_mesh::face_handle, const noob::vec3& normal, float magnitude);
			void connect_faces(noob::active_mesh::face_handle first_handle, noob::active_mesh::face_handle second_handle);
			//void move_vertex(const noob::vec3& vertex, const noob::vec3& normal, float magnitude);
			void move_vertex(uint32_t index, const noob::vec3& direction);
			void move_vertices(const std::vector<uint32_t>& indices, const noob::vec3& direction);


			// This function is static because it acts on two objects. The static modifier makes it explicit.
			// TODO: Possibly move into mesh_utils
			static void join_meshes(const noob::active_mesh& first, noob::active_mesh::face_handle first_handle, const noob::active_mesh& second, noob::active_mesh::face_handle second_handle);
			
			
		protected:

			PolyMesh half_edges;
			// This function does life-saving checks. The first tuple element is the valid flag.
			std::tuple<bool, PolyMesh::Vertex*> get_vertex_ptr(size_t);

			// Data members.
			size_t current_generation;
			typedef noob::component<noob::active_mesh::face> faces_holder;
			faces_holder faces;
			std::unordered_map<size_t, PolyMesh::Face*> faces_handles_to_face_ptr;
			std::vector<PolyMesh::Vertex*> vertices_list;

			// For serialization; DOesn't need to get updated often and mostly lies fallow
			noob::indexed_polymesh index;
	};
}
