// A so-called "active mesh" is one whose primary representation is a half-edge structure. It converts to a basic_mesh whenever one is needed, This allows fast application of algorithms as there is no need to convert an indexed list into a half-edge structure every time we use any algorithms. Also, as a bonus the faces on these meshes can be of any number of vertices, instead of being forced to use triangles.
// NOTE: Thjo class adds overhead on top of the PolyMesh structure backing it. This comes from validity checks that prevent the app from crashing on bad values. In order to make more cool algorithms function as fast as possible, it is recommended that you extend this class and send me the patches. :)


// TODO: As a utility, this mesh provides snapshotting and retrieval of previous generation.
// TODO: Split faces
// TODO: Add face merging (single-mesh and two meshes)

#pragma once

#include <cereal/access.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

#include "Component.hpp"

#include "MathFuncs.hpp"
#include "BasicMesh.hpp"

#include "MeshUtils.hpp"

typedef OpenMesh::PolyMesh_ArrayKernelT<> PolyMesh;

namespace noob
{
	class active_mesh
	{
		public:
			struct face
			{
				face() : hole(false) {}
				std::vector<noob::vec2> get_2d() const;

				std::vector<noob::vec3> verts;
				noob::vec3 normal;
				bool hole;
			};

			void init(const noob::basic_mesh&);

			// void init(const noob::indexed_polymesh&);

			size_t add_vertex(const noob::vec3&);

			// Basic functionality
			// size_t add_face(const noob::active_mesh::face&);
			
			size_t add_face(const std::vector<size_t>&);
			
			// Constants
			bool vertex_exists(const noob::vec3&) const;
			
			bool vertex_exists(const size_t) const;

			noob::vec3 get_vertex(size_t index) const;

			
			bool face_exists(const face&) const;
			bool face_exists(size_t index) const;
			
			size_t num_vertices() const;

			// size_t num_half_edges() const;

			size_t num_edges() const;

			size_t num_faces() const;

			std::vector<size_t> get_verts_for_face(size_t) const;
			
			noob::active_mesh::face get_face(size_t) const;
			
			// Caution: This function has the potential to pass around lots of data. Use cautiously.
			std::vector<noob::active_mesh::face> get_face_list() const;
			
			noob::basic_mesh to_basic_mesh() const;
			
			// noob::indexed_polymesh to_indexed_polymesh() const;
			
			std::vector<size_t> get_adjacent_faces(size_t) const;

			// Generation-related utilities (meshes change and we sometimes/often need to retrieve older copies)
			
			// size_t get_current_generation() const;
			
			// noob::active_mesh retrieve_by_generation(size_t) const;
			
			// bool is_generation_current(size_t) const;
		
			std::vector<noob::active_mesh> topological_split(const std::vector<size_t>& sampling_points, size_t max_vertices) const;

			// Destructive utiiities. Those take full advantage of the speed benefits of half-edged meshes
			void make_hole(size_t);
			
			void fill_holes();
			
			void cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal);
			
			void cut_faces(std::vector<size_t>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal);
			
			void extrude(size_t, const noob::vec3& normal, float magnitude);
			
			void connect_faces(size_t first, size_t second);
			
			//void move_vertex(const noob::vec3& vertex, const noob::vec3& normal, float magnitude);
			
			void move_vertex(size_t index, const noob::vec3& direction);
			
			void move_vertices(const std::vector<size_t>& indices, const noob::vec3& direction);
			
			void merge_adjacent_coplanars();
			
			// This function is static because it acts on two objects. The static modifier makes it explicit.
			// TODO: Possibly move into mesh_utils
			static void join_meshes(const noob::active_mesh& first_mesh, size_t first_handle, const noob::active_mesh& second, size_t second_handle);
			
			
		protected:

			// This function does life-saving checks. The first tuple element is the valid flag.
			// std::tuple<bool, PolyMesh::Vertex*> get_vertex_ptr(size_t) const;
			
			// Data members.
			PolyMesh half_edges;
			
			std::map<std::array<float, 3>, PolyMesh::VertexHandle> xyz_to_vhandle;

	};
}
