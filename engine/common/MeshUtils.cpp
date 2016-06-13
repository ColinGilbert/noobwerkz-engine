#include "MeshUtils.hpp"
#include "ActiveMesh.hpp"

#include <LinearMath/btConvexHull.h>


#include <Eigen/Geometry> 


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
		mesh.vertices.push_back(hull_result.m_OutputVertices[i]);
	}

	for (unsigned int i = 0; i < hull_result.mNumIndices; ++i)
	{
		mesh.indices.push_back(static_cast<uint16_t>(hull_result.m_Indices[i]));
	}

	return mesh;
}


static noob::basic_mesh lathe(const std::vector<noob::vec2>& points, float clip_one, float clip_two)
{
	noob::active_mesh substrate;

	std::vector<noob::vec3> points_in_three_dee;
	for (noob::vec2 p : points)
	{
		points_in_three_dee.push_back(noob::vec3(p.v[0], p.v[1], 0.0));
	}

	std::vector<noob::active_mesh::vertex_h> first_ring, last_ring;

	

}

// noob::basic_mesh noob::mesh_utils::csg(const noob::basic_mesh& _a, const noob::basic_mesh& _b, const noob::csg_op op)
// {



/*
	NewtonWorld* const csg_world = NewtonCreate ();
	
	NewtonMesh* a = noob_to_newton(_a, csg_world);
	NewtonMesh* b = noob_to_newton(_b, csg_world);

	NewtonMesh* raw_results;
	noob::mat4 id = noob::identity_mat4();

	switch(op)
	{
		case(noob::csg_op::UNION):
		{	
			raw_results = NewtonMeshUnion(a, b, &id.m[0]);	
		}
		case(noob::csg_op::DIFFERENCE):
		{
			raw_results = NewtonMeshDifference(a, b, &id.m[0]);
		}
		case(noob::csg_op::INTERSECTION):
		{
			raw_results = NewtonMeshIntersection(a, b, &id.m[0]);
		}
	}
	
	return newton_to_noob(raw_results);
*/
// }


/*
noob::basic_mesh noob::mesh_utils::csg(const noob::basic_mesh& _a, const noob::basic_mesh& _b, const noob::csg_op op)
{
	// noob::basic_mesh a = _a;
	// noob::basic_mesh b = _b;
	// a.update_normals();
	// b.update_normals();

	csgjs_model model_a = get_csg_model(_a);
	csgjs_model model_b = get_csg_model(_b);

	csgjs_model resulting_csg_model;
	switch (op)
	{
		case (noob::csg_op::UNION):
			resulting_csg_model = csgjs_union(model_a, model_b);
			break;

		case(noob::csg_op::DIFFERENCE):
			resulting_csg_model = csgjs_difference(model_a, model_b);
			break;
		case(noob::csg_op::INTERSECTION):
			resulting_csg_model = csgjs_intersection(model_a, model_b);
			break;
	};
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

	results.update_normals();

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

*/
/*
NewtonMesh* noob::mesh_utils::noob_to_newton(const noob::basic_mesh& m_arg, const NewtonWorld* const w_arg)
{
	NewtonMesh* newton_mesh = NewtonMeshCreate(w_arg);
	for(size_t i = 0; i < m_arg.indices.size(); i += 3)
	{
		NewtonMeshBeginFace(newton_mesh);
		
		std::array<noob::vec3, 3> verts;
		verts[0] = m_arg.vertices[i];
		verts[1] = m_arg.vertices[i+1];
		verts[2] = m_arg.vertices[i+2];

		NewtonMeshAddFace(newton_mesh, 3, &verts[0].v[0], sizeof(noob::vec3)*3, 0);

		NewtonMeshEndFace(newton_mesh);
	}

	return newton_mesh;
}


noob::basic_mesh noob::mesh_utils::newton_to_noob(const NewtonMesh* const newton_mesh)
{


	NewtonMeshSaveOFF(newton_mesh, "temp/temp_newton_to_noob.off");
	noob::basic_mesh results;
	results.load_file("temp/temp_newton_to_noob.off", "temp-newton-to-noob");
	return results;
	// void NewtonMeshGetIndirectVertexStreams(const NewtonMesh* const mesh, int vertexStrideInByte, dFloat* const vertex, int* const vertexIndices, int* const vertexCount, int normalStrideInByte, dFloat* const normal, int* const normalIndices, int* const normalCount, int uvStrideInByte0, dFloat* const uv0, int* const uvIndices0, int* const uvCount0, int uvStrideInByte1, dFloat* const uv1, int* const uvIndices1, int* const uvCount1);
	//
	// NewtonMeshGetIndirectVertexStreams(newton_mesh, NewtonMeshGetVertexStrideInByte(newton_mesh), dFloat* const vertex, int* const vertexIndices, int* const vertexCount, int normalStrideInByte, dFloat* const normal, int* const normalIndices, int* const normalCount, int uvStrideInByte0, dFloat* const uv0, int* const uvIndices0, int* const uvCount0, int uvStrideInByte1, dFloat* const uv1, int* const uvIndices1, int* const uvCount1);

	// NewtonMeshGetVertexStrideInByte (const NewtonMesh* const mesh)
	//
	// NewtonMeshGetPointStrideInByte (const NewtonMesh* const mesh);
	// NewtonMeshGetPointCount (const NewtonMesh* const mesh);
	// NewtonMeshGetTotalIndexCount (const NewtonMesh* const mesh);
	// NewtonMeshMaterialGetIndexStreamShort (const NewtonMesh* const mesh, void* const handle, int materialId, short int* const index); 
	//
	//
}

*/
