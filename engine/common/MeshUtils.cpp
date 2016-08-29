#include "MeshUtils.hpp"
#include "ActiveMesh.hpp"

#include <LinearMath/btConvexHull.h>

#include <algorithm>

#include <Eigen/Geometry> 


noob::basic_mesh noob::mesh_utils::sphere(float radius)
{
	noob::basic_mesh results = noob::mesh_utils::cone(radius, radius*2);
	return results;
}


noob::basic_mesh noob::mesh_utils::cone(float radius, float height)
{
	double angle = std::atan(radius / height);
	voro::wall_cone cone_cutter(0.0, height/2.0, 0.0, 0.0, 1.0, 0.0, angle);
	voro::voronoicell cell;
	const float offset = 5.0;
	cell.init(-radius-offset, radius+offset, -height-offset, height+offset, radius+offset, -radius-offset);
	cone_cutter.cut_cell(cell, 0.0, height*0.5, 0.0);

	voro::wall_plane plane_cutter(0.0, -1.0, 0.0, -height*0.5);
	plane_cutter.cut_cell(cell, 0.0, 0.0, 0.0);
	return voro_to_mesh(&cell);

}

noob::basic_mesh noob::mesh_utils::cylinder(float radius, float height, size_t segments)
{

	return noob::mesh_utils::cone(radius, radius*2);
	
}


noob::basic_mesh noob::mesh_utils::box(float width, float height, float depth)
{

	return noob::mesh_utils::cone(width* 0.5, height);//radius, radius*2);
}


noob::basic_mesh noob::mesh_utils::hull(const std::vector<noob::vec3>& points)
{

	return noob::mesh_utils::cone(1.0, 1.0);//radius, radius*2);
	/*

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
	*/
}

noob::basic_mesh noob::mesh_utils::voro_to_mesh(voro::voronoicell* cell)
{
	double centroid_x, centroid_y, centroid_z;
	cell->centroid(centroid_x, centroid_y, centroid_z);

	int num_faces = cell->number_of_faces();

	int num_edges = cell->number_of_edges();

	std::vector<double> verts;
	cell->vertices(verts);

	std::vector<double> face_normals;
	cell->normals(face_normals);

	std::vector<int> face_orders;
	cell->face_orders(face_orders);

	std::vector<int> face_verts;
	cell->face_vertices(face_verts);

	noob::basic_mesh results;

	uint32_t accum = 0; // This accumulates the face_orders counter, so you can quickly tap into face_verts.
	for (uint32_t face_num = 0; face_num < num_faces; ++face_num)
	{
		const size_t num_face_verts = face_orders[face_num];

		const noob::vec3 normal(face_normals[face_num * 3], face_normals[3 * face_num + 1], face_normals[3 * face_num + 2]);

		// Now, we duplicate verts belonging to different faces.
		std::vector<uint32_t> output_face_indices;
		for (uint32_t face_vert_num = 0; face_vert_num < num_face_verts; ++face_vert_num)
		{
			const noob::vec3 vert(verts[accum + (3 * face_vert_num)], verts[accum + (3 * face_vert_num) + 1], verts[accum + (3 * face_vert_num) + 2]);

			results.vertices.push_back(vert);
			results.normals.push_back(normal);

			assert(results.vertices.size() == results.normals.size() && "Vertex and normal arrays should be the same size!");

			output_face_indices.push_back(results.vertices.size() - 1);
		}

		// Now, triangulate via triangle fan:
		for (uint32_t i = 2; i < num_face_verts; ++i)
		{
			results.indices.push_back(output_face_indices[0]);
			results.indices.push_back(output_face_indices[i]);
			results.indices.push_back(output_face_indices[i - 1]);
		}

		accum += num_face_verts*3;
	}

	fmt::MemoryWriter ww;
	ww << "[MeshUtils] Voro to mesh success! Num verts " << results.vertices.size() << ", num indices = " << results.indices.size();
	logger::log(ww.str());

	return results;
}
