#include "Logger.hpp"
#include <iostream>
void logger::log(const std::string& message)
{
//			asio::ip::tcp::iostream s;
//			s.connect(SERVER_ADDRESS, SERVER_PORT);
//			if (s)
//			{
//				s << message << std::flush;
//			}
std::cout << message << std::endl;
}
