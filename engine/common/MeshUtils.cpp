#include "MeshUtils.hpp"

#include <LinearMath/btConvexHull.h>

// #include <tinympl/functional.hpp>

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

noob::basic_mesh noob::mesh_utils::sphere(float radius)
{
	return catmull_sphere(radius);
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

noob::basic_mesh noob::mesh_utils::capsule(float radius, float height, size_t segments)
{
	return noob::mesh_utils::cylinder(radius, height, segments);
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
	//OpenMesh::IO::write_mesh(half_edges, "temp/box.off");

	noob::basic_mesh mesh;
	mesh.from_half_edges(half_edges);
	//mesh.load_file("temp/box.off", "box-temp");
	// logger::log(fmt::format("Created box with width = {0}, height = {1}, depth = {2} with {3} subdivides.", width, height, depth, subdivides));
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


noob::basic_mesh noob::mesh_utils::csg(const noob::basic_mesh& _a, const noob::basic_mesh& _b, const noob::csg_op op)
{
	noob::basic_mesh a = _a;
	noob::basic_mesh b = _b;
	a.normalize();
	b.normalize();

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
		results.indices.push_back(i);
	}
	
	results.normalize();

	return results;
}


noob::basic_mesh noob::mesh_utils::copy(const noob::basic_mesh& m)
{
	noob::basic_mesh temp;
	temp.vertices.reserve(m.vertices.size());
	temp.normals.reserve(m.normals.size());
	temp.texcoords.reserve(m.texcoords.size());
	temp.indices.reserve(m.indices.size());

	for (noob::vec3 v : m.vertices)
	{
		temp.vertices.emplace_back(v);
	}

	for (noob::vec3 n : m.normals)
	{
		temp.normals.emplace_back(n);
	}

	for (noob::vec3 u : m.texcoords)
	{
		temp.texcoords.emplace_back(u);
	}

	for (size_t i : m.indices)
	{
		temp.indices.emplace_back(i);
	}

	return temp;
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
