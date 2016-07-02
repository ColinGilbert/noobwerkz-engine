#include "MeshUtils.hpp"
#include "ActiveMesh.hpp"

#include <LinearMath/btConvexHull.h>

#include <algorithm>

#include <Eigen/Geometry> 


noob::basic_mesh noob::mesh_utils::sphere(float radius)
{
	return catmull_sphere(radius);
/*
	float _segments = 12;

	double increment_amount = TWO_PI / _segments;

	std::vector<noob::active_mesh::vertex_h> verts;

	// Eigen::Vector3f p(0.0f, 0.0f, radius);

	// for (size_t seg = 0; seg < _segments; seg++)
	// {
	// 	double diff = increment_amount * seg;
	// 	Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());
	//	Eigen::Vector3f rotated_point = angle_axis * p;
*/
}


noob::basic_mesh noob::mesh_utils::cone(float radius, float height, size_t segments)
{
	TriMesh half_edges;
	size_t _segments;
	if (segments == 0)
	{
		_segments = 12;
	}
	else 
	{
		_segments = segments;
	}

	TriMesh::VertexHandle top = half_edges.add_vertex(TriMesh::Point(0.0f, height, 0.0f));
	TriMesh::VertexHandle origin = half_edges.add_vertex(TriMesh::Point(0.0f, 0.0f, 0.0f));

	double increment_amount = TWO_PI / _segments;

	std::vector<TriMesh::VertexHandle> verts;

	Eigen::Vector3f p(0.0f, 0.0f, radius);

	for (size_t seg = 0; seg < _segments; seg++)
	{
		double diff = increment_amount * seg;
		Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());
		Eigen::Vector3f rotated_point = angle_axis * p;
		verts.push_back(half_edges.add_vertex(TriMesh::Point(rotated_point[0], rotated_point[1], rotated_point[2])));
	}

	std::vector<TriMesh::VertexHandle> face_verts;	
	for(size_t i = 1; i < verts.size(); i++)
	{
		face_verts.clear();
		face_verts.push_back(top);
		face_verts.push_back(verts[i]);
		face_verts.push_back(verts[i-1]);
		half_edges.add_face(face_verts);

		face_verts.clear();
		face_verts.push_back(origin);
		face_verts.push_back(verts[i-1]);
		face_verts.push_back(verts[i]);
		half_edges.add_face(face_verts);
	}

	face_verts.clear();
	face_verts.push_back(top);
	face_verts.push_back(verts[0]);
	face_verts.push_back(verts[verts.size()-1]);
	half_edges.add_face(face_verts);

	face_verts.clear();
	face_verts.push_back(origin);
	face_verts.push_back(verts[verts.size()-1]);
	face_verts.push_back(verts[0]);
	half_edges.add_face(face_verts);

	// half_edges.garbage_collection();
	//OpenMesh::IO::write_mesh(half_edges, "temp/cone.off");
	noob::basic_mesh mesh;
	mesh.from_half_edges(half_edges);

	//mesh.load_file("temp/cone.off","cone-temp");
	// logger::log(fmt::format("Created cone with height = {0}, radius = {1}, and {2} segments.", height, radius, _segments));
	return mesh;
}

