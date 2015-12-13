#include "MeshUtils.hpp"

#include <LinearMath/btConvexHull.h>
#include <set>

/*
   noob::basic_mesh noob::mesh_utils::swept_sphere(float radius, size_t x_segments, size_t _y_segments)
   {
   PolyMesh half_edges;
   size_t _x_segments, _y_segments, _radius;
   if (radius 0.0 )
   {
   _radius = 1.0;
   }

   if (x_segments == 0) 
   {
   _x_segments = 12;
   }
   else
   {
   _x_segments = x_segments;
   }

   if (y_segments == 0)
   {
   _y_segments = 12;
   }
   else
   {
   _x_segments = x_segments;
   }

   double increment_amount = TWO_PI / static_cast<double>(_x_segments);
   std::vector<std::tuple<PolyMesh::VertexHandle, PolyMesh::VertexHandle>> verts;

// TODO: Center on origin
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
//OpenMesh::IO::write_mesh(half_edges, "temp/swept_sphere.off");
noob::basic_mesh mesh;
mesh.from_half_edges(half_edges);

//mesh.load("temp/sphere.off","cylinder-temp");
// logger::log(fmt::format("Created cylinder with height = {0}, radius = {1} with {2} segments.", height, radius, _segments));
return mesh;
}

*/

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

	noob::basic_mesh mesh;
	mesh.from_half_edges(half_edges);

	return mesh;
}


noob::basic_mesh noob::mesh_utils::swept_shape(const std::vector<noob::vec2>& points, size_t segments)
{
	// TODO: Sort line of points by Y-value, descending order.
	PolyMesh half_edges;

	std::vector<noob::vec3> _points;
	for (noob::vec2 p : points)
	{
		_points.push_back(noob::vec3(p.v[0], p.v[1], 0.0));
	}

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

	std::vector<std::vector<PolyMesh::VertexHandle>> topology_verts;

	for (size_t i = 0; i < _points.size(); ++i)
	{
		Eigen::Vector3f p(_points[i].v[0], _points[i].v[1], _points[i].v[2]);

		std::vector<PolyMesh::VertexHandle> current_ring;

		for (size_t seg = 0; seg < _segments; ++seg)
		{
			double diff = increment_amount * seg;
			Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());
			Eigen::Vector3f p_rotated = angle_axis * p;
			PolyMesh::VertexHandle v_handle = half_edges.add_vertex(PolyMesh::Point(p_rotated[0], p_rotated[1], p_rotated[2]));
			current_ring.push_back(v_handle);
		}

		topology_verts.push_back(current_ring);
	}

	std::vector<PolyMesh::VertexHandle> face_verts;

	// face_verts.clear();	
	// face_verts.push_back(std::get<0>(verts[verts.size()-1]));
	// face_verts.push_back(std::get<1>(verts[verts.size()-1]));
	// face_verts.push_back(std::get<1>(verts[0]));
	// face_verts.push_back(std::get<0>(verts[0]));
	// half_edges.add_face(face_verts);

	for (size_t i = 1; i < topology_verts.size(); ++i)
	{
		std::vector<PolyMesh::VertexHandle> previous_ring = topology_verts[i - 1];
		std::vector<PolyMesh::VertexHandle> current_ring = topology_verts[i];
		// TODO: Enforce at the language level that both vectors must have the same number of elements.
		// Currently it's secured by mathematics, which is quite solid but still rather sloppy from a C++ standpoint.
		for (size_t k = 0; k < std::min(previous_ring.size(), current_ring.size()); ++k)
		{
			std::vector<PolyMesh::VertexHandle> face_verts;

		}
	}

	noob::basic_mesh results;
	return results;
}


