#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "FileSystem.hpp"
#include "Logger.hpp"

void noob::filesystem::init(const std::string& prefix)
{
	_prefix = prefix;
}

std::string noob::filesystem::load_file(const std::string& name, std::ios_base::openmode mode)
{
	std::string file_contents = "";

	if (exists(name))
	{
		boost::filesystem::path p;

		p += _prefix;
		p += name;

		boost::system::error_code ec;

		std::ifstream infile(p.generic_string(), mode);
		infile >> file_contents;
		infile.close();
	}

	return file_contents;
}

bool noob::filesystem::write_file(const std::string& name, const std::string& content, std::ios_base::openmode mode)
{
	boost::filesystem::path p;

	p += _prefix;
	p += name;

	std::ofstream outfile(p.generic_string(), mode);
	if (!outfile.good())
	{
		return false;
	}

	outfile << content;
	outfile.close();
	return true;
}

noob::filesystem::file_id noob::filesystem::write_temp(const std::string& content)
{
	file_id token;
	token.inner = temp_counter;
	temp_counter++;

	temp_cache.insert(std::make_pair(token.inner, content));

	return token;
}

size_t noob::filesystem::file_size(const std::string& name)
{
	if (exists(name))
	{
		boost::filesystem::path p;

		p += _prefix;
		p += name;

		boost::system::error_code ec;

		size_t f_size = boost::filesystem::file_size(p, ec);
		if (ec != 0)
		{
			
			fmt::MemoryWriter ww;
			ww << "[FileSystem] Error checking file size of file " << p.generic_string();
			logger::log(ww.str());
			return f_size;
		}
	}

	return 0;
	
}

size_t noob::filesystem::file_size(noob::filesystem::file_id id)
{

	if (exists(id))
	{
		return temp_cache.at(id.inner).size();
	}
	return 0;
}


bool noob::filesystem::exists(const std::string& name)
{
	boost::filesystem::path p;

	p += _prefix;
	p += name;

	boost::system::error_code ec;

	bool file_exists = boost::filesystem::exists(p, ec);
	if (ec != 0)
	{
		fmt::MemoryWriter ww;
		ww << "[FileSystem] Error checking existence of file " << p.generic_string() << ": " << ec.message();
		logger::log(ww.str());
		return false;
	}

	return file_exists;
}


bool noob::filesystem::exists(file_id id)
{
	if (temp_cache.find(id.inner) != temp_cache.end())
	{
		return true;
	}

	return false;
}
