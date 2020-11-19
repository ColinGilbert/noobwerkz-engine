// Header file we're implementing
#include "MeshUtils.hpp"

// std
#include <algorithm>

// External libs
#include <Eigen/Geometry> 
#include <LinearMath/btConvexHull.h>
#include <voro++.hh>

// Project-local
#include "Logger.hpp"

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
noob::mesh_3d noob::mesh_utils::sphere(float radius, uint32_t detail_arg)
{
	const float X = 0.525731112119133606;
	const float Z = 0.850650808352039932;

	std::vector<noob::vec3f> verts;

	verts.push_back(noob::vec3f(-X, 0.0, Z));
	verts.push_back(noob::vec3f(X, 0.0, Z));
	verts.push_back(noob::vec3f(-X, 0.0, -Z));
	verts.push_back(noob::vec3f(X, 0.0, -Z));
	verts.push_back(noob::vec3f(0.0, Z, X));
	verts.push_back(noob::vec3f(0.0, Z, -X));
	verts.push_back(noob::vec3f(0.0, -Z, X));
	verts.push_back(noob::vec3f(0.0, -Z, -X));
	verts.push_back(noob::vec3f(Z, X, 0.0));
	verts.push_back(noob::vec3f(-Z, X, 0.0));
	verts.push_back(noob::vec3f(Z, -X, 0.0));
	verts.push_back(noob::vec3f(-Z, -X, 0.0));

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
		std::vector<noob::vec3f> verts2;
		for(uint32_t j = 0, idx = 0; j < indices.size(); j += 3)
		{
			indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);
			indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);
			indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);
			indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);

			noob::vec3f v1 = verts[ indices[j+0] ]; v1 = noob::normalize(v1);
			noob::vec3f v2 = verts[ indices[j+1] ]; v2 = noob::normalize(v2);
			noob::vec3f v3 = verts[ indices[j+2] ]; v3 = noob::normalize(v3);

			noob::vec3f a = (v1 + v2) * 0.5f; a = noob::normalize(a);
			noob::vec3f b = (v2 + v3) * 0.5f; b = noob::normalize(b);
			noob::vec3f c = (v3 + v1) * 0.5f; c = noob::normalize(c);

			verts2.push_back(v1); verts2.push_back(a); verts2.push_back(c);
			verts2.push_back(a); verts2.push_back(v2); verts2.push_back(b);
			verts2.push_back(a); verts2.push_back(b); verts2.push_back(c);
			verts2.push_back(c); verts2.push_back(b); verts2.push_back(v3);
		}
		verts = verts2;
		indices = indices2;
	}

	noob::mesh_3d results;

	// results.position.reserve(verts.size());
	// results.normal.reserve(verts.size());
	// results.indices.reserve(indices.size());

	for (uint32_t i = 0; i < verts.size(); ++i)
	{
		noob::mesh_3d::vert vv;
		vv.position = verts[i] * radius;
		vv.normal = noob::normalize(verts[i]);
		vv.colour = noob::vec4f(1.0, 1.0, 1.0, 1.0);

		results.vertices.push_back(vv);

	}

	for(uint32_t i : indices)
	{
		results.indices.push_back(i);
	}

	//	results.bbox.min = noob::vec3f(-radius, -radius, -radius);
	//	results.bbox.max = noob::vec3f(radius, radius, radius);

	results.calculate_dims();
	return results;
}

// Box code adapted from https://github.com/danginsburg/opengles3-book/blob/master/Common/Source/esShapes.c
// I recommend anyone serious about OpenGLES get a copy of Dan Ginsburg's excellent book!
// 
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// License below:


// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

