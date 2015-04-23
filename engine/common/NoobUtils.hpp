#include <fstream>

// #include "glHeaders.h"
#include "Logger.hpp"

namespace noob
{
	class utils
	{
		public:
			// static GLuint load_shader(GLenum type, const std::string& shader_str, bool debug);
			// static GLuint load_program(const std::string& vert_shader_str, const std::string& frag_shader_str, bool debug);
			// static void log_gl_error(const std::string& message);
			static std::string load_file_as_string(const std::string& filename);
	};
}
