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

std::map<const std::string, std::unique_ptr<noob::drawable3d>> noob::drawable3d::globals;

/*noob::drawable3d::drawable3d() : ready(false)
{
	noob::graphics::mesh_vertex::init();
}
*/

noob::drawable3d::~drawable3d()
{
	kill_videocard_buffers();
}


void noob::drawable3d::init(const noob::mesh& m)
{
	logger::log(fmt::format("[Drawable3D] init. Mesh verts = {0}, indices = {1}, normals = {2}", m.vertices.size(), m.indices.size(), m.normals.size()));

	for ( size_t n = 0; n < m.vertices.size(); ++n)
	{
		noob::graphics::mesh_vertex vertex;
		// Verts
		vertex.x_pos = m.vertices[n].v[0];
		vertex.y_pos = m.vertices[n].v[1];
		vertex.z_pos = m.vertices[n].v[2];
		
		vertex.x_normal = m.normals[n].v[0];
		vertex.y_normal = m.normals[n].v[1];
		vertex.z_normal = m.normals[n].v[2];

		bgfx_vertices.push_back(vertex);
	}

	logger::log(fmt::format("[Drawable3D] uploaded {0} verts", bgfx_vertices.size()));
	
	//std::copy(m.indices.begin(), m.indices.end(), std::back_inserter(indices));
	
	for (uint16_t i : m.indices)
	{
		indices.push_back(i);
	}


	logger::log(fmt::format("[Drawable3D] uploaded {0} indices", indices.size()));

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


std::tuple<bool, const noob::drawable3d*> noob::drawable3d::get(const std::string& name)
{
	auto it = globals.find(name);
	if (it != globals.end())
	{
		return std::make_tuple(true, it->second.get());
	}
	return std::make_tuple(false,nullptr);
}


void noob::drawable3d::add(const std::string& name, const noob::mesh& m)
{
	std::unique_ptr<noob::drawable3d> d = std::unique_ptr<noob::drawable3d>(new noob::drawable3d());
	d->init(m);
	globals.insert(std::make_pair(name, std::move(d)));
}