noob::basic_mesh noob::mesh_utils::cylinder(float radius, float height, size_t segments)
{
	PolyMesh half_edges;
	size_t _segments;
	if (segments == 0) 
	{
		_segments = 12;
	}
	else
	{
		_segments = segments;
	}
	double increment_amount = TWO_PI / static_cast<double>(_segments);

	std::vector<std::tuple<PolyMesh::VertexHandle, PolyMesh::VertexHandle>> verts;

	PolyMesh::VertexHandle top = half_edges.add_vertex(PolyMesh::Point(0.0f, height, 0.0f));
	PolyMesh::VertexHandle origin = half_edges.add_vertex(PolyMesh::Point(0.0f, 0.0f, 0.0f));

	Eigen::Vector3f p_upper(0.0f, height, radius);
	Eigen::Vector3f p_lower(0.0f, 0.0f, radius);

	for (size_t seg = 0; seg < _segments; seg++)
	{
		double diff = increment_amount * seg;
		Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());

		Eigen::Vector3f rotated_point_lower = angle_axis * p_lower;
		Eigen::Vector3f rotated_point_upper = angle_axis * p_upper;

		PolyMesh::VertexHandle v1 = half_edges.add_vertex(PolyMesh::Point(rotated_point_lower[0], rotated_point_lower[1], rotated_point_lower[2]));
		PolyMesh::VertexHandle v2 = half_edges.add_vertex(PolyMesh::Point(rotated_point_upper[0], rotated_point_upper[1], rotated_point_upper[2]));
		verts.push_back(std::make_tuple(v1, v2));
	}

	std::vector<PolyMesh::VertexHandle> face_verts;	
	for(size_t i = 1; i < verts.size(); i++)
	{
		std::tuple<PolyMesh::VertexHandle, PolyMesh::VertexHandle> previous_verts = verts[i-1];
		std::tuple<PolyMesh::VertexHandle, PolyMesh::VertexHandle> current_verts = verts[i];

		face_verts.clear();
		face_verts.push_back(top);
		face_verts.push_back(std::get<1>(current_verts));
		face_verts.push_back(std::get<1>(previous_verts));
		half_edges.add_face(face_verts);

		face_verts.clear();	
		face_verts.push_back(std::get<0>(previous_verts));
		face_verts.push_back(std::get<1>(previous_verts));
		face_verts.push_back(std::get<1>(current_verts));
		face_verts.push_back(std::get<0>(current_verts));
		half_edges.add_face(face_verts);

		face_verts.clear();
		face_verts.push_back(origin);
		face_verts.push_back(std::get<0>(previous_verts));
		face_verts.push_back(std::get<0>(current_verts));
		half_edges.add_face(face_verts);
	}

	face_verts.clear();
	face_verts.push_back(top);
	face_verts.push_back(std::get<1>(verts[0]));
	face_verts.push_back(std::get<1>(verts[verts.size()-1]));
	half_edges.add_face(face_verts);

	face_verts.clear();	
	face_verts.push_back(std::get<0>(verts[verts.size()-1]));
	face_verts.push_back(std::get<1>(verts[verts.size()-1]));
	face_verts.push_back(std::get<1>(verts[0]));
	face_verts.push_back(std::get<0>(verts[0]));
	half_edges.add_face(face_verts);

	face_verts.clear();
	face_verts.push_back(origin);
	face_verts.push_back(std::get<0>(verts[verts.size()-1]));
	face_verts.push_back(std::get<0>(verts[0]));
	half_edges.add_face(face_verts);


	half_edges.triangulate();
	half_edges.garbage_collection();
	//OpenMesh::IO::write_mesh(half_edges, "temp/cylinder.off");
	noob::basic_mesh mesh;
	mesh.from_half_edges(half_edges);

	//mesh.load_file("temp/cylinder.off","cylinder-temp");
	// logger::log(fmt::format("Created cylinder with height = {0}, radius = {1} with {2} segments.", height, radius, _segments));
	return mesh;
}


noob::basic_mesh noob::mesh_utils::box(float width, float height, float depth, size_t subdivides)
{
	PolyMesh half_edges;
	PolyMesh::VertexHandle vhandle[8];

	vhandle[0] = half_edges.add_vertex(PolyMesh::Point(-width, -height, depth));
	vhandle[1] = half_edges.add_vertex(PolyMesh::Point( width, -height, depth));
	vhandle[2] = half_edges.add_vertex(PolyMesh::Point( width,  height, depth));
	vhandle[3] = half_edges.add_vertex(PolyMesh::Point(-width,  height, depth));
	vhandle[4] = half_edges.add_vertex(PolyMesh::Point(-width, -height, -depth));
	vhandle[5] = half_edges.add_vertex(PolyMesh::Point( width, -height, -depth));
	vhandle[6] = half_edges.add_vertex(PolyMesh::Point( width,  height, -depth));
	vhandle[7] = half_edges.add_vertex(PolyMesh::Point(-width,  height, -depth));


	// generate (quadrilateral) faces
	std::vector<PolyMesh::VertexHandle> face_vhandles;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[4]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[5]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[6]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[7]);
	half_edges.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[4]);
	half_edges.add_face(face_vhandles);

	if (subdivides != 0)
	{
		OpenMesh::Subdivider::Uniform::CatmullClarkT<PolyMesh> catmull;
		catmull.attach(half_edges);
		catmull(subdivides);
		catmull.detach();
	}

	half_edges.triangulate();
	half_edges.garbage_collection();

	noob::basic_mesh mesh;
	mesh.from_half_edges(half_edges);
	return mesh;
}


noob::basic_mesh noob::mesh_utils::catmull_sphere(float radius)
{
	float diameter = radius * 2;
	noob::basic_mesh mesh = noob::mesh_utils::box(diameter, diameter, diameter, 2);
	return mesh;
}


noob::basic_mesh noob::mesh_utils::hull(const std::vector<noob::vec3>& points)
{
	// TODO: Optimize this	
	std::vector<btVector3> bt_points;
	for (noob::vec3 p : points)
	{
		bt_points.push_back(btVector3(p.v[0], p.v[1], p.v[2]));
	}

	HullDesc hull_desc(QF_DEFAULT, points.size(), &bt_points[0]);

	HullLibrary hull_lib;
	HullResult hull_result;

	HullError error_msg = hull_lib.CreateConvexHull(hull_desc, hull_result);
	if (error_msg == HullError::QE_FAIL) logger::log("FAILED TO CREATE CONVEX HULL. WTF?");

	noob::basic_mesh mesh;

	for (unsigned int i = 0; i < hull_result.mNumOutputVertices; ++i)
	{
		mesh.vertices.push_back(noob::vec3_from_bullet(hull_result.m_OutputVertices[i]));
	}

	for (unsigned int i = 0; i < hull_result.mNumIndices; ++i)
	{
		mesh.indices.push_back(static_cast<uint16_t>(hull_result.m_Indices[i]));
	}

	return mesh;
}


