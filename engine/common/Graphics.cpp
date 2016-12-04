#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Graphics.hpp"

#include <algorithm>
#include "GraphicsGLInternal.hpp"
#include "ShadersGL.hpp"
#include "NoobUtils.hpp"
#include "StringFuncs.hpp"

void noob::graphics::init(uint32_t width, uint32_t height) noexcept(true)
{
	instanced_shader = noob::graphics::program_handle::make(load_program_gl(noob::glsl::vs_instancing_src, noob::glsl::fs_instancing_src));

	u_eye_pos = glGetUniformLocation(instanced_shader.index(), "eye_pos");

	check_error_gl();

	u_light_directional = glGetUniformLocation(instanced_shader.index(), "directional_light");

	check_error_gl();

	frame(width, height);

	noob::logger::log(noob::importance::INFO, "[Graphics] Done init.");
}


void noob::graphics::destroy() noexcept(true)
{

}
/*
   noob::model_handle noob::graphics::model(noob::model::geom_type geom, const noob::basic_mesh& mesh) noexcept(true)
   {
   noob::model_handle results;
   GLuint vao_id = 0;

   switch (geom)
   {
   case(noob::model::geom_type::INDEXED_MESH):
   {
   break;
   }
   case(noob::model::geom_type::DYNAMIC_TERRAIN):
   {
   break;
   }
   case(noob::model::geom_type::BILLBOARD):
   {
   break;
   }
   case(noob::model::geom_type::POINT_SPRITE):
   {
   break;
   }
   default:
   {
   noob::logger::log(noob::importance::ERROR, "[Graphics] Reached past the valid enum values in switch statement. WTF?!");
   }
   }

// Reset to the default VAO
glBindVertexArray(0);

return noob::model_handle::make(vao_id);
}
*/


void noob::graphics::use_program(noob::graphics::program_handle arg) noexcept(true)
{
	glUseProgram(arg.index());
	check_error_gl();
}

noob::model_handle noob::graphics::model_instanced(const noob::basic_mesh& Mesh, uint32_t NumInstances) noexcept(true)
{
	noob::model result;

	if (NumInstances == 0)
	{
		noob::model_handle h;
		return h;
	}

	result.type = noob::model::geom_type::INDEXED_MESH;

	result.n_instances = NumInstances;

	const uint32_t num_indices = Mesh.indices.size();
	result.n_indices = num_indices;

	const uint32_t num_verts = Mesh.vertices.size();
	result.n_vertices = num_verts;

	GLuint vao_id = 0;

	glGenVertexArrays(1, &vao_id);

	glBindVertexArray(vao_id);
	result.vao = vao_id;

	////////////////////////////////
	// Create & bind attrib buffers
	////////////////////////////////

	std::array<GLuint, 4> vbo_ids;
	glGenBuffers(4, &vbo_ids[0]);

	const GLuint vertices_vbo = vbo_ids[0];
	const GLuint colours_vbo = vbo_ids[1];
	const GLuint matrices_vbo = vbo_ids[2];
	const GLuint indices_vbo = vbo_ids[3];

	/////////////////////////////////////////////////
	// Setup non-instanced, interleaved attribs VBO:
	/////////////////////////////////////////////////

	std::vector<noob::vec4> interleaved;
	interleaved.resize(num_verts * 3);

	// Interleave our vertex positions, normals, and colours
	for(uint32_t i = 0; i < num_verts; i++)
	{
		const uint32_t current_offset = i * 3;
		interleaved[current_offset] = noob::vec4(Mesh.vertices[i].v[0], Mesh.vertices[i].v[1], Mesh.vertices[i].v[2], 1.0);
		interleaved[current_offset + 1] = noob::vec4(Mesh.normals[i].v[0], Mesh.normals[i].v[1], Mesh.normals[i].v[2], 0.0);
		interleaved[current_offset + 2] = Mesh.colours[i];
	}

	// Upload interleaved buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(noob::vec4), &interleaved[0], GL_STATIC_DRAW);
	result.vertices_vbo = vertices_vbo;


	//////////////////////
	// Setup attrib specs
	//////////////////////

	// Positions
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4)*3, reinterpret_cast<const void *>(0));
	// Normals
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4)*3, reinterpret_cast<const void *>(sizeof(noob::vec4)));
	// Vertex colours
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4)*3, reinterpret_cast<const void *>(sizeof(noob::vec4)*2));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Setup colours VBO:
	// std::vector<noob::vec4> colours(NumInstances, noob::vec4(1.0, 1.0, 1.0, 1.0));
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::materials_stride, nullptr, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::materials_stride, &colours[0].v[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(0));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	result.instanced_colour_vbo = colours_vbo;

	// Setup matrices VBO:
	// std::vector<noob::mat4> matrices(NumInstances * 2, noob::identity_mat4());
	glBindBuffer(GL_ARRAY_BUFFER, matrices_vbo);
	glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::matrices_stride, nullptr, GL_DYNAMIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::matrices_stride, &matrices[0].m[0], GL_DYNAMIC_DRAW);


	// Per instance model matrices
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(0));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*2));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*3));
	// Per-instance MVP matrices
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*4));
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*5));
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*6));
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*7));

	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glEnableVertexAttribArray(9);
	glEnableVertexAttribArray(10);
	glEnableVertexAttribArray(11);

	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
	glVertexAttribDivisor(10, 1);
	glVertexAttribDivisor(11, 1);

	result.instanced_matrices_vbo = matrices_vbo;

	// Upload to indices buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mesh.indices.size() * sizeof(uint32_t), &Mesh.indices[0], GL_STATIC_DRAW);
	result.indices_vbo = indices_vbo;


	glBindVertexArray(0);

	check_error_gl();

	noob::model_handle h = models.add(result);

	const noob::bbox bb = Mesh.bbox;
	noob::logger::log(noob::importance::INFO, noob::concat("[Graphics] Created model with handle ", noob::to_string(h.index()), " and ", noob::to_string(NumInstances), " instances. Verts = ", noob::to_string(Mesh.vertices.size()), ", indices = ", noob::to_string(Mesh.indices.size()), ". Dims: ", noob::to_string(bb.max - bb.min)));

	return h;
}


