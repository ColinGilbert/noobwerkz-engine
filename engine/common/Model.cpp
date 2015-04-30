#include "Model.hpp"
#include "bgfx_utils.h"

noob::model::mesh::~mesh()
{
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
	//	path = std::unique_ptr<std::string>(new std::string(filepath));

	logger::log("Creating mesh.");

	elementCount = mesh->mNumFaces * 3;
	{
		std::stringstream ss;
		ss << "Mesh has " << mesh->mNumFaces << " faces.";
		logger::log(ss.str());
	}

//	GLuint vert_buff, index_buff, uv_buff, norm_buff;

	if(mesh->HasPositions())
	{
		logger::log("Mesh has positions");

		float *vertices = new float[mesh->mNumVertices * 3];
		for(int i = 0; i < mesh->mNumVertices; ++i)
		{
			vertices[i * 3] = mesh->mVertices[i].x;
			vertices[i * 3 + 1] = mesh->mVertices[i].y;
			vertices[i * 3 + 2] = mesh->mVertices[i].z;
		}


		// Add vertices

/*		glGenBuffers(1, &vert_buff);
		glBindBuffer(GL_ARRAY_BUFFER, vert_buff);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (0);

		vbo.push_back(vert_buff); */

		delete vertices;
	}

	if(mesh->HasTextureCoords(0))
	{
		logger::log("Mesh has texcoords");

		float *texCoords = new float[mesh->mNumVertices * 2];
		for(int i = 0; i < mesh->mNumVertices; ++i)
		{
			texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}

		// Add texcoords

/*		glGenBuffers(1, &uv_buff);
		glBindBuffer(GL_ARRAY_BUFFER, uv_buff);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (1);

		vbo.push_back(uv_buff);*/

		delete texCoords;
	}

	if(mesh->HasNormals()) 
	{
		logger::log("Mesh has normals");
		float *normals = new float[mesh->mNumVertices * 3];
		for(int i = 0; i < mesh->mNumVertices; ++i)
		{
			normals[i * 3] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}

		// Add normals

/*		glGenBuffers(1, &norm_buff);
		glBindBuffer(GL_ARRAY_BUFFER, norm_buff);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		vbo.push_back(norm_buff); */
		delete normals;
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

		// Add indices

/*		glGenBuffers(1, &index_buff);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (3);

		vbo.push_back(index_buff); */
		delete indices;
	}


	if(mesh->mMaterialIndex >= 0)
	{
		{
			std::stringstream ss;
			ss << "Mesh has " << mesh->mMaterialIndex << " materials";
			logger::log(ss.str());
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


		// unsigned int tex_count = material->
		load_textures(material, aiTextureType_DIFFUSE, filepath);
		load_textures(material, aiTextureType_SPECULAR, filepath);
		load_textures(material, aiTextureType_NORMALS, filepath);
		load_textures(material, aiTextureType_AMBIENT, filepath);
		load_textures(material, aiTextureType_EMISSIVE, filepath);
		load_textures(material, aiTextureType_OPACITY, filepath);
		load_textures(material, aiTextureType_HEIGHT, filepath);
		load_textures(material, aiTextureType_UNKNOWN, filepath);
	}

//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
}


// -----------------------------------------
/*

   int num_materials = scene->mNumMaterials;

   vector<int> materialRemap(num_materials);
   FOR(i, num_materials)
   {
   const aiMaterial* material = scene->mMaterials[i];
   int a = 5;
   int texIndex = 0;
   aiString path;  // filename
   if(material->Gettexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
   {
   string sDir = GetDirectoryPath(sFilePath);
   string stextureName = path.data;
   string sFullPath = sDir+stextureName;
   int iTexFound = -1;
   FOR(j, ESZ(ttextures))if(sFullPath == ttextures[j].GetPath())
   {
   iTexFound = j;
   break;
   }
   if(iTexFound != -1)materialRemap[i] = iTexFound;
   else
   {
   Ctexture tNew;
   tNew.Loadtexture2D(sFullPath, true);
   materialRemap[i] = ESZ(ttextures);
   ttextures.push_back(tNew);
   }
   }
   }


*/


// ------------------------------------------

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

		noob::graphics::texture tex;

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture

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
			}
			/* logger::log(std::string("Attempting to load ") + s + std::string(" texture"));

			 tex.id = SOIL_load_OGL_texture(str.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

			{
				std::stringstream ss;
				ss << "Loading " << s << " texture -> ID: " << tex.id << " -> PATH: " << tex_path;
				logger::log(ss.str());
			}

			global_textures[tex_path] = tex;
			textures.push_back(tex); */
		}
	}
}
