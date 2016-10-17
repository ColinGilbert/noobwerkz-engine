#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Graphics.hpp"

#include "ShadersGL.hpp"
#include "NoobUtils.hpp"

#include <glad/glad.h>


#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define VERTEX_STRIDE         ( sizeof(GLfloat) *     \
		( VERTEX_POS_SIZE +    \
		  VERTEX_COLOR_SIZE ) )



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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a
}

void noob::graphics::destroy() noexcept(true)
{

}

noob::graphics::model_handle noob::graphics::model(const noob::basic_mesh& mesh) noexcept(true)
{
	noob::graphics::model_handle results;

	// GLuint programObject = programObject;
	// VertexBufferObject Ids
	std::array<GLuint, 2> vbo_ids;

	// VertexArrayObject Id
	GLuint vao_id;

	// Generate VBO Ids and load the VBOs with data
	glGenBuffers(2, &vbo_ids[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[0]);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(mesh.vertices[0]), &mesh.vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(mesh.indices[0]), &mesh.indices[0], GL_STATIC_DRAW);

	// Generate VAO Id
	glGenVertexArrays(1, &vao_id);

	// Bind the VAO and then setup the vertex
	// attributes
	glBindVertexArray(vao_id);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[1]);

	glEnableVertexAttribArray(VERTEX_POS_INDX);
	glEnableVertexAttribArray(VERTEX_COLOR_INDX);

	glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, reinterpret_cast<const void *>(0));

	glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, reinterpret_cast<const void *>((VERTEX_POS_SIZE * sizeof(GLfloat))));

	// Reset to the default VAO
	glBindVertexArray(0);

	return noob::graphics::model_handle::make(vao_id);
}

noob::graphics::texture_2d_handle noob::graphics::texture_2d(const std::string& file, noob::graphics::tex_compression compress, uint32_t width, uint32_t height) noexcept(true)
{

}

noob::graphics::texture_3d_handle noob::graphics::texture_3d(const std::string& file, noob::graphics::tex_compression compress, uint32_t width, uint32_t height) noexcept(true)
{

}

noob::graphics::texture_cube_handle noob::graphics::texture_cube(const std::string& file, noob::graphics::tex_compression compress, uint32_t width, uint32_t height) noexcept(true)
{

}

void noob::graphics::frame(uint32_t width, uint32_t height) noexcept(true)
{
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glUseProgram (programObject);

	// Bind the VAO
	// glBindVertexArray ( userData->vao_id );

	// Draw with the VAO settings
	// glDrawElements ( GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, ( const void * ) 0 );

	// Return to the default VAO
	glBindVertexArray(0);

}
