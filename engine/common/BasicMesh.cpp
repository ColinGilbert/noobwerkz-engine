// TODO: Remove the following comment if it works across all builds
//#define CSGJS_HEADER_ONLY
//#include <csgjs.hpp>
//


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <iostream>
/*
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Tools/Decimater/CollapseInfoT.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModAspectRatioT.hh>
#include <OpenMesh/Tools/Decimater/ModEdgeLengthT.hh>
#include <OpenMesh/Tools/Decimater/ModHausdorffT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalDeviationT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Tools/Decimater/ModProgMeshT.hh>
#include <OpenMesh/Tools/Decimater/ModIndependentSetsT.hh>
#include <OpenMesh/Tools/Decimater/ModRoundnessT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>
*/

#include "Logger.hpp"
#include "BasicMesh.hpp"
#include "format.h"

//#include "VHACD.h"


#include <Eigen/Geometry>

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


void noob::basic_mesh::decimate(const std::string& filename, size_t num_verts) const
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
		return;
	}

	size_t verts_removed = decimator.decimate_to(num_verts);
	logger::log(fmt::format("[Mesh] decimate() - Vertices removed = {0}", verts_removed));
	decimator.mesh().garbage_collection();
	half_edges.garbage_collection();
	OpenMesh::IO::write_mesh(half_edges, filename);
	// logger::log("[Mesh] decimate() - done");
}

/*
   noob::basic_mesh noob::basic_mesh::decimate(size_t num_verts) const
   {
//decimate("./temp/temp-decimated.off", num_verts);
noob::basic_mesh temp;
//temp.load("./temp/temp-decimated.off", "temp-decimated");
return temp;
}
*/

void noob::basic_mesh::normalize() 
{
	std::string temp = save();
	load_mem(temp);
}


