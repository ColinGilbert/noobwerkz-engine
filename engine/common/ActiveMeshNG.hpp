

namespace noob
{
	class active_mesh
	{
		public:
			noob::component<uint32_t> vertex_handles_holder;
			noob::component<uint32_t> face_handles_holder;
			noob::component<uint32_t> half_edge_handles_holder;
			
			vertex_handles_holder::handle vert_h;
			face_handles_holder::handle face_h;
			half_edge_handles_holder::handle halfedge_h;

			bool vertex_exists(float, float, float) const;
			bool vertex_exists(const noob::active_mesh::vert_h) const;

			std::tuple<bool, float, float, float> get_vertex_pos(noob::active_mesh::vert_h) const;
			
			std::tuple<bool, float, float, float> get_face_normal(noob::active_mesh::face_h) const;

			std::tuple<bool, noob::active_mesh::vert_h> get_vertex_handle(float, float, float) const;

			// Caution: This function has the potential to do lots of iterations, as it tests against all faces sharing the first vertex
			bool face_exists(const std::vector<std::tuple<float, float, float>>&) const;

			bool face_exists(noob::active_mesh::face_handle) const;
			
			// noob::active_mesh::face_h get_face_handle(const noob::vec3&) const;

			const std::tuple<bool, std::vector<noob::active_mesh::face_h> get_faces_with_vert(noob::active_mesh::vert_h) const; 

			uint32_t num_vertices() const;

			uint32_t num_half_edges() const;

			uint32_t num_edges() const;

			uint32_t num_faces() const;

			std::vector<noob::active_mesh::vert_h> get_verts_in_face(const noob::active_mesh::face_h) const;

			std::vector<noob::active_mesh::halfedge_h> get_adjacent_halfedges(const noob::active_mesh::face_h, const noob::active_mesh::face_h) const;

			noob::basic_mesh to_basic_mesh() const;
			
			std::vector<noob::active_mesh::face_h> get_adjacent_faces(const noob::active_mesh::face_h) const;

			// Implemented as an iterator that copies verts + faces until certain size is reached, then creates new one. Repeats until exhaustion.
			std::vector<noob::active_mesh> split(size_t max_vertices) const;

			// -----------------------
			// Destructive utiiities.
			// -----------------------

			void reset();
			
			void from_basic_mesh(const noob::basic_mesh&);

			noob::active_mesh::vert_h add_vertex(const std::tuple<float, float, float>&);

			noob::active_mesh::face_h add_face(const std::array<noob::active_mesh::vert_h, 3>&);
			
			noob::active_mesh::face_h add_face(const std::array<noob::active_mesh::vert_h, 4>&);
			
			noob::active_mesh::face_h add_face(const std::vector<noob::active_mesh::vert_h>&);
			
			void make_hole(const noob::active_mesh::face_h);
			
			void fill_holes();
			
			// TODO: Before implementing the next two methods, come up with a proper plane representation (probably using Eigen or Geometric Tools.)
			// void cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal);
			// void cut_faces(std::vector<noob::active_mesh::face_h>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal);
			
			void extrude_face(const noob::active_mesh::face_h, float magnitude);
			
			// void connect_faces(noob::active_mesh::face_h first, noob::active_mesh::face_h second);
			
			void move_vertex(const noob::active_mesh::vert_h, const std::tuple<float, float, float>& direction);
			
			void move_vertices(const std::vector<noob::active_mesh::vert_h>&, const std::tuple<float, float, float>& direction);
			
			void merge_adjacent_coplanars();
			
			// This function is static because it acts on two objects. The static modifier makes it explicit.
			// static void join_meshes_at_face(const noob::active_mesh& first_mesh, const noob::active_mesh::face_h first_handle, const noob::active_mesh& second, const noob::active_mesh::face_h second_handle);
			
			
		protected:
			
			// PolyMesh half_edges;
			rde::hash_map<rde::string, uint32_t> xyz_to_vert_h;
			rde::sorted_vector<rde::tuple<uint32_t, uint32_t>> half_edges;
	};
}
