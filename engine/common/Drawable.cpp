#include "Drawable.hpp"

bgfx::VertexDecl noob::drawable::position_normal_vertex::ms_decl;

noob::drawable::~drawable()
{
	bgfx::destroyVertexBuffer(vertex_buffer);
	bgfx::destroyIndexBuffer(index_buffer);
	delete [] vertices;
	delete [] indices;
}

void noob::drawable::draw(uint8_t view_id, const float* transform, bgfx::ProgramHandle program_handle)
{

	bgfx::setTransform(transform);
	bgfx::setProgram(program_handle);
	bgfx::setIndexBuffer(index_buffer);
	bgfx::setVertexBuffer(vertex_buffer);
	bgfx::setState(flags);
	bgfx::submit(view_id);
}

noob::drawable::drawable(aiScene* scene, aiMesh* drawable, const std::string& filepath)
{
	logger::log("Creating drawable.");

	position_normal_vertex::init();

	element_count = drawable->mNumFaces * 3;
	{
		std::stringstream ss;
		ss << "Mesh has " << drawable->mNumFaces << " faces and " << element_count << " indices";
		logger::log(ss.str());
	}

	int num_verts = drawable->mNumVertices;
	vertices = new position_normal_vertex[num_verts];

	if(drawable->HasPositions())
	{
		std::stringstream ss;
		ss << "Mesh has " << num_verts << " vertices.";
		logger::log(ss.str());

		
		for(int i = 0; i < num_verts; ++i)
		{
		
			vertices[i].m_x = drawable->mVertices[i].x;
			vertices[i].m_y = drawable->mVertices[i].y;
			vertices[i].m_z = drawable->mVertices[i].z;
		}

	}

	// TODO: Convert following old code to bgfx
	/*
	   
	   if(drawable->HasTextureCoords(0))
	   {
	   logger::log("Mesh has texcoords");

	   float *texCoords = new float[num_verts * 2];
	   for(int i = 0; i < num_verts; ++i)
	   {
	   texCoords[i * 2] = drawable->mTextureCoords[0][i].x;
	   texCoords[i * 2 + 1] = drawable->mTextureCoords[0][i].y;
	   }

	//	delete texCoords;

	}
	*/

	if(drawable->HasNormals()) 
	{
		logger::log("Mesh has normals");
		for(int i = 0; i < num_verts; ++i)
		{

			vertices[i].m_normal = pack_floats_to_uint32(drawable->mNormals[i].x, drawable->mNormals[i].y, drawable->mNormals[i].z);
		}
	}


	if(drawable->HasFaces())
	{
		logger::log("Mesh has faces");

		indices = new uint16_t[drawable->mNumFaces * 3];

		for(int i = 0; i < drawable->mNumFaces; ++i)
		{
			indices[i * 3] = (uint16_t) drawable->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = (uint16_t) drawable->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = (uint16_t) drawable->mFaces[i].mIndices[2];
		}

		{
			std::stringstream ss;
			ss << "Mesh has "<< drawable->mNumFaces * 3 << " indices";
			logger::log(ss.str());
		}

		{
			std::stringstream ss;
			ss << "sizeof(indices) = " << sizeof(indices); 
			logger::log(ss.str());
		}

		const bgfx::Memory* mem = bgfx::makeRef(indices, sizeof(indices));
		logger::log("Made ref to indices");
		index_buffer = bgfx::createIndexBuffer(mem);
		logger::log("Created index buffer");
	}

	bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), position_normal_vertex::ms_decl);

	/*
	if(drawable->mMaterialIndex >= 0)
	{
		{
			std::stringstream ss;
			ss << "Mesh has " << drawable->mMaterialIndex << " materials";
			logger::log(ss.str());
		}

		aiMaterial* material = scene->mMaterials[drawable->mMaterialIndex];

		load_textures(material, aiTextureType_DIFFUSE, filepath);

		// Those lines below are commented out because gradient mapping uses a single texture + uniforms/vert attributes instead of mutliple textures.
		
		// load_textures(material, aiTextureType_SPECULAR, filepath);
		// load_textures(material, aiTextureType_NORMALS, filepath);
		// load_textures(material, aiTextureType_AMBIENT, filepath);
		// load_textures(material, aiTextureType_EMISSIVE, filepath);
		// load_textures(material, aiTextureType_OPACITY, filepath);
		// load_textures(material, aiTextureType_HEIGHT, filepath);
		// load_textures(material, aiTextureType_UNKNOWN, filepath);
		   
	}
	*/
}


// TODO: Find out if this is the best place for this function
/*
void noob::drawable::load_textures(aiMaterial* mat, aiTextureType type, const std::string& filepath)
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

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture

		if(noob::graphics::global_textures.find(tex_path) != noob::graphics::global_textures.end())
		{
			bgfx::TextureHandle tex = noob::graphics::global_textures.find(str)->second;
			texture = tex;

			std::stringstream ss;
			ss << "Found already loaded texture";
			logger::log(ss.str());
		}
		else
		{
			logger::log(std::string("Could not find texture ") + tex_path + std::string(" in map. Attempting to load."));
			texture = noob::graphics::load_texture(tex_path);
		}
			
	}	
}
*/
