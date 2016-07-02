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
	for (auto it = half_edges.cfv_iter(fh); it.is_valid() && count < 3; ++it)
	{
		tri[count] = noob::vec3_from_polymesh(half_edges.point(*it));
	}

	return noob::get_normal(tri);
}


std::vector<noob::active_mesh::face_h> noob::active_mesh::get_faces_with_vert(const noob::active_mesh::vertex_h vh) const
{
	std::vector<noob::active_mesh::face_h> results;
	for (auto it = half_edges.cvf_iter(vh); it.is_valid(); ++it)
	{
		results.push_back(*it);
	}
	return results;
}

/*
   std::vector<noob::active_mesh::face_h> noob::active_mesh::get_faces_with_edge(const noob::active_mesh::edge_h eh) const
   {
   }


   noob::active_mesh::face_h noob::active_mesh::get_face_with_halfedge(const noob::active_mesh::halfedge_h) const
   {

   }
   */

std::vector<noob::active_mesh::halfedge_h> noob::active_mesh::get_halfedges_in_face(const noob::active_mesh::face_h fh) const
{
	std::vector<noob::active_mesh::halfedge_h> results;
	for (auto it = half_edges.cfh_iter(fh); it.is_valid(); ++it)
	{
		results.push_back(*it);
	}
	return results;
}


bool noob::active_mesh::face_exists(const std::vector<noob::active_mesh::vertex_h>& verts) const
{
	// Now we must test all faces sharing the first vertex, with the full number of vertices needing testing.
	// We are able to get away with not testing the other verts since a vertex->face search with any vertex in the list would already return all relevant faces.
	for (PolyMesh::VertexFaceIter vf_it = half_edges.cvf_iter(verts[0]); vf_it.is_valid(); ++vf_it)
	{
		size_t iteration = 0;
		for (PolyMesh::FaceVertexIter fv_it = half_edges.cfv_iter(*vf_it); fv_it.is_valid(); ++fv_it)
		{
			// This inner loop is basically graph isomorphism (ie: brute force search to see if two groups of vertices are lined up the same way.)
			if (iteration > verts.size() - 1) break;
			if (verts[iteration] != *fv_it) break;
			if (verts[iteration] == *fv_it && iteration == verts.size() - 1) return true;
			++iteration;
		}
	}

	return false;
}


bool noob::active_mesh::face_exists(const noob::active_mesh::face_h h) const 
{
	return half_edges.is_valid_handle(h);
}


noob::active_mesh::vertex_results noob::active_mesh::get_vertex(const noob::active_mesh::vertex_h h) const 
{
	noob::active_mesh::vertex_results results;
	if (vertex_exists(h))
	{
		results.valid = true;
		auto p = half_edges.point(h);
		results.vertex = noob::vec3(p[0], p[1], p[2]);
	}
	return results;
}


noob::active_mesh::vertex_handle_results noob::active_mesh::get_vertex_handle(const noob::vec3& v) const
{
	noob::active_mesh::vertex_handle_results results;
	if (vertex_exists(v))
	{
		results.valid = true;
		results.handle = xyz_to_vhandles[v.v];
	}
	return results;
}


