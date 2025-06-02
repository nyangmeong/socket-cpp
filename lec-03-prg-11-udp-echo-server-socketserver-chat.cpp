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
#include <set>

using boost::asio::ip::udp;
std::set<udp::endpoint> group_queue;

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
    std::string recvCmd(data, length);
    if (recvCmd[0] == '#' or recvCmd == "quit")
    {
      if (recvCmd == "#REG")
      {
        std::cout << "> client registered " << sender_endpoint << "\n";
        group_queue.insert(sender_endpoint);
      }
      else if (recvCmd == "#DEREG" or recvCmd == "quit")
      {
        if (group_queue.find(sender_endpoint) != group_queue.end())
        {
          std::cout << "> client de-registered " << sender_endpoint << "\n";
          group_queue.erase(sender_endpoint);
        }
      }
    }
    else
    {
      if (group_queue.size() == 0)
      {
        std::cout << "> no clients to echo\n";
      }
      else if (group_queue.find(sender_endpoint) == group_queue.end())
      {
        std::cout << "> ignores a message from un-registered client\n";
      }
      else
      {
        std::cout << "> received ( " << std::string(data, length) << " ) and echoed to "
                  << group_queue.size() << " clients\n";
        for (const auto& endpoint : group_queue)
        {
            sock.send_to(boost::asio::buffer(data, length), endpoint);
        }
      }
    }
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