noob::mesh_3d noob::mesh_utils::box(float width, float height, float depth)
{
	const uint32_t num_vertices = 24;
	const uint32_t num_indices = 36;

	noob::mesh_3d results;
	/*
	   noob::vec3f point_a(-0.5*width, 0.5*height, -0.5*depth);
	   noob::vec3f point_b(0.5*width, 0.5*height, -0.5*depth);
	   noob::vec3f point_c(0.5*width, 0.5*height, 0.5*depth);
	   noob::vec3f point_d(-0.5*width, 0.5*height, 0.5*depth);
	   noob::vec3f point_e(-0.5*width, -0.5*height, -0.5*depth);
	   noob::vec3f point_f(0.5*width, -0.5*height, -0.5*depth);
	   noob::vec3f point_g(0.5*width, -0.5*height, 0.5*depth);
	   noob::vec3f point_h(-0.5*width, -0.5*height, 0.5*depth);

	   results.position.push_back(point_a);
	   results.position.push_back(point_b);
	   results.position.push_back(point_c);
	   results.verts.push_back(point_d);
	   results.push_back(point_e);
	   results.push_back(point_f);
	   results.push_back(point_g);
	   results.push_back(point_h);

	 */	

	const float cube_verts[] =
	{
		-0.5f * width, -0.5f * height, -0.5f * depth,
		-0.5f * width, -0.5f * height,  0.5f * depth,
		0.5f * width, -0.5f * height,  0.5f * depth,
		0.5f * width, -0.5f * height, -0.5f * depth,
		-0.5f * width,  0.5f * height, -0.5f * depth,
		-0.5f * width,  0.5f * height,  0.5f * depth,
		0.5f * width,  0.5f * height,  0.5f * depth,
		0.5f * width,  0.5f * height, -0.5f * depth,
		-0.5f * width, -0.5f * height, -0.5f * depth,
		-0.5f * width,  0.5f * height, -0.5f * depth,
		0.5f * width,  0.5f * height, -0.5f * depth,
		0.5f * width, -0.5f * height, -0.5f * depth,
		-0.5f * width, -0.5f * height, 0.5f * depth,
		-0.5f * width,  0.5f * height, 0.5f * depth,
		0.5f * width,  0.5f * height, 0.5f * depth,
		0.5f * width, -0.5f * height, 0.5f * depth,
		-0.5f * width, -0.5f * height, -0.5f * depth,
		-0.5f * width, -0.5f * height,  0.5f * depth,
		-0.5f * width,  0.5f * height,  0.5f * depth,
		-0.5f * width,  0.5f * height, -0.5f * depth,
		0.5f * width, -0.5f * height, -0.5f * depth,
		0.5f * width, -0.5f * height,  0.5f * depth,
		0.5f * width,  0.5f * height,  0.5f * depth,
		0.5f * width,  0.5f * height, -0.5f * depth,
	};

	const float cube_normals[] =
	{
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};

	for (uint32_t i = 0; i < num_vertices * 3; i += 3)
	{
		noob::mesh_3d::vert vv;

		vv.position = noob::vec3f(cube_verts[i], cube_verts[i + 1], cube_verts[i + 2]);
		vv.normal = noob::vec3f(cube_normals[i], cube_normals[i + 1], cube_normals[i + 2]);
		vv.colour = noob::vec4f(1.0, 1.0, 1.0, 1.0);

		results.vertices.push_back(vv);
	}

	/*
	   const float cube_tex[] =
	   {
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	   1.0f, 0.0f,
	   1.0f, 1.0f,
	   0.0f, 1.0f,
	   0.0f, 0.0f,
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	   };

	   for (uint32_t i = 0; i < num_vertices * 2; i += 2)
	   {
	   results.position[i/2].texcoords = noob::vec3f(cube_tex[i], cube_tex[i+1], 0);
	   }
	 */
	const uint32_t cube_indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 15, 14,
		12, 14, 13,
		16, 17, 18,
		16, 18, 19,
		20, 23, 22,
		20, 22, 21
	};

	for (uint32_t i = 0; i < num_indices; ++i)
	{
		results.indices.push_back(cube_indices[i]);
	}

	results.bbox.min = noob::vec3f(-0.5f * width, -0.5f * height, -0.5f * depth);
	results.bbox.max = noob::vec3f(0.5f * width, 0.5f * height, 0.5f * depth);

	return results;
}


