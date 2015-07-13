#pragma once
#include <asio/ip/tcp.hpp>
#include "Config.hpp"
#include "format.h"

//#include "Logger.hpp"
class logger
{
	public:
		static void log(const std::string& message);
};


