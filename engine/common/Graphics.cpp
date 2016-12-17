// Header files we're implementing
#include "Graphics.hpp"

// std
#include <algorithm>

// Project-local
#include "GraphicsGLInternal.hpp"
#include "ShadersGL.hpp"
#include "NoobUtils.hpp"
#include "StringFuncs.hpp"

void noob::graphics::init(const noob::vec2ui Dims, const noob::texture_loader_2d& TexLoader) noexcept(true)
{
	instanced_shader = noob::graphics::program_handle::make(load_program_gl(noob::glsl::vs_instancing_src, noob::glsl::fs_instancing_src));
	check_error_gl();
	u_eye_pos = glGetUniformLocation(instanced_shader.index(), "eye_pos");
	check_error_gl();	
	u_light_directional = glGetUniformLocation(instanced_shader.index(), "directional_light");
	check_error_gl();

	noob::logger::log(noob::importance::INFO, "Loading terrain shader!");

	terrain_shader = noob::graphics::program_handle::make(load_program_gl(noob::glsl::vs_terrain_src, noob::glsl::fs_terrain_src));
	check_error_gl();

	// Init terrain
	max_terrain_verts = 4096;

	terrain_unis.texture_weights = noob::vec3f(0.3, 0.3, 0.4);
	terrain_unis.colours[0] = noob::vec4f(0.0, 0.5, 0.4, 1.0);
	terrain_unis.colours[1] = noob::vec4f(0.1, 0.5, 0.5, 1.0);
	terrain_unis.colours[2] = noob::vec4f(0.5, 0.5, 0.5, 1.0);
	terrain_unis.colours[3] = noob::vec4f(1.0, 1.0, 1.0, 1.0);	
	terrain_unis.colour_offsets = noob::vec2f(0.6, 0.8);
	terrain_unis.texture_scales = noob::vec3f(0.5, 0.5, 0.5);

	u_mvp_terrain = glGetUniformLocation(terrain_shader.index(), "mvp");
	u_texture_0 = glGetUniformLocation(terrain_shader.index(), "texture_0");
	u_colour_0 = glGetUniformLocation(terrain_shader.index(), "colour_0");
	u_colour_1 = glGetUniformLocation(terrain_shader.index(), "colour_1");
	u_colour_2 = glGetUniformLocation(terrain_shader.index(), "colour_2");
	u_colour_3 = glGetUniformLocation(terrain_shader.index(), "colour_3");
	u_blend_0 = glGetUniformLocation(terrain_shader.index(), "blend_0");
	u_blend_1 = glGetUniformLocation(terrain_shader.index(), "blend_1");
	u_tex_scales = glGetUniformLocation(terrain_shader.index(), "tex_scales");
	u_eye_pos_terrain = glGetUniformLocation(terrain_shader.index(), "eye_pos");
	u_light_directional_terrain = glGetUniformLocation(terrain_shader.index(), "directional_light");
	check_error_gl();

	noob::texture_info texinfo;
	terrain_tex = texture_2d(TexLoader, true);
	check_error_gl();

	texture_wrap_mode(std::array<noob::tex_wrap_mode, 2>({tex_wrap_mode::REPEAT, tex_wrap_mode::REPEAT}));
	texture_mag_filter(noob::tex_mag_filter::NEAREST);
	texture_min_filter(noob::tex_min_filter::NEAREST);
	check_error_gl();

	glGenerateMipmap(GL_TEXTURE_2D);

	glGenVertexArrays(1, &terrain_model.vao);
	glBindVertexArray(terrain_model.vao);
	glGenBuffers(1, &terrain_model.vertices_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_model.vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, max_terrain_verts * noob::model::terrain_stride, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, noob::model::terrain_stride, reinterpret_cast<const void *>(0));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, noob::model::terrain_stride, reinterpret_cast<const void *>(sizeof(noob::vec4f)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, noob::model::terrain_stride, reinterpret_cast<const void *>(sizeof(noob::vec4f)*2));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	check_error_gl();

	glBindVertexArray(0);

	// Do an initial frame and log
	frame(Dims);
	noob::logger::log(noob::importance::INFO, "[Graphics] Done init.");
}


