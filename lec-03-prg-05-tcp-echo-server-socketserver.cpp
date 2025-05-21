//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#define HOST "127.0.0.1" 
#define PORT 65456 
#define DEFAULT_BUFLEN 1024

using boost::asio::ip::tcp;

int main()
{
  std::cout << "> echo-server is activated\n";

  

  try
  {
    boost::asio::io_context io_context;

    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));

    for (;;)
    {
      tcp::socket socket(io_context);
      acceptor.accept(socket);

      std::cout << "> client connected by IP address " << socket.remote_endpoint().address().to_string() 
      << " with Port number " << socket.remote_endpoint().port() << "\n";
      for (;;)
      {
        boost::array<char, DEFAULT_BUFLEN> recvbuf;
        boost::system::error_code error;

        recvbuf.fill(0);

        size_t recvbuf_len = socket.read_some(boost::asio::buffer(recvbuf), error);

        if (error == boost::asio::error::eof)
          break; // Connection closed cleanly by peer.
        else if (error)
          throw boost::system::system_error(error); // Some other error.

        std::cout << "> echoed: " << recvbuf.data()<< "\n";

        socket.send(boost::asio::buffer(recvbuf, recvbuf_len), 0, error);
        if (std::string(recvbuf.data(), recvbuf_len)  == "quit"){
          break;
        }
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  std::cout << "> echo-server is de-activated\n";
  
  return 0;
}