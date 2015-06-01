#pragma once
#include <dirent.h>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <stdio.h>

#include "Logger.hpp"

namespace noob
{
	class utils
	{
		public:
			static std::string load_file_as_string(const std::string& filename);
			// return type is specialized to have pointer to start of memory location, and length of file 
			static void load_file_to_memory(const std::string& filename);
			static const std::string& get_data(const std::string&);
			static void set_data(const std::string&, const std::string&);
			static long int fsize(FILE* _file);
			std::vector<std::string> read_dir(const std::string&);

			static std::map<std::string, std::string> data;

			// These functions are still poentially useful, but unused.
			// static GLuint load_shader(GLenum type, const std::string& shader_str, bool debug);
			// static GLuint load_program(const std::string& vert_shader_str, const std::string& frag_shader_str, bool debug);
			// static void log_gl_error(const std::string& message);
	};
}
