#include "Logger.hpp"

void logger::log(const std::string& message)
		{
			asio::ip::tcp::iostream s;
			s.connect(SERVER_ADDRESS, SERVER_PORT);
			if (s)
			{
				s << message << std::flush;
			}
}
