#include "BasicModel.hpp"
/*
bgfx::VertexDecl noob::basic_model::vertex::ms_decl;

noob::basic_model::basic_model(const noob::basic_mesh& input_mesh)
{
	ready = false;
	noob::basic_model::vertex::init();
	for (size_t n = 0; n < input_mesh.vertices.size(); ++n)
	{
		noob::basic_model::vertex vertex;
		vertex.position = input_mesh.vertices[n].v;
		vertex.normal = input_mesh.normals[n].v;
		vertices.push_back(vertex);
	}

	for (uint16_t i : input_mesh.indices)
	{
		indices.push_back(i);
	}

	vertex_buffer = bgfx::createVertexBuffer(bgfx::copy(&vertices[0], vertices.size() * sizeof(noob::basic_model::vertex)), noob::basic_model::vertex::ms_decl);
	index_buffer = bgfx::createIndexBuffer(bgfx::copy(&indices[0], indices.size() * sizeof(uint16_t)));

	noob::basic_mesh::bbox_info bbox = input_mesh.get_bbox();
	dimensions = (bbox.max - bbox.min).v;
	ready = true;
//	logger::log("[[Model] - load successful :)");
}


noob::basic_model::~basic_model()
{
	if (vertex_buffer.idx != bgfx::invalidHandle)
	{
		bgfx::destroyVertexBuffer(vertex_buffer);
	}

	if (index_buffer.idx != bgfx::invalidHandle)
	{
		bgfx::destroyIndexBuffer(index_buffer);
	}
	ready = false;
}


void noob::basic_model::draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags) const
{
	if (ready)
	{
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
*/
