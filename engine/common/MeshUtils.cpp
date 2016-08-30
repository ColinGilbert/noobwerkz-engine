#include "MeshUtils.hpp"
#include "ActiveMesh.hpp"

#include <LinearMath/btConvexHull.h>

#include <algorithm>

#include <Eigen/Geometry> 

// icosphere code https://github.com/MicBosi/VisualizationLibrary
noob::basic_mesh noob::mesh_utils::sphere(float radius, uint32_t detail_arg)
{
	const float X = 0.525731112119133606;
	const float Z = 0.850650808352039932;
	std::vector<noob::vec3> verts;
	verts.push_back(noob::vec3(-X, 0.0, Z));
	verts.push_back(noob::vec3(X, 0.0, Z));
	verts.push_back(noob::vec3(-X, 0.0, -Z));
	verts.push_back(noob::vec3(X, 0.0, -Z));
	verts.push_back(noob::vec3(0.0, Z, X));
	verts.push_back(noob::vec3(0.0, Z, -X));
	verts.push_back(noob::vec3(0.0, -Z, X));
	verts.push_back(noob::vec3(0.0, -Z, -X));
	verts.push_back(noob::vec3(Z, X, 0.0));
	verts.push_back(noob::vec3(-Z, X, 0.0));
	verts.push_back(noob::vec3(Z, -X, 0.0));
	verts.push_back(noob::vec3(-Z, -X, 0.0));

	uint32_t idxs[] = {
		1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4, 
		1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2, 
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7 };

	std::vector<uint32_t> indices;
	for(uint32_t i = 0; i < 4*5*3; ++i)
	{
		indices.push_back(idxs[i]);
	}


	// Triangulate the icosahedron
	const uint32_t detail = std::min(static_cast<uint32_t>(8), detail_arg);

	for(int i=0; i < detail; ++i)
	{
		std::vector<uint32_t> indices2;
		std::vector<noob::vec3> verts2;
		for(uint32_t j = 0, idx = 0; j < indices.size(); j += 3)
		{
			indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);
			indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);
			indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);
			indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);

			vec3 v1 = verts[ indices[j+0] ]; v1 = noob::normalize(v1);
			vec3 v2 = verts[ indices[j+1] ]; v2 = noob::normalize(v2);
			vec3 v3 = verts[ indices[j+2] ]; v3 = noob::normalize(v3);

			vec3 a = (v1 + v2) * 0.5f; a = noob::normalize(a);
			vec3 b = (v2 + v3) * 0.5f; b = noob::normalize(b);
			vec3 c = (v3 + v1) * 0.5f; c = noob::normalize(c);

			verts2.push_back(v1); verts2.push_back(a); verts2.push_back(c);
			verts2.push_back(a); verts2.push_back(v2); verts2.push_back(b);
			verts2.push_back(a); verts2.push_back(b); verts2.push_back(c);
			verts2.push_back(c); verts2.push_back(b); verts2.push_back(v3);
		}
		verts = verts2;
		indices = indices2;
	}

	noob::basic_mesh results;

	// results.vertices.reserve(verts.size());
	// results.normals.reserve(verts.size());
	// results.indices.reserve(indices.size());

	for (uint32_t i = 0; i < verts.size(); ++i)
	{
		const noob::vec3 v = verts[i];
		results.vertices.push_back(v*radius);
		noob::vec3 n = noob::normalize(v);
		results.normals.push_back(n);
	}

	for(uint32_t i : indices)
	{
		results.indices.push_back(i);
	}


	return results;
}


