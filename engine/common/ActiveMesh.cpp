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
#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>


bool noob::active_mesh::vertex_exists(const noob::vec3& v) const 
{
	auto search = xyz_to_vhandles.find(v.v);
	if (search == xyz_to_vhandles.end())
	{
		return false;
	}
	return true;
}


bool noob::active_mesh::vertex_exists(const noob::active_mesh::vertex_h h) const 
{
	return half_edges.is_valid_handle(h);
}


noob::vec3 noob::active_mesh::get_face_normal(const noob::active_mesh::face_h fh) const
{

	std::array<noob::vec3, 3> tri;
	size_t count = 0;
	for (auto it = half_edges.cfv_iter(fh); it.is_valid() && count < 3; ++it, ++it)
	{
		tri[count] = noob::vec3(half_edges.point(*it));
	}

	return noob::get_normal(tri);
}


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

		std::vector<noob::active_mesh::vertex_h> vhandles_to_test;

		for (noob::vec3 v : verts)
		{
			vhandles_to_test.push_back(xyz_to_vhandles[v.v]);
		}

		// Now we must test all verts in faces with the first vertex to see if one of them matches.
		for (PolyMesh::VertexFaceIter vf_it = half_edges.cvf_iter(get_vertex_handle(verts[0])); vf_it.is_valid(); ++vf_it)
		{
			// int iteration = -1;
			size_t iteration = 0;
			for (PolyMesh::FaceVertexIter fv_it = half_edges.cfv_iter(*vf_it); fv_it.is_valid(); ++fv_it)
			{
				// ++iteration;
				// if (static_cast<size_t>(iteration) > verts.size()) break;
				// if (vhandles_to_test[static_cast<size_t>(iteration)] != *fv_it) break;
				// if (vhandles_to_test[static_cast<size_t>(iteration)] == *fv_it && static_cast<size_t>(iteration) == verts.size()) return true;
				if (iteration > verts.size() - 1) break;
				if (vhandles_to_test[iteration] != *fv_it) break;
				if (vhandles_to_test[iteration] == *fv_it && iteration == verts.size() - 1) return true;
				++iteration;
			}
		}
	}
	return false;
}


bool noob::active_mesh::face_exists(const noob::active_mesh::face_h h) const 
{
	return half_edges.is_valid_handle(h);
}


std::tuple<bool, noob::vec3> noob::active_mesh::get_vertex(const noob::active_mesh::vertex_h h) const 
{
	if (vertex_exists(h))
	{
		auto p = half_edges.point(h);
		return std::make_tuple(true, noob::vec3(p[0], p[1], p[2]));
	}
	return std::make_tuple(false, noob::vec3(0.0, 0.0, 0.0));
}


noob::active_mesh::vertex_h noob::active_mesh::get_vertex_handle(const noob::vec3& v) const
{
	if (!vertex_exists(v)) return noob::active_mesh::vertex_h(-1);
	else return xyz_to_vhandles[v.v];
}


std::vector<noob::active_mesh::vertex_h> noob::active_mesh::get_vertex_handles_for_face(const noob::active_mesh::face_h h) const 
{
	std::vector<noob::active_mesh::vertex_h> vert_handles;
	for (auto vit = half_edges.cfv_iter(h); vit.is_valid(); ++vit)
	{
		vert_handles.push_back(*vit);
	}
	return vert_handles;
}


std::vector<noob::vec3> noob::active_mesh::get_verts_for_face(const noob::active_mesh::face_h h) const
{
	std::vector<noob::vec3> verts;
	for (auto vit = half_edges.cfv_iter(h); vit.is_valid(); ++vit)
	{
		verts.push_back(half_edges.point(*vit));
	}
	return verts;
}


std::tuple<bool, std::vector<noob::vec3>> noob::active_mesh::get_face(const noob::active_mesh::face_h h) const 
{
	std::vector<noob::vec3> results;
	if (face_exists(h))
	{
		results = get_verts_for_face(h);
		return std::make_tuple(true, results);
	}
	return std::make_tuple(false, results);
}


size_t noob::active_mesh::num_vertices() const
{
	return half_edges.n_vertices();
}


size_t noob::active_mesh::num_half_edges() const
{
	return half_edges.n_halfedges();
}


size_t noob::active_mesh::num_edges() const
{
	return half_edges.n_edges();

}


size_t noob::active_mesh::num_faces() const
{
	return half_edges.n_faces();

}


