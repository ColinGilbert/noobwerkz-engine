#include "ActiveMesh.hpp"

// #include <OpenMesh/Core/IO/MeshIO.hh>
// #include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include <OpenMesh/Core/Utils/vector_cast.hh>
#include <OpenMesh/Tools/Utils/getopt.h>
#include <OpenMesh/Tools/Utils/Timer.hh>
#include <OpenMesh/Tools/Dualizer/meshDualT.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModAspectRatioT.hh>
#include <OpenMesh/Tools/Decimater/ModEdgeLengthT.hh>
#include <OpenMesh/Tools/Decimater/ModHausdorffT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalDeviationT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Tools/Decimater/ModProgMeshT.hh>
#include <OpenMesh/Tools/Decimater/ModIndependentSetsT.hh>
#include <OpenMesh/Tools/Decimater/ModRoundnessT.hh>



// struct face
// {
// std::vector<noob::vec3> verts;
// noob::vec3 normal;
// bool hole;
// };


void noob::active_mesh::init(const noob::basic_mesh& m)
{
	// TriMesh::VertexIter v_it, v_end(m.half_edges.vertices_end());
	// std::vector<TriMesh::VertexHandle> vhandles;
	// for (v_it = m.half_edges.vertices_begin(); v_it != v_end; v_it += 3)
	// {
	// vhandles.clear();
	// vhandles.push_back(m.half_edges.vertex(v_it));
	// vhandles.push_back(m.half_edges.vertex(v_it+1));
	// vhandles.push_back(m.half_edges.vertex(v_it+2));
	// half_edges.add_face();
	// }
	// auto results = vertex_value_to_vertex_ptr.find();

}


// void noob::active_mesh::init(const noob::indexed_polymesh& m) 
// {

// }


// Basic functionality
/*
   size_t noob::active_mesh::add_face(const noob::active_mesh::face&) 
   {

   }
   */

size_t noob::active_mesh::add_face(const std::vector<size_t>&) 
{

}


bool noob::active_mesh::vertex_exists(const noob::vec3&) const 
{

}


bool noob::active_mesh::vertex_exists(size_t) const 
{

}


bool noob::active_mesh::face_exists(const face&) const 
{

}


bool noob::active_mesh::face_exists(size_t) const 
{

}


noob::vec3 noob::active_mesh::get_vertex(size_t index) const 
{

}


std::vector<size_t> noob::active_mesh::get_verts_for_face(size_t) const 
{

}


noob::active_mesh::face noob::active_mesh::get_face(size_t) const 
{

}


std::vector<noob::active_mesh::face> noob::active_mesh::get_face_list() const
{

}


noob::basic_mesh noob::active_mesh::to_basic_mesh() const 
{

}


std::vector<size_t> noob::active_mesh::get_adjacent_faces(size_t) const 
{

}


// Generation-related utilities (meshes change and we sometimes/often need to retrieve older copies)
/*
   size_t noob::active_mesh::get_current_generation() const 
   {

   }


   noob::active_mesh noob::active_mesh::retrieve_by_generation(size_t) const 
   {

   }


   bool noob::active_mesh::is_generation_current(size_t) const 
   {

   }
   */

// TODO: Find out if "topological split" makes proper sense in this context
std::vector<noob::active_mesh> noob::active_mesh::topological_split(const std::vector<size_t>& sampling_points, size_t max_vertices) const 
{

}


// Destructive utiiities. Those take full advantage of the speed benefits of half-edged meshes
void noob::active_mesh::make_hole(size_t) 
{

}


void noob::active_mesh::fill_holes() 
{

}


void noob::active_mesh::cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal) 
{

}


void noob::active_mesh::cut_faces(std::vector<size_t>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal) 
{

}


void noob::active_mesh::extrude(size_t, const noob::vec3& normal, float magnitude) 
{

}


void noob::active_mesh::connect_faces(size_t first_handle, size_t second_handle) 
{

}


void noob::active_mesh::move_vertex(size_t index, const noob::vec3& direction) 
{

}


void noob::active_mesh::move_vertices(const std::vector<size_t>& indices, const noob::vec3& direction) 
{

}


void noob::active_mesh::merge_adjacent_coplanars()
{

}


// void noob::active_mesh::refresh_index() 
// {

// }


// std::tuple<bool, PolyMesh::Vertex*> noob::active_mesh::get_vertex_ptr(size_t) const 
// {

// }
