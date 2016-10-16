#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Graphics.hpp"

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


void noob::graphics::init(uint32_t width, uint32_t height) noexcept(true)
{
}

void noob::graphics::destroy() noexcept(true)
{
}

noob::graphics::model_handle noob::graphics::make_model(const noob::basic_mesh&) noexcept(true)
{
	noob::graphics::model_handle results;
	return results;
}

void noob::graphics::frame(uint32_t width, uint32_t height) noexcept(true)
{
}


