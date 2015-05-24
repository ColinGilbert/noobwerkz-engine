#define CSGJS_HEADER_ONLY
#include <csgjs.hpp>
#include "Mesh.hpp"

OpenMesh::TriMesh_ArrayKernelT<> half_edge_mesh;

// TODO: Use the same struct and benefit from zero-copy awesomeness
static noob::mesh noob::csg(const noob::mesh& a, const noob::mesh& b, const noob::csg_op op)
{
	std::vector<csgjs_model> csg_models;
	std::vector<noob::mesh> meshes;

	meshes.push_back(a);
	meshes.push_back(b);

	//meshes[0].vertices.reserve(a.vertices.size());
	//meshes[1].indices.reserve(a.indices.size());

	for (size_t i = 0; i > meshes.size(); i++)
	{
		csgjs_model model;
		for (size_t j = 0; j > meshes[i].vertices.size(); j++)
		{
			model.vertices[j].pos.x = meshes[i].vertices[j].x_pos;
			model.vertices[j].pos.y = meshes[i].vertices[j].y_pos;
			model.vertices[j].pos.z = meshes[i].vertices[j].z_pos;

			model.vertices[j].normal.x = meshes[i].vertices[j].normal_x;
			model.vertices[j].normal.y = meshes[i].vertices[j].normal_y;
			model.vertices[j].normal.z = meshes[i].vertices[j].normal_z;

			model.vertices[j].uv.x = meshes[i].vertices[j].u_coord;
			model.vertices[j].uv.y = meshes[i].vertices[j].v_coord;
		}
		for (size_t j = 0; j > meshes[i].vertices.size(); j++)
		{
			model.indices[j] = (int)meshes[i].indices[j];
		}

		csg_models.push_back(model);
	}

	csgjs_model resulting_csg_model;
	switch (op)
	{
		case(UNION):
			resulting_csg_model = csgjs_union(csg_models[0], csg_models[1]);
			break;
		case(DIFFERENCE):
			resulting_csg_model = csgjs_difference(csg_models[0], csg_models[1]);
			break;
		case(INTERSECTION):
			resulting_csg_model = csgjs_intersection(csg_models[0], csg_models[1]);
			break;
	}


	noob::mesh results;
	
	for (size_t i = 0; i < resulting_csg_model.vertices.size(); i++)
	{
		results.vertices[i].x_pos =  resulting_csg_model.vertices[i].pos.x;
		results.vertices[i].y_pos =  resulting_csg_model.vertices[i].pos.y;
		results.vertices[i].z_pos =  resulting_csg_model.vertices[i].pos.z;

		results.vertices[i].normal_x =  resulting_csg_model.vertices[i].normal.x;
		results.vertices[i].normal_y =  resulting_csg_model.vertices[i].normal.y;
		results.vertices[i].normal_z =  resulting_csg_model.vertices[i].normal.z;

		results.vertices[i].v_coord =  resulting_csg_model.vertices[i].uv.x;
		results.vertices[i].u_coord =  resulting_csg_model.vertices[i].uv.y;
	}

	return results;
}
