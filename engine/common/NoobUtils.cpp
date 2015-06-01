#include "NoobUtils.hpp"

std::map<std::string, std::string> noob::utils::data;

std::string noob::utils::load_file_as_string(const std::string& filename)
{
	std::ifstream input(filename);
	return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

void noob::utils::load_file_to_memory(const std::string& filename)
{
	data.insert(std::make_pair(filename, noob::utils::load_file_as_string(filename)));
}

long int noob::utils::fsize(FILE* _file)
{
	long int pos = ftell(_file);
	fseek(_file, 0L, SEEK_END);
	long int size = ftell(_file);
	fseek(_file, pos, SEEK_SET);
	return size;
}


/*
   GLuint noob::utils::load_shader(GLenum type, const std::string &shader_str, bool debug)
   {
   GLuint shader;
   GLint compiled;
   const char *shader_strRawArray = shader_str.c_str();
   shader = glCreateShader(type);

   if (shader == 0)
   {
   return 0;
   }

   glShaderSource(shader, 1, &shader_strRawArray, NULL);
   glCompileShader(shader);
   glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
   if (!compiled)
   {
   if (debug == true)
   {
   GLint infoLen = 0;
   glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
   if (infoLen > 1)
   { 
   char *info = (char*)malloc(sizeof(char) * infoLen);
   glGetShaderInfoLog(shader, infoLen, NULL, info);
   std::stringstream ss;
   ss << "Error compiling shader: " << info;
   logger::log(ss.str());
   free(info);
   }
   }
   glDeleteShader(shader);
   return 0;
   }
   return shader;
   }

   GLuint noob::utils::load_program(const std::string &vert_shader_str, const std::string &frag_shader_str, bool debug)
   {
   GLuint vertex_shader;
   GLuint fragment_shader;
   GLuint program_object;
   GLint linked;

   vertex_shader = load_shader(GL_VERTEX_SHADER, vert_shader_str, debug);
   if (vertex_shader == 0)
   {
   return 0;
   }

   fragment_shader = load_shader(GL_FRAGMENT_SHADER, frag_shader_str, debug);
   if (fragment_shader == 0)
   {
   glDeleteShader(vertex_shader);
   return 0;
   }

   program_object = glCreateProgram();
   if (program_object == 0)
   {
   return 0;
   }

   glAttachShader(program_object, vertex_shader);
   glAttachShader(program_object, fragment_shader);
   glLinkProgram(program_object);

   if (debug == true)
   {
   glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
if (!linked)
{
	GLint infoLen = 0;
	glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &infoLen);
	if (infoLen > 1)
	{
		char *info = (char*)malloc(sizeof (char) * infoLen);
		glGetProgramInfoLog(program_object, infoLen, NULL, info);
		std::stringstream ss;
		ss << "Error linking program: " << info;
		logger::log(ss.str());
		free(info);
	}
	glDeleteProgram(program_object);
	return 0;
}
}

glDeleteShader(vertex_shader);
glDeleteShader(fragment_shader);
return program_object;
}

void noob::utils::log_gl_error(const std::string& message)
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
		ss << "OpenGL error: " << error << ", message: " << message;
		logger::log(ss.str());
	}
}
*/
