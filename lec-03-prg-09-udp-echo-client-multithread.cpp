//
// blocking_udp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <thread>

using boost::asio::ip::udp;

const char *host = "127.0.0.1";
const char *port = "65456";
enum
{
  max_length = 1024
};

void receive_thread(udp::socket *s)
{
  while (true)
  {
    char reply[max_length];
    udp::endpoint sender_endpoint;
    size_t reply_length = s->receive_from(
        boost::asio::buffer(reply, max_length), sender_endpoint);
    std::cout << "> received: " << std::string(reply, reply_length) << "\n";
    if (std::string(reply, reply_length) == "quit")
    {
      break;
    }
  }
}

int main()
{
  std::cout << "> echo-client is activated\n";
  try
  {

    boost::asio::io_context io_context;

    udp::socket s(io_context, udp::endpoint(udp::v4(), 0));

    udp::resolver resolver(io_context);
    udp::endpoint endpoint =
        *resolver.resolve(udp::v4(), host, port).begin();

    std::thread(receive_thread, &s).detach();

    while (true)
    {
      std::cout << "> ";
      char request[max_length];
      std::cin.getline(request, max_length);
      size_t request_length = std::strlen(request);
      if (request_length == 0)
      {
        continue; // Skip empty input
      }
      s.send_to(boost::asio::buffer(request, request_length), endpoint);
      if (std::string(request, request_length) == "quit")
      {
        break;
      }
    }
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  std::cout << "> echo-client is de-activated\n";
  return 0;
}