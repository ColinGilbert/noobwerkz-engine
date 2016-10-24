#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Graphics.hpp"

#include <algorithm>
#include <GLES3/gl3.h>

#include "ShadersGL.hpp"
#include "NoobUtils.hpp"



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


void noob::graphics::set_program(noob::graphics::program_handle arg) noexcept(true)
{
	glUseProgram(arg.index());
}

noob::model_handle noob::graphics::model_instanced(const noob::basic_mesh& mesh, uint32_t num_instances) noexcept(true)
{
	noob::model result;

	if (num_instances == 0)
	{
		noob::model_handle h;
		return h;
	}

	result.type = noob::model::geom_type::INDEXED_MESH;

	result.num_instances = num_instances;

	const uint32_t num_indices = mesh.indices.size();
	result.num_indices = num_indices;

	const uint32_t num_verts = mesh.vertices.size();
	result.num_vertices = num_verts;

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
	for(uint32_t i = 0; i < num_verts; ++i)
	{
		const uint32_t current_offset = i * 3;
		interleaved[current_offset] = noob::vec4(mesh.vertices[i].v[0], mesh.vertices[i].v[1], mesh.vertices[i].v[2], 1.0);
		interleaved[current_offset + 1] = noob::vec4(mesh.normals[i].v[0], mesh.normals[i].v[1], mesh.normals[i].v[2], 1.0);
		interleaved[current_offset + 2] = mesh.colours[i];
	}

	// Upload interleaved buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(interleaved[0]), &interleaved[0], GL_STATIC_DRAW);
	result.vertices_vbo = vertices_vbo;

	// Upload to indices buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(mesh.indices[0]), &mesh.indices[0], GL_STATIC_DRAW);
	result.indices_vbo = indices_vbo;

	////////////////////////
	// Setup instanced VBOs
	////////////////////////

	// Setup colours VBO:
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_instances * noob::model::instanced_colour_stride, nullptr, GL_DYNAMIC_DRAW);
	result.instanced_colour_vbo = colours_vbo;

	// Setup matrices VBO:
	glBindBuffer(GL_ARRAY_BUFFER, matrices_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_instances * noob::model::instanced_matrices_stride, nullptr, GL_DYNAMIC_DRAW);
	result.instanced_matrices_vbo = matrices_vbo;

	//////////////////////
	// Setup attrib specs
	//////////////////////

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(0));

	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(sizeof(GLfloat)*4));

	// Vertex colours
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(sizeof(GLfloat)*8));

	// Per instance colour
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(0));
	glVertexAttribDivisor(3, 1);

	// Per instance MVP
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(0));
	glVertexAttribDivisor(4, 1);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(sizeof(GLfloat)*4));
	glVertexAttribDivisor(5, 1);

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(sizeof(GLfloat)*8));
	glVertexAttribDivisor(6, 1);

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(sizeof(GLfloat)*12));
	glVertexAttribDivisor(7, 1);

	// Per-instance normal matrix
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(sizeof(GLfloat)*16));
	glVertexAttribDivisor(8, 1);

	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(sizeof(GLfloat)*20));
	glVertexAttribDivisor(9, 1);

	glEnableVertexAttribArray(10);
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(sizeof(GLfloat)*24));
	glVertexAttribDivisor(10, 1);

	glEnableVertexAttribArray(11);
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(sizeof(GLfloat)*28));
	glVertexAttribDivisor(11, 1);

	glBindVertexArray(0);

	noob::model_handle h = models.add(result);

	return h;
}


noob::graphics::texture_handle noob::graphics::reserve_textures_2d(uint32_t width, uint32_t height, uint32_t slots, uint32_t mips, noob::graphics::attrib::unit_type unit_arg, noob::graphics::texture::compression_type compress) noexcept(true)
{
	noob::graphics::texture_handle t;
	return t;
}

noob::graphics::texture_handle noob::graphics::texture_3d(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type unit_arg, noob::graphics::texture::compression_type compress, const std::string&) noexcept(true)
{
	noob::graphics::texture_handle t;
	return t;
}

noob::graphics::texture_handle noob::graphics::texture_cube(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type unit_arg, noob::graphics::texture::compression_type compress, const std::string&) noexcept(true)
{
	noob::graphics::texture_handle t;
	return t;
}

void noob::graphics::set_view_transform(const noob::mat4& view, const noob::mat4& proj) noexcept(true)
{
	view_mat = view;
	proj_mat = proj;
}

void noob::graphics::draw(const noob::model& m, uint32_t num) noexcept(true)
{
	glBindVertexArray(m.vao);	
	glDrawElementsInstanced(GL_TRIANGLES, m.num_indices, GL_UNSIGNED_INT, reinterpret_cast<const void *>(0), std::min(m.num_instances, num));
	glBindVertexArray(0);
}

void noob::graphics::frame(uint32_t width, uint32_t height) noexcept(true)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(0);

}

std::tuple<bool, noob::gpu_write_buffer> noob::graphics::map_buffer(const noob::model& m, noob::model::instanced_data_type t) noexcept(true)
{
	if (m.type != noob::model::geom_type::INDEXED_MESH)
	{
		return std::make_tuple(false, noob::gpu_write_buffer::invalid());
	}

	glBindVertexArray(m.vao);	

	uint32_t stride_in_bytes;

	switch (t)
	{
		case (noob::model::instanced_data_type::COLOUR):
			{
				stride_in_bytes = noob::model::instanced_colour_stride;
				glBindBuffer(GL_ARRAY_BUFFER, m.instanced_colour_vbo);
				break;
			}
		case (noob::model::instanced_data_type::MATRICES):
			{
				stride_in_bytes = noob::model::instanced_matrices_stride;
				glBindBuffer(GL_ARRAY_BUFFER, m.instanced_matrices_vbo);
				break;
			}
	}

	const uint32_t total_size = stride_in_bytes * m.num_instances;

	uint8_t* ptr = reinterpret_cast<uint8_t*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, total_size, GL_MAP_WRITE_BIT));

	if (ptr != nullptr)
	{
		noob::gpu_write_buffer results(ptr, total_size);
		return std::make_tuple(true, results);
	}
	else
	{
		noob::gpu_write_buffer results(nullptr, 0);
		return std::make_tuple(false, noob::gpu_write_buffer::invalid());	
	}
}

void noob::graphics::unmap_buffer() noexcept(true)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);	
}
