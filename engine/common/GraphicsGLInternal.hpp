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

// TODO: Use sets to do better than this.
noob::return_type<GLenum> get_internal_format(const noob::texture_info TexInfo)
{
	if (!TexInfo.compressed)
	{
		switch (TexInfo.channels)
		{
			case (noob::texture_channels::R):
				{
					switch (TexInfo.channel_scalars[0])
					{
						case(noob::scalar_type::UINT):
							{
								switch(TexInfo.channel_bits[0])
								{
									case(8):
										{
											return noob::return_type<GLenum>(true, GL_R8);
										}
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_R16UI);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_R32UI);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}
						case(noob::scalar_type::INT):
							{
								if (TexInfo.sign_normalized)
								{
									return noob::return_type<GLenum>(true, GL_R8_SNORM);
								}

								switch(TexInfo.channel_bits[0])
								{
									case(8):
										{
											return noob::return_type<GLenum>(true, GL_R8I);
										}
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_R16I);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_R32I);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}

						case(noob::scalar_type::FLOAT):
							{
								switch(TexInfo.channel_bits[0])
								{
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_R16F);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_R32F);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}
						default:
							{
								return noob::return_type<GLenum>(false, 0);
							}
					};
					break; // Likely unnecessary
				} // End of R
			case (noob::texture_channels::RG):
				{
					if (TexInfo.sign_normalized)
					{
						return noob::return_type<GLenum>(true, GL_RG8_SNORM);

					}

					switch (TexInfo.channel_scalars[0])
					{
						case(noob::scalar_type::UINT):
							{
								switch(TexInfo.channel_bits[0])
								{
									case(8):
										{
											return noob::return_type<GLenum>(true, GL_RG8);
										}
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_RG16UI);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_RG32UI);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}
						case(noob::scalar_type::INT):
							{
								if (TexInfo.sign_normalized)
								{
									return noob::return_type<GLenum>(true, GL_R8_SNORM);
								}

								switch(TexInfo.channel_bits[0])
								{
									case(8):
										{
											return noob::return_type<GLenum>(true, GL_RG8I);
										}
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_RG16I);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_RG32I);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}

						case(noob::scalar_type::FLOAT):
							{
								switch(TexInfo.channel_bits[0])
								{
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_RG16F);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_RG32F);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}
						default:
							{
								return noob::return_type<GLenum>(false, 0);
							}
					};
					break;
				} // End of RG
			case (noob::texture_channels::RGB):
				{
					if (TexInfo.sign_normalized)
					{
						return noob::return_type<GLenum>(true, GL_RGB8_SNORM);

					}
					if (TexInfo.srgb)
					{	
						return noob::return_type<GLenum>(true, GL_SRGB8);
					}

					switch (TexInfo.channel_scalars[0])
					{
						case(noob::scalar_type::UINT):
							{
								switch(TexInfo.channel_bits[0])
								{
									case(5):
										{
											return noob::return_type<GLenum>(true, GL_RGB565);
										}
									case(8):
										{
											return noob::return_type<GLenum>(true, GL_RGB8);
										}
									case(9):
										{
											return noob::return_type<GLenum>(true, GL_RGB9_E5);
										}
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_RGB16UI);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_RGB32UI);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}
						case(noob::scalar_type::INT):
							{
								if (TexInfo.sign_normalized)
								{
									return noob::return_type<GLenum>(true, GL_R8_SNORM);
								}

								switch(TexInfo.channel_bits[0])
								{
									case(8):
										{
											return noob::return_type<GLenum>(true, GL_RGB8I);
										}
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_RGB16I);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_RGB32I);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}
						case(noob::scalar_type::FLOAT):
							{
								switch(TexInfo.channel_bits[0])
								{
									case(11):
										{
											return noob::return_type<GLenum>(true, GL_R11F_G11F_B10F);
										}
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_RGB16F);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_RGB32F);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}
						default:
							{
								return noob::return_type<GLenum>(false, 0);
							}
					};
					break; // Likely unnecessary
				} // End of RGB
			case (noob::texture_channels::RGBA):
				{
					if (TexInfo.sign_normalized)
					{
						return noob::return_type<GLenum>(true, GL_RGBA8_SNORM);
					}
					if (TexInfo.srgb)
					{
						return noob::return_type<GLenum>(true, GL_SRGB8_ALPHA8);
					}

					switch (TexInfo.channel_scalars[0])
					{
						case(noob::scalar_type::UINT):
							{
								switch(TexInfo.channel_bits[0])
								{
									case(4):
										{
											return noob::return_type<GLenum>(true, GL_RGBA4);
										}
									case(5):
										{
											return noob::return_type<GLenum>(true, GL_RGB5_A1);
										}
									case(8):
										{
											return noob::return_type<GLenum>(true, GL_RGBA8);
										}
									case(10):
										{
											return noob::return_type<GLenum>(true, GL_RGB10_A2);
										}
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_RGBA16UI);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_RGBA32UI);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}
						case(noob::scalar_type::INT):
							{
								if (TexInfo.sign_normalized)
								{
									return noob::return_type<GLenum>(true, GL_R8_SNORM);
								}

								switch(TexInfo.channel_bits[0])
								{
									case(8):
										{
											return noob::return_type<GLenum>(true, GL_RGBA8I);
										}
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_RGBA16I);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_RGBA32I);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}

						case(noob::scalar_type::FLOAT):
							{
								switch(TexInfo.channel_bits[0])
								{
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_RGBA16F);
										}
									case(32):
										{
											return noob::return_type<GLenum>(true, GL_RGBA32F);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}
						default:
							{
								return noob::return_type<GLenum>(false, 0);
							}
					};
					break; // Likely unnecessary
				} // End of RGBA
			case (noob::texture_channels::DEPTH):
				{
					switch (TexInfo.channel_scalars[0])
					{
						case(noob::scalar_type::UINT):
							{
								switch(TexInfo.channel_bits[0])
								{
									case(16):
										{
											return noob::return_type<GLenum>(true, GL_DEPTH_COMPONENT16);
										}
									case(24):
										{
											return noob::return_type<GLenum>(true, GL_DEPTH_COMPONENT24);
										}
									default:
										{
											return noob::return_type<GLenum>(false, 0);
										}
								};
								break; // Likely unnecessary
							}

						case(noob::scalar_type::FLOAT):
							{
								return noob::return_type<GLenum>(true, GL_DEPTH_COMPONENT32F);
							}
						default:
							{
								return noob::return_type<GLenum>(false, 0);
							}
					};
					break;
				} // End of DEPTH
			case (noob::texture_channels::DEPTH_STENCIL):
				{
					switch (TexInfo.channel_scalars[0])
					{
						case(noob::scalar_type::UINT):
							{
								return noob::return_type<GLenum>(true, GL_DEPTH24_STENCIL8);
							}
						case(noob::scalar_type::FLOAT):
							{
								return noob::return_type<GLenum>(true, GL_DEPTH32F_STENCIL8);
							}
						default:
							{
								return noob::return_type<GLenum>(false, 0);
							}
					};
					break;
				} // End of DEPTH_STENCIL
			default:
				{
					return noob::return_type<GLenum>(false, 0);
				}
		};
	}
	else // We are using compression
	{
		switch (TexInfo.channels)
		{
			case (noob::texture_channels::R):
				{
					switch (TexInfo.channel_scalars[0])
					{
						case(noob::scalar_type::UINT):
							{
								return noob::return_type<GLenum>(true, GL_COMPRESSED_R11_EAC);
							}
						case(noob::scalar_type::INT):
							{
								return noob::return_type<GLenum>(true, GL_COMPRESSED_SIGNED_R11_EAC);
							}
						default:
							{
								return noob::return_type<GLenum>(false, 0);
							}
					};
					break;
				} // End of R
			case (noob::texture_channels::RG):
				{
					switch (TexInfo.channel_scalars[0])
					{
						case(noob::scalar_type::UINT):
							{
								return noob::return_type<GLenum>(true, GL_COMPRESSED_RG11_EAC);

							}
						case(noob::scalar_type::INT):
							{
								return noob::return_type<GLenum>(true, GL_COMPRESSED_SIGNED_RG11_EAC);
							}
						default:
							{
								return noob::return_type<GLenum>(false, 0);
							}
					};
					break;
				} // End of RG
			case (noob::texture_channels::RGB):
				{
					if (TexInfo.srgb)
					{	
						return noob::return_type<GLenum>(true, GL_COMPRESSED_SRGB8_ETC2);
					}
					else
					{
						return noob::return_type<GLenum>(true, GL_COMPRESSED_RGB8_ETC2);
					}
					break;
				} // End of RGB
			case (noob::texture_channels::RGBA):
				{
					if (TexInfo.channel_bits[3] == 1)
					{
						if (TexInfo.srgb)
						{
							return noob::return_type<GLenum>(true, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2);
						}
						else
						{
							return noob::return_type<GLenum>(true, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2);
						}
					}
					else
					{
						if (TexInfo.srgb)
						{
							return noob::return_type<GLenum>(true, GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC);
						}
						else
						{
							return noob::return_type<GLenum>(true, GL_COMPRESSED_RGBA8_ETC2_EAC);
						}
					}
					break;			
				} // End of RGBA
			default:
				{
					return noob::return_type<GLenum>(false, 0);
				}
		};
	}

	// Just in case...
	return noob::return_type<GLenum>(false, 0);
}

GLuint prep_texture()
{
	// Prevent leftover from previous calls from harming this.
	glBindVertexArray(0);

	GLuint texture_id;

	glGenTextures(1, &texture_id);

	return texture_id;
}