std::vector<noob::active_mesh::vertex_h> noob::active_mesh::get_verts_in_face(const noob::active_mesh::face_h h) const 
{
	std::vector<noob::active_mesh::vertex_h> vert_handles;
	for (auto vit = half_edges.cfv_iter(h); vit.is_valid(); ++vit)
	{
		vert_handles.push_back(*vit);
	}
	return vert_handles;
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
	m.to_origin();
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


std::vector<noob::active_mesh> noob::active_mesh::reduce_verts(size_t max_vertices) const 
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
				verts_list.push_back(output_mesh.add_vertex(noob::vec3_from_polymesh(half_edges.point(*inner_it))));
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
	std::vector<noob::active_mesh::vertex_h> faces;
	faces.reserve(3);
	for (size_t i = 0; i < m.indices.size(); i += 3)
	{
		faces[0] = add_vertex(m.vertices[i]);
		faces[1] = add_vertex(m.vertices[i+1]);
		faces[2] = add_vertex(m.vertices[i+2]);
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

/*
   void sweep_line(const noob::active_mesh::halfedge_h, const noob::vec3&)
   {

   }
   */

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
		noob::vec3 p(vec3_from_polymesh(half_edges.point(h)));
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


noob::active_mesh::face_split_results noob::active_mesh::split_face(const noob::active_mesh::face_h fh, const noob::plane& splitter)
{
	std::vector<noob::active_mesh::edge_h> edges;
	for (auto it = half_edges.cfe_iter(fh); it.is_valid(); ++it)
	{
		edges.push_back(*it);
	}

	std::vector<noob::vec3> split_points(2);
	noob::active_mesh::edge_h created_edge;
	bool valid = false;
	
	noob::active_mesh::halfedge_h he1, he2;
	for (auto e : edges)
	{
		auto half = half_edges.halfedge_handle(e, 1);
		auto p1 = half_edges.point(half_edges.from_vertex_handle(half));
		auto p2 = half_edges.point(half_edges.to_vertex_handle(half));

		// If the two signs differ...
		bool first = true;
		noob::active_mesh::vertex_h v1, v2;
		noob::active_mesh::edge_h e1, e2;

		if (splitter.signed_distance(noob::vec3_from_polymesh(p1)) > 0 ^ splitter.signed_distance(vec3_from_polymesh(p2)) > 0)
		{
			if (first)
			{
				e1 = e;
				first = false;
			}
			else
			{
				e2 = e;
				valid = true;
				break;
			}
		}

		if (valid)
		{

			v1 = add_vertex(splitter.projection(vec3_from_polymesh(p1)));

			half_edges.split(e1, v1);
			v2 = add_vertex(splitter.projection(vec3_from_polymesh(p2)));

			half_edges.split(e2, v2);
			he1 = half_edges.new_edge(v1, v2);
			he2 = half_edges.new_edge(v2, v1);
		}
	}

	noob::active_mesh::face_split_results results;
	if (valid)
	{
		results.valid = true;
		results.faces[0] = half_edges.face_handle(he1);
		results.faces[1] = half_edges.face_handle(he2);
	}
	return results;
}

/*
   void noob::active_mesh::connect_faces(noob::active_mesh::face_h first_handle, noob::active_mesh::face_h second_handle) 
   {
   std::vector<noob::active_mesh::> face_one_verts;
   for (auto it = half_egdes.cfv_iter(first_handle); it.id_valid(); ++it)
   {
   face
   }
   }
   */

void noob::active_mesh::move_vertex(const noob::active_mesh::vertex_h vh, const noob::vec3& direction) 
{
	half_edges.set_point(vh, half_edges.point(vh) + PolyMesh::Point(direction.v[0], direction.v[1], direction.v[2]));
}


void noob::active_mesh::move_vertices(const std::vector<noob::active_mesh::vertex_h>& handles, const noob::vec3& direction) 
{
	for (noob::active_mesh::vertex_h vh : handles)
	{
		move_vertex(vh, direction);
	}
}

/*
   void move_edge(const noob::active_mesh::edge_h, const noob::vec3&)
   {

   }


   void move_edges(const std::vector<noob::active_mesh::edge_h>&, const noob::vec3&)
   {

   }


   void move_halfedge(const noob::active_mesh::halfedge_h, const noob::vec3&)
   {

   }


   void move_halfedges(const std::vector<noob::active_mesh::halfedge_h>&, const noob::vec3&)
   {

   }


   void move_face(const noob::active_mesh::face_h, const noob::vec3&)
   {

   }


   void move_faces(const std::vector<noob::active_mesh::face_h>&, const noob::vec3&)
   {

   }
   */

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

			if (vec3_equality(first_normal, second_normal))
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





// std::vector<noob::active_mesh::face_h> get_faces_with_vert(const noob::active_mesh::vertex_h) const; 

// std::vector<noob::active_mesh::face_h> get_faces_with_edge(const noob::active_mesh::edge_h) const; 

// noob::active_mesh::face_h get_face_with_halfedge(const noob::active_mesh::halfedge_h) const;

// std::vector<noob::active_mesh::halfedge_h> get_halfedges_in_face(const noob::active_mesh::face_h) const;



// void sweep_line(const noob::active_mesh::halfedge_h, const noob::vec3&);

// void move_edge(const noob::active_mesh::edge_h, const noob::vec3&);

// void move_halfedge(const noob::active_mesh::halfedge_h, const noob::vec3&);

// void move_face(const noob::active_mesh::face_h, const noob::vec3&);

// void move_edges(const std::vector<noob::active_mesh::edge_h>&, const noob::vec3&);

// void move_halfedges(const std::vector<noob::active_mesh::halfedge_h>&, const noob::vec3&);

// void move_faces(const std::vector<noob::active_mesh::face_h>&, const noob::vec3&);
