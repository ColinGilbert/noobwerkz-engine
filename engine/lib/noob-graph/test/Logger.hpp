#pragma once

#include <iostream>

namespace noob
{
	struct logger
	{
		static void log(const std::string& msg)
		{
			std::cout << msg << "\n";
		}
	};
}