std::vector<noob::active_mesh::edge_h> noob::active_mesh::get_adjacent_edges(const noob::active_mesh::face_h first_face, const noob::active_mesh::face_h second_face) const
{
	std::vector<noob::active_mesh::edge_h> results;
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


noob::basic_mesh noob::active_mesh::to_basic_mesh() const 
{
	noob::basic_mesh m;
	m.from_half_edges(half_edges);
	return m;
}


std::vector<noob::active_mesh::face_h> noob::active_mesh::get_adjacent_faces(const noob::active_mesh::face_h fh) const 
{
	std::vector<noob::active_mesh::face_h> face_handles;
	for (auto it = half_edges.cff_iter(fh); it.is_valid(); ++it)
	{
		face_handles.push_back(*it);
	}
	return face_handles;
}


std::vector<noob::active_mesh> noob::active_mesh::split(size_t max_vertices) const 
{
	std::vector<noob::active_mesh> results;

	noob::active_mesh output_mesh;

	for (auto outer_it = half_edges.faces_begin(); outer_it != half_edges.faces_end(); ++outer_it)
	{
		if (output_mesh.num_vertices() < max_vertices)
		{
			std::vector<noob::active_mesh::vertex_h> verts_list;

			for (auto inner_it = half_edges.cfv_iter(*outer_it); inner_it.is_valid(); ++inner_it)
			{
				verts_list.push_back(output_mesh.add_vertex(half_edges.point(*inner_it)));
			}

			if ((verts_list.size() + output_mesh.num_vertices()) > max_vertices)
			{
				output_mesh.add_face(verts_list);
			}
			else
			{
				results.push_back(output_mesh);
				output_mesh.reset();
				output_mesh.add_face(verts_list);
			}
		}
	}

	if (output_mesh.num_vertices() > 0)
	{
		results.push_back(output_mesh);
	}

	return results;
}


// -----------------------
// Destructive utiiities.
// -----------------------


void noob::active_mesh::reset()
{
	half_edges.clear();
	xyz_to_vhandles.empty();
}


void noob::active_mesh::from_basic_mesh(const noob::basic_mesh& m)
{
	reset();

	std::array<noob::vec3, 3> faces;
	for (size_t i = 0; i < m.indices.size(); i += 3)
	{
		faces[0] = m.vertices[i];
		faces[1] = m.vertices[i+1];
		faces[2] = m.vertices[i+2];
		add_face(faces);
	}
}


noob::active_mesh::vertex_h noob::active_mesh::add_vertex(const noob::vec3& v)
{
	auto search = xyz_to_vhandles.find(v.v);
	if (search == xyz_to_vhandles.end())
	{
		xyz_to_vhandles.insert(std::make_pair(v.v, half_edges.add_vertex(PolyMesh::Point(v.v[0], v.v[1], v.v[2]))));
	}
	return xyz_to_vhandles[v.v];
	// return temp;
}


noob::active_mesh::face_h noob::active_mesh::add_face(const std::vector<noob::active_mesh::vertex_h>& vhandles) 
{
	return half_edges.add_face(vhandles);
}


void noob::active_mesh::make_hole(const noob::active_mesh::face_h fh) 
{
	half_edges.delete_face(fh);
	half_edges.garbage_collection();
}


void noob::active_mesh::fill_holes() 
{
	std::vector<noob::active_mesh::vertex_h> face_verts;
	for (auto h_it = half_edges.halfedges_begin(); h_it != half_edges.halfedges_end(); ++h_it)
	{
		if (half_edges.is_boundary(*h_it))
		{
			std::vector<noob::active_mesh::vertex_h> verts;
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


// void noob::active_mesh::cut_mesh(const noob::vec3& point_on_plane, const noob::vec3 plane_normal) 
// {
// }


// void noob::active_mesh::cut_faces(std::vector<noob::active_mesh::face_h>& face_handles, const noob::vec3& point_on_plane, const noob::vec3& plane_normal) 
// {
// }


void noob::active_mesh::extrude_face(const noob::active_mesh::face_h fh, float magnitude) 
{
	std::vector<noob::active_mesh::vertex_h> original_points;
	for (auto it = half_edges.cfv_iter(fh); it.is_valid(); ++it)
	{
		original_points.push_back(*it);
	}

	// Multiply the normal by the magnitude.
	noob::vec3 weighted_normal = get_face_normal(fh) * magnitude;

	std::vector<noob::active_mesh::vertex_h> extruded_points;

	// Calculate the extruded points and add them to the mesh
	for (noob::active_mesh::vertex_h h : original_points)
	{
		noob::vec3 p(half_edges.point(h));
		extruded_points.push_back(add_vertex(p + weighted_normal));
	}

	// Generate the side faces.
	for (size_t i = 0; i < original_points.size(); i += 2)
	{
		std::vector<noob::active_mesh::vertex_h> side_face_verts;
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


//   void noob::active_mesh::connect_faces(noob::active_mesh::face_h first_handle, noob::active_mesh::face_h second_handle) 
//   {

//   }


void noob::active_mesh::move_vertex(const noob::active_mesh::vertex_h vh, const noob::vec3& direction) 
{
	half_edges.set_point(vh, half_edges.point(vh) + PolyMesh::Point(direction[0], direction[1], direction[2]));
}


void noob::active_mesh::move_vertices(const std::vector<noob::active_mesh::vertex_h>& handles, const noob::vec3& direction) 
{
	for (noob::active_mesh::vertex_h vh : handles)
	{
		half_edges.set_point(vh, half_edges.point(vh) + PolyMesh::Point(direction[0], direction[1], direction[2]));
	}
}


void noob::active_mesh::merge_adjacent_coplanars()
{
	size_t removed_edges = 0;
	for (auto f_it = half_edges.faces_sbegin(); f_it != half_edges.faces_end(); ++f_it)
	{
		noob::active_mesh::face_h first_face = *f_it;
		noob::vec3 first_normal = get_face_normal(first_face); // TODO: Find out why the following doesn't compile: half_edges.face_handle(*f_it));
		for (auto ff_it = half_edges.ff_iter(*f_it); ff_it.is_valid(); ++ff_it)
		{
			noob::active_mesh::face_h second_face = *ff_it;
			noob::vec3 second_normal = get_face_normal(second_face);

			if (first_normal == second_normal)
			{
				// Find adjacent edge(s). TODO: Move out of this and keep it around as function
				std::vector<noob::active_mesh::edge_h> edges = get_adjacent_edges(*f_it, *ff_it);
				for (noob::active_mesh::edge_h e : edges)
				{
					half_edges.remove_edge(e);
					removed_edges++;
				}
			}
		}
	}
	fmt::MemoryWriter ww;
	ww << "[ActiveMesh] merge_adjacent_coplanars(), Edges removed = " << removed_edges;
	logger::log(ww.str());

	half_edges.garbage_collection();
}