// This code is adapted from http://www.songho.ca/opengl/gl_cylinder.html
noob::mesh_3d noob::mesh_utils::cylinder(float radius, float height, uint32_t segments, bool narrowed_end, float narrow_to) {
	// Generate a unit circle on XY-plane
	const float PI = 3.1415926f;
	const float sector_step = 2 * PI / static_cast<float>(segments);
	float sector_angle;  // radians

	noob::mesh_3d results;
	std::vector<noob::vec3f> unit_circle_vertices;
	for(int i = 0; i <= segments; ++i)
	{
		sector_angle = i * sector_step;
		unit_circle_vertices.push_back(noob::vec3f(std::cos(sector_angle), std::sin(sector_angle), 0.0));
	}

	for(int i = 0; i < 2; ++i)
	{
		const float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		const float t = 1.0f - i;                              // vertical tex coord; 1 to 0

		const bool narrow_me = (i == 0 && narrowed_end);
		for(uint32_t j = 0, k = 0; j <= segments; ++j)
		{

			noob::vec3f temp = unit_circle_vertices[j];
			const float ux = temp.v[0];
			const float uy = temp.v[1];
			const float uz = temp.v[2];

			noob::mesh_3d::vert v;
			if (narrow_me)
			{
				v.position = noob::vec3f(ux * std::fabs(narrow_to), uy * fabs(narrow_to), h);
			} else
			{
				v.position = noob::vec3f(ux * radius, uy * radius, h);
			}
			v.normal = noob::vec3f(ux, uy, uz);
			v.colour = noob::vec4f(1.0, 1.0, 1.0, 1.0);
			v.texcoords = noob::vec3f(static_cast<float>(j) / static_cast<float>(segments), t, 0.0);

			results.vertices.push_back(v);

		}
	}

	// The starting index for the base/top surface
	// NOTE: It is used for generating indices later
	uint32_t base_center_index = results.vertices.size();
	uint32_t top_center_index = base_center_index + segments + 1;

	// Put base and top vertices to arrays
	for (uint32_t i = 0; i < 2; ++i)
	{
		const bool narrow_me = (i == 0 && narrowed_end);

		float h = -height / 2.0f + static_cast<float>(i) * height;
		float nz = -1.0 + static_cast<float>(i) * 2.0;

		noob::mesh_3d::vert center;
		// Center point
		center.position = noob::vec3f(0.0, 0.0, h);
		center.normal = noob::vec3f(0.0, 0.0, nz);
		center.texcoords = noob::vec3f(0.5, 0.5, 0.0);
		center.colour = noob::vec4f(1.0, 1.0, 1.0, 1.0);
		results.vertices.push_back(center);

		for (uint32_t j = 0, k = 0; j < segments; ++j)
		{

			float ux = unit_circle_vertices[j].v[0];
			float uy = unit_circle_vertices[j].v[1];
			noob::mesh_3d::vert v;

			if (narrow_me)
			{
				v.position = noob::vec3f(ux * narrow_to, uy * narrow_to, h);
			}
			else
			{
				v.position = noob::vec3f(ux * radius, uy * radius, h);
			}			


			v.normal = noob::vec3f(0.0, 0.0, nz);
			v.colour = noob::vec4f(1.0, 1.0, 1.0, 1.0);
			v.texcoords = noob::vec3f(-ux * 0.5 + 0.5, -uy * 0.5f + 0.5f, 0.0);
			results.vertices.push_back(v);
		}
	}

	uint32_t k1 = 0;
	uint32_t k2 = segments + 1;

	// Indices for the side surface
	for(uint32_t i = 0; i < segments; ++i, ++k1, ++k2)
	{
		// 2 triangles per sector
		// k1 => k1+1 => k2
		results.indices.push_back(k1);
		results.indices.push_back(k1 + 1);
		results.indices.push_back(k2);

		// k2 => k1+1 => k2+1
		results.indices.push_back(k2);
		results.indices.push_back(k1 + 1);
		results.indices.push_back(k2 + 1);
	}

	// Indices for the base surface
	for(uint32_t i = 0, k = base_center_index + 1; i < segments; ++i, ++k)
	{
		if(i < segments - 1)
		{
			results.indices.push_back(base_center_index);
			results.indices.push_back(k + 1);
			results.indices.push_back(k);
		}
		else // Last triangle
		{
			results.indices.push_back(base_center_index);
			results.indices.push_back(base_center_index + 1);
			results.indices.push_back(k);
		}
	}

	// Indices for the top surface
	for(int i = 0, k = top_center_index + 1; i < segments; ++i, ++k)
	{
		if(i < segments - 1)
		{
			results.indices.push_back(top_center_index);
			results.indices.push_back(k);
			results.indices.push_back(k + 1);
		}
		else // Last triangle
		{
			results.indices.push_back(top_center_index);
			results.indices.push_back(k);
			results.indices.push_back(top_center_index + 1);
		}
	}

	return results;

}

