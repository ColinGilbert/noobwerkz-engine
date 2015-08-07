#include "Logger.hpp"
//#include <asio/ip/tcp.hpp>
#include <iostream>

void logger::log(const std::string& message)
{
//			asio::ip::tcp::iostream s;
//			s.connect(SERVER_ADDRESS, SERVER_PORT);
//			if (s)
//			{
//				s << message << std::flush;
//			}
//			else std::cout << "Could not init logger :(" << std::endl;
	std::cout << message << std::endl;
}