void noob::graphics::reset_instances(noob::model_handle Handle, uint32_t NumInstances) noexcept(true)
{
	noob::model m = models.get(Handle);
	m.n_instances = NumInstances;
	models.set(Handle, m);

	glBindVertexArray(m.vao);

	// Setup colours VBO:
	glBindBuffer(GL_ARRAY_BUFFER, m.instanced_colour_vbo);
	glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::materials_stride, nullptr, GL_DYNAMIC_DRAW);

	// Setup matrices VBO:
	glBindBuffer(GL_ARRAY_BUFFER, m.instanced_matrices_vbo);
	glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::matrices_stride, nullptr, GL_DYNAMIC_DRAW);

	check_error_gl();

	glBindVertexArray(0);
}

noob::texture_2d_handle noob::graphics::reserve_texture_2d(uint32_t Width, uint32_t Height, const noob::texture_info TexInfo) noexcept(true)
{
	const noob::return_type<GLenum> fmt = get_internal_format(TexInfo);

	if (fmt.valid)
	{
		const GLuint texture_id = prep_texture();
		glBindTexture(GL_TEXTURE_2D, texture_id);

		uint32_t mips = 1;
		if (TexInfo.mips)
		{
			mips = noob::get_num_mips(Width, Height);
		}

		glTexStorage2D(GL_TEXTURE_2D, mips, fmt.value, Width, Height);

		check_error_gl();

		noob::texture_2d t(texture_id, TexInfo, Width, Height);

		textures_2d.push_back(t);

		return noob::texture_2d_handle::make(textures_2d.size() - 1);
	}

	// If we haven't returned our texture by now, we can assume the format test failed. Boo.
	return noob::texture_2d_handle::make_invalid();
}


noob::texture_array_2d_handle noob::graphics::reserve_texture_array_2d(uint32_t Width, uint32_t Height, uint32_t Indices, const noob::texture_info TexInfo) noexcept(true)
{
	const noob::return_type<GLenum> fmt = get_internal_format(TexInfo);

	if (fmt.valid)
	{
		const GLuint texture_id = prep_texture();
		glBindTexture(GL_TEXTURE_2D, texture_id);

		uint32_t mips = 1;
		if (TexInfo.mips)
		{
			mips = noob::get_num_mips(Width, Height);
		}

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mips, fmt.value, Width, Height, Indices);

		check_error_gl();

		noob::texture_array_2d t(texture_id, TexInfo, Width, Height, Indices);

		texture_arrays_2d.push_back(t);

		return noob::texture_array_2d_handle::make(texture_arrays_2d.size() - 1);
	}

	return noob::texture_array_2d_handle::make_invalid();
}


noob::texture_3d_handle noob::graphics::reserve_texture_3d(uint32_t Width, uint32_t Height, uint32_t Depth, const noob::texture_info TexInfo) noexcept(true)
{
	const noob::return_type<GLenum> fmt = get_internal_format(TexInfo);

	if (fmt.valid)
	{
		const GLuint texture_id = prep_texture();
		glBindTexture(GL_TEXTURE_2D, texture_id);

		uint32_t mips = 1;
		if (TexInfo.mips)
		{
			mips = noob::get_num_mips(Width, Height, Depth);
		}

		// TODO: Add check for devices that don't support compressed 3D texture storage. For now, we'll find out the exciting way.

		glTexStorage3D(GL_TEXTURE_3D, mips, fmt.value, Width, Height, Depth);

		check_error_gl();

		noob::texture_3d t(texture_id, TexInfo, Width, Height, Depth);

		textures_3d.push_back(t);

		return noob::texture_3d_handle::make(textures_3d.size() - 1);
	}

	return noob::texture_3d_handle::make_invalid();
}



