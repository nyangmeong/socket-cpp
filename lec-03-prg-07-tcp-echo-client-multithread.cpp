//
// blocking_tcp_echo_client.cpp
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
#include <thread>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

constexpr int max_length = 1024;
const char *host = "127.0.0.1";
const char *port = "65456";

void receive_thread(tcp::socket *s)
{
    while (true)
    {
        char reply[max_length];
        boost::system::error_code error;
        size_t reply_length = s->read_some(boost::asio::buffer(reply), error);
        if (error)
        {
            break;
        }
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

        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(s, resolver.resolve(host, port));

        std::thread(receive_thread, &s).detach();

        while (true)
        {
            std::cout << "> ";
            char request[max_length];
            std::cin >> request;
            size_t request_length = std::strlen(request);
            boost::asio::write(s, boost::asio::buffer(request, request_length));
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