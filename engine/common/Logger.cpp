#include "Logger.hpp"
<<<<<<< HEAD
#include <iostream>
=======
//#include <asio/ip/tcp.hpp>
#include <iostream>

>>>>>>> dev
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