void noob::graphics::texture_data(noob::texture_2d_handle Handle, uint32_t Mip, const std::string& Data) const noexcept(true)
{

	check_error_gl();

}


void noob::graphics::texture_data(noob::texture_array_2d_handle Handle, uint32_t Index, uint32_t Mip, const std::string& Data) const noexcept(true)
{

	check_error_gl();

}


void noob::graphics::texture_data(noob::texture_3d_handle Handle, uint32_t Mip, const std::string& Data) const noexcept(true)
{

	check_error_gl();

}


// Texture parameter setters, made typesafe. :)
void noob::graphics::texture_base_level(uint32_t Mip) const noexcept(true)
{

	check_error_gl();

}


void noob::graphics::texture_compare_mode(noob::tex_compare_mode CompareMode) const noexcept(true)
{

	check_error_gl();

}


void noob::graphics::texture_compare_func(noob::tex_compare_func CompareFunc) const noexcept(true)
{

	check_error_gl();

}


void noob::graphics::texture_min_filter(noob::tex_min_filter MinFilter) const noexcept(true)
{
	check_error_gl();

}


void noob::graphics::texture_min_lod(int32_t MinLod) const noexcept(true)
{
	check_error_gl();

}


void noob::graphics::texture_max_lod(int32_t MaxLod) const noexcept(true)
{
	check_error_gl();

}


void noob::graphics::texture_swizzle(const std::array<noob::tex_swizzle, 4> Swizzles) const noexcept(true)
{
	check_error_gl();

}


void noob::graphics::texture_wrap_mode(const std::array<noob::tex_wrap_mode, 3> WrapModes) const noexcept(true)
{
	check_error_gl();

}


void noob::graphics::texture_pack_alignment(uint32_t) const noexcept(true)
{
	// GL_PACK_ALIGNMENT 	integer 	4 	1, 2, 4, or 8
	check_error_gl();

}


void noob::graphics::texture_unpack_alignment(uint32_t) const noexcept(true)
{
	// GL_UNPACK_ALIGNMENT 	integer 	4 	1, 2, 4, or 8 
	check_error_gl();
}



void noob::graphics::draw(const noob::model_handle handle, uint32_t num) const noexcept(true)
{
	const noob::model m = models.get(handle);
	glBindVertexArray(m.vao);

	glDrawElementsInstanced(GL_TRIANGLES, m.n_indices, GL_UNSIGNED_INT, reinterpret_cast<const void *>(0), std::min(m.n_instances, num));

	check_error_gl();
	glBindVertexArray(0);
}


void noob::graphics::frame(uint32_t width, uint32_t height) const noexcept(true)
{
	glBindVertexArray(0);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glClearColor(0.2f, 0.0f, 0.2f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


noob::gpu_write_buffer noob::graphics::map_buffer(noob::model_handle h, noob::model::instanced_data_type t, uint32_t min, uint32_t max) const noexcept(true)
{
	noob::model m = models.get(h);

	if (m.type != noob::model::geom_type::INDEXED_MESH)
	{
		return noob::gpu_write_buffer::make_invalid();
	}

	// glBindVertexArray(m.vao);	

	uint32_t stride_in_bytes;

	switch (t)
	{
		case (noob::model::instanced_data_type::COLOUR):
			{
				stride_in_bytes = noob::model::materials_stride;
				glBindBuffer(GL_ARRAY_BUFFER, m.instanced_colour_vbo);
				check_error_gl();
				break;
			}
		case (noob::model::instanced_data_type::MATRICES):
			{
				stride_in_bytes = noob::model::matrices_stride;
				glBindBuffer(GL_ARRAY_BUFFER, m.instanced_matrices_vbo);
				check_error_gl();
				break;
			}
	}

	const uint32_t total_size = stride_in_bytes * m.n_instances;

	float* ptr = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, min, max, GL_MAP_WRITE_BIT));

	check_error_gl();

	if (ptr != nullptr)
	{
		return noob::gpu_write_buffer(ptr, total_size / sizeof(float));
	}
	else
	{
		return noob::gpu_write_buffer::make_invalid();	
	}


}


void noob::graphics::unmap_buffer() const noexcept(true)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);

	check_error_gl();
}


noob::graphics::program_handle noob::graphics::get_instanced() const noexcept(true)
{
	return instanced_shader;
}


void noob::graphics::eye_pos(const noob::vec3& arg) const noexcept(true)
{
	glUniform3fv(u_eye_pos, 1, &arg.v[0]);

	check_error_gl();
}


void noob::graphics::light_direction(const noob::vec3& arg) const noexcept(true)
{
	glUniform3fv(u_light_directional, 1, &arg.v[0]);

	check_error_gl();
}