// TODO
void noob::basic_mesh::to_origin()
{
	// noob::basic_mesh temp;
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i] = (vertices[i] - bbox_info.center);
	}
	normalize();

}
/*
   std::tuple<size_t, const char*> noob::basic_mesh::save() const
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

   const char* mem = w.data();
   size_t size = w.size();

   return std::make_tuple(size, mem);
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
	const aiScene* scene = aiImportFileFromMemory(file.c_str(), file.size(), post_process, "");//aiProcessPreset_TargetRealtime_Fast | aiProcess_FixInfacingNormals, "");
	return load_assimp(scene, name);
}


bool noob::basic_mesh::load_file(const std::string& filename, const std::string& name)
{
	// logger::log(fmt::format("[Mesh] loading file {0}", filename ));
	const aiScene* scene = aiImportFile(filename.c_str(), post_process);//aiProcessPreset_TargetRealtime_Fast | aiProcess_FixInfacingNormals);
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

	// logger::log(fmt::format("[Mesh] load({0}) - Attempting to obtain mesh data", name));

	size_t num_verts = mesh_data->mNumVertices;
	size_t num_faces = mesh_data->mNumFaces;
	// TODO: Fix?
	// auto num_indices = mesh_data->mNumFaces / 3;

	bool has_normals = mesh_data->HasNormals();
	//logger::log(fmt::format("[Mesh] load({0}) - Mesh has {1} verts, normals? {2}", name, num_verts, has_normals));

	bool has_texcoords = mesh_data->HasTextureCoords(0);
	// double accum_x, accum_y, accum_z = 0.0f;
	bbox_info.min = bbox_info.max = bbox_info.center = noob::vec3(0.0, 0.0, 0.0);	

	for (size_t n = 0; n < num_verts; ++n)
	{
		aiVector3D pt = mesh_data->mVertices[n];
		noob::vec3 v;
		v.v[0] = pt[0];
		v.v[1] = pt[1];
		v.v[2] = pt[2];
		vertices.push_back(v);

		bbox_info.min[0] = std::min(bbox_info.min[0], v[0]);
		bbox_info.min[1] = std::min(bbox_info.min[1], v[1]);
		bbox_info.min[2] = std::min(bbox_info.min[2], v[2]);

		bbox_info.max[0] = std::max(bbox_info.max[0], v[0]);
		bbox_info.max[1] = std::max(bbox_info.max[1], v[1]);
		bbox_info.max[2] = std::max(bbox_info.max[2], v[2]);

		if (has_normals)
		{
			aiVector3D normal = mesh_data->mNormals[n];
			noob::vec3 norm;
			norm.v[0] = normal[0];
			norm.v[1] = normal[1];
			norm.v[2] = normal[2];
			normals.push_back(norm);
		}
		if (has_texcoords)
		{
			aiVector3D tex = mesh_data->mTextureCoords[0][n];
			noob::vec3 uvw;
			uvw.v[0] = tex[0];
			uvw.v[1] = tex[1];
			uvw.v[2] = tex[2];
			texcoords.push_back(uvw);
		}
		else
		{
			texcoords.push_back(noob::vec3(0.0, 0.0, 0.0));
		}
	}
		

	if (num_verts == 0)
	{
		num_verts = 1;
	}

	bbox_info.center = noob::vec3((bbox_info.max[0] + bbox_info.min[0])/2, (bbox_info.max[1] + bbox_info.min[1])/2, (bbox_info.max[2] + bbox_info.min[2])/2);

	for (size_t n = 0; n < num_faces; ++n)
	{
		// Allows for degenerate triangles. Worth keeping?
		const struct aiFace* face = &mesh_data->mFaces[n];
		indices.push_back(face->mIndices[0]);
		indices.push_back(face->mIndices[1]);
		indices.push_back(face->mIndices[2]);
	}

	aiReleaseImport(scene);

	return true;
}

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

	normalize();
}

void noob::basic_mesh::translate(const noob::vec3& translation)
{
	noob::transform_helper t;
	t.translate(translation);
	transform(t.get_matrix());
	// for (auto v : vertices)
	// {
	//	v += translation;
	// }
	// normalize();
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
	std::ostringstream oss;
	if (!OpenMesh::IO::write_mesh(half_edges, oss, "temp.off")) 
	{
		logger::log("[BasicMesh] Could not import from OpenMesh!");
	}
	else
	{
		load_mem(oss.str());
	}
}


void noob::basic_mesh::from_half_edges(PolyMesh half_edges)
{
	std::ostringstream oss;
	PolyMesh _half_edges = half_edges;
	_half_edges.triangulate();
	_half_edges.garbage_collection();
	
	if (!OpenMesh::IO::write_mesh(half_edges, oss, "temp.off")) 
	{
		logger::log("[BasicMesh] Could not import from OpenMesh!");
	}
	else
	{
		load_mem(oss.str());
	}
}


/*
   noob::basic_mesh noob::basic_mesh::bone()
   {
   TriMesh half_edges;
   TriMesh::VertexHandle vhandles[6];

   vhandles[0] = half_edges.add_vertex(TriMesh::Point(1.0, 0.0, 0.0));
   vhandles[1] = half_edges.add_vertex(TriMesh::Point(0.8, 0.2, 0.2));
   vhandles[2] = half_edges.add_vertex(TriMesh::Point(0.8, 0.2, -0.2));
   vhandles[3] = half_edges.add_vertex(TriMesh::Point(0.8, -0.2, -0.2));
   vhandles[4] = half_edges.add_vertex(TriMesh::Point(0.8, -0.2, 0.2));
   vhandles[5] = half_edges.add_vertex(TriMesh::Point(-1.0, 0.0, 0.0));

   std::vector<TriMesh::VertexHandle> face_vhandles;

   face_vhandles.clear();
   face_vhandles.push_back(vhandles[0]);
   face_vhandles.push_back(vhandles[2]);
   face_vhandles.push_back(vhandles[1]);
   half_edges.add_face(face_vhandles);

   face_vhandles.clear();
   face_vhandles.push_back(vhandles[5]);
   face_vhandles.push_back(vhandles[1]);
   face_vhandles.push_back(vhandles[2]);
   half_edges.add_face(face_vhandles);

   face_vhandles.clear();
   face_vhandles.push_back(vhandles[0]);
   face_vhandles.push_back(vhandles[3]);
   face_vhandles.push_back(vhandles[2]);
   half_edges.add_face(face_vhandles);

   face_vhandles.clear();
   face_vhandles.push_back(vhandles[5]);
   face_vhandles.push_back(vhandles[2]);
   face_vhandles.push_back(vhandles[3]);
   half_edges.add_face(face_vhandles);

   face_vhandles.clear();
   face_vhandles.push_back(vhandles[0]);
   face_vhandles.push_back(vhandles[4]);
   face_vhandles.push_back(vhandles[3]);
   half_edges.add_face(face_vhandles);

   face_vhandles.clear();
   face_vhandles.push_back(vhandles[5]);
   face_vhandles.push_back(vhandles[3]);
   face_vhandles.push_back(vhandles[4]);
   half_edges.add_face(face_vhandles);

   face_vhandles.clear();
   face_vhandles.push_back(vhandles[0]);
   face_vhandles.push_back(vhandles[1]);
   face_vhandles.push_back(vhandles[4]);
   half_edges.add_face(face_vhandles);

   face_vhandles.clear();
   face_vhandles.push_back(vhandles[5]);
   face_vhandles.push_back(vhandles[4]);
   face_vhandles.push_back(vhandles[1]);
   half_edges.add_face(face_vhandles);

   face_vhandles.clear();
   face_vhandles.push_back(vhandles[0]);
   face_vhandles.push_back(vhandles[1]);
   face_vhandles.push_back(vhandles[2]);
   half_edges.add_face(face_vhandles);

   OpenMesh::IO::write_mesh(half_edges, "temp/bone.off");
   noob::basic_mesh mesh;
mesh.load("temp/bone.off", "bone-temp");

return mesh;
}
*/

