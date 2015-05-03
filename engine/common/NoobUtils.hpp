#pragma once

#include <fstream>
#include <tuple>
#include <bgfx.h>

#include "Logger.hpp"

namespace noob
{
	class utils
	{
		public:
			static std::string load_file_as_string(const std::string& filename);
			// return type is specialized to have pointer to start of memory location, and length of file 
			static std::tuple<const uint8_t*,size_t> load_to_memory(const std::string& filename);

			static const bgfx::Memory* load_to_memory_bgfx(const std::string& filename);
			// These functions are still poentially useful, but unused.
			// static GLuint load_shader(GLenum type, const std::string& shader_str, bool debug);
			// static GLuint load_program(const std::string& vert_shader_str, const std::string& frag_shader_str, bool debug);
			// static void log_gl_error(const std::string& message);

	};
}
