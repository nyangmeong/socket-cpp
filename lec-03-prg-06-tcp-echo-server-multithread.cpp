//
// blocking_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using boost::asio::ip::tcp;

const int max_length = 1024;
int threadCounter = 1;

void session(tcp::socket sock)
{
  threadCounter++;
  try
  {
    std::cout << "> client connected by IP address " << sock.remote_endpoint().address().to_string()
              << " with Port number " << sock.remote_endpoint().port() << "\n";
    for (;;)
    {
      char data[max_length];
      std::thread::id cur_thread = std::this_thread::get_id();
      boost::system::error_code error;
      size_t length = sock.read_some(boost::asio::buffer(data), error);
      if (error == boost::asio::stream_errc::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw std::system_error(error); // Some other error.

      std::cout << "> echoed: " << data << "by Thread-" << cur_thread << "\n";

      boost::asio::write(sock, boost::asio::buffer(data, length));

      if(std::string(data, length) == "quit")
      {
        break;
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
  }
  threadCounter--;
}

void server(boost::asio::io_context& io_context, const char* host, unsigned short port)
{
  std::cout << "> server loop running in thread (main thread): " << std::this_thread::get_id() << "\n";
  tcp::acceptor a(io_context, tcp::endpoint(boost::asio::ip::make_address(host), port));
  for (;;)
  {
    std::string msg;
    tcp::socket sock(io_context);
    a.accept(sock);
    std::thread(session, std::move(sock)).detach();
  }
}

int main()
{
  const char* host = "127.0.0.1";
  int port = 65456;
  std::cout << "> echo-server is activated\n";
  try
  {
    boost::asio::io_context io_context;

    std::thread(server, std::ref(io_context), host, port).detach(); 
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}