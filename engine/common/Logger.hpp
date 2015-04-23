#pragma once
#include <asio/ip/tcp.hpp>
#include "Untracked.hpp"
//#include "Logger.hpp"
class logger
{
	public:
		static void log(const std::string& message);
};


