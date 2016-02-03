#include "ActiveMesh.hpp"
#include <algorithm>
#include <set>
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


void noob::active_mesh::init(const noob::basic_mesh& m)
{
	half_edges.clear();
	xyz_to_vhandles.empty();

	std::array<noob::vec3, 3> faces;
	for (size_t i = 0; i < m.indices.size(); i += 3)
	{
		faces[0] = m.vertices[i];
		faces[1] = m.vertices[i+1];
		faces[2] = m.vertices[i+2];
		add_face(faces);
	}
}


PolyMesh::VertexHandle noob::active_mesh::add_vertex(const noob::vec3& v)
{
	auto search = xyz_to_vhandles.find(v.v);
	if (search == xyz_to_vhandles.end())
	{
		xyz_to_vhandles.insert(std::make_pair(v.v, half_edges.add_vertex(PolyMesh::Point(v.v[0], v.v[1], v.v[2]))));
	}
	return xyz_to_vhandles[v.v];
	//return temp;
}


PolyMesh::FaceHandle noob::active_mesh::add_face(const std::array<noob::vec3, 3>& verts)
{
	std::vector<PolyMesh::VertexHandle> vhandles;
	vhandles.reserve(3);
	for(noob::vec3 v : verts)
	{
		vhandles.emplace_back(add_vertex(v));
	}
	return add_face(vhandles);
}


PolyMesh::FaceHandle noob::active_mesh::add_face(const std::array<noob::vec3, 4>& verts)
{
	std::vector<PolyMesh::VertexHandle> vhandles;
	vhandles.reserve(4);
	for(noob::vec3 v : verts)
	{
		vhandles.emplace_back(add_vertex(v));
	}
	return add_face(vhandles);
}


PolyMesh::FaceHandle noob::active_mesh::add_face(const std::vector<noob::vec3>& verts) 
{
	std::vector<PolyMesh::VertexHandle> vhandles;

	for(noob::vec3 v : verts)
	{
		vhandles.push_back(add_vertex(v));
	}
	return add_face(vhandles);
}


PolyMesh::FaceHandle noob::active_mesh::add_face(const std::vector<PolyMesh::VertexHandle>& vhandles) 
{
	return half_edges.add_face(vhandles);
}


bool noob::active_mesh::vertex_exists(const noob::vec3& v) const 
{
	auto search = xyz_to_vhandles.find(v.v);
	if (search == xyz_to_vhandles.end())
	{
		return false;
	}
	return true;
}


bool noob::active_mesh::vertex_exists(PolyMesh::VertexHandle h) const 
{
	return half_edges.is_valid_handle(h);
}


noob::vec3 noob::active_mesh::get_face_normal(PolyMesh::FaceHandle& fh) const
{

	std::array<noob::vec3, 3> tri;
	size_t count = 0;
	for (auto it = half_edges.cfv_iter(fh); it.is_valid() && count < 3; ++it, ++it)
	{
		tri[count] = noob::vec3(half_edges.point(*it));
	}

	return noob::get_normal(tri);
}


/*

   bool noob::active_mesh::face_exists(const std::array<noob::vec3, 3>& verts) const
   {

   }


   bool noob::active_mesh::face_exists(const std::array<noob::vec3, 4>& verts) const
   {

   }
   */


bool noob::active_mesh::face_exists(const std::vector<noob::vec3>& verts) const 
{
	// First the base case
	if (!verts.empty())
	{
		// Then if one of the verts is invalid, we can shortcut this away.
		for (noob::vec3 v : verts)
		{
			if (!vertex_exists(v))
			{
				return false;
			}
		}

		std::vector<PolyMesh::VertexHandle> vhandles_to_test;

		for (noob::vec3 v : verts)
		{
			vhandles_to_test.push_back(xyz_to_vhandles[v.v]);
		}

		// Now we must test all verts in faces with the first vertex to see if one of them matches.
		for (PolyMesh::VertexFaceIter vf_it = half_edges.cvf_iter(get_vertex_handle(verts[0])); vf_it.is_valid(); ++vf_it)
		{
			int iteration = -1;
			for (PolyMesh::FaceVertexIter fv_it = half_edges.cfv_iter(*vf_it); fv_it.is_valid(); ++fv_it)
			{
				++iteration;
				if (static_cast<size_t>(iteration) > verts.size()) break;
				if (vhandles_to_test[static_cast<size_t>(iteration)] != *fv_it) break;
				if (vhandles_to_test[static_cast<size_t>(iteration)] == *fv_it && static_cast<size_t>(iteration) == verts.size()) return true;
			}
		}
	}
	return false;
}


