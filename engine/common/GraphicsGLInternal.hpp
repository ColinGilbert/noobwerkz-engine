#pragma once

#include <GLES3/gl3.h>

#include "NoobUtils.hpp"


GLenum check_error_gl(const char *file, int line)
{
	GLenum error_code;
	while ((error_code = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (error_code)
		{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
							       // TODO: Find out if these are still part of the modern GL
							       // case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
							       // case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		noob::logger::log(noob::importance::ERROR, noob::concat("OpenGL: ", error, file, " (", noob::to_string(line), ")"));
	}
	return error_code;
}

#define check_error_gl() check_error_gl(__FILE__, __LINE__) 

/*
   GL_COMPRESSED_R11_EAC,
   GL_COMPRESSED_SIGNED_R11_EAC,
   GL_COMPRESSED_RG11_EAC,
   GL_COMPRESSED_SIGNED_RG11_EAC,
   GL_COMPRESSED_RGB8_ETC2,
   GL_COMPRESSED_SRGB8_ETC2,
   GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
   GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
   GL_COMPRESSED_RGBA8_ETC2_EAC, or
   GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
   */


GLenum get_gl_storage_format(const noob::pixel_format Pixels)
{
	//	R8, RG8, RGB8, SRGB8, RGBA8, SRGBA8, RGB8_COMPRESSED, SRGB8_COMPRESSED, RGB8_A1_COMPRESSED, SRGB8_A1_COMPRESSED, RGBA8_COMPRESSED, SRGBA8_COMPRESSED
	GLenum results;

	switch(Pixels)
	{
		case(noob::pixel_format::R8):
			{
				results = GL_R8;
			}
		case(noob::pixel_format::RG8):
			{
				results = GL_RG8;
			}
		case(noob::pixel_format::RGB8):
			{
				results = GL_RGB8;				
			}
		case(noob::pixel_format::SRGB8):
			{
				results = GL_SRGB8;		
			}
		case(noob::pixel_format::RGBA8):
			{
				results = GL_RGBA8;		
			}
		case(noob::pixel_format::SRGBA8):
			{
				results = GL_SRGB8_ALPHA8;		
			}
		case(noob::pixel_format::RGB8_COMPRESSED):
			{
				results = GL_COMPRESSED_RGB8_ETC2;		
			}
		case(noob::pixel_format::SRGB8_COMPRESSED):
			{
				results = GL_COMPRESSED_SRGB8_ETC2;		
			}
		case(noob::pixel_format::RGB8_A1_COMPRESSED):
			{
				results = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;		
			}
		case(noob::pixel_format::SRGB8_A1_COMPRESSED):
			{
				results = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;		
			}
		case(noob::pixel_format::RGBA8_COMPRESSED):
			{
				results = GL_COMPRESSED_RGBA8_ETC2_EAC;		
			}
		case(noob::pixel_format::SRGBA8_COMPRESSED):
			{		
				results = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
			}
	}

	return results;
}

// This one gives you the "format" and "type" arguments for glTexSubImage2D and glTexSubImage3D
// Visit: https://www.khronos.org/opengles/sdk/docs/man3/html/glTexSubImage2D.xhtml and https://www.khronos.org/opengles/sdk/docs/man3/html/glTexSubImage3D.xhtml
// As you can see, OpenGL doesn't just allow you to use the same storage format as the argument to the functions, as that would be far too straightforward. Instead, we must write otherwise pointless functions such as these to make our dreams into reality.
std::tuple<GLenum, GLenum> deduce_pixel_format_and_type(noob::pixel_format Pixels)
{
	GLenum results_a, results_b;
	// GL_RED, GL_RED_INTEGER, GL_RG, GL_RG_INTEGER, GL_RGB, GL_RGB_INTEGER, GL_RGBA, GL_RGBA_INTEGER, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL
	// GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT
	results_b = GL_UNSIGNED_BYTE;
	switch(Pixels)
	{
		case(noob::pixel_format::R8):
			{
				results_a =  GL_RED;//, GL_RED_INTEGER, GL_RG, GL_RG_INTEGER, GL_RGB, GL_RGB_INTEGER, GL_RGBA, GL_RGBA_INTEGER, GL_DEPTH_COMPONENT;
				// results_b = GL_UNSIGNED_BYTE;
				break;
			}
		case(noob::pixel_format::RG8):
			{
				results_a = GL_RG;
				break;				
			}
		case(noob::pixel_format::RGB8):
			{
				results_a = GL_RGB;
				break;				
			}
		case(noob::pixel_format::SRGB8):
			{
				results_a = GL_RGB;		
				break;			
			}
		case(noob::pixel_format::RGBA8):
			{
				results_a = GL_RGBA;		
				break;
			}
		case(noob::pixel_format::SRGBA8):
			{
				results_a = GL_RGBA;		
				break;
			}
		case(noob::pixel_format::RGB8_COMPRESSED):
			{
				results_a = GL_COMPRESSED_RGB8_ETC2;		
				break;
			}
		case(noob::pixel_format::SRGB8_COMPRESSED):
			{
				results_a = GL_COMPRESSED_SRGB8_ETC2;		
				break;
			}
		case(noob::pixel_format::RGB8_A1_COMPRESSED):
			{
				results_a = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;		
				break;
			}
		case(noob::pixel_format::SRGB8_A1_COMPRESSED):
			{
				results_a = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;		
				break;
			}
		case(noob::pixel_format::RGBA8_COMPRESSED):
			{
				results_a = GL_COMPRESSED_RGBA8_ETC2_EAC;		
				break;
			}
		case(noob::pixel_format::SRGBA8_COMPRESSED):
			{		
				results_a = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
				break;
			}
	}

	return std::make_tuple(results_a, results_b);

}


bool is_compressed(noob::pixel_format Pixels)
{
	bool results;
	// GL_RED, GL_RED_INTEGER, GL_RG, GL_RG_INTEGER, GL_RGB, GL_RGB_INTEGER, GL_RGBA, GL_RGBA_INTEGER, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL
	// GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT
	switch(Pixels)
	{
		case(noob::pixel_format::RGB8_COMPRESSED):
			{
				results = true;
				break;
			}
		case(noob::pixel_format::SRGB8_COMPRESSED):
			{
				results = true;
				break;
			}
		case(noob::pixel_format::RGB8_A1_COMPRESSED):
			{
				results = true;
				break;
			}
		case(noob::pixel_format::SRGB8_A1_COMPRESSED):
			{
				results = true;
				break;
			}
		case(noob::pixel_format::RGBA8_COMPRESSED):
			{
				results = true;
				break;
			}
		case(noob::pixel_format::SRGBA8_COMPRESSED):
			{		
				results = true;
				break;
			}
		default:
			{
				results = false;
				break;
			}
	}

	return results;
}


// rgb8a1 == same size
uint32_t get_compressed_size_rgb8(uint32_t Width, uint32_t Height)
{
	return std::ceil(Width / 4) * std::ceil(Height / 4) * 8;
}

uint32_t get_compressed_size_rgba8(uint32_t Width, uint32_t Height)
{
	return std::ceil(Width / 4) * std::ceil(Height / 4) * 16;
}

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

	// Just to make sure no dirty state gets to ruin our shader compile. :)
	glBindVertexArray(0);

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
			noob::logger::log(noob::importance::ERROR, noob::concat("[Graphics] Error linking program:", info_log));

		}

		glDeleteProgram(program_object);
		return 0;
	}

	// Free up no longer needed shader resources
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program_object;
}

// Per-channel:
// GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT
// Per-pixel:
// GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_INT_2_10_10_10_REV, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_UNSIGNED_INT_5_9_9_9_REV, GL_UNSIGNED_INT_24_8, and GL_FLOAT_32_UNSIGNED_INT_24_8_REV.
// Per-pixel, compressed:
// GL_COMPRESSED_RGB8_ETC2, GL_COMPRESSED_SRGB8_ETC2, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC



GLuint prep_texture()
{
	// Prevent leftover from previous calls from harming this.
	glBindVertexArray(0);

	GLuint texture_id;

	glGenTextures(1, &texture_id);

	return texture_id;
}
