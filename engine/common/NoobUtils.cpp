#include "NoobUtils.hpp"
#include <algorithm>

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
noob::indexed_triangles noob::utils::index_triangles(const std::vector<std::vector<std::array<float,3>>>& triangles)
{
	// First pass removes uniques and places every vert into a slot
	noob::indexed_triangles indexed_tris;
	// For each triangle
	for (std::vector<std::array<float,3>> tris : triangles)
	{
		// For each vertex
		for (std::array<float,3> vert: tris)
		{
			auto results = std::find(indexed_tris.vertices.begin(), indexed_tris.vertices.end(), vert);
			if (results == indexed_tris.vertices.end())
			{
				indexed_tris.vertices.push_back(vert);
			}
		}
	}
	
	for (std::vector<std::array<float, 3>> tris : triangles)
	{
		for (std::array<float,3> vert : tris)
		{
			auto results = std::find(indexed_tris.vertices.begin(), indexed_tris.vertices.end(), vert);
			if (results == indexed_tris.vertices.end())
			{
				logger::log("index_triangles() - second sweep - vert not found in list - this message should not be seen.");
			}
			else 
			{
				indexed_tris.indices.push_back(std::distance(indexed_tris.vertices.begin(), results));
			}
		}

	}
}
*/
