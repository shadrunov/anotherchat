//
// echo_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

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

using boost::asio::ip::tcp;
using json = nlohmann::json;


// template <typename Class, typename Function>
// auto delegate(std::shared_ptr<Class> ptr, Function fun) {
//     // return [ptr = std::move(ptr), fun]() {
//     return [ ptr, fun ]<typename... Args>(Args && ... arg) {
//         return (ptr.get()->*fun)(std::forward<Args>(arg)...);
//     };
// }


class session : public std::enable_shared_from_this<session> {
    // Curiosly recurring template pattern (CRTP)
  public:
    explicit session(boost::asio::io_context &io_context, tcp::socket t_socket)
        : socket(std::move(t_socket)), timer(io_context),
          strand(io_context.get_executor()) {}

    void go() 
    {
        auto self(shared_from_this());
        boost::asio::spawn(strand, 
            [this, self](boost::asio::yield_context yield) 
            {
                try 
                {
                    // std::array<char, 128> data;
                    std::locale loc{""};
                    for (;;) 
                    { 
                        std::string data;
                        // timer.expires_from_now(std::chrono::seconds(10));

                        size_t m = boost::asio::async_read_until(socket, boost::asio::dynamic_buffer(data), "\n", yield);
                        // https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/overview/core/line_based.html

                        std::string_view text{data};

                        if (m > 0 && !text.empty() && std::isdigit(text[0], loc)) 
                        {
                            // char str[] = "hello, ";
                            // int n = 0;
                            // auto [ptr, ec] = std::from_chars(text.data(), std::to_address(text.end()), n);

                            // if (ec == std::errc{}) 
                            // {
                                // text.remove_prefix(size_t(ptr - text.data()));
                                // for (int i = 0; i < n; ++i)
                                    // boost::asio::async_write(socket, boost::asio::buffer(str), yield);
                            // }
                        }
                        boost::asio::async_write(socket, boost::asio::buffer(text), yield);
                    }
                } 
                catch (std::exception &e) {
                    socket.close();
                    // timer.cancel();
                }
            });

        // boost::asio::spawn(strand, delegate(self, &session::timer_callback));
    }

    // void timer_callback(boost::asio::yield_context yield) 
    // {
    //     while (socket.is_open()) 
    //     {
    //         boost::system::error_code ignored_ec;
    //         timer.async_wait(yield[ignored_ec]);
    //         if (timer.expires_from_now() <= std::chrono::seconds(0))
    //             socket.close();
    //     }
    // }

  private:
    tcp::socket socket;
    boost::asio::steady_timer timer;
    boost::asio::strand<boost::asio::io_context::executor_type> strand;
};


int main() 
{
    unsigned short port = 1234;

    boost::asio::io_context io_context;
    boost::asio::spawn(io_context, [&](boost::asio::yield_context yield) {
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        while (true) 
        {
            tcp::socket socket(io_context);
            acceptor.async_accept(socket, yield);
            std::make_shared<session>(io_context, std::move(socket))->go();
        }
    });

    io_context.run();
    return 0;
}
