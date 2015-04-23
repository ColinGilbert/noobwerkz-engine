#include "Logger.hpp"

void logger::log(const std::string& message)
		{
			asio::ip::tcp::iostream s;
			s.connect(server_address.c_str(), server_port.c_str());
			if (s)
			{
				s << message << std::flush;
			}

}
