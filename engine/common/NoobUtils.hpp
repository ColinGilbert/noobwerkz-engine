#pragma once

#include <noob/logger/logger.hpp>
#include <noob/strings/strings.hpp>

#include <iostream>
#include <fstream>

// With any luck, this class should become obsolete.
namespace noob
{
	struct utils
	{
		static std::string load_file_as_string(const std::string& filename);
	};
	
	struct log_funct
	{
		static void log(noob::importance imp, const std::string& s) noexcept(true)
		{
			std::cout << noob::concat(noob::to_string(imp), ": ", s, "\n");
		}
	};

	typedef logger_impl<log_funct> logger;
}
