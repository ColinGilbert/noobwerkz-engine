#include "Model.hpp"
#include "Logger.hpp"

bgfx::VertexDecl noob::model::mesh_vertex::ms_decl;

noob::model::model(const std::string& filename) 
{
	ready = false;
	noob::model::mesh_vertex::init();
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


noob::model::model(const noob::basic_mesh& input_mesh)
{
	ready = false;
	noob::model::mesh_vertex::init();
	noob::model::mesh temp_mesh;
	// TODO: Find out why it breaks like this
	//meshes.push_back(temp_mesh);
	//temp_mesh = meshes[0];

	for (size_t n = 0; n < input_mesh.vertices.size(); ++n)
	{
		noob::model::mesh_vertex vertex;
		vertex.position = input_mesh.vertices[n].v;
		vertex.normal = input_mesh.normals[n].v;
		temp_mesh.vertices.push_back(vertex);
	}

	// logger::log(fmt::format("[Model] uploaded {0} verts", bgfx_vertices.size()));

	//std::copy(m.indices.begin(), m.indices.end(), std::back_inserter(indices));

	for (uint16_t i : input_mesh.indices)
	{
		temp_mesh.indices.push_back(i);
	}


	// logger::log(fmt::format("[Model] uploaded {0} indices", indices.size()));
	temp_mesh.vertex_buffer = bgfx::createVertexBuffer(bgfx::copy(&temp_mesh.vertices[0], temp_mesh.vertices.size() * sizeof(noob::model::mesh_vertex)), noob::model::mesh_vertex::ms_decl);
	temp_mesh.index_buffer = bgfx::createIndexBuffer(bgfx::copy(&temp_mesh.indices[0], temp_mesh.indices.size() * sizeof(uint16_t)));

	noob::basic_mesh::bbox_info bbox = input_mesh.get_bbox();
	temp_mesh.dimensions = (bbox.max - bbox.min).v;
	meshes.push_back(temp_mesh);
	ready = true;
	logger::log("[[Model] - load successful :)");
}


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
