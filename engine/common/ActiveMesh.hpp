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

			typedef PolyMesh::VertexHandle vertex_handle;
			typedef PolyMesh::FaceHandle face_handle;
			typedef PolyMesh::EdgeHandle edge_handle;
			typedef PolyMesh::HalfedgeHandle halfedge_handle;
			
			bool vertex_exists(const noob::vec3&) const;
			bool vertex_exists(const PolyMesh::VertexHandle) const;

			std::tuple<bool, noob::vec3> get_vertex(const PolyMesh::VertexHandle) const;

			noob::vec3 get_face_normal(const PolyMesh::FaceHandle) const;

			PolyMesh::VertexHandle get_vertex_handle(const noob::vec3&) const;

			// Caution: This function has the potential to do lots of iterations, as it tests against all faces sharing the first vertex/
			bool face_exists(const std::vector<noob::vec3>&) const;

			bool face_exists(const PolyMesh::FaceHandle) const;
			
			// PolyMesh::FaceHandle get_face_handle(const noob::vec3&) const;

			const std::tuple<bool, std::vector<noob::vec3>> get_face(const PolyMesh::VertexHandle) const; 

			size_t num_vertices() const;

			size_t num_half_edges() const;

			size_t num_edges() const;

			size_t num_faces() const;

			std::vector<PolyMesh::VertexHandle> get_vertex_handles_for_face(const PolyMesh::FaceHandle) const;
			
			std::vector<noob::vec3> get_verts_for_face(const PolyMesh::FaceHandle) const;

			std::tuple<bool, std::vector<noob::vec3>> get_face(const PolyMesh::FaceHandle) const;
		
			std::vector<PolyMesh::EdgeHandle> get_adjacent_edges(const PolyMesh::FaceHandle, const PolyMesh::FaceHandle) const;

			noob::basic_mesh to_basic_mesh() const;
			
			std::vector<PolyMesh::FaceHandle> get_adjacent_faces(const PolyMesh::FaceHandle) const;

			// Implemented as an iterator that copies verts + faces until certain size is reached, then creates new one. Repeats until exhaustion.
			std::vector<noob::active_mesh> split(size_t max_vertices) const;

			// -----------------------
			// Destructive utiiities.
			// -----------------------

			void reset();
			
			void from_basic_mesh(const noob::basic_mesh&);

			PolyMesh::VertexHandle add_vertex(const noob::vec3&);

			PolyMesh::FaceHandle add_face(const std::array<noob::vec3, 3>&);
			
			PolyMesh::FaceHandle add_face(const std::array<noob::vec3, 4>&);
			
			PolyMesh::FaceHandle add_face(const std::vector<noob::vec3>&);
			
			PolyMesh::FaceHandle add_face(const std::vector<PolyMesh::VertexHandle>&);

			void make_hole(const PolyMesh::FaceHandle);
			
			void fill_holes();
			
			// --------------------------------------------------------------------------------------------------------------------------------------
			// TODO: Before implementing the next two methods, come up with a proper plane representation (probably using Eigen or Geometric Tools.)
			// --------------------------------------------------------------------------------------------------------------------------------------

			// void cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal);
			
			// void cut_faces(std::vector<PolyMesh::FaceHandle>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal);
			
			void extrude_face(const PolyMesh::FaceHandle, float magnitude, const noob::vec3& normal);
			
			// void connect_faces(PolyMesh::FaceHandle first, PolyMesh::FaceHandle second);
			
			// void move_vertex(const noob::vec3& vertex, const noob::vec3& normal, float magnitude);
			
			void move_vertex(const PolyMesh::VertexHandle, const noob::vec3& direction);
			
			void move_vertices(const std::vector<PolyMesh::VertexHandle>&, const noob::vec3& direction);
			
			void merge_adjacent_coplanars();
			
			// This function is static because it acts on two objects. The static modifier makes it explicit.
			// static void join_meshes_at_face(const noob::active_mesh& first_mesh, const PolyMesh::FaceHandle first_handle, const noob::active_mesh& second, const PolyMesh::FaceHandle second_handle);
			
			
		protected:
			PolyMesh half_edges;
			std::map<std::array<float, 3>, PolyMesh::VertexHandle> xyz_to_vhandles;

	};
}
