#include <iostream>
#include <memory>
#include <thread>
#include <string>
#include <asio.hpp>

#include "Untracked.hpp"

int main()
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
	std::cout << line << std::endl;
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