void noob::graphics::frame(const noob::vec2ui Dims) const noexcept(true)
{
	glBindVertexArray(0);
	glViewport(0, 0, Dims[0], Dims[1]);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glClearColor(0.2f, 0.0f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	check_error_gl();
}


void noob::graphics::destroy() noexcept(true)
{

}


void noob::graphics::use_program(noob::graphics::program_handle Arg) const noexcept(true)
{
	glUseProgram(Arg.index());
	check_error_gl();
}


void noob::graphics::set_view_mat(const noob::mat4f ViewMat) noexcept(true)
{
	eye_pos = noob::translation_from_mat4(ViewMat);
	view_mat = ViewMat;
}


void noob::graphics::set_projection_mat(const noob::mat4f ProjMat) noexcept(true)
{
	proj_mat = ProjMat;
}


void noob::graphics::upload_instanced_uniforms() const noexcept(true)
{
	glUniform3fv(u_eye_pos_terrain, 1, &eye_pos[0]);
	glUniform3fv(u_light_directional_terrain, 1, &light_direction[0]);
	check_error_gl();
}


void noob::graphics::draw_instanced(const noob::model_handle Handle, uint32_t NumInstances) const noexcept(true)
{
	const noob::model m = models[Handle.index()];

	if (m.type == noob::model::geom_type::INDEXED_MESH)
	{
		glBindVertexArray(m.vao);
		glDrawElementsInstanced(GL_TRIANGLES, m.n_indices, GL_UNSIGNED_INT, reinterpret_cast<const void *>(0), std::min(m.n_instances, NumInstances));
		check_error_gl();
		glBindVertexArray(0);		
	}
}


void noob::graphics::draw_terrain(uint32_t Verts) const noexcept(true)
{
	glBindVertexArray(terrain_model.vao);
	use_program(terrain_shader);
	glActiveTexture(GL_TEXTURE0);
	check_error_gl();
	bind_texture(terrain_tex);
	upload_terrain_uniforms();
	glDrawArrays(GL_TRIANGLES, 0, Verts);
	check_error_gl();
	glBindVertexArray(0);
}


noob::model_handle noob::graphics::model_instanced(const noob::mesh_3d& Mesh, uint32_t NumInstances) noexcept(true)
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

	std::vector<noob::vec4f> interleaved;
	interleaved.resize(num_verts * 3);

	// Interleave our vertex positions, normals, and colours
	for(uint32_t i = 0; i < num_verts; ++i)
	{
		const uint32_t current_offset = i * 3;
		interleaved[current_offset] = noob::vec4f(Mesh.vertices[i].v[0], Mesh.vertices[i].v[1], Mesh.vertices[i].v[2], 1.0);
		interleaved[current_offset + 1] = noob::vec4f(Mesh.normals[i].v[0], Mesh.normals[i].v[1], Mesh.normals[i].v[2], 0.0);
		interleaved[current_offset + 2] = Mesh.colours[i];
	}

	// Upload interleaved buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(noob::vec4f), &interleaved[0], GL_STATIC_DRAW);
	result.vertices_vbo = vertices_vbo;

	//////////////////////
	// Setup attrib specs
	//////////////////////

	// Positions
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4f)*3, reinterpret_cast<const void *>(0));
	// Normals
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4f)*3, reinterpret_cast<const void *>(sizeof(noob::vec4f)));
	// Vertex colours
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4f)*3, reinterpret_cast<const void *>(sizeof(noob::vec4f)*2));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Setup colours VBO:
	// std::vector<noob::vec4f> colours(NumInstances, noob::vec4f(1.0, 1.0, 1.0, 1.0));
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::materials_stride, nullptr, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::materials_stride, &colours[0].v[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4f), reinterpret_cast<const void *>(0));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	result.instanced_colours_vbo = colours_vbo;

	// Setup matrices VBO:
	// std::vector<noob::mat4f> matrices(NumInstances * 2, noob::identity_mat4());
	glBindBuffer(GL_ARRAY_BUFFER, matrices_vbo);
	glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::matrices_stride, nullptr, GL_DYNAMIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::matrices_stride, &matrices[0].m[0], GL_DYNAMIC_DRAW);

	// Per instance model matrices
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(0));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*2));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*3));
	// Per-instance MVP matrices
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*4));
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*5));
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*6));
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*7));

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

	result.valid = true;
	models.push_back(result);

	const noob::bbox_type<float> bb = Mesh.bbox;

	const noob::model_handle results = noob::model_handle::make(models.size() - 1);

	noob::logger::log(noob::importance::INFO, noob::concat("[Graphics] Created instanced model with handle ", noob::to_string(results.index()), " and ", noob::to_string(NumInstances), " instances. Verts = ", noob::to_string(Mesh.vertices.size()), ", indices = ", noob::to_string(Mesh.indices.size()), ". Dims: ", noob::to_string(bb.max - bb.min)));

	return results;
}


