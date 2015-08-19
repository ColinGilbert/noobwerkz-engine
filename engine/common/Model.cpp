#include "Model.hpp"
#include "Logger.hpp"

bgfx::VertexDecl noob::model::mesh_vertex::ms_decl;

noob::model::~model()
{
	for (noob::model::mesh m : meshes)
	{
	if (m.vertex_buffer.idx != bgfx::invalidHandle)
	{
		bgfx::destroyVertexBuffer(m.vertex_buffer);
	}

	if (m.index_buffer.idx != bgfx::invalidHandle)
	{
		bgfx::destroyIndexBuffer(m.index_buffer);
	}
	}
	ready = false;
}


void noob::model::load(const std::string& filename)
{
	if (!ready)
	{	
		std::ifstream is(filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive archive(is);

		try
		{
			archive(*this);
		}
		catch (const cereal::Exception& e)
		{
			logger::log(fmt::format("Could not properly open archive {0}", filename));
			return;
		}
		
		for (noob::model::mesh m : meshes)
		{
			m.vertex_buffer = bgfx::createVertexBuffer(bgfx::copy(&m.vertices[0], m.vertices.size() * sizeof(noob::model::mesh_vertex)), noob::model::mesh_vertex::ms_decl);
			m.index_buffer = bgfx::createIndexBuffer(bgfx::copy(&m.indices[0], m.indices.size() * sizeof(uint16_t)));
		}
		
		ready = true;
	}
}


void noob::model::draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags) const
{
	if (ready)
	{
		for (noob::model::mesh m : meshes)
		{
			bgfx::setTransform(&model_mat.m[0]);
			bgfx::setVertexBuffer(m.vertex_buffer);
			bgfx::setIndexBuffer(m.index_buffer);
			bgfx::setState(bgfx_state_flags);
			bgfx::submit(view_id, prog);
		}
	}
	else
	{
		logger::log("Attempting to draw item with improper state.");
	}
}
