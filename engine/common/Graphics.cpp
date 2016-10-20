#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Graphics.hpp"

#include "ShadersGL.hpp"
#include "NoobUtils.hpp"

#include <glad/glad.h>



noob::graphics* noob::graphics::ptr_to_instance;

GLuint load_shader_gl(GLenum type, const std::string& shader_arg)
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);

	if (shader == 0)
	{
		return 0;
	}

	const char* shader_src = shader_arg.c_str();
	// Load the shader source
	glShaderSource(shader, 1, &shader_src, NULL);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint info_len = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);

		if (info_len > 1)
		{
			std::string info_log;
			info_log.resize(info_len);

			glGetShaderInfoLog(shader, info_len, NULL, &info_log[0]);

			noob::logger::log(noob::importance::ERROR, noob::concat("[Graphics] Error compiling shader: ", info_log));
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint load_program_gl(const std::string& vert_shader_arg, const std::string frag_shader_arg)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program_object;
	GLint linked;

	const char* vert_shader_src = vert_shader_arg.c_str();
	// Load the vertex/fragment shaders
	vertex_shader = load_shader_gl(GL_VERTEX_SHADER, vert_shader_arg);

	if (vertex_shader == 0)
	{
		return 0;
	}

	const char* frag_shader_src  = frag_shader_arg.c_str();
	fragment_shader = load_shader_gl(GL_FRAGMENT_SHADER, frag_shader_arg);

	if (fragment_shader == 0)
	{
		glDeleteShader(vertex_shader);
		return 0;
	}

	// Create the program object
	program_object = glCreateProgram();

	if (program_object == 0)
	{
		return 0;
	}

	glAttachShader(program_object, vertex_shader);
	glAttachShader(program_object, fragment_shader);

	// Link the program
	glLinkProgram(program_object);

	// Check the link status
	glGetProgramiv(program_object, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint info_len = 0;

		glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &info_len);

		if (info_len > 1)
		{
			std::string info_log;
			info_log.resize(info_len);

			glGetProgramInfoLog(program_object, info_len, NULL, &info_log[0]);
			noob::logger::log(noob::importance::INFO, noob::concat("[Graphics] Error linking program:", info_log));

		}

		glDeleteProgram(program_object);
		return 0;
	}

	// Free up no longer needed shader resources
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program_object;
}

void noob::graphics::init(uint32_t width, uint32_t height) noexcept(true)
{
	glClearColor(0.2f, 0.0f, 0.2f, 0.0f);
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a
}

void noob::graphics::destroy() noexcept(true)
{

}

noob::graphics::model_handle noob::graphics::model(noob::graphics::model::geom_type geom, const noob::basic_mesh& mesh) noexcept(true)
{
	noob::graphics::model_handle results;
	GLuint vao_id = 0;

	switch (geom)
	{
		case(noob::graphics::model::geom_type::INDEXED_MESH):
			{
				break;
			}
		case(noob::graphics::model::geom_type::DYNAMIC_TERRAIN):
			{
				break;
			}
		case(noob::graphics::model::geom_type::BILLBOARD):
			{
				break;
			}
		case(noob::graphics::model::geom_type::POINT_SPRITE):
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

	return noob::graphics::model_handle::make(vao_id);
}

noob::graphics::instanced_model_info noob::graphics::model_instanced(const noob::basic_mesh& mesh, const std::vector<noob::vec4>& colours_buffer) noexcept(true)
{
	GLuint vao_id = 0;

	glGenVertexArrays(1, &vao_id);

	glBindVertexArray(vao_id);

	////////////////////////////////
	// Create & bind attrib buffers
	////////////////////////////////
	std::array<GLuint, 4> vbo_ids;
	glGenBuffers(4, &vbo_ids[0]);

	const GLuint pos_normals_colours_vbo = vbo_ids[0];
	const GLuint colour_multiplier_vbo = vbo_ids[1];
	const GLuint mvp_vbo = vbo_ids[2];
	const GLuint indices_vbo = vbo_ids[3];



	const uint32_t num_verts = mesh.vertices.size();
	const uint32_t num_instances = std::max(static_cast<uint32_t>(colours_buffer.size()), static_cast<uint32_t>(1));




	////////////////////////////
	// Setup non-instanced VBOs
	////////////////////////////
	
	std::vector<noob::vec4> pos_normals_colours;
	pos_normals_colours.resize(num_verts * 3);

	// Interleave our vertex positions, normals, and colours
	for(uint32_t i = 0; i < num_verts; ++i)
	{
		const uint32_t current_offset = i * 3;
		pos_normals_colours[current_offset] = noob::vec4(mesh.vertices[i].v[0], mesh.vertices[i].v[1], mesh.vertices[i].v[2], 1.0);
		pos_normals_colours[current_offset + 1] = noob::vec4(mesh.normals[i].v[0], mesh.normals[i].v[1], mesh.normals[i].v[2], 1.0);
		pos_normals_colours[current_offset + 2] = mesh.colours[i];
	}

	// Upload to pos-normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, pos_normals_colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, pos_normals_colours.size() * sizeof(pos_normals_colours[0]), &pos_normals_colours[0], GL_STATIC_DRAW);

	// Upload to indices buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(mesh.indices[0]), &mesh.indices[0], GL_STATIC_DRAW);

	////////////////////////
	// Setup instanced VBOs
	////////////////////////

	// Upload to instance colour buffer
	glBindBuffer(GL_ARRAY_BUFFER, colour_multiplier_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_instances * sizeof(colours_buffer[0]), &colours_buffer[0], GL_DYNAMIC_DRAW);

	// Upload to mvp buffer
	glBindBuffer(GL_ARRAY_BUFFER, mvp_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_instances * sizeof(noob::mat4), nullptr, GL_DYNAMIC_DRAW);

	//////////////////////
	// Setup attrib specs
	//////////////////////

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 12, reinterpret_cast<const void *>(0));

	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 12, reinterpret_cast<const void *>(4));

	// Vertex colours
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 12, reinterpret_cast<const void *>(8));

	// Per instance colour
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void *>(0));

	// Per instance MVP
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(0, 16, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void *>(0));

	glBindVertexArray(0);

	noob::graphics::instanced_model_info results;

	uint32_t old_size = colour_storage.size();
	results.colour_offset = old_size;
	colour_storage.resize(old_size + num_instances);

	old_size = mvp_storage.size();
	results.mvp_offset = old_size;
	mvp_storage.resize(old_size + num_instances);
	
	results.m_model.handle = noob::graphics::model_handle::make(vao_id);

	return results;
}


noob::graphics::texture noob::graphics::reserve_textures_2d(uint32_t width, uint32_t height, uint32_t slots, uint32_t mips, noob::graphics::attrib::unit_type unit_arg, noob::graphics::texture::compression_type compress) noexcept(true)
{
	noob::graphics::texture t;
	return t;
}

noob::graphics::texture noob::graphics::texture_3d(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type unit_arg, noob::graphics::texture::compression_type compress, const std::string&) noexcept(true)
{
	noob::graphics::texture t;
	return t;
}

noob::graphics::texture noob::graphics::texture_cube(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type unit_arg, noob::graphics::texture::compression_type compress, const std::string&) noexcept(true)
{
	noob::graphics::texture t;
	return t;
}

void noob::graphics::frame(uint32_t width, uint32_t height) noexcept(true)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glUseProgram(programObject);
	// glBindVertexArray(vao_id );
	// glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, reinterpret_cast<const void *>(0));

	glBindVertexArray(0);

}
