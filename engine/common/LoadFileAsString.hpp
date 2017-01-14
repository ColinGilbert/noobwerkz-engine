#pragma once

namespace noob
{
	static std::string load_file_as_string(const std::string& Filename)
	{
		noob::logger::log(noob::importance::INFO, noob::concat("[Utils] Attempting to load file: ", Filename));
		std::ifstream input(Filename);
		return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
	}
}
