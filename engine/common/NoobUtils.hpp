#pragma once
#include <dirent.h>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <stdio.h>

#include "Logger.hpp"
//#include "MathFuncs.hpp"

//#include <Eigen/Dense>
// With any luck, this class should become obsolete.

namespace noob
{
	/*	
		struct indexed_triangles
		{
		std::vector<std::array<float,3>> vertices;
		std::vector<uint32_t> indices;
		};
		*/
	struct utils
	{
		//	static noob::indexed_triangles index_triangles(const std::vector<std::vector<std::array<float,3>>>& triangles);
		static std::string load_file_as_string(const std::string& filename);
		static long int fsize(FILE* _file);
		/*
		// return type is specialized to have pointer to start of memory location, and length of file 
		static void load_file_to_memory(const std::string& filename);
		static const std::string& get_data(const std::string&);
		static void set_data(const std::string&, const std::string&);

		std::vector<std::string> read_dir(const std::string&);
		static std::map<std::string, std::string> data;
		*/	
	};

}
