/**
 * \file
 * \brief Chat server -- stores users and messages
 * supports 4 methods:
 * - login <username>
 * - signup <username>
 * - get <username>
 * - send <sender> <receiver> <text of a message>
**/

//
// echo_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "server_lib.hpp"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <charconv>
#include <iostream>
#include <locale>
#include <memory>
#include <nlohmann/json.hpp>
#include <string_view>
#include <utility>
#include <vector>
#include <algorithm>

using boost::asio::ip::tcp;
using json = nlohmann::json;


// example from boost documentation
int main() {
    try {
        unsigned short port = 1234;
        boost::asio::io_context io_context;
        boost::asio::spawn(io_context, [&](boost::asio::yield_context yield) {
            tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

            while (true) {
                boost::system::error_code ec;
                tcp::socket socket(io_context);
                acceptor.async_accept(socket, yield[ec]);
                if (!ec) {
                    std::make_shared<session>(io_context, std::move(socket)) ->go();
                } else { std::cerr << ec << "\n"; }
            }
        });
        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
// end of the example