noob::basic_mesh noob::mesh_utils::box(float width, float height, float depth)
{
	/*
	   1--------0 
	   |\       |\
	   | 5------|-4
	   2--------3 |
	   \|       \| 
	   6------- 7 
	   */

	const float x = width * 0.5;
	const float y = height * 0.5;
	const float z = depth * 0.5;


	noob::vec3 a0(+x,+y,+z);
	noob::vec3 a1(-x,+y,+z);
	noob::vec3 a2(-x,-y,+z);
	noob::vec3 a3(+x,-y,+z);
	noob::vec3 a4(+x,+y,-z);
	noob::vec3 a5(-x,+y,-z);
	noob::vec3 a6(-x,-y,-z);
	noob::vec3 a7(+x,-y,-z);


	noob::basic_mesh results;
	results.vertices = {
		a1, a2, a3, a3, a0, a1,
		a2, a6, a7, a7, a3, a2,
		a6, a5, a4, a4, a7, a6,
		a5, a1, a0, a0, a4, a5,
		a0, a3, a7, a7, a4, a0,
		a5, a6, a2, a2, a1, a5
	};

	results.normals.resize(6*4);
	results.indices.resize(6*6);


	// Build the top

	results.vertices[0] = a0;
	results.vertices[1] = a1;
	results.vertices[2] = a4;
	results.vertices[3] = a5;


	results.normals[0] = results.normals[1] = results.normals[2] = results.normals[3] = noob::vec3(0.0, 1.0, 0.0);

	results.indices[0] = 0;
	results.indices[1] = 3;
	results.indices[2] = 2;

	results.indices[3] = 0;
	results.indices[4] = 1;
	results.indices[5] = 3;


	// Build the back

	results.vertices[4] = a0;
	results.vertices[5] = a1;
	results.vertices[6] = a2;
	results.vertices[7] = a3;

	results.normals[4] = results.normals[5] = results.normals[6] = results.normals[7] = noob::vec3(0.0, 0.0, -1.0);

	results.indices[6] = 4;
	results.indices[7] = 6;
	results.indices[8] = 7;

	results.indices[9] = 4;
	results.indices[10] = 5;
	results.indices[11] = 6;


	// Build the left side (proper left of programmer)

	results.vertices[8] = a1;
	results.vertices[9] = a2;
	results.vertices[10] = a5;
	results.vertices[11] = a6;

	results.normals[8] = results.normals[9] = results.normals[10] = results.normals[11] = noob::vec3(-1.0, 0.0, 0.0);

	results.indices[12] = 8;
	results.indices[13] = 9;
	results.indices[14] = 10;

	results.indices[15] = 10;
	results.indices[16] = 9;
	results.indices[17] = 11;


	// Build the bottom

	results.vertices[12] = a2;
	results.vertices[13] = a3;
	results.vertices[14] = a6;
	results.vertices[15] = a7;

	results.normals[12] = results.normals[13] = results.normals[14] = results.normals[15] = noob::vec3(0.0, -1.0, 0.0);

	results.indices[18] = 12;
	results.indices[19] = 14;
	results.indices[20] = 13;

	results.indices[21] = 13;
	results.indices[22] = 14;
	results.indices[23] = 15;


	// Build the right side (proper right of programmer)

	results.vertices[16] = a0;
	results.vertices[17] = a3;
	results.vertices[18] = a4;
	results.vertices[19] = a7;

	results.normals[16] = results.normals[17] = results.normals[18] = results.normals[19] = noob::vec3(1.0, 0.0, 0.0);

	results.indices[24] = 16;
	results.indices[25] = 18;
	results.indices[26] = 17;

	results.indices[27] = 17;
	results.indices[28] = 18;
	results.indices[29] = 19;


	// Now, finally build the front.

	results.vertices[20] = a4;
	results.vertices[21] = a5;
	results.vertices[22] = a6;
	results.vertices[23] = a7;

	results.normals[20] = results.normals[21] = results.normals[22] = results.normals[21] = noob::vec3(0.0, 0.0, 1.0);

	results.indices[30] = 23;
	results.indices[31] = 22;
	results.indices[32] = 20;

	results.indices[33] = 20;
	results.indices[34] = 21;
	results.indices[35] = 22;

	// Man, that kinda sucked!

	return results;

}