bool noob::active_mesh::face_exists(PolyMesh::FaceHandle h) const 
{
	return half_edges.is_valid_handle(h);
}


std::tuple<bool, noob::vec3> noob::active_mesh::get_vertex(PolyMesh::VertexHandle h) const 
{
	if (vertex_exists(h))
	{
		auto p = half_edges.point(h);
		return std::make_tuple(true, noob::vec3(p[0], p[1], p[2]));
	}
	return std::make_tuple(false, noob::vec3(0.0, 0.0, 0.0));
}


PolyMesh::VertexHandle noob::active_mesh::get_vertex_handle(const noob::vec3& v) const
{
	if (!vertex_exists(v)) return PolyMesh::VertexHandle(-1);
	else return xyz_to_vhandles[v.v];
}


std::vector<PolyMesh::VertexHandle> noob::active_mesh::get_vertex_handles_for_face(PolyMesh::FaceHandle h) const 
{
	std::vector<PolyMesh::VertexHandle> vert_handles;
	for (auto vit = half_edges.cfv_iter(h); vit.is_valid(); ++vit)
	{
		vert_handles.push_back(*vit);
	}
	return vert_handles;
}


std::vector<noob::vec3> noob::active_mesh::get_verts_for_face(PolyMesh::FaceHandle h) const
{
	std::vector<noob::vec3> verts;
	for (auto vit = half_edges.cfv_iter(h); vit.is_valid(); ++vit)
	{
		verts.push_back(half_edges.point(*vit));
	}
	return verts;
}


std::tuple<bool, std::vector<noob::vec3>> noob::active_mesh::get_face(PolyMesh::FaceHandle h) const 
{
	std::vector<noob::vec3> results;
	if (face_exists(h))
	{
		results = get_verts_for_face(h);
		return std::make_tuple(true, results);
	}
	return std::make_tuple(false, results);
}


std::vector<PolyMesh::EdgeHandle> noob::active_mesh::get_adjacent_edges(PolyMesh::FaceHandle first_face, PolyMesh::FaceHandle second_face) const
{
	std::vector<PolyMesh::EdgeHandle> results;
	for (auto first_iter = half_edges.fe_iter(first_face); first_iter.is_valid(); ++first_iter)
	{
		for (auto second_iter = half_edges.fe_iter(second_face); second_iter.is_valid(); ++second_iter)
		{
			if (first_iter == second_iter)
			{
				results.push_back(*first_iter);
			}
		}
	}
	return results;
}



/*
   std::vector<std::vector<noob::vec3>> noob::active_mesh::get_all_faces() const
   {

   }
   */


noob::basic_mesh noob::active_mesh::to_basic_mesh() const 
{
	noob::basic_mesh m;
	m.from_half_edges(half_edges);
	return m;
}


std::vector<PolyMesh::FaceHandle> noob::active_mesh::get_adjacent_faces(PolyMesh::FaceHandle fh) const 
{
	std::vector<PolyMesh::FaceHandle> face_handles;
	for (auto it = half_edges.cff_iter(fh); it.is_valid(); ++it)
	{
		face_handles.push_back(*it);
	}
	return face_handles;
}


// TODO: Find out if "topological split" makes proper sense in this context
std::vector<noob::active_mesh> noob::active_mesh::topological_split(size_t max_vertices) const 
{
	std::vector<PolyMesh::FaceHandle> taken_list;
	std::vector<noob::active_mesh> results;


	return results;
}


// Destructive utiiities.
void noob::active_mesh::make_hole(PolyMesh::FaceHandle fh) 
{
	half_edges.delete_face(fh);
	half_edges.garbage_collection();
}


void noob::active_mesh::fill_holes() 
{
	std::vector<PolyMesh::VertexHandle> face_verts;
	for (auto h_it = half_edges.halfedges_begin(); h_it != half_edges.halfedges_end(); ++h_it)
	{
		if (half_edges.is_boundary(*h_it))
		{
			std::vector<PolyMesh::VertexHandle> verts;
			verts.push_back(half_edges.from_vertex_handle(*h_it));

			// TODO: See if reversing is necessary
			std::reverse(verts.begin(), verts.end());

			for (auto loop_it = half_edges.hl_begin(*h_it); loop_it != half_edges.hl_end(*h_it); ++loop_it)
			{
				verts.push_back(half_edges.to_vertex_handle(*loop_it));
			}

			half_edges.add_face(verts);
		}
	}
}


