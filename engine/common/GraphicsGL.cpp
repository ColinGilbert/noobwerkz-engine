#include "GraphicsGL.hpp"



void noob::graphics_gl::init(uint32_t width, uint32_t height) noexcept(true) 
{

}

void noob::graphics_gl::destroy() noexcept(true)
{

}

void noob::graphics_gl::frame(uint32_t width, uint32_t height) const noexcept(true)
{
	glViewport ( 0, 0, width, height );
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram ( userData->programObject );
}

// ---------------- Asset loaders (builds from files and returns handles) -----------------
noob::graphics::shader_handle noob::graphics_gl::load_shader(const std::string& shader_arg) noexcept(true)
{

}

noob::graphics::program_handle noob::graphics_gl::load_program(const std::string& vs, const std::string& fs) noexcept(true)
{

}

noob::graphics::texture noob::graphics_gl::load_texture(const std::string& friendly_name, const std::string& filename, uint32_t flags) noexcept(true)
{

}

// ---------------- Asset creators (make assets available from getters) ----------------
/*
noob::graphics::uniform noob::graphics_gl::add_uniform(const std::string& name, noob::graphics::uniform_type type, uint16_t count) noexcept(true)
{

}
bool noob::graphics_gl::add_shader(const std::string&, const noob::graphics::shader&) noexcept(true)
{

}

bool noob::graphics_gl::add_sampler(const std::string&) noexcept(true)
{

}

// ---------------- Getters -----------------
noob::graphics::texture noob::graphics_gl::get_texture(const std::string&) const noexcept(true)
{

}

noob::graphics::uniform noob::graphics_gl::get_uniform(const std::string&) const noexcept(true)
{

}

noob::graphics::sampler noob::graphics_gl::get_sampler(const std::string&) const noexcept(true)
{

}
*/
void noob::graphics_gl::set_view_transform(uint8_t, const noob::mat4& view, const noob::mat4& proj) noexcept(true)
{

}

void noob::graphics_gl::set_view_rect(uint8_t, uint16_t x, uint16_t y) noexcept(true)
{

}

GLuint noob::graphics_gl::load_shader_gl(GLenum type, const std::string& shader_src_arg) const noexcept(true)
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);

	if (shader == 0)
	{
		return 0;
	}

	const char* shader_src = shader_src_arg.c_str();
	
	// Load the shader source
	glShaderSource(shader, 1, &shader_src, NULL);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint info_len = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);

		if (info_len > 1)
		{
			std::string info_log;
			info_log.reserve(info_len);
			glGetShaderInfoLog(shader, info_len, NULL, &info_log[0]);

			noob::logger::log(noob::importance::ERROR, noob::concat("Error compiling shader: ", info_log));
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}
