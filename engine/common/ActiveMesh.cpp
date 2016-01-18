#include "ActiveMesh.hpp"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Dualizer/meshDualT.hh>

void noob::active_mesh::init(const noob::basic_mesh&) {}
void noob::active_mesh::init(const noob::indexed_polymesh&) {}

// Basic functionality
noob::active_mesh::face_handle noob::active_mesh::add_face(const noob::active_mesh::face&) {}
noob::active_mesh::face_handle noob::active_mesh::add_face(const std::vector<std::array<float, 3>>&) {}
			
bool noob::active_mesh::exists(const noob::vec3&) const {}
bool noob::active_mesh::exists(const face&) const {}
bool noob::active_mesh::exists(face_handle) const {}
		
noob::vec3 noob::active_mesh::get_vertex(uint32_t index) const {}
std::vector<uint32_t> noob::active_mesh::get_face_indices(face_handle) const {}
noob::active_mesh::face noob::active_mesh::get_face_values(face_handle) const {}
					
noob::basic_mesh noob::active_mesh::to_basic_mesh() const {}
noob::indexed_polymesh noob::active_mesh::to_indexed_polymesh() const {}

std::vector<noob::active_mesh::face_handle> noob::active_mesh::get_adjacent_faces(noob::active_mesh::face_handle) const {}

// Generation-related utilities (meshes change and we sometimes/often need to retrieve older copies)
size_t noob::active_mesh::get_current_generation() const {}
noob::active_mesh noob::active_mesh::retrieve_by_generation(size_t) const {}
bool noob::active_mesh::is_generation_current(size_t) const {}
		
// TODO: Find out if "topological split" makes proper sense in this context
std::vector<noob::active_mesh> noob::active_mesh::topological_split(const std::vector<noob::active_mesh::face_handle>&) const {}

// Destructive utiiities. Those take full advantage of the speed benefits of half-edged meshes
void noob::active_mesh::make_hole(noob::active_mesh::face_handle) {}
void noob::active_mesh::fill_holes() {}
void noob::active_mesh::cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal) {}
void noob::active_mesh::cut_faces(std::vector<noob::active_mesh::face_handle>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal) {}
void noob::active_mesh::extrude(noob::active_mesh::face_handle, const noob::vec3& normal, float magnitude) {}
void noob::active_mesh::connect_faces(noob::active_mesh::face_handle first_handle, noob::active_mesh::face_handle second_handle) {}
void noob::active_mesh::move_vertex(uint32_t index, const noob::vec3& direction) {}
void noob::active_mesh::move_vertices(const std::vector<uint32_t>& indices, const noob::vec3& direction) {}
