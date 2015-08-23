#pragma once

#include <iostream>
#include <memory>
#include <thread>
#include <string>
#include <asio.hpp>

// TODO: Cleanup this dependency
#include "Config.hpp"
/*
namespace noob
{
	class receiver 
	{
		public:

			void init(std::function<void(std::string)> callback)
			{
				std::unique_ptr<noob::receiver> a = std::unique_ptr<noob::receiver>(new noob::receiver());
				std::thread worker([this, callback]()
						{
						try
						{
						asio::io_service io_service;

						asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), SERVER_PORT);
						asio::ip::tcp::acceptor acceptor(io_service, endpoint);

						for (;;)
						{
						asio::ip::tcp::iostream stream;
						acceptor.accept(*stream.rdbuf());

							std::string line;
							std::getline(stream,line);
							callback(line);
						//	std::cout << line << std::endl;
						}
						}
						catch (std::exception& e)
						{
						}


						});
				worker.join();
			}

	};

	class sender
	{
		void send(const std::string& message, const std::string& server_address, const std::string& server_port)
		{
			asio::ip::tcp::iostream s;
			s.connect(server_address.c_str(), server_port.c_str());
			if (s)
			{
				s << message << std::flush;
			}
		}
	};
}
*/
