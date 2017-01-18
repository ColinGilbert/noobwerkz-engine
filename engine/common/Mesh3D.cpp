#include "Mesh3D.hpp"

#include <Eigen/Geometry>

float noob::mesh_3d::get_volume()
{
	if (!volume_calculated)
	{
		// Proudly gleaned from U of R website!
		// http://mathcentral.uregina.ca/QQ/database/QQ.09.09/h/ozen1.html
		// The volume of the tetrahedron with vertices (0 ,0 ,0), (a1 ,a2 ,a3), (b1, b2, b3) and (c1, c2, c3) is [a1b2c3 + a2b3c1 + a3b1c2 - a1b3c2 - a2b1c3 - a3b2c1] / 6.
		double accum = 0.0;
		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			const noob::vec3f first = vertices[i].position;
			const noob::vec3f second = vertices[i+1].position;
			const noob::vec3f third = vertices[i+2].position;

			accum += ((first[0] * second[1] * third[2]) + (first[1] * second[2] * third[0]) + (first[2] * second[0] * third.v[1]) - (first[0] * second.v[2] * third.v[1]) - (first.v[1] * second[0] * third.v[2]) - (first[2] * second[1] * third.v[0])) / 6.0;

		}

		volume_calculated = true;
		volume = static_cast<float>(accum);
	}

	return volume;
}


void noob::mesh_3d::calculate_dims() noexcept(true)
{
	if (vertices.size() > 0)
	{
		bbox.min = bbox.max = vertices[0].position;
		for (noob::mesh_3d::vert v : vertices)
		{
			bbox.min[0] = std::min(bbox.min[0], v.position[0]);
			bbox.min[1] = std::min(bbox.min[1], v.position[1]);
			bbox.min[2] = std::min(bbox.min[2], v.position[2]);
			bbox.max[0] = std::max(bbox.max[0], v.position[0]);
			bbox.max[1] = std::max(bbox.max[1], v.position[1]);
			bbox.max[2] = std::max(bbox.max[2], v.position[2]);
		}
	}
}
/*
   void noob::mesh_3d::to_origin()
   {
   noob::vec3f dims = bbox.get_dims();
   for (size_t i = 0; i < vertices.size(); ++i)
   {
   vertices[i] = (vertices[i] + dims);
   }
   }

   std::string noob::mesh_3d::save() const
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

   void noob::mesh_3d::save(const std::string& filename) const
   {

   }
   */

#if defined(NOOB_USE_ASSIMP)
bool noob::mesh_3d::load_mem_assimp(const std::string& file)
{
	const aiScene* scene = aiImportFileFromMemory(file.c_str(), file.size(), aiProcessPreset_TargetRealtime_Fast, "");
	return load_assimp(scene);
}


bool noob::mesh_3d::load_file_assimp(const std::string& filename)
{
	const aiScene* scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_Fast);
	return load_assimp(scene);
}

bool noob::mesh_3d::load_assimp(const aiScene* scene)
{
	logger::log(noob::importance::INFO, "[BasicMesh] Load Asssimp");
	// logger::log(noob::importance::INFO, "[mesh] load() - begin");
	if (!scene)
	{
		logger::log(noob::importance::INFO, "[BasicMesh] (Assimp) cannot open!");
		return false;
	}

	vertices.clear();
	indices.clear();

	const aiMesh* mesh_data = scene->mMeshes[0];

	size_t num_verts = mesh_data->mNumVertices;
	size_t num_faces = mesh_data->mNumFaces;
	auto num_indices = mesh_data->mNumFaces / 3;

	bool has_normals = mesh_data->HasNormals();

	double accum_x, accum_y, accum_z;
	accum_x = accum_y = accum_z = 0.0f;
	bbox.min = bbox.max = noob::vec3f(0.0, 0.0, 0.0);	

	for (size_t n = 0; n < num_verts; ++n)
	{
		aiVector3D pt = mesh_data->mVertices[n];

		noob::vec3f pos(pt[0], pt[1], pt[2]);

		noob::vec3f normal(0.0, 1.0, 0.0);

		if (has_normals)
		{
			aiVector3D nrml = mesh_data->mNormals[n];
			normal = noob::vec3(nrml[0], nrml[1], nrml[2]);
		}

		// TODO: Check for UVW, vert colours, and bones info.

		update_bbox(bbox, v);
	}

	if (num_verts == 0)
	{
		num_verts = 1;
	}

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
#endif
