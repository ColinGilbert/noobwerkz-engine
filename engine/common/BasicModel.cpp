#include "BasicModel.hpp"

#include <assimp/quaternion.h>
#include <assimp/anim.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

bgfx::VertexDecl noob::basic_model::vertex::ms_decl;


noob::basic_model::~basic_model()
{
	ready = false;
	if (vertex_buffer.idx != bgfx::invalidHandle)
	{
		bgfx::destroyVertexBuffer(vertex_buffer);
	}

	if (index_buffer.idx != bgfx::invalidHandle)
	{
		bgfx::destroyIndexBuffer(index_buffer);
	}
}


void noob::basic_model::init(const noob::basic_mesh& input_mesh)
{
	if (!ready)
	{
		noob::basic_model::vertex::init();

		const std::string file = input_mesh.save();

		const aiScene* scene = aiImportFileFromMemory(file.c_str(), file.size(), aiProcessPreset_TargetRealtime_Fast, "");

		if (!scene)
		{
			logger::log("[BasicModel] load - cannot open");
			return;
		}

		vertices.clear();
		indices.clear();

		const aiMesh* mesh_data = scene->mMeshes[0];

		size_t num_verts = mesh_data->mNumVertices;
		size_t num_faces = mesh_data->mNumFaces;
		auto num_indices = mesh_data->mNumFaces / 3;

		bool has_normals = mesh_data->HasNormals();

		// bbox.min = bbox.max = bbox.center = noob::vec3(0.0, 0.0, 0.0);	

		for ( size_t n = 0; n < num_verts; ++n)
		{
			noob::basic_model::vertex vertex;
			aiVector3D pt = mesh_data->mVertices[n];

			noob::vec3 vert;

			vert.v[0] = pt[0];
			vert.v[1] = pt[1];
			vert.v[2] = pt[2];

			bbox.min[0] = std::min(bbox.min[0], vert.v[0]);
			bbox.min[1] = std::min(bbox.min[1], vert.v[1]);
			bbox.min[2] = std::min(bbox.min[2], vert.v[2]);

			bbox.max[0] = std::max(bbox.max[0], vert.v[0]);
			bbox.max[1] = std::max(bbox.max[1], vert.v[1]);
			bbox.max[2] = std::max(bbox.max[2], vert.v[2]);

			vertex.position = vert.v;

			if (has_normals)
			{
				aiVector3D normal = mesh_data->mNormals[n];
				noob::vec3 norm;
				norm.v[0] = normal[0];
				norm.v[1] = normal[1];
				norm.v[2] = normal[2];
				vertex.normal = norm.v;
			}

			vertices.push_back(vertex);
		}

		// TODO: Test if necessary
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

		vertex_buffer = bgfx::createVertexBuffer(bgfx::copy(&vertices[0], vertices.size() * sizeof(noob::basic_model::vertex)), noob::basic_model::vertex::ms_decl);
		index_buffer = bgfx::createIndexBuffer(bgfx::copy(&indices[0], indices.size() * sizeof(uint16_t)));

		noob::vec3 dimensions = bbox.get_dims();
		fmt::MemoryWriter ww;
		ww << "[BasicModel] Load successful - " << vertices.size() << " vertices, " << indices.size() << " indices, max" << bbox.max.to_string() << ", min" << bbox.min.to_string() << ", dims" << dimensions.to_string();
		logger::log(ww.str());

		ready = true;
	}
}


void noob::basic_model::draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags) const
{
	if (ready)
	{
		// logger::log("[BasicModel] draw()");
		bgfx::setTransform(&model_mat.m[0]);
		bgfx::setVertexBuffer(vertex_buffer);
		bgfx::setIndexBuffer(index_buffer);
		bgfx::setState(bgfx_state_flags);
		bgfx::submit(view_id, prog);
	}
	else
	{
		logger::log("Attempting to draw item with improper state.");
	}
}