noob::basic_mesh noob::mesh_utils::cone(float radius, float height, uint32_t segments_arg)
{
	const float half_height = height * 0.5;
	const uint32_t segments = std::max(segments_arg, static_cast<uint32_t>(3));
	
	noob::basic_mesh top = noob::mesh_utils::circle(radius, segments);
	noob::basic_mesh bottom = noob::mesh_utils::circle(radius, segments);
	
	const uint32_t num_verts = top.vertices.size();
	const uint32_t num_indices = top.indices.size();

	// Position our vertices
	
	top.vertices[0] = noob::vec3(0.0, half_height, 0.0);
	bottom.vertices[0] = noob::vec3(0.0, -half_height, 0.0);
	
	for (uint32_t i = 1; i < num_verts; ++i)
	{
		noob::vec3 temp(top.vertices[i]);
		temp[1] = -half_height;
		top.vertices[i] = temp;
		bottom.vertices[i] = temp;
	}
	
	// Now, we split the triangles on the top mesh, in order to be able to give each vertex its normal

	noob::basic_mesh results;

	const uint32_t num_split_verts = num_verts * 3;
	
	results.vertices.resize(num_split_verts + num_verts);
	results.normals.resize(num_split_verts + num_verts);
	results.indices.resize(num_indices * 2);

	const noob::vec3 top_origin = top.vertices[0];	
	
	for (uint32_t i = 0; i < num_verts; i += 2)
	{
		results.vertices[i * 3] = top_origin;
		results.vertices[(i * 3) + 1] = top.vertices[i];
		results.vertices[(i * 3) + 2] = top.vertices[i + 1];
	}
	
	// Now, calculate the normals: TODO:
	for (uint32_t i = 0; i < num_verts; i += 2)
	{
		const noob::vec3 first = top.vertices[i] - top_origin;
		const noob::vec3 second = top.vertices[i+1] - top_origin;
		const noob::vec3 n = noob::normalize(noob::cross(first, second));
		
		results.normals[i * 3] = n;
		results.normals[(i * 3) + 1] = n;
		results.normals[(i * 3) + 2] = n;
	}
	
	// Now, add our bottom to the results
	const uint32_t total_verts = results.vertices.size();
	for (uint32_t i = num_split_verts; i < total_verts; ++i)
	{
		results.vertices[i] = bottom.vertices[i - num_split_verts];
		results.normals[i] = noob::vec3(0.0, -1.0, 0.0);
	}
	const uint32_t total_indices = results.indices.size();
	for (uint32_t i = num_indices; i < total_indices; ++i)
	{
		results.indices[i] = bottom.indices[i-num_indices];
	}
	
	// std::fill(bottom.normals.begin(), bottom.normals.end(), noob::vec3(0.0, -1.0, 0.0));

	return results;	
}


noob::basic_mesh noob::mesh_utils::cylinder(float radius, float height, uint32_t segments)
{

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

	for (uint32_t i = 0; i < hull_result.mNumOutputVertices; ++i)
	{
		mesh.vertices.push_back(noob::vec3_from_bullet(hull_result.m_OutputVertices[i]));
	}

	for (uint32_t i = 0; i < hull_result.mNumIndices; ++i)
	{
		mesh.indices.push_back(static_cast<uint16_t>(hull_result.m_Indices[i]));
	}

	return mesh;
}


noob::basic_mesh noob::mesh_utils::circle(float radius, uint32_t segments_arg)
{
	const uint32_t segments = std::max(segments_arg, static_cast<uint32_t>(3));
	const float increment_amount = TWO_PI / segments;
	const Eigen::Vector3f p(radius, 0.0, radius);

	noob::basic_mesh results;
	results.vertices.reserve(segments + 1);
	results.normals.reserve(segments + 1);
	const uint32_t num_indices = segments * 3;
	results.indices.reserve(num_indices);

	std::fill(results.normals.begin(), results.normals.end(), noob::vec3(0.0, 1.0, 0.0));
	
	// Our origin :)
	results.vertices[0] = noob::vec3(0.0, 0.0, 0.0);

	for (uint32_t seg = 1; seg < segments; ++seg)
	{
		const float diff = increment_amount * static_cast<float>(seg);
		const Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());
		const Eigen::Vector3f rotated_point = angle_axis * p;
		noob::vec3 pt = noob::vec3_from_eigen_vec3(rotated_point);
		results.vertices[seg] = pt;
	}
	
	uint32_t accum = 1;;
	for (uint32_t i = 0; i < num_indices; i += 3)
	{
		results.indices[i] = 0;
		results.indices[i + 1] = accum;
		++accum;
		results.indices[i + 2] = accum;
	}

	return results;
}
