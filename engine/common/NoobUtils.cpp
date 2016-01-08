#include "NoobUtils.hpp"

std::string noob::utils::load_file_as_string(const std::string& filename)
{
	std::ifstream input(filename);
	return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

long int noob::utils::fsize(FILE* _file)
{
	long int pos = ftell(_file);
	fseek(_file, 0L, SEEK_END);
	long int size = ftell(_file);
	fseek(_file, pos, SEEK_SET);
	return size;
}
