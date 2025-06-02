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
#include <future>
#include <iostream>
#include <thread>
#include <utility>
#include <atomic>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <set>
#include <mutex>

using boost::asio::ip::tcp;

const int max_length = 1024;
std::atomic<int> threadCounter = 1;
std::set<tcp::socket *> group_queue;
std::mutex group_queue_mutex;

void session(tcp::socket sock)
{
  threadCounter++;
  {
    std::lock_guard<std::mutex> lock(group_queue_mutex);
    group_queue.insert(&sock);
  }
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

      if (std::string(data, length) == "quit")
      {
        break;
      }
      else
      {
        std::cout << "> received ( " << std::string(data, length) << " ) and echoed to " << group_queue.size() << " clients\n";
        {
          std::lock_guard<std::mutex> lock(group_queue_mutex);
          for (auto &conn : group_queue)
          {
            boost::asio::write(*conn, boost::asio::buffer(data, length));
          }
        }
      }
    }
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
  }
  threadCounter--;
  {
    std::lock_guard<std::mutex> lock(group_queue_mutex);
    group_queue.erase(&sock);
  }
}

void server(boost::asio::io_context &io_context, const char *host, unsigned short port, std::promise<void> &ready)
{
  std::cout << "> server loop running in thread (main thread): Thread-" << std::this_thread::get_id() << "\n";
  tcp::acceptor a(io_context, tcp::endpoint(boost::asio::ip::make_address(host), port));
  ready.set_value();
  for (;;)
  {
    tcp::socket sock(io_context);
    a.accept(sock);
    std::thread(session, std::move(sock)).detach();
  }
}

int main()
{
  const char *host = "127.0.0.1";
  const int port = 65456;
  std::cout << "> echo-server is activated\n";
  try
  {
    boost::asio::io_context io_context;
    std::promise<void> ready;
    std::future<void> ready_future = ready.get_future();

    std::thread(server, std::ref(io_context), host, port, std::ref(ready)).detach();
    ready_future.get();
    while (true)
    {
      std::string msg;
      std::cout << "> ";
      std::cin >> msg;
      if (msg == "quit")
      {
        if (threadCounter == 1)
        {
          std::cout << "> stop procedure started\n";
          break;
        }
        else
        {
          std::cout << "> active threads are remained : " << threadCounter - 1
                    << " threads\n";
        }
      }
    }
    std::cout << "> echo-server is de-activated\n";
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}