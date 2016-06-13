// A so-called "active mesh" is one whose primary representation is a half-edge structure. It converts to a basic_mesh whenever one is needed, This allows fast application of algorithms as there is no need to convert an indexed list into a half-edge structure every time we use any algorithms. Also, as a bonus the faces on these meshes can be of any number of vertices, instead of being forced to use triangles.
// NOTE: Thjo class adds overhead on top of the PolyMesh structure backing it. This comes from validity checks that prevent the app from crashing on bad values. In order to make more cool algorithms function as fast as possible, it is recommended that you extend this class and send me the patches. :)


// TODO: As a utility, this mesh provides snapshotting and retrieval of previous generation.
// TODO: Split faces
// TODO: Add face merging (single-mesh and two meshes)

#pragma once


#include <set>

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

			typedef PolyMesh::VertexHandle vertex_h;
			typedef PolyMesh::FaceHandle face_h;
			typedef PolyMesh::EdgeHandle edge_h;
			typedef PolyMesh::HalfedgeHandle halfedge_h;
			
			bool vertex_exists(const noob::vec3&) const;
			
			bool vertex_exists(const noob::active_mesh::vertex_h) const;

			std::tuple<bool, noob::vec3> get_vertex(const noob::active_mesh::vertex_h) const;

			std::tuple<bool, noob::active_mesh::vertex_h> get_vertex_handle(const noob::vec3&) const;
			
			// Caution: This function has the potential to do lots of iterations, as it tests against all faces sharing the first vertex
			bool face_exists(const std::vector<noob::vec3>&) const;

			bool face_exists(const noob::active_mesh::face_h) const;

			noob::vec3 get_face_normal(const noob::active_mesh::face_h) const;

			std::vector<noob::active_mesh::face_h> get_faces_with_vert(const noob::active_mesh::vertex_h) const; 

			std::vector<noob::active_mesh::face_h> get_faces_with_edge(const noob::active_mesh::edge_h) const; 

			noob::active_mesh::face_h get_face_with_halfedge(const noob::active_mesh::halfedge_h) const;
			
			size_t num_vertices() const;

			size_t num_half_edges() const;

			size_t num_edges() const;

			size_t num_faces() const;

			std::vector<noob::active_mesh::vertex_h> get_verts_in_face(const noob::active_mesh::face_h) const;
			
			std::vector<noob::active_mesh::halfedge_h> get_halfedges_in_face(const noob::active_mesh::face_h) const;

			std::vector<noob::active_mesh::edge_h> get_adjacent_edges(const noob::active_mesh::face_h, const noob::active_mesh::face_h) const;

			std::vector<noob::active_mesh::face_h> get_adjacent_faces(const noob::active_mesh::face_h) const;
		
			noob::basic_mesh to_basic_mesh() const;

			// Implemented as an iterator that copies verts + faces until certain size is reached, then creates new one. Repeats until exhaustion.
			std::vector<noob::active_mesh> split(size_t max_vertices) const;

			// -----------------------
			// Destructive utiiities.
			// -----------------------

			void reset();
			
			void from_basic_mesh(const noob::basic_mesh&);

			noob::active_mesh::vertex_h add_vertex(const noob::vec3&);

			noob::active_mesh::face_h add_face(const std::vector<noob::active_mesh::vertex_h>&);

			void make_hole(const noob::active_mesh::face_h);
			
			void fill_holes();
			
			// TODO: Before implementing the next two methods, come up with a proper plane representation (probably using Eigen or Geometric Tools.)
			// void cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal);
			// void cut_faces(std::vector<noob::active_mesh::face_h>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal);
			
			void sweep_line(const noob::active_mesh::halfedge_h, const noob::vec3&);

			void extrude_face(const noob::active_mesh::face_h, float magnitude);
			
			// void connect_faces(noob::active_mesh::face_h first, noob::active_mesh::face_h second);
			
			void move_vertex(const noob::active_mesh::vertex_h, const noob::vec3&);
			
			void move_edge(const noob::active_mesh::edge_h, const noob::vec3&);

			void move_halfedge(const noob::active_mesh::halfedge_h, const noob::vec3&);

			void move_face(const noob::active_mesh::face_h, const noob::vec3&);

			void move_vertices(const std::vector<noob::active_mesh::vertex_h>&, const noob::vec3&);
			
			void move_edges(const std::vector<noob::active_mesh::edge_h>&, const noob::vec3&);

			void move_halfedges(const std::vector<noob::active_mesh::halfedge_h>&, const noob::vec3&);

			void move_faces(const std::vector<noob::active_mesh::face_h>&, const noob::vec3&);
			
			void merge_adjacent_coplanars();
			
			// This function is static because it acts on two objects. The static modifier makes it explicit.
			// static void join_meshes_at_face(const noob::active_mesh& first_mesh, const noob::active_mesh::face_h first_handle, const noob::active_mesh& second, const noob::active_mesh::face_h second_handle);
			
		protected:
			PolyMesh half_edges;
			std::map<std::array<float, 3>, noob::active_mesh::vertex_h> xyz_to_vhandles;

	};
}
