#include "ModelLoader.hpp"

noob::model_loader::model_loader(const std::string& filename, const std::string& filepath)
{
	Assimp::Importer importer;

	path = std::unique_ptr<std::string>(new std::string(filepath));

	std::string full_path;

	if (*path != "")
	{
		full_path = *path + std::string("/") + filename;
	}

	else
	{
		full_path = filename;
	}

	const aiScene* scene = importer.ReadFile(full_path.c_str(), aiProcess_ValidateDataStructure | aiProcess_LimitBoneWeights | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace | aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals | aiProcess_FindInvalidData);

	{
		std::stringstream ss;
		ss << "Attempting to load model_loader " << full_path;
		logger::log(ss.str());
	}

	if(!scene)
	{
		std::stringstream ss;
		ss << "Unable to load model_loader: Path = " << full_path << "  Error: " << importer.GetErrorString();
		logger::log(ss.str());
	}

	for(int i = 0; i < scene->mNumMeshes; ++i)
	{
		std::stringstream ss;
		ss << "Loading drawable " << i;
		logger::log(ss.str());
		drawable_entries.push_back(new noob::drawable(const_cast<aiScene*>(scene), scene->mMeshes[i], filepath));
	}
}
// Clears all loaded MeshEntries
noob::model_loader::~model_loader(void)
{
	for(int i = 0; i < drawable_entries.size(); ++i)
	{
		delete drawable_entries.at(i);
	}
	drawable_entries.clear();
}

/*
// Renders all loaded MeshEntries
void noob::model_loader::draw()
{
	for(int i = 0; i < drawable_entries.size(); ++i)
	{
		drawable_entries.at(i)->draw();
	}
}
*/
