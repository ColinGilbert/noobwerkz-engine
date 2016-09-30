#include "BasicModel.hpp"
#include "NoobUtils.hpp"
#include "StringFuncs.hpp"

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


void noob::basic_model::init(const noob::basic_mesh& arg)
{
	if (!ready)
	{
		logger::log(noob::importance::INFO, "[BasicModel] About to load.");
		noob::basic_model::vertex::init();

		vertices.clear();
		indices.clear();

		const uint32_t num_verts = arg.vertices.size();
		for (uint32_t i = 0; i < num_verts; ++i)
		{
			noob::basic_model::vertex v;
			noob::vec3 pos(arg.vertices[i]);
			noob::vec3 norml(arg.normals[i]);
			v.position[0] = pos.v[0];
			v.position[1] = pos.v[1];
			v.position[2] = pos.v[2];
			v.normal[0] = norml.v[0];
			v.normal[1] = norml.v[1];
			v.normal[2] = norml.v[2];
			vertices.push_back(v);
		}
		// NOTE: This doesn't take into account any integer overflow; the programmer is responsible for splitting huge meshes. This is to enforce machine-imposed size limits for 3D models.
		const uint32_t num_indices = std::min(static_cast<uint32_t>(arg.indices.size()), static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()));
		for (uint32_t i = 0; i < num_indices; ++i)
		{
			indices.push_back(static_cast<uint16_t>(arg.indices[i]));
		}

		vertex_buffer = bgfx::createVertexBuffer(bgfx::copy(&vertices[0], vertices.size() * sizeof(noob::basic_model::vertex)), noob::basic_model::vertex::ms_decl);
		index_buffer = bgfx::createIndexBuffer(bgfx::copy(&indices[0], indices.size() * sizeof(uint16_t)));

		bbox = arg.bbox;

		// bbox = arg.bbox;
		noob::vec3 dimensions = bbox.get_dims();
		noob::logger::log(noob::importance::INFO, noob::concat("[BasicModel] Load successful - ", noob::to_string(vertices.size()), " vertices, ", noob::to_string(indices.size()), " indices, max ", noob::to_string(bbox.max), ", min", noob::to_string(bbox.min), ", dims", noob::to_string(dimensions)));


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
		logger::log(noob::importance::ERROR, "[BasicModel] Attempting to draw item with improper state.");
	}
}