noob::basic_mesh noob::mesh_utils::lathe(const std::vector<noob::vec2>& points, float taper_one, float taper_two, size_t segments)
{
	std::vector<noob::vec3> initial_points;

	for (noob::vec2 p : points)
	{
		initial_points.push_back(noob::vec3(p.v[0], p.v[1], 0.0));
	}

	size_t _segments = 12;

	if (segments < 3)
	{
		_segments = segments;
	}

	double increment_amount = TWO_PI / _segments;

	size_t num_points = initial_points.size();
	
	// first_ring and last_ring refers to the verts at  each endpoint
	std::vector<noob::active_mesh::vertex_h> first_ring(_segments);
	std::vector<noob::active_mesh::vertex_h> last_ring(_segments);
	std::vector<noob::active_mesh::vertex_h> current_ring(_segments);
	std::vector<noob::active_mesh::vertex_h> previous_ring(_segments);

	noob::active_mesh halfedges;

	for (size_t ring_count = 0; ring_count < num_points; ++ring_count)
	{
		// Create a segment's worth of vertices and store them into a temporary vector.
		// Also, if the vertex being evaluated happens to be first or last in our original points add it to first_ring and last_ring, respectively.
		for (size_t seg = 0; seg <= _segments; ++seg)
		{
			double diff = increment_amount * seg;
			Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());
			
			Eigen::Vector3f p(initial_points[ring_count].v[0], initial_points[ring_count].v[1], 0.0);
			Eigen::Vector3f rotated_point = angle_axis * p;
			
			// {
			// 	fmt::MemoryWriter ww;
			//	ww << "[MeshUtils] adding vertex " << seg << " of ring " << ring_count;
			//	logger::log(ww.str());
			// }

			noob::active_mesh::vertex_h temp_vert = halfedges.add_vertex(noob::vec3(rotated_point[0], rotated_point[1], rotated_point[2]));
			
			if (ring_count == 0)
			{
				first_ring[seg] = temp_vert;
			}
			
			if (ring_count == num_points - 1)
			{
				last_ring[seg] = temp_vert;
			
			}

			else
			{
				previous_ring[seg] = current_ring[seg]; // Warning. May copy garbage into previous_ring when ring_count == 1. However, previous_ring won't get used if ring_count == 1. Therefore we're good. :)
				current_ring[seg] = temp_vert;
			}
		}


		// {
		// 	fmt::MemoryWriter ww;
		// 	ww << "[MeshUtils] Adding faces for ring " << ring_count;
		//	logger::log(ww.str());
		// }

		// Now that we have a full ring of vertices all the way around, use those vertices to make a full ring of faces.

		if (ring_count == 0)
		{
			continue;
		}
		
		else if (ring_count == 1)
		{

			// Use first_ring and current_ring to create the faces.
			std::vector<noob::active_mesh::vertex_h> face_verts(4);
			for (size_t seg = 1; seg <= _segments; ++seg)
			{
				face_verts[0] = first_ring[seg];
				face_verts[1] = first_ring[seg-1];
				face_verts[2] = current_ring[seg-1];
				face_verts[3] = current_ring[seg];
				halfedges.add_face(face_verts);
			}
		}

		else if (ring_count == num_points - 1)
		{
			// Use current_ring and last_ring to create the faces.
			// logger::log("Last ring!");

			std::vector<noob::active_mesh::vertex_h> face_verts(4);
			for (size_t seg = 1; seg <= _segments; ++seg)
			{
				face_verts[0] = current_ring[seg];
				face_verts[1] = current_ring[seg-1];
				face_verts[2] = last_ring[seg-1];
				face_verts[3] = last_ring[seg];
				halfedges.add_face(face_verts);
			}
		
		}

		else
		{
			// Use previous_ring and current_ring to create the faces.
			std::vector<noob::active_mesh::vertex_h> face_verts(4);
			for (size_t seg = 1; seg <= _segments; ++seg)
			{
				face_verts[0] = previous_ring[seg];
				face_verts[1] = previous_ring[seg-1];
				face_verts[2] = current_ring[seg-1];
				face_verts[3] = current_ring[seg];
				halfedges.add_face(face_verts);
			}		
		}
	}

	std::vector<noob::active_mesh::vertex_h> face_verts(3);
	
	// Now, cap the ends
	noob::active_mesh::vertex_h first_cap = halfedges.add_vertex(noob::vec3(0.0, initial_points[0].v[1] + taper_one, 0.0));
	noob::active_mesh::vertex_h last_cap = halfedges.add_vertex(noob::vec3(0.0, initial_points[initial_points.size()-1].v[1] + taper_two, 0.0));
	
	for (size_t seg = 1; seg <= _segments; ++seg)
	{

		face_verts[0] = first_cap;
		face_verts[1] = first_ring[seg-1];
		face_verts[2] = first_ring[seg];
		halfedges.add_face(face_verts);

		face_verts[0] = last_cap;
		face_verts[1] = last_ring[seg];
		face_verts[2] = last_ring[seg-1];
		halfedges.add_face(face_verts);
	}


	// Return our friend the basic_mesh
	return halfedges.to_basic_mesh();
}
