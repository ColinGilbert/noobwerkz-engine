#include "Model.hpp"
#include "Logger.hpp"

bgfx::VertexDecl noob::model::mesh::vertex::ms_decl;

void noob::model::init(const std::string& filename) 
{
	ready = false;
	noob::model::mesh::vertex::init();
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
		m.vertex_buffer = bgfx::createVertexBuffer(bgfx::copy(&m.vertices[0], m.vertices.size() * sizeof(noob::model::mesh::vertex)), noob::model::mesh::vertex::ms_decl);
		m.index_buffer = bgfx::createIndexBuffer(bgfx::copy(&m.indices[0], m.indices.size() * sizeof(uint16_t)));
	}
	ready = true;
}


void noob::model::init(const noob::basic_mesh& input_mesh)
{
	ready = false;
	
	noob::model::mesh::vertex::init();
	
	noob::model::mesh temp;
	
	for (size_t n = 0; n < input_mesh.vertices.size(); ++n)
	{
		noob::model::mesh::vertex vertex;
		vertex.position = input_mesh.vertices[n].v;
		vertex.normal = input_mesh.normals[n].v;
		temp.vertices.push_back(vertex);
	}

	for (uint32_t i : input_mesh.indices)
	{
		temp.indices.push_back(static_cast<uint16_t>(i));
	}
	
	temp.vertex_buffer = bgfx::createVertexBuffer(bgfx::copy(&temp.vertices[0], temp.vertices.size() * sizeof(noob::model::mesh::vertex)), noob::model::mesh::vertex::ms_decl);
	temp.index_buffer = bgfx::createIndexBuffer(bgfx::copy(&temp.indices[0], temp.indices.size() * sizeof(uint16_t)));

	noob::basic_mesh::bbox_info bbox = input_mesh.get_bbox();
	temp.dimensions = (bbox.max - bbox.min).v;
	meshes.push_back(temp);
	ready = true;
	logger::log("[Model] - load successful :)");
}


noob::model::~model()
{
	ready = false;
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
}


void noob::model::draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags) const
{
	if (ready)
	{
		logger::log("Attempting to draw model");
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
