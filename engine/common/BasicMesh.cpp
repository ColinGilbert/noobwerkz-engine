// TODO: Remove the following comment if it works across all builds
//#define CSGJS_HEADER_ONLY
//#include <csgjs.hpp>
//


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <iostream>

#include "Logger.hpp"
#include "BasicMesh.hpp"
#include "format.h"

//#include "VHACD.h"


#include <Eigen/Geometry>

noob::vec3 noob::basic_mesh::get_vertex(unsigned int i)
{
	if (i > vertices.size())
	{
		logger::log("[BasicMesh] - Attempting to get invalid vertex");
		return vertices[0];
	}
	else return vertices[i];
}


noob::vec3 noob::basic_mesh::get_normal(unsigned int i)
{
	if (i > normals.size())
	{
		logger::log("[BasicMesh] - Attempting to get invalid normal");
		return normals[0];
	}
	else return normals[i];
}


noob::vec3 noob::basic_mesh::get_texcoord(unsigned int i)
{
	if (i > texcoords.size())
	{
		logger::log("[BasicMesh] - Attempting to get invalid texcoord");
		return texcoords[0];
	}
	else return texcoords[i];
}


unsigned int noob::basic_mesh::get_index(unsigned int i)
{
	if (i > indices.size())
	{
		logger::log("[BasicMesh] - Attempting to get invalid index");
		return indices[0];
	}
	else return indices[i];

}


void noob::basic_mesh::set_vertex(unsigned int i, const noob::vec3& v)
{
	if (i > vertices.size())
	{
		logger::log("[BasicMesh] - Attempting to set invalid vertex");
	}
	else vertices[i] = v;
}


void noob::basic_mesh::set_normal(unsigned int i, const noob::vec3& v)
{
	if (i > normals.size())
	{
		logger::log("[BasicMesh] - Attempting to set invalid normal");
	}
	else normals[i] = v;
}


void noob::basic_mesh::set_texcoord(unsigned int i, const noob::vec3& v)
{
	if (i > texcoords.size())
	{
		logger::log("[BasicMesh] - Attempting to set invalid texcoord");
	}
	else texcoords[i] = v;
}


void noob::basic_mesh::set_index(unsigned int i, unsigned int v)
{
	if (i > indices.size())
	{
		logger::log("[BasicMesh] - Attempting to set invalid index");
	}
	else indices[i] = static_cast<uint32_t>(v);

}


double noob::basic_mesh::get_volume()
{
	if (!volume_calculated)
	{
		// Proudly gleaned from U of R website!
		// http://mathcentral.uregina.ca/QQ/database/QQ.09.09/h/ozen1.html
		// Woot alma mater! (... Soon :P)
		// The volume of the tetrahedron with vertices (0 ,0 ,0), (a1 ,a2 ,a3), (b1, b2, b3) and (c1, c2, c3) is [a1b2c3 + a2b3c1 + a3b1c2 - a1b3c2 - a2b1c3 - a3b2c1] / 6.
		double accum = 0.0;
		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			noob::vec3 first = vertices[i];
			noob::vec3 second = vertices[i+1];
			noob::vec3 third = vertices[i+2];

			accum += ((static_cast<double>(first.v[0]) * static_cast<double>(second.v[1]) * static_cast<double>(third.v[2])) + (static_cast<double>(first.v[1]) * static_cast<double>(second.v[2]) * static_cast<double>(third.v[0])) + (static_cast<double>(first.v[2]) * static_cast<double>(second.v[0]) * static_cast<double>(third.v[1])) - (static_cast<double>(first.v[0]) * static_cast<double>(second.v[2]) * static_cast<double>(third.v[1])) - (static_cast<double>(first.v[1]) * static_cast<double>(second.v[0]) * static_cast<double>(third.v[2])) - (static_cast<double>(first.v[2]) * static_cast<double>(second.v[1]) * static_cast<double>(third.v[0]))) / 6.0;

		}
		volume_calculated = true;
		return accum;
	}

	return volume;
}


noob::basic_mesh noob::basic_mesh::decimate(size_t num_verts) const
{
	//logger::log("[Mesh] decimating");
	TriMesh half_edges = to_half_edges();
	// logger::log(fmt::format("[Mesh] decimating - Half edges generated. num verts = {0}", half_edges.n_vertices()));

	OpenMesh::Decimater::DecimaterT<TriMesh> decimator(half_edges);
	OpenMesh::Decimater::ModQuadricT<TriMesh>::Handle mod;
	decimator.add(mod);
	decimator.initialize();

	auto did_init = decimator.initialize();
	if (!did_init) 
	{
		// logger::log("[Mesh] decimate() - init failed!");
		return noob::basic_mesh(*this);
	}

	size_t verts_removed = decimator.decimate_to(num_verts);
	logger::log(fmt::format("[Mesh] decimate() - Vertices removed = {0}", verts_removed));
	decimator.mesh().garbage_collection();
	half_edges.garbage_collection();
	noob::basic_mesh temp;
	temp.from_half_edges(half_edges);
	return temp;
}


