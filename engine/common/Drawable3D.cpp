#include <bx/fpumath.h>
#include <bx/timer.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

#include "Graphics.hpp"
#include "Logger.hpp"
#include "Drawable3D.hpp"



noob::drawable3d::~drawable3d()
{
	kill_videocard_buffers();
}


void noob::drawable3d::init(const noob::basic_mesh& m)
{
	// logger::log(fmt::format("[Drawable3D] init. Mesh verts = {0}, indices = {1}, normals = {2}", m.vertices.size(), m.indices.size(), m.normals.size()));

	for ( size_t n = 0; n < m.vertices.size(); ++n)
	{
		noob::graphics::mesh_vertex vertex;
		
		vertex.pos = m.vertices[n].v;
		vertex.normal = m.normals[n].v;
		
		bgfx_vertices.push_back(vertex);
	}

	// logger::log(fmt::format("[Drawable3D] uploaded {0} verts", bgfx_vertices.size()));

	//std::copy(m.indices.begin(), m.indices.end(), std::back_inserter(indices));

	for (uint16_t i : m.indices)
	{
		indices.push_back(i);
	}


	// logger::log(fmt::format("[Drawable3D] uploaded {0} indices", indices.size()));

	vertex_buffer = bgfx::createVertexBuffer(bgfx::copy(&bgfx_vertices[0], bgfx_vertices.size() * sizeof(noob::graphics::mesh_vertex)), noob::graphics::mesh_vertex::ms_decl);
	index_buffer = bgfx::createIndexBuffer(bgfx::copy(&indices[0], indices.size() * sizeof(uint32_t)), BGFX_BUFFER_INDEX32);


	bbox = m.get_bbox();


	ready = true;
}


void noob::drawable3d::kill_videocard_buffers()
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


void noob::drawable3d::draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags) const
{
	bgfx::submit(view_id);
	if (ready)
	{
		bgfx::setTransform(&model_mat.m[0]);
		bgfx::setProgram(prog);
		bgfx::setVertexBuffer(vertex_buffer);
		bgfx::setIndexBuffer(index_buffer);
		bgfx::setState(bgfx_state_flags);
		bgfx::submit(view_id);
	}
	else
	{
		logger::log("Attempting to draw item with improper state.");
	}
}