noob::mesh_3d noob::mesh_utils::cone(float radius, float height, uint32_t segments)
{
	return cylinder(radius, height, segments, true, 0.0);
}


/*
   noob::mesh_3d noob::mesh_utils::voro(float width, float height, float depth)
   {
   const double x = width * 0.5;
   const double y = height * 0.5;
   const double z = depth * 0.5;

   voro::voronoicell cell;
   cell.init(-x, x, -y, y, -z, z);

   noob::mesh_3d results;

   std::vector<double> verts;
   cell.vertices(verts);

   std::vector<double> face_normals;
   cell.normals(face_normals);

   std::vector<int> face_orders; 
   cell.face_orders(face_orders);

   std::vector<int> face_vert_indices;
   cell.face_vertices(face_vert_indices);

// Using the number of values in face_order to tell us how many faces we have.
for (uint32_t i = 0; i < face_orders.size(); ++i)
{

}


results.bbox.min = noob::vec3f(-x, -y, -z);
results.bbox.max = noob::vec3f(x, y, z);

return results;

}
 */

/*
   noob::mesh_3d noob::mesh_utils::cone(float radius, float height, uint32_t segments_arg)
   {
   const float half_height = height * 0.5;
   const uint32_t segments = std::max(segments_arg, static_cast<uint32_t>(3));

   noob::mesh_3d top = noob::mesh_utils::circle(radius, segments);
   noob::mesh_3d bottom = noob::mesh_utils::circle(radius, segments);

   const uint32_t num_verts = top.vertices.size();
   const uint32_t num_indices = top.indices.size();

   {
   fmt::MemoryWriter ww;
   ww << "[MeshUtils] Cone mesh circles verts = " << num_verts << ", indices = " << num_indices;
   logger::log(ww.str());
   }
// Position our vertices

top.vertices[0] = noob::vec3f(0.0, half_height, 0.0);
bottom.vertices[0] = noob::vec3f(0.0, -half_height, 0.0);

for (uint32_t i = 1; i < num_verts; ++i)
{
noob::vec3f temp(top.vertices[i]);
temp[1] = -half_height;
top.vertices[i] = temp;
bottom.vertices[i] = temp;
}

// Now, we split the triangles on the top mesh, in order to be able to give each vertex its normal

noob::mesh_3d results;

const uint32_t num_split_verts = num_verts * 3;

results.position.resize(num_split_verts + num_verts);
results.normal.resize(num_split_verts + num_verts);
results.indices.resize(num_indices * 2);

const noob::vec3f top_origin = top.vertices[0];	

for (uint32_t i = 0; i < num_verts; i += 2)
{
results.position[i * 3] = top_origin;
results.position[(i * 3) + 1] = top.vertices[i];
results.position[(i * 3) + 2] = top.vertices[i + 1];
}

// Now, calculate the normals: TODO:
for (uint32_t i = 0; i < num_verts; i += 2)
{
const noob::vec3f first = top.vertices[i] - top_origin;
const noob::vec3f second = top.vertices[i+1] - top_origin;
const noob::vec3f n = noob::normalize(noob::cross(first, second));

results.normal[i * 3] = n;
results.normal[(i * 3) + 1] = n;
results.normal[(i * 3) + 2] = n;
}

// Now, add our bottom to the results
const uint32_t total_verts = results.position.size();
for (uint32_t i = num_split_verts; i < total_verts; ++i)
{
results.position[i] = bottom.vertices[i - num_split_verts];
results.normal[i] = noob::vec3f(0.0, -1.0, 0.0);
}
const uint32_t total_indices = results.indices.size();
for (uint32_t i = num_indices; i < total_indices; ++i)
{
results.indices[i] = bottom.indices[i-num_indices];
}

// std::fill(bottom.normals.begin(), bottom.normals.end(), noob::vec3f(0.0, -1.0, 0.0));

results.bbox.min = noob::vec3f(-radius, -half_height, -radius);
results.bbox.max = noob::vec3f(radius, half_height, radius);

return results;	
}


noob::mesh_3d noob::mesh_utils::cylinder(float radius, float height, uint32_t segments)
{
	// TODO: Implement!
	return noob::mesh_utils::cone(radius, height, segments);
}
*/