void noob::basic_mesh::normalize() 
{
	std::string temp = save();
	load_mem(temp);
}

/*
// TODO
void noob::basic_mesh::to_origin()
{
// noob::basic_mesh temp;
for (size_t i = 0; i < vertices.size(); ++i)
{
vertices[i] = (vertices[i] - bbox_info.center);
}
update_normals();

}
*/

std::string noob::basic_mesh::save() const
{
	fmt::MemoryWriter w;
	w << "OFF" << "\n" << vertices.size() << " " << indices.size() / 3 << " " << 0 <<  "\n";
	for (auto v : vertices)
	{
		w << v.v[0] << " " << v.v[1] << " " << v.v[2] <<  "\n";
	}
	for (size_t i = 0; i < indices.size(); i = i + 3)
	{
		w << 3 << " " << indices[i] << " " << indices[i+1] << " " << indices[i+2] << "\n";
	}

	return w.str();
}


void noob::basic_mesh::save(const std::string& filename) const
{
	// logger::log("[Mesh] - save() - begin");
	TriMesh half_edges = to_half_edges();
	// logger::log("[Mesh] - save() - half-edges created");
	OpenMesh::IO::write_mesh(half_edges, filename);
}

bool noob::basic_mesh::load_mem(const std::string& file, const std::string& name)
{
	const aiScene* scene = aiImportFileFromMemory(file.c_str(), file.size(), aiProcessPreset_TargetRealtime_Fast, "");
	return load_assimp(scene, name);
}


bool noob::basic_mesh::load_file(const std::string& filename, const std::string& name)
{
	const aiScene* scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_Fast);
	return load_assimp(scene, name);
}

bool noob::basic_mesh::load_assimp(const aiScene* scene, const std::string& name)
{
	// logger::log("[Mesh] load() - begin");
	if (!scene)
	{
		logger::log(fmt::format("[Mesh] load({0}) - cannot open", name));
		return false;
	}

	vertices.clear();
	indices.clear();
	normals.clear();

	const aiMesh* mesh_data = scene->mMeshes[0];

	
	{
		// fmt::MemoryWriter ww;
		// ww << "[BasicMesh[ Loading " << name << " - Attempting to obtain mesh data";
		// logger::log(ww.str());
	}

	size_t num_verts = mesh_data->mNumVertices;
	size_t num_faces = mesh_data->mNumFaces;
	auto num_indices = mesh_data->mNumFaces / 3;

	bool has_normals = mesh_data->HasNormals();
	
	{
		// fmt::MemoryWriter ww;
		// ww << "[BasicMesh] Loading: " << name << ", mesh has " << num_verts << " verts and " << has_normals << " normals.";
		// logger::log(ww.str());
	}

	double accum_x, accum_y, accum_z = 0.0f;
	bbox.min = bbox.max = bbox.center = noob::vec3(0.0, 0.0, 0.0);	

	for ( size_t n = 0; n < num_verts; ++n)
	{
		aiVector3D pt = mesh_data->mVertices[n];
		noob::vec3 v;
		v.v[0] = pt[0];
		v.v[1] = pt[1];
		v.v[2] = pt[2];
		vertices.push_back(v);

		bbox.min[0] = std::min(bbox.min[0], v[0]);
		bbox.min[1] = std::min(bbox.min[1], v[1]);
		bbox.min[2] = std::min(bbox.min[2], v[2]);

		bbox.max[0] = std::max(bbox.max[0], v[0]);
		bbox.max[1] = std::max(bbox.max[1], v[1]);
		bbox.max[2] = std::max(bbox.max[2], v[2]);

		if (has_normals)
		{
			aiVector3D normal = mesh_data->mNormals[n];
			noob::vec3 norm;
			norm.v[0] = normal[0];
			norm.v[1] = normal[1];
			norm.v[2] = normal[2];
			normals.push_back(norm);
		}
	}

	if (num_verts == 0)
	{
		num_verts = 1;
	}

	bbox.center = noob::vec3((bbox.max[0] + bbox.min[0])/2, (bbox.max[1] + bbox.min[1])/2, (bbox.max[2] + bbox.min[2])/2);

	for (size_t n = 0; n < num_faces; ++n)
	{
		const struct aiFace* face = &mesh_data->mFaces[n];
		indices.push_back(face->mIndices[0]);
		indices.push_back(face->mIndices[1]);
		indices.push_back(face->mIndices[2]);
	}

	aiReleaseImport(scene);

	return true;
}

