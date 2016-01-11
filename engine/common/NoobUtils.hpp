#pragma once

#include <dirent.h>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <stdio.h>

#include "Logger.hpp"
// With any luck, this class should become obsolete.

namespace noob
{
	struct utils
	{
		static std::string load_file_as_string(const std::string& filename);
	};
}