noob::mesh_3d noob::mesh_utils::hull(const std::vector<noob::vec3f>& points)
{
	// TODO: Optimize this	
	std::vector<btVector3> bt_points;

	noob::mesh_3d mesh;

	noob::bbox_type<float> accum;
	for (noob::vec3f p : points)
	{
		accum.min[0] = std::min(accum.min[0], p[0]);
		accum.min[1] = std::min(accum.min[1], p[1]);
		accum.min[2] = std::min(accum.min[2], p[2]);

		accum.max[0] = std::max(accum.max[0], p[0]);
		accum.max[1] = std::max(accum.max[1], p[1]);
		accum.max[2] = std::max(accum.max[2], p[2]);


		bt_points.push_back(btVector3(p[0], p[1], p[2]));
	}

	mesh.bbox = accum;//.max - accum.min;//noob::vec3f();//accum.max + accum.min

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
		noob::mesh_3d::vert vv;
		vv.position = noob::vec3f_from_bullet(hull_result.m_OutputVertices[i]);
		vv.colour = noob::vec4f(1.0, 1.0, 1.0, 1.0);
		mesh.vertices.push_back(vv);
	}

	for (uint32_t i = 0; i < hull_result.mNumIndices; ++i)
	{
		mesh.indices.push_back(static_cast<uint16_t>(hull_result.m_Indices[i]));
	}

	return mesh;
}


noob::mesh_3d noob::mesh_utils::circle(float radius, uint32_t segments_arg)
{
	const uint32_t segments = std::max(segments_arg, static_cast<uint32_t>(3));
	const float increment_amount = NOOB_TWO_PI / segments;
	const Eigen::Vector3f p(radius, 0.0, radius);

	noob::mesh_3d results;
	//results.position.reserve(segments + 1);
	// results.normal.reserve(segments + 1);
	const uint32_t num_indices = segments * 3;
	// results.indices.reserve(num_indices);

	// std::fill(results.normal.begin(), results.normal.end(), noob::vec3f(0.0, 1.0, 0.0));

	noob::mesh_3d::vert vv;

	vv.normal = noob::vec3f(0.0, 1.0, 0.0);
	vv.position = noob::vec3f(0.0, 0.0, 0.0);
	vv.colour = noob::vec4f(1.0, 1.0, 1.0, 1.0);

	results.vertices.push_back(vv);

	for (uint32_t seg = 1; seg < segments; ++seg)
	{
		const float diff = increment_amount * static_cast<float>(seg);
		const Eigen::AngleAxis<float> angle_axis(diff, Eigen::Vector3f::UnitY());
		const Eigen::Vector3f rotated_point = angle_axis * p;
		vv.position = noob::vec3f_from_eigen(rotated_point);
		results.vertices.push_back(vv);
	}

	uint32_t accum = 1;;
	for (uint32_t i = 0; i < num_indices; i += 3)
	{
		results.indices.push_back(0);
		results.indices.push_back(accum);
		++accum;
		results.indices.push_back(accum);
	}

	results.bbox.min = noob::vec3f(-radius, 0.0, -radius);
	results.bbox.max = noob::vec3f(radius, 0.0 , radius);

	return results;
}
