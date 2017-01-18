noob::results<noob::mesh_3d> noob::assimp::load_mem(const std::string& file)
{
	const aiScene* scene = aiImportFileFromMemory(file.c_str(), file.size(), aiProcessPreset_TargetRealtime_Fast, "");
	return load_assimp(scene);
}


noob::results<noob::mesh_3d> noob::assimp::load_file(const std::string& filename)
{
	const aiScene* scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_Fast);
	return load_assimp(scene);
}


noob::results<noob::mesh_3d> noob::assimp::load(const aiScene* scene)
{
	logger::log(noob::importance::INFO, "[BasicMesh] Load Asssimp");
	// logger::log(noob::importance::INFO, "[mesh] load() - begin");
	if (!scene)
	{
		logger::log(noob::importance::INFO, "[BasicMesh] (Assimp) cannot open!");
		return noob::results<noob::mesh_3d>::make_invalid();
	}

	const aiMesh* mesh_data = scene->mMeshes[0];

	const size_t num_verts = mesh_data->mNumVertices;
	const size_t num_faces = mesh_data->mNumFaces;
	
	if (num_verts == 0 || num_faces == 0)
	{
		return noob::results<noob::mesh_3d>::make_invalid();
	}

	const auto num_indices = mesh_data->mNumFaces / 3;

	const bool has_normals = mesh_data->HasNormals();

	noob::vec3f accum = noob::vec3f(0.0, 0.0, 0.0);
	bbox.min = bbox.max = noob::vec3f(0.0, 0.0, 0.0);	

	noob::mesh_3d m;
	
	for (size_t n = 0; n < num_verts; ++n)
	{
		// TODO: Implement assimp-to-noob in math_funcs.hpp, if not already done.
		const aiVector3D pt = mesh_data->mVertices[n];

		noob::vec3f pos(pt[0], pt[1], pt[2]);

		m.bbox.min = noob::min(m.bbox.min, pos);
		m.bbox.max = noob::max(m.bbox.max, pos);

		// TODO: Add proper normal-fixing code
		noob::vec3f normal = noob::vec3f(0.0, 1.0, 0.0);

		if (has_normals)
		{
			aiVector3D nrml = mesh_data->mNormals[n];
			normal = noob::vec3(nrml[0], nrml[1], nrml[2]);
		}

		noob::mesh_3d::vert vv;
		vv.position = pos;
		vv.normal = normal;
		vv.texcoords = noob::vec3f(0.0, 0.0, 0.0);		
		vv.colour = noob::vec4f(1.0, 1.0, 1.0, 1.0);
		

		// TODO: Check for UVW, vert colours, and bones info.
		

		m.vertices.push_back(vv);
	}
	
	for (size_t n = 0; n < num_faces; ++n)
	{
		const struct aiFace* face = &mesh_data->mFaces[n];
		m.indices.push_back(face->mIndices[0]);
		m.indices.push_back(face->mIndices[1]);
		m.indices.push_back(face->mIndices[2]);
	}

	aiReleaseImport(scene);

}	return noob::results::make_valid(m);