void noob::graphics::resize_instanced_data_buffers(noob::model_handle Handle, uint32_t NumInstances) noexcept(true)
{
	if( models[Handle.index()].type == noob::model::geom_type::INDEXED_MESH)
	{
		noob::model m = models[Handle.index()];
		m.n_instances = NumInstances;
		models[Handle.index()] = m;

		glBindVertexArray(m.vao);

		// Setup colours VBO:
		glBindBuffer(GL_ARRAY_BUFFER, m.instanced_colours_vbo);
		glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::materials_stride, nullptr, GL_DYNAMIC_DRAW);

		// Setup matrices VBO:
		glBindBuffer(GL_ARRAY_BUFFER, m.instanced_matrices_vbo);
		glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::model::matrices_stride, nullptr, GL_DYNAMIC_DRAW);

		check_error_gl();

		glBindVertexArray(0);
	}
}


void noob::graphics::resize_terrain(uint32_t MaxVerts) noexcept(true)
{
	max_terrain_verts = MaxVerts;

	glBindVertexArray(terrain_model.vao);

	glBindBuffer(GL_ARRAY_BUFFER, terrain_model.vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, max_terrain_verts * noob::model::terrain_stride, nullptr, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	check_error_gl();

}


uint32_t noob::graphics::get_max_terrain_verts() const noexcept(true)
{
	return max_terrain_verts;
}


void noob::graphics::set_terrain_uniforms(const noob::terrain_shading Shading) noexcept(true)
{
	terrain_unis = Shading;

}

void noob::graphics::upload_terrain_uniforms() const noexcept(true)
{
	const noob::mat4f mvp = proj_mat * view_mat;
	glUniform1i(u_texture_0, 0);
	glUniformMatrix4fv(u_mvp_terrain, 1, false, &mvp[0]);
	glUniform4fv(u_colour_0, 1, &terrain_unis.colours[0][0]);
	glUniform4fv(u_colour_1, 1, &terrain_unis.colours[1][0]);
	glUniform4fv(u_colour_2, 1, &terrain_unis.colours[2][0]);
	glUniform4fv(u_colour_3, 1, &terrain_unis.colours[3][0]);
	glUniform3fv(u_blend_0, 1, &terrain_unis.texture_weights[0]);
	glUniform2fv(u_blend_1, 1, &terrain_unis.colour_offsets[0]);
	glUniform3fv(u_tex_scales, 1, &terrain_unis.texture_scales[0]);
	glUniform3fv(u_eye_pos_terrain, 1, &eye_pos[0]);
	glUniform3fv(u_light_directional_terrain, 1, &light_direction[0]);
}


void noob::graphics::set_light_direction(const noob::vec3f& Arg) noexcept(true)
{
	light_direction = noob::normalize(Arg);
}


noob::gpu_write_buffer noob::graphics::map_instanced_data_buffer(noob::model_handle Handle, noob::model::instanced_data_type DataType, uint32_t Min, uint32_t Max) const noexcept(true)
{
	const noob::model m = models[Handle.index()];
	uint32_t stride_in_bytes;

	if (m.type == noob::model::geom_type::INDEXED_MESH)
	{
		// glBindVertexArray(m.vao);	
		switch (DataType)
		{
			case (noob::model::instanced_data_type::COLOUR):
				{
					stride_in_bytes = noob::model::materials_stride;
					glBindBuffer(GL_ARRAY_BUFFER, m.instanced_colours_vbo);
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
	}
	else
	{
		return noob::gpu_write_buffer::make_invalid();	
	}

	const uint32_t total_size = stride_in_bytes * m.n_instances;

	float* ptr = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, Min, Max, GL_MAP_WRITE_BIT));

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


noob::gpu_write_buffer noob::graphics::map_terrain_buffer(uint32_t Min, uint32_t Max) const noexcept(true)
{
	// if(terrain_initialized)
	//{
	if (Max < max_terrain_verts)
	{
		// glBindVertexArray(terrain_model.vao);
		glBindBuffer(GL_ARRAY_BUFFER, terrain_model.vertices_vbo);
		check_error_gl();

		const uint32_t total_size = noob::model::terrain_stride * (Max - Min);

		float* ptr = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, Min, Max, GL_MAP_WRITE_BIT));
		check_error_gl();

		if (ptr != nullptr)
		{
			return noob::gpu_write_buffer(ptr, total_size / sizeof(float));
		}
	}
	// }
	return noob::gpu_write_buffer::make_invalid();	

}