/*
   void noob::active_mesh::cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal) 
   {

   }


   void noob::active_mesh::cut_faces(std::vector<PolyMesh::FaceHandle>& face_handles, const noob::vec3& point_on_plane, const noob::vec3& plane_normal) 
   {

   }
   */


void noob::active_mesh::extrude(PolyMesh::FaceHandle fh, float magnitude) 
{
	std::vector<PolyMesh::VertexHandle> original_points;
	for (auto it = half_edges.cfv_iter(fh); it.is_valid(); ++it)
	{
		original_points.push_back(*it);
	}

	// Multiply the normal by the magnitude.
	noob::vec3 weighted_normal = get_face_normal(fh) * magnitude;

	std::vector<PolyMesh::VertexHandle> extruded_points;

	// Calculate the extruded points and add them to the mesh
	for (PolyMesh::VertexHandle h : original_points)
	{
		noob::vec3 p(half_edges.point(h));
		extruded_points.push_back(add_vertex(p + weighted_normal));
	}

	// Generate the side faces.
	for (size_t i = 0; i < original_points.size(); i += 2)
	{
		std::vector<PolyMesh::VertexHandle> side_face_verts;
		side_face_verts.reserve(4);
		side_face_verts[0] = original_points[i];
		side_face_verts[1] = original_points[i+1];
		side_face_verts[2] = extruded_points[i+1];
		side_face_verts[3] = extruded_points[i];
		add_face(side_face_verts);
	}

	// Add the face to cap off the extrusion
	add_face(extruded_points);

	// Now, delete the original face and garbage collect
	half_edges.delete_face(fh);
	half_edges.garbage_collection();
}


//   void noob::active_mesh::connect_faces(PolyMesh::FaceHandle first_handle, PolyMesh::FaceHandle second_handle) 
//   {

//   }


void noob::active_mesh::move_vertex(PolyMesh::VertexHandle vh, const noob::vec3& direction) 
{
	half_edges.set_point(vh, half_edges.point(vh) + PolyMesh::Point(direction[0], direction[1], direction[2]));
}


void noob::active_mesh::move_vertices(const std::vector<PolyMesh::VertexHandle>& handles, const noob::vec3& direction) 
{
	for (PolyMesh::VertexHandle vh : handles)
	{
		half_edges.set_point(vh, half_edges.point(vh) + PolyMesh::Point(direction[0], direction[1], direction[2]));
	}
}


void noob::active_mesh::merge_adjacent_coplanars()
{
	for (auto f_it = half_edges.faces_sbegin(); f_it != half_edges.faces_end(); ++f_it)
	{
		PolyMesh::FaceHandle first_face = *f_it;
		noob::vec3 first_normal = get_face_normal(first_face); // TODO: Find out why the following doesn't compile: half_edges.face_handle(*f_it));
		for (auto ff_it = half_edges.ff_iter(*f_it); ff_it.is_valid(); ++ff_it)
		{
			PolyMesh::FaceHandle second_face = *ff_it;
			noob::vec3 second_normal = get_face_normal(second_face);

			if (first_normal == second_normal)
			{
				// Find adjacent edge(s). TODO: Move out of this and keep it around as function
				std::vector<PolyMesh::EdgeHandle> edges = get_adjacent_edges(*f_it, *ff_it);
				for (PolyMesh::EdgeHandle e : edges)
				{
					half_edges.remove_edge(e);
				}
			}
		}
	}

	half_edges.garbage_collection();
}


std::vector<PolyMesh::FaceHandle> noob::active_mesh::topo_split_helper(const PolyMesh::FaceHandle sample, std::set<PolyMesh::FaceHandle>& taken, size_t max_vertices) const
{
	std::set<PolyMesh::FaceHandle> working_results;
	std::vector<PolyMesh::FaceHandle> results;
	std::vector<PolyMesh::FaceHandle> adjacents = get_adjacent_faces(sample);

	for (PolyMesh::FaceHandle adj : adjacents)
	{
		if (std::find(taken.begin(), taken.end(), adj) == taken.end())
		{
			if (working_results.size() < max_vertices)
			{
				working_results.insert(adj);
				taken.insert(adj);
			}
			else
			{
				std::vector<PolyMesh::FaceHandle> v(working_results.begin(), working_results.end());
				results = v;
			}
		}
	}
	return results;
}
