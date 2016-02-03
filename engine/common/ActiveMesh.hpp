// A so-called "active mesh" is one whose primary representation is a half-edge structure. It converts to a basic_mesh whenever one is needed, This allows fast application of algorithms as there is no need to convert an indexed list into a half-edge structure every time we use any algorithms. Also, as a bonus the faces on these meshes can be of any number of vertices, instead of being forced to use triangles.
// NOTE: Thjo class adds overhead on top of the PolyMesh structure backing it. This comes from validity checks that prevent the app from crashing on bad values. In order to make more cool algorithms function as fast as possible, it is recommended that you extend this class and send me the patches. :)


// TODO: As a utility, this mesh provides snapshotting and retrieval of previous generation.
// TODO: Split faces
// TODO: Add face merging (single-mesh and two meshes)

#pragma once

#include <set>

#include <cereal/access.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/set.hpp>
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
			//	class face
			// {
			//	friend class active_mesh;
			//	public:
			//	face() : hole(false) {}
				
			//	void add_vertex(const noob::vec3& v)
			//	{
			//		vertices.push_back(v.v);
			//	}
				
			//	void set_vertex(size_t i, const noob::vec3& v)
			//	{
			//		if (i < vertices.size())
			//		{
			//			vertices[i] = v;
			//		}
			//	}

			//	noob::vec3 get_vertex(size_t i) const
			//	{
			//		if (i > vertices.size())
			//		{
			//			return vertices[vertices.size()-1];
			//		}
			//		return vertices[i];
			//	}
				
			//	noob::vec3 get_normal() const
			//	{
			//		return normal;
			//	}


			//	protected:
			//	std::vector<noob::vec3> vertices;
			//	noob::vec3 normal;
			//	bool hole;
			// };

			void init(const noob::basic_mesh&);

			// void init(const noob::indexed_polymesh&);

			PolyMesh::VertexHandle add_vertex(const noob::vec3&);

			// Basic functionality
			PolyMesh::FaceHandle add_face(const std::array<noob::vec3, 3>&);
			PolyMesh::FaceHandle add_face(const std::array<noob::vec3, 4>&);
			PolyMesh::FaceHandle add_face(const std::vector<noob::vec3>&);

			PolyMesh::FaceHandle add_face(const std::vector<PolyMesh::VertexHandle>&);
			
			bool vertex_exists(const noob::vec3&) const;
			bool vertex_exists(const PolyMesh::VertexHandle) const;

			std::tuple<bool, noob::vec3> get_vertex(PolyMesh::VertexHandle) const;

			noob::vec3 get_face_normal(PolyMesh::FaceHandle&) const;

			PolyMesh::VertexHandle get_vertex_handle(const noob::vec3&) const;

			// bool face_exists(const std::array<noob::vec3, 3>&) const;
			// bool face_exists(const std::array<noob::vec3, 4>&) const;
			
			// Caution: This function has the potential to do lots of iterations, as it tests against all faces sharing the first vertex/
			bool face_exists(const std::vector<noob::vec3>&) const;

			bool face_exists(PolyMesh::FaceHandle) const;
			
			PolyMesh::FaceHandle get_face_handle(const noob::vec3&) const;

			const std::tuple<bool, std::vector<noob::vec3>> get_face(PolyMesh::VertexHandle) const; 

			size_t num_vertices() const;

			size_t num_half_edges() const;

			size_t num_edges() const;

			size_t num_faces() const;

			std::vector<PolyMesh::VertexHandle> get_vertex_handles_for_face(PolyMesh::FaceHandle) const;
			
			std::vector<noob::vec3> get_verts_for_face(PolyMesh::FaceHandle) const;

			std::tuple<bool, std::vector<noob::vec3>> get_face(PolyMesh::FaceHandle) const;
		
			std::vector<PolyMesh::EdgeHandle> get_adjacent_edges(PolyMesh::FaceHandle, PolyMesh::FaceHandle) const;

			// Caution: This function has the potential to pass around lots of data. Use cautiously.
			// std::vector<std::vector<noob::vec3>> get_all_faces() const;
			
			noob::basic_mesh to_basic_mesh() const;
			
			// noob::indexed_polymesh to_indexed_polymesh() const;
			
			std::vector<PolyMesh::FaceHandle> get_adjacent_faces(PolyMesh::FaceHandle) const;

			// Basically a floodfill where all the edjacent faces get collected one-by-one and all final meshes end up =< max_vertices. Great for splitting meshes prior to uploading them to video card (if you only have 65k indices.) Returns copies of the original.
			std::vector<noob::active_mesh> topological_split(size_t max_vertices) const;

			// Destructive utiiities.
			void make_hole(PolyMesh::FaceHandle);
			
			void fill_holes();
			
			// TODO: Before implementing the next two methods, come up with a proper plane representation (probably using Eigen or Geometric Tools.)
			// void cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal);
			// void cut_faces(std::vector<PolyMesh::FaceHandle>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal);
			
			void extrude(PolyMesh::FaceHandle, float magnitude);
			
			// void connect_faces(PolyMesh::FaceHandle first, PolyMesh::FaceHandle second);
			
			// void move_vertex(const noob::vec3& vertex, const noob::vec3& normal, float magnitude);
			
			void move_vertex(PolyMesh::VertexHandle, const noob::vec3& direction);
			
			void move_vertices(const std::vector<PolyMesh::VertexHandle>&, const noob::vec3& direction);
			
			void merge_adjacent_coplanars();
			
			// This function is static because it acts on two objects. The static modifier makes it explicit.
			// TODO: Possibly move into mesh_utils
			static void join_meshes_at_face(const noob::active_mesh& first_mesh, PolyMesh::FaceHandle first_handle, const noob::active_mesh& second, PolyMesh::FaceHandle second_handle);
			
			
		protected:

			// Returns the adjacent faces that aren't in the taken list.
			// Side effects: Updates and sorts the taken list argument.
			std::vector<PolyMesh::FaceHandle> topo_split_helper(PolyMesh::FaceHandle sample, std::set<PolyMesh::FaceHandle>& taken, size_t max_vertices) const;

			// This function does life-saving checks. The first tuple element is the valid flag.
			// std::tuple<bool, PolyMesh::Vertex*> get_vertex_ptr(size_t) const;
			
			// Data members.
			PolyMesh half_edges;
			
			std::map<std::array<float, 3>, PolyMesh::VertexHandle> xyz_to_vhandles;

	};
}
