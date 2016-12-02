
#include "MeshUtils.hpp"

#include <algorithm>

#include <Eigen/Geometry> 
#include <LinearMath/btConvexHull.h>

#include "NoobUtils.hpp"
#include "voro++.hh"

// icosphere code https://github.com/MicBosi/VisualizationLibrary
// LICENSE:
/*
Visualization Library
http://visualizationlibrary.org 

Copyright (c) 2005-2011, Michele Bosi
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
		results.vertices.push_back(v * radius);
		noob::vec3 n = noob::normalize(v);
		results.normals.push_back(n);
		results.colours.push_back(noob::vec4(1.0, 1.0, 1.0, 1.0));
	}

	for(uint32_t i : indices)
	{
		results.indices.push_back(i);
	}

//	results.bbox.min = noob::vec3(-radius, -radius, -radius);
//	results.bbox.max = noob::vec3(radius, radius, radius);

	results.calculate_dims();
	return results;
}


noob::basic_mesh noob::mesh_utils::box(float width, float height, float depth)
{

	const double x = width * 0.5;
	const double y = height * 0.5;
	const double z = depth * 0.5;

	voro::voronoicell cell;
	cell.init(-x, x, -y, y, -z, z);

	noob::basic_mesh results;
	
	const int num_faces = cell.number_of_faces();

	std::vector<double> verts;
	cell.vertices(verts);

	std::vector<double> face_normals;
	cell.normals(face_normals);

	std::vector<int> face_vert_indices;
	cell.face_vertices(face_vert_indices);

	std::vector<int> face_orders; 
	cell.face_orders(face_orders);

	// TODO: Continue here.
	for (int i = 0; i < num_faces; ++i)
	{
		
	}


	results.bbox.min = noob::vec3(-x, -y, -z);
	results.bbox.max = noob::vec3(x, y, z);

	return results;

}

/*
noob::basic_mesh noob::mesh_utils::cone(float radius, float height, uint32_t segments_arg)
{
	const float half_height = height * 0.5;
	const uint32_t segments = std::max(segments_arg, static_cast<uint32_t>(3));
	
	noob::basic_mesh top = noob::mesh_utils::circle(radius, segments);
	noob::basic_mesh bottom = noob::mesh_utils::circle(radius, segments);
	
	const uint32_t num_verts = top.vertices.size();
	const uint32_t num_indices = top.indices.size();

	{
	fmt::MemoryWriter ww;
	ww << "[MeshUtils] Cone mesh circles verts = " << num_verts << ", indices = " << num_indices;
	logger::log(ww.str());
	}
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

	results.bbox.min = noob::vec3(-radius, -half_height, -radius);
	results.bbox.max = noob::vec3(radius, half_height, radius);

	return results;	
}


noob::basic_mesh noob::mesh_utils::cylinder(float radius, float height, uint32_t segments)
{
	// TODO: Implement!
	return noob::mesh_utils::cone(radius, height, segments);
}
*/

noob::basic_mesh noob::mesh_utils::hull(const std::vector<noob::vec3>& points)
{
	// TODO: Optimize this	
	std::vector<btVector3> bt_points;
	
	noob::basic_mesh mesh;
	
	noob::bbox accum;
	for (noob::vec3 p : points)
	{
		accum.min[0] = std::min(accum.min[0], p[0]);
		accum.min[1] = std::min(accum.min[1], p[1]);
		accum.min[2] = std::min(accum.min[2], p[2]);
		
		accum.max[0] = std::max(accum.max[0], p[0]);
		accum.max[1] = std::max(accum.max[1], p[1]);
		accum.max[2] = std::max(accum.max[2], p[2]);


		bt_points.push_back(btVector3(p[0], p[1], p[2]));
	}

	mesh.bbox = accum;//.max - accum.min;//noob::vec3();//accum.max + accum.min

	HullDesc hull_desc(QF_DEFAULT, points.size(), &bt_points[0]);

	HullLibrary hull_lib;
	HullResult hull_result;

	HullError error_msg = hull_lib.CreateConvexHull(hull_desc, hull_result);
	
	if (error_msg == HullError::QE_FAIL) 
	{
		logger::log(noob::importance::ERROR, "FAILED TO CREATE CONVEX HULL. WTF?");
	}

	for (uint32_t i = 0; i < hull_result.mNumOutputVertices; ++i)
	{
		mesh.vertices.push_back(noob::vec3_from_bullet(hull_result.m_OutputVertices[i]));
		mesh.colours.push_back(noob::vec4(1.0, 1.0, 1.0, 1.0));
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
	const float increment_amount = NOOB_TWO_PI / segments;
	const Eigen::Vector3f p(radius, 0.0, radius);

	noob::basic_mesh results;
	results.vertices.reserve(segments + 1);
	results.normals.reserve(segments + 1);
	const uint32_t num_indices = segments * 3;
	// results.indices.reserve(num_indices);

	std::fill(results.normals.begin(), results.normals.end(), noob::vec3(0.0, 1.0, 0.0));
	
	// Our origin :)
	results.vertices[0] = noob::vec3(0.0, 0.0, 0.0);

	for (uint32_t seg = 1; seg < segments; ++seg)
	{
		const float diff = increment_amount * static_cast<float>(seg);
		const Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());
		const Eigen::Vector3f rotated_point = angle_axis * p;
		results.vertices.push_back(noob::vec3_from_eigen(rotated_point));
		results.colours.push_back(noob::vec4(1.0, 1.0, 1.0, 1.0));
	}
	
	uint32_t accum = 1;;
	for (uint32_t i = 0; i < num_indices; i += 3)
	{
		results.indices.push_back(0);
		results.indices.push_back(accum);
		++accum;
		results.indices.push_back(accum);
	}
	
	results.bbox.min = noob::vec3(-radius, 0.0, -radius);
	results.bbox.max = noob::vec3(radius, 0.0 , radius);

	return results;
}