noob::basic_mesh noob::mesh_utils::capsule(float radius, float height, size_t segments)
{
	noob::basic_mesh s_top = catmull_sphere(radius);
	noob::basic_mesh s_bottom;
	s_bottom.load_mem(s_top.save());

	s_top.translate(noob::vec3(0.0, height - radius, 0.0));
	s_bottom.translate(noob::vec3(0.0, -(height - radius), 0.0));

	std::vector<noob::vec3> points;

	for (noob::vec3 p : s_top.vertices)
	{
		points.push_back(p);
	}

	for (noob::vec3 p : s_bottom.vertices)
	{
		points.push_back(p);
	}

	return noob::mesh_utils::hull(points);
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
	//OpenMesh::IO::write_mesh(half_edges, "temp/cube.off");

	noob::basic_mesh mesh;
	mesh.from_half_edges(half_edges);
	//mesh.load_file("temp/cube.off", "cube-temp");
	// logger::log(fmt::format("Created cube with width = {0}, height = {1}, depth = {2} with {3} subdivides.", width, height, depth, subdivides));
	return mesh;
}


noob::basic_mesh noob::mesh_utils::catmull_sphere(float radius)
{
	float diameter = radius * 2;
	noob::basic_mesh mesh = noob::mesh_utils::box(diameter, diameter, diameter, 3);
	// logger::log(fmt::format("Created sphere of radius {0}.", radius));
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
		mesh.vertices.push_back(hull_result.m_OutputVertices[i]);
	}

	for (unsigned int i = 0; i < hull_result.mNumIndices; ++i)
	{
		mesh.indices.push_back(static_cast<uint16_t>(hull_result.m_Indices[i]));
	}

	mesh.normalize();
	return mesh;
}


noob::basic_mesh noob::mesh_utils::csg(const noob::basic_mesh& a, const noob::basic_mesh& b, const noob::csg_op op)
{
	csgjs_model model_a = get_csg_model(a);
	csgjs_model model_b = get_csg_model(b);

	csgjs_model resulting_csg_model;
	if (op == noob::csg_op::UNION)
	{
		resulting_csg_model = csgjs_union(model_a, model_b);
	}
	else if (op == noob::csg_op::DIFFERENCE)
	{
		resulting_csg_model = csgjs_difference(model_a, model_b);
	}
	else
	{
		resulting_csg_model = csgjs_intersection(model_a, model_b);
	}

	noob::basic_mesh results;

	for (size_t i = 0; i < resulting_csg_model.vertices.size(); i++)
	{
		noob::vec3 vert;
		vert.v[0] = resulting_csg_model.vertices[i].pos.x;
		vert.v[1] = resulting_csg_model.vertices[i].pos.y;
		vert.v[2] = resulting_csg_model.vertices[i].pos.z;
		results.vertices.push_back(vert);

		noob::vec3 norm;
		norm.v[0] = resulting_csg_model.vertices[i].normal.x;
		norm.v[1] = resulting_csg_model.vertices[i].normal.y;
		norm.v[2] = resulting_csg_model.vertices[i].normal.z;
		results.normals.push_back(norm);

		noob::vec3 uvw;
		uvw.v[0] = resulting_csg_model.vertices[i].uv.x;
		uvw.v[1] = resulting_csg_model.vertices[i].uv.y;
		results.texcoords.push_back(uvw);
	}

	for (int i : resulting_csg_model.indices)
	{
		results.indices.push_back(static_cast<uint16_t>(i));
	}

	return results;
}


csgjs_model noob::mesh_utils::get_csg_model(const noob::basic_mesh& m)
{
	csgjs_model model;

	for (size_t j = 0; j < m.vertices.size(); j++)
	{
		csgjs_vertex vert;
		vert.pos.x = m.vertices[j].v[0];
		vert.pos.y = m.vertices[j].v[1];
		vert.pos.z = m.vertices[j].v[2];

		vert.normal.x = m.normals[j].v[0];
		vert.normal.y = m.normals[j].v[1];
		vert.normal.z = m.normals[j].v[2];

		vert.uv.x = m.texcoords[j].v[0];
		vert.uv.y = m.texcoords[j].v[1];

		model.vertices.push_back(vert);
	}

	for (uint16_t i : m.indices)
	{
		model.indices.push_back(static_cast<int>(i));
	}

	return model;
}
