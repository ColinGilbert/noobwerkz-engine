#include "AnimatedModel.hpp"
#include "Logger.hpp"


bgfx::VertexDecl noob::animated_model::mesh::vertex::ms_decl;


noob::animated_model::~animated_model()
{
	for (noob::animated_model::mesh m : meshes)
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


void noob::animated_model::init(const std::string& filename) 
{
	ready = false;
	noob::animated_model::mesh::vertex::init();
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

	for (noob::animated_model::mesh m : meshes)
	{
		m.vertex_buffer = bgfx::createVertexBuffer(bgfx::copy(&m.vertices[0], m.vertices.size() * sizeof(noob::animated_model::mesh::vertex)), noob::animated_model::mesh::vertex::ms_decl);
		m.index_buffer = bgfx::createIndexBuffer(bgfx::copy(&m.indices[0], m.indices.size() * sizeof(uint16_t)));
	}
	ready = true;
}


void noob::animated_model::draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags) const
{
	if (ready)
	{
		for (noob::animated_model::mesh m : meshes)
		{
			bgfx::setTransform(&model_mat.m[0]);
			// bgfx::setUniform(noob::graphics::get_uniform("normal_mat").handle, &normal_mat.m[0]);
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