noob::gpu_write_buffer noob::graphics::map_text_buffer(uint32_t Idx, uint32_t Min, uint32_t Max) const noexcept(true)
{
/*	
	if (Max < max_text_verts)
	{
		// glBindVertexArray(terrain_model.vao);
		glBindBuffer(GL_ARRAY_BUFFER, terrain_model.vertices_vbo);
		check_error_gl();

		const uint32_t total_size = noob::model::text_stride * (Max - Min);

		float* ptr = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, Min, Max, GL_MAP_WRITE_BIT));
		check_error_gl();

		if (ptr != nullptr)
		{
			return noob::gpu_write_buffer(ptr, total_size / sizeof(float));
		}
	}
*/	
	
	return noob::gpu_write_buffer::make_invalid();	
}


void noob::graphics::unmap_buffer() const noexcept(true)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);

	check_error_gl();
}


noob::texture_2d_handle noob::graphics::texture_2d(const noob::texture_loader_2d& TextureLoader, bool GenMips) noexcept(true)
{
	const GLenum sized_fmt = get_gl_storage_format(TextureLoader.format());

	// Prevent leftover from previous calls from harming this.
	glBindVertexArray(0);

	GLuint texture_id;

	glGenTextures(1, &texture_id);

	glBindTexture(GL_TEXTURE_2D, texture_id);

	const noob::vec2ui dims = TextureLoader.dimensions();

	uint32_t mips = 1;
	if (TextureLoader.mips() || GenMips)
	{
		mips = noob::get_num_mips(dims);
	}

	//	glTexStorage2D(GL_TEXTURE_2D, mips, sized_fmt, dims[0], dims[1]);

	check_error_gl();

	if (TextureLoader.compressed())
	{
		uint32_t data_size;
		if (TextureLoader.format() == noob::pixel_format::RGBA8_COMPRESSED || TextureLoader.format() == noob::pixel_format::SRGBA8_COMPRESSED)
		{
			data_size = get_compressed_size_rgba8(dims[0], dims[1]);
		}
		else
		{
			data_size = get_compressed_size_rgb8(dims[0], dims[1]);
		}
		// glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dims[0], dims[1], sized_fmt, data_size, reinterpret_cast<const GLvoid*>(TextureLoader.buffer()));

		// glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data);

		check_error_gl();		
	}
	else
	{
		const GLenum unsized_fmt = get_pixel_format_unsized(TextureLoader.format());
		// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dims[0], dims[1], fmt, GL_UNSIGNED_BYTE, reinterpret_cast<const GLvoid*>(TextureLoader.buffer()));	

		glTexImage2D(GL_TEXTURE_2D, 0, unsized_fmt, dims[0], dims[1], 0, unsized_fmt, GL_UNSIGNED_BYTE, reinterpret_cast<const GLvoid*>(TextureLoader.buffer()));
		check_error_gl();
	}

	if (GenMips)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	noob::texture_2d t(texture_id, noob::texture_info(mips > 1, TextureLoader.format()), dims[0], dims[1]);
	textures_2d.push_back(t);
	noob::logger::log(noob::importance::INFO, noob::concat("[Graphics] Created 2D texture of size ", noob::to_string(dims[0]), "-", noob::to_string(dims[1]), " with format ", noob::to_string(TextureLoader.format()), " and ", noob::to_string(mips) ," mips."));

	return noob::texture_2d_handle::make(textures_2d.size() - 1);
}



noob::texture_array_2d_handle noob::graphics::texture_array_2d(const noob::vec2ui Dims, uint32_t Indices, const noob::texture_info TexInfo) noexcept(true)
{
	const GLenum fmt = get_gl_storage_format(TexInfo.pixels);
	const GLuint texture_id = prep_texture();
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

	uint32_t mips = 1;
	if (TexInfo.mips)
	{
		mips = noob::get_num_mips(Dims);
	}

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mips, fmt, Dims[0], Dims[1], Indices);

	check_error_gl();

	noob::texture_array_2d t(texture_id, TexInfo, Dims[0], Dims[1], Indices);

	texture_arrays_2d.push_back(t);

	return noob::texture_array_2d_handle::make(texture_arrays_2d.size() - 1);
}


