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
	half_edges.clear();
	xyz_to_vhandles.empty();
	
	for (size_t i = 0; i < m.indices.size(); ++i)
	{
		
	}

// TriMesh tri_edges = m.to_half_edges():
// TriMesh::VertexIter v_it, v_end(tri_edges.vertices_end());
	
// std::vector<TriMesh::VertexHandle> vhandles;
	
// for (v_it = tri_edges.vertices_begin(); v_it != v_end; v_it += 3)
// {
//      vhandles.clear();
//      vhandles.push_back(m.tri_edges.vertex(v_it));
//      vhandles.push_back(m.tri_edges.vertex(v_it+1));
//      vhandles.push_back(m.tri_edges.vertex(v_it+2));
//      half_edges.add_face();
//      xyz_to_vhandle;
//      xyz_to_vhandle();
//      xyz_to_vhandle();
// }

// auto results = vertex_value_to_vertex_ptr.find();



}


// void noob::active_mesh::init(const noob::indexed_polymesh& m) 
// {

// }


// Basic functionality
//

PolyMesh::VertexHandle noob::active_mesh::add_vertex(const noob::vec3& v)
{
	auto search = xyz_to_vhandles.find(v.v);
	if (search == xyz_to_vhandles.end())
	{
		xyz_to_vhandles.insert(std::make_pair(v.v, half_edges.add_vertex(PolyMesh::Point(v.v[0], v.v[1], v.v[2]))));
	}
	PolyMesh::VertexHandle temp = xyz_to_vhandles[v.v];
	return temp;
}

PolyMesh::FaceHandle noob::active_mesh::add_face(const noob::active_mesh::face&) 
{

}


PolyMesh::FaceHandle noob::active_mesh::add_face(const std::vector<PolyMesh::VertexHandle>&) 
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


bool noob::active_mesh::face_exists(PolyMesh::FaceHandle h) const 
{

}


noob::vec3 noob::active_mesh::get_vertex(PolyMesh::VertexHandle h) const 
{

}


std::vector<PolyMesh::VertexHandle> noob::active_mesh::get_verts_for_face(PolyMesh::FaceHandle) const 
{

}


noob::active_mesh::face noob::active_mesh::get_face(PolyMesh::FaceHandle) const 
{

}


std::vector<noob::active_mesh::face> noob::active_mesh::get_face_list() const
{

}


noob::basic_mesh noob::active_mesh::to_basic_mesh() const 
{

}


std::vector<PolyMesh::FaceHandle> noob::active_mesh::get_adjacent_faces(PolyMesh::FaceHandle) const 
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
std::vector<noob::active_mesh> noob::active_mesh::topological_split(const std::vector<PolyMesh::FaceHandle>& sampling_points, size_t max_vertices) const 
{

}


// Destructive utiiities. Those take full advantage of the speed benefits of half-edged meshes
void noob::active_mesh::make_hole(PolyMesh::FaceHandle) 
{

}


void noob::active_mesh::fill_holes() 
{

}


void noob::active_mesh::cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal) 
{

}


void noob::active_mesh::cut_faces(std::vector<PolyMesh::FaceHandle>&, const noob::vec3& point_on_plane, const noob::vec3& plane_normal) 
{

}


void noob::active_mesh::extrude(PolyMesh::FaceHandle, const noob::vec3& normal, float magnitude) 
{

}


void noob::active_mesh::connect_faces(PolyMesh::FaceHandle first_handle, PolyMesh::FaceHandle second_handle) 
{

}


void noob::active_mesh::move_vertex(PolyMesh::VertexHandle index, const noob::vec3& direction) 
{

}


void noob::active_mesh::move_vertices(const std::vector<PolyMesh::VertexHandle>& indices, const noob::vec3& direction) 
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
