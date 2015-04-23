#include "CommonIncludes.hpp"
/*
void log_gl_error(const std::string& message)
{
	GLenum err(glGetError());
	while (err != GL_NO_ERROR)
	{
		std::string error;
		switch(err)
		{
			case GL_INVALID_OPERATION:
				error = "INVALID OPERATION";
				break;
			case GL_INVALID_ENUM:
				error = "INVALID ENUM";
				break;
			case GL_INVALID_VALUE:
				error = "INVALID VALUE";
				break;
			case GL_OUT_OF_MEMORY:
				error = "OUT OF MEMORY";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				error = "INVALID FRAMEBUFFER OPERATION";
				break;
		}

		std::stringstream ss;
		ss << message << " -> OpenGL error: " << error;
		logger::log(ss.str());
	}
}*/