/*
   void noob::basic_mesh::transform(const noob::mat4& transform)
   {

   for (size_t i = 0; i < vertices.size(); i++)
   {
   noob::vec3 v = vertices[i];
   noob::vec4 temp_vert(v, 1.0);
   noob::vec4 temp_transform = transform * temp_vert;
   noob::vec3 transformed_vert;

   transformed_vert[0] = temp_transform[0];
   transformed_vert[1] = temp_transform[1];
   transformed_vert[2] = temp_transform[2];

   vertices[i] = temp_transform;
   }

   update_normals();
   }


   void noob::basic_mesh::translate(const noob::vec3& translation)
   {
   noob::transform_helper t;
   t.translate(translation);
   transform(t.get_matrix());
   }


   void noob::basic_mesh::rotate(const noob::versor& orientation)
   {
   noob::transform_helper t;
   t.rotate(orientation);
   transform(t.get_matrix());
   }


   void noob::basic_mesh::scale(const noob::vec3& scale)
   {
   noob::transform_helper t;
   t.scale(scale);
   transform(t.get_matrix());

   }
   */


TriMesh noob::basic_mesh::to_half_edges() const
{
	TriMesh half_edges;
	std::vector<TriMesh::VertexHandle> vert_handles;

	for (auto v : vertices)
	{
		vert_handles.push_back(half_edges.add_vertex(TriMesh::Point(v.v[0], v.v[1], v.v[2])));
	}

	for (size_t i = 0; i < indices.size(); i = i + 3)
	{
		std::vector<TriMesh::VertexHandle> face_verts;
		face_verts.push_back(vert_handles[indices[i]]);
		face_verts.push_back(vert_handles[indices[i+1]]);
		face_verts.push_back(vert_handles[indices[i+2]]);
		half_edges.add_face(face_verts);
	}

	return half_edges;
}


void noob::basic_mesh::from_half_edges(TriMesh half_edges)
{
	vertices.resize(half_edges.n_vertices());
	indices.clear();

	for (TriMesh::ConstVertexIter v_it = half_edges.vertices_begin(); v_it != half_edges.vertices_end(); ++v_it)
	{
		vertices[v_it->idx()] = noob::vec3(half_edges.point(*v_it));
	}

	for (TriMesh::ConstFaceIter f_it = half_edges.faces_begin(); f_it != half_edges.faces_end(); ++f_it)
	{
		for (TriMesh::FaceVertexCCWIter fv_it = half_edges.fv_ccwiter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			indices.push_back(static_cast<uint16_t>(fv_it->idx()));
		}
	}
	normalize();
	// vert_normals_from_trimesh(half_edges);
	// fmt::MemoryWriter ww;
	// ww << "[BasicMesh] Created basic_mesh with verts = " << vertices.size() << ", indices = " << indices.size() << " from TriMesh with verts = " << half_edges.n_vertices();
	// logger::log(ww.str());
}


void noob::basic_mesh::from_half_edges(PolyMesh half_edges)
{
	PolyMesh temp_polymesh(half_edges);

	temp_polymesh.triangulate();
	temp_polymesh.garbage_collection();

	TriMesh temp_trimesh;

	for (PolyMesh::ConstVertexIter v_it = temp_polymesh.vertices_begin(); v_it != temp_polymesh.vertices_end(); ++v_it)
	{
		PolyMesh::Point temp(temp_polymesh.point(*v_it));
		TriMesh::Point p;

		p[0] = temp[0];
		p[1] = temp[1];
		p[2] = temp[2];

		temp_trimesh.add_vertex(p);
	}

	std::vector<PolyMesh::VertexHandle> temp_face;
	temp_face.resize(3);
	for (PolyMesh::ConstFaceIter f_it = temp_polymesh.faces_begin(); f_it != temp_polymesh.faces_end(); ++f_it)
	{
		size_t index = 0;
		for (PolyMesh::FaceVertexCCWIter fv_it = temp_polymesh.fv_ccwiter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			temp_face[index] = *fv_it;
			++index;
		}
		temp_trimesh.add_face(temp_face);
	}

	from_half_edges(temp_trimesh);
}

/*
void noob::basic_mesh::vert_normals_from_trimesh(TriMesh half_edges)
{
	half_edges.request_vertex_normals();
	// Assure we have vertex normals
	assert(half_edges.has_vertex_normals());
	if (!half_edges.has_vertex_normals())
	{
		logger::log("[BasicMesh] RUNTIME BAD NONO: Vertex normals not available. Mesh goes *pop* and disappears!"); //Standard vertex property 'Normals' not available!");
	}
	else
	{
		logger::log("[BasicMesh] Got our vertex normals!");
		// Ee need face normals to update the vertex normals
		half_edges.request_face_normals();
		// Let the half_edges update the normals
		half_edges.update_normals();
		normals.resize(half_edges.n_vertices());
		for (TriMesh::VertexIter v_it = half_edges.vertices_begin(); v_it != half_edges.vertices_end(); ++v_it)
		{	
			// auto p = half_edges.normal(*v_it);
			normals[v_it->idx()] = half_edges.normal(*v_it);
		}
	}
}
*/
