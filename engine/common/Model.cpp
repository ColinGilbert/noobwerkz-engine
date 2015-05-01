#include "Model.hpp"
//#include "bgfx_utils.h"
bgfx::VertexDecl noob::model::mesh::position_normal_vertex::ms_decl;


noob::model::mesh::~mesh()
{
	// delete [] indices;
	bgfx::destroyVertexBuffer(vertex_buffer);
	bgfx::destroyIndexBuffer(index_buffer);
}


// Renders this mesh
void noob::model::mesh::render()
{
}

// noob::graphics::Model constructor, loads the specified filename if supported by Assimp
noob::model::model(const std::string& filename, const std::string& filepath)
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
		ss << "Attempting to load model " << full_path;
		logger::log(ss.str());
	}

	if(!scene)
	{
		std::stringstream ss;
		ss << "Unable to load model: Path = " << full_path << "  Error: " << importer.GetErrorString();
		logger::log(ss.str());
	}

	for(int i = 0; i < scene->mNumMeshes; ++i)
	{
		std::stringstream ss;
		ss << "Loading mesh " << i;
		logger::log(ss.str());
		mesh_entries.push_back(new noob::model::mesh(const_cast<aiScene*>(scene), scene->mMeshes[i], filepath));
	}
}

noob::model::mesh::mesh(aiScene* scene, aiMesh* mesh, const std::string& filepath)
{
	logger::log("Creating mesh.");
	
	position_normal_vertex::init();

	element_count = mesh->mNumFaces * 3;
	{
		std::stringstream ss;
		ss << "Mesh has " << mesh->mNumFaces << " faces.";
		logger::log(ss.str());
	}

	int num_verts = mesh->mNumVertices;

	vertices = new position_normal_vertex[num_verts];

	if(mesh->HasPositions())
	{
		logger::log("Mesh has positions");

		//float *vertices = new float[num_verts * 3];
		for(int i = 0; i < num_verts; ++i)
		{
			//vertices[i * 3] = mesh->mVertices[i].x;
			//vertices[i * 3 + 1] = mesh->mVertices[i].y;
			//vertices[i * 3 + 2] = mesh->mVertices[i].z;

			vertices[i].m_x = mesh->mVertices[i].x;
			vertices[i].m_y = mesh->mVertices[i].y;
			vertices[i].m_z = mesh->mVertices[i].z;
		}

		// delete vertices;
	}

	/*
	   if(mesh->HasTextureCoords(0))
	   {
	   logger::log("Mesh has texcoords");

	   float *texCoords = new float[num_verts * 2];
	   for(int i = 0; i < num_verts; ++i)
	   {
	   texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
	   texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
	   }
	//	delete texCoords;
	}
	*/

	if(mesh->HasNormals()) 
	{
		logger::log("Mesh has normals");
		//float *normals = new float[num_verts * 3];
		for(int i = 0; i < num_verts; ++i)
		{
			//normals[i * 3] = mesh->mNormals[i].x;
			//normals[i * 3 + 1] = mesh->mNormals[i].y;
			//normals[i * 3 + 2] = mesh->mNormals[i].z;
			vertices[i].m_normal = pack_floats_to_uint32(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		}
		//	delete normals;
	}


	if(mesh->HasFaces())
	{
		logger::log("Mesh has faces");
		unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
		for(int i = 0; i < mesh->mNumFaces; ++i)
		{
			indices[i * 3] = mesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		index_buffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices) ) );
		delete indices;
	}

	bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), position_normal_vertex::ms_decl);


	if(mesh->mMaterialIndex >= 0)
	{
		{
			std::stringstream ss;
			ss << "Mesh has " << mesh->mMaterialIndex << " materials";
			logger::log(ss.str());
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		load_textures(material, aiTextureType_DIFFUSE, filepath);

		// Those lines below are commented out because gradient mapping uses a single texture + uniforms/vert attributes instead of mutliple textures.
	/*
	 	load_textures(material, aiTextureType_SPECULAR, filepath);
		load_textures(material, aiTextureType_NORMALS, filepath);
		load_textures(material, aiTextureType_AMBIENT, filepath);
		load_textures(material, aiTextureType_EMISSIVE, filepath);
		load_textures(material, aiTextureType_OPACITY, filepath);
		load_textures(material, aiTextureType_HEIGHT, filepath);
		load_textures(material, aiTextureType_UNKNOWN, filepath);
	*/
	}
}

// Clears all loaded MeshEntries
noob::model::~model(void)
{
	for(int i = 0; i < mesh_entries.size(); ++i)
	{
		delete mesh_entries.at(i);
	}
	mesh_entries.clear();
	// delete path;
}


// Renders all loaded MeshEntries
void noob::model::render()
{
	for(int i = 0; i < mesh_entries.size(); ++i)
	{
		mesh_entries.at(i)->render();
	}
}


void noob::model::mesh::load_textures(aiMaterial* mat, aiTextureType type, const std::string& filepath)
{
	for(size_t i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString ai_str;
		mat->GetTexture(type, i, &ai_str);

		std::string str(ai_str.C_Str());
		str = str.erase(0,1);
		logger::log("Assimp path to texture = " + str);

		std::string tex_path(filepath + str);

		logger::log("Attempting to load texture from " + tex_path);

	//	texture = bgfx_utils::loadTexture(tex_path.c_str());
	//	noob::graphics::texture tex;

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
/*
		if(noob::graphics::global_textures.find(tex_path) != noob::graphics::global_textures.end())
		{
			tex = noob::graphics::global_textures.find(str)->second;
			textures.push_back(tex);

			std::stringstream ss;
			ss << "Found already loaded texture"; // << tex.id;
			logger::log(ss.str());
		}
		else
		{
			logger::log(std::string("Could not find texture ") + tex_path + std::string(" in map. Attempting to load."));

			std::string s; // for logging
			switch (type)
			{
				case aiTextureType_DIFFUSE:
					{
						tex.type = noob::graphics::texture_type::DIFFUSE_MAP;
						s = "diffuse";
						break;
					}
				case aiTextureType_SPECULAR:
					{
						tex.type = noob::graphics::texture_type::SPECULAR_MAP;
						s = "specular";
						break;
					}
				case aiTextureType_AMBIENT:
					{
						tex.type = noob::graphics::texture_type::AMBIENT_MAP;
						s = "ambient";
						break;
					}
				case aiTextureType_EMISSIVE:
					{
						tex.type = noob::graphics::texture_type::EMISSIVE_MAP;
						s = "emissive";
						break;
					}
				case aiTextureType_NORMALS:
					{
						tex.type = noob::graphics::texture_type::NORMAL_MAP;
						s = "normals";
						break;
					}
				case aiTextureType_OPACITY:
					{
						tex.type = noob::graphics::texture_type::OPACITY_MAP;
						s = "opacity";
						break;
					}
				case aiTextureType_DISPLACEMENT:
					{
						tex.type = noob::graphics::texture_type::DISPLACEMENT_MAP;
						s = "displacement";
						break;
					}
				case aiTextureType_HEIGHT:
					{
						tex.type = noob::graphics::texture_type::HEIGHT_MAP;
						s = "heightmap";
						break;
					}
				default:
					{
						tex.type = noob::graphics::texture_type::DATA;
						s = "data";
						break;
					}


					logger::log(std::string("Loading " ) + s + std::string(" texture at ") + tex_path);

			}
	
		}*/
	}
}
