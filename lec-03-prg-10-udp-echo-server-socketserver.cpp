//
// blocking_udp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using boost::asio::ip::udp;

enum { max_length = 1024 };

void server(boost::asio::io_context& io_context, const char* host, unsigned short port)
{
  udp::socket sock(io_context, udp::endpoint(boost::asio::ip::make_address(host), port));
  for (;;)
  {
    char data[max_length];
    udp::endpoint sender_endpoint;
    size_t length = sock.receive_from(
        boost::asio::buffer(data, max_length), sender_endpoint);
    std::cout << "> echoed: " << std::string(data, length) << "\n";
    if (std::string(data, length) == "quit")
    {
      continue;
    }
    
    sock.send_to(boost::asio::buffer(data, length), sender_endpoint);
  }
}

int main()
{
  const char* host = "127.0.0.1";
  const int port = 65456;
  std::cout << "> echo-server is activated\n";
  try
  {
    

    boost::asio::io_context io_context;

    server(io_context, host, port);
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  std::cout << "> echo-server is de-activated\n";

  return 0;
}