noob::texture_3d_handle noob::graphics::texture_3d(const noob::vec3ui Dims, const noob::texture_info TexInfo) noexcept(true)
{
	const GLenum fmt = get_gl_storage_format(TexInfo.pixels);
	const GLuint texture_id = prep_texture();
	glBindTexture(GL_TEXTURE_3D, texture_id);

	uint32_t mips = 1;
	if (TexInfo.mips)
	{
		mips = noob::get_num_mips(Dims);
	}

	// TODO: Add check for devices that don't support compressed 3D texture storage. For now, we'll find out the exciting way.

	glTexStorage3D(GL_TEXTURE_3D, mips, fmt, Dims[0], Dims[1], Dims[2]);

	check_error_gl();

	noob::texture_3d t(texture_id, TexInfo, Dims[0], Dims[1], Dims[2]);

	textures_3d.push_back(t);

	return noob::texture_3d_handle::make(textures_3d.size() - 1);
}


void noob::graphics::bind_texture(noob::texture_2d_handle Handle) const noexcept(true)
{
	glBindTexture(GL_TEXTURE_2D, textures_2d[Handle.index()].driver_handle);
	check_error_gl();
}


void noob::graphics::bind_texture(noob::texture_array_2d_handle Handle) const noexcept(true)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_arrays_2d[Handle.index()].driver_handle);
	check_error_gl();
}


void noob::graphics::bind_texture(noob::texture_3d_handle Handle) const noexcept(true)
{
	glBindTexture(GL_TEXTURE_3D, textures_3d[Handle.index()].driver_handle);
	check_error_gl();
}

/*
   void noob::graphics::texture_data(noob::texture_2d_handle Handle, uint32_t Mip, const noob::vec2ui Offsets, const noob::vec2ui Dims, const uint8_t* DataPtr) const noexcept(true)
   {
   if (Handle.index() < textures_2d.size())
   {
   }
   }
   */

void noob::graphics::texture_data(noob::texture_array_2d_handle Handle, uint32_t Mip, uint32_t Index, const noob::vec2ui Offset, const noob::vec2ui Dims, const uint8_t* DataPtr) const noexcept(true)
{
	// void glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data);

	// void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * data);

	check_error_gl();
}


void noob::graphics::texture_data(noob::texture_3d_handle Handle, uint32_t Mip, const std::array<uint32_t, 3> Offset, const std::array<uint32_t, 3> Dims, const uint8_t* DataPtr) const noexcept(true)
{
	// void glCompressedTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data);

	// void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * data);

	check_error_gl();
}

void noob::graphics::texture_base_level(uint32_t Mip) const noexcept(true)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, Mip);
	check_error_gl();
}


void noob::graphics::texture_compare_mode(noob::tex_compare_mode CompareMode) const noexcept(true)
{
	GLint param;
	switch (CompareMode)
	{
		case(noob::tex_compare_mode::COMPARE_REF_TO_TEXTURE):
			{
				param = GL_COMPARE_REF_TO_TEXTURE;
				break;
			}
		case(noob::tex_compare_mode::NONE):
			{
				param = GL_NONE;
				break;
			}
	};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, param);

	check_error_gl();
}


void noob::graphics::texture_compare_func(noob::tex_compare_func CompareFunc) const noexcept(true)
{
	GLenum param;
	switch(CompareFunc)
	{
		case(noob::tex_compare_func::LEQUAL):
			{
				param = GL_LEQUAL;
				break;
			}
		case(noob::tex_compare_func::GEQUAL):
			{
				param = GL_GEQUAL;
				break;
			}
		case(noob::tex_compare_func::LESS):
			{
				param = GL_LESS;
				break;
			}
		case(noob::tex_compare_func::GREATER):
			{
				param = GL_GREATER;
				break;
			}
		case(noob::tex_compare_func::EQUAL):
			{
				param = GL_EQUAL;
				break;
			}
		case(noob::tex_compare_func::NOTEQUAL):
			{
				param = GL_NOTEQUAL;
				break;
			}
		case(noob::tex_compare_func::ALWAYS):
			{
				param = GL_ALWAYS;
				break;
			}
		case(noob::tex_compare_func::NEVER):
			{
				param = GL_NEVER;
				break;
			}
	};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, param);

	check_error_gl();
}

void noob::graphics::texture_min_filter(noob::tex_min_filter MinFilter) const noexcept(true)
{
	GLenum param;
	switch(MinFilter)
	{
		case(noob::tex_min_filter::NEAREST):
			{
				param = GL_NEAREST;
				break;
			}
		case(noob::tex_min_filter::LINEAR):
			{
				param = GL_LINEAR;
				break;
			}
		case(noob::tex_min_filter::NEAREST_MIPMAP_NEAREST):
			{
				param = GL_NEAREST_MIPMAP_NEAREST;
				break;
			}
		case(noob::tex_min_filter::LINEAR_MIPMAP_NEAREST):
			{
				param = GL_LINEAR_MIPMAP_NEAREST;
				break;
			}
		case(noob::tex_min_filter::NEAREST_MIPMAP_LINEAR):
			{
				param = GL_NEAREST_MIPMAP_LINEAR;
				break;
			}
		case(noob::tex_min_filter::LINEAR_MIPMAP_LINEAR):
			{
				param = GL_LINEAR_MIPMAP_LINEAR;
				break;
			}
	};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);

	check_error_gl();
}


void noob::graphics::texture_mag_filter(noob::tex_mag_filter MagFilter) const noexcept(true)
{
	GLenum param;
	switch(MagFilter)
	{
		case(noob::tex_mag_filter::NEAREST):
			{
				param = GL_NEAREST;
				break;
			}
		case(noob::tex_mag_filter::LINEAR):
			{
				param = GL_LINEAR;
				break;
			}
	};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);

	check_error_gl();
}


void noob::graphics::texture_min_lod(int32_t MinLod) const noexcept(true)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, MinLod);
	check_error_gl();
}


void noob::graphics::texture_max_lod(int32_t MaxLod) const noexcept(true)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, MaxLod);
	check_error_gl();
}


void noob::graphics::texture_swizzle(const std::array<noob::tex_swizzle, 2> Swizzles) const noexcept(true)
{
	for (uint32_t i = 0; i < 4; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_SWIZZLE_R;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_SWIZZLE_G;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
		};
	}

	check_error_gl();
}


void noob::graphics::texture_swizzle(const std::array<noob::tex_swizzle, 3> Swizzles) const noexcept(true)
{
	for (uint32_t i = 0; i < 4; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_SWIZZLE_R;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_SWIZZLE_G;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (2):
				{
					pname = GL_TEXTURE_SWIZZLE_B;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
		};
	}

	check_error_gl();
}


void noob::graphics::texture_swizzle(const std::array<noob::tex_swizzle, 4> Swizzles) const noexcept(true)
{
	for (uint32_t i = 0; i < 4; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_SWIZZLE_R;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_SWIZZLE_G;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (2):
				{
					pname = GL_TEXTURE_SWIZZLE_B;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (3):
				{
					pname = GL_TEXTURE_SWIZZLE_A;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
		};
	}

	check_error_gl();
}


void noob::graphics::texture_wrap_mode(const std::array<noob::tex_wrap_mode, 2> WrapModes) const noexcept(true)
{
	for (uint32_t i = 0; i < 2; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_WRAP_S;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_WRAP_T;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
		};
	}

	check_error_gl();
}


void noob::graphics::texture_wrap_mode(const std::array<noob::tex_wrap_mode, 3> WrapModes) const noexcept(true)
{
	for (uint32_t i = 0; i < 3; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_WRAP_S;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_3D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_WRAP_T;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_3D, pname, param);
					break;
				}
			case (2):
				{
					pname = GL_TEXTURE_WRAP_R;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_3D, pname, param);
					break;
				}

		};
	}


	check_error_gl();
}


// File-local function that helps reduce repeated code.
static bool texture_packing_valid(uint32_t Arg) noexcept(true)
{
	// GL_PACK_ALIGNMENT 	integer 	4 	1, 2, 4, or 8
	switch (Arg)
	{
		case (1):
		case (2):
		case (4):
		case (8):
			{
				return true;

			}
	};

	return false;

}


void noob::graphics::texture_pack_alignment(uint32_t Arg) const noexcept(true)
{
	if (texture_packing_valid(Arg))
	{
		glPixelStorei(GL_PACK_ALIGNMENT, Arg);
	}
}


void noob::graphics::texture_unpack_alignment(uint32_t Arg) const noexcept(true)
{
	if (texture_packing_valid(Arg))
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, Arg); 
	}
}


void noob::graphics::generate_mips(noob::texture_2d_handle) const noexcept(true)
{
	glGenerateMipmap(GL_TEXTURE_2D);
}


void noob::graphics::generate_mips(noob::texture_array_2d_handle) const noexcept(true)
{
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}


void noob::graphics::generate_mips(noob::texture_3d_handle) const noexcept(true)
{
	glGenerateMipmap(GL_TEXTURE_3D);
}


noob::graphics::program_handle noob::graphics::get_instanced_shader() const noexcept(true)
{
	return instanced_shader;
}
