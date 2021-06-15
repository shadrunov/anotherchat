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

#include <vector>
#include <algorithm>

using boost::asio::ip::tcp;
using json = nlohmann::json;

// =====================
class DataBase {
  public:
    bool check_user(std::string &username){  // true if user exists
        return (std::find(this->users.begin(), this->users.end(), username) != this->users.end());
    }

    bool create_user(std::string &username){  // true if created, false if existed already
        if (std::find(this->users.begin(), this->users.end(), username) != this->users.end()) 
            return false;
        this->users.emplace_back(username);
        return true;
    }

    auto sync_messages(std::string &username) -> json
    {
        
        json result;

        for (auto e : messages.items())
        {
            std::string chat_id{e.key()};
            std::string person1 = chat_id.substr(0, chat_id.find("+"));
            std::string person2 = chat_id.substr(chat_id.find("+")+1, chat_id.size());

            if ((username == person1) or (username == person2))  // this user participates in chat
            {
                std::string other = (username == person2) ? person1 : person2;
                result[other] = e.value();
            }
        }

        return result;
    }

    bool accept_message(std::string &sender, std::string &receiver, std::string &text)
    {
        // generate chat_id
        std::string chat_id = (sender < receiver) ? sender+"+"+receiver : receiver+"+"+sender;

        // generate number
        int message_id = messages[chat_id].size();

        // save message to db
        messages[chat_id][std::to_string(message_id)] = {
            {"sender", sender},
            {"receiver", receiver},
            {"text", text}
        };
        return true;
    }

    void print_messages() { std::cout << "\n messages: \n" << messages.dump(4) << "\n"; }

  private:
    std::vector<std::string> users;
    json messages;

};

DataBase db;
// ================================





template <typename Class, typename Function>
auto delegate(std::shared_ptr<Class> ptr, Function fun) {
    // return [ptr = std::move(ptr), fun]() {
    return [ ptr, fun ]<typename... Args>(Args && ... arg) {
        return (ptr.get()->*fun)(std::forward<Args>(arg)...);
    };
}

class session : public std::enable_shared_from_this<session> {
    // Curiosly recurring template pattern (CRTP)
  public:
    explicit session(boost::asio::io_context &io_context, tcp::socket t_socket)
        : socket(std::move(t_socket)), timer(io_context),
          strand(io_context.get_executor()) {}

    void go() {
        auto self(shared_from_this());
        boost::asio::spawn(strand, [this,
                                    self](boost::asio::yield_context yield) {
            try {
                // std::array<char, 128> data;
                std::locale loc{""};
                for (;;) { // while (true)
                    std::string data1;
                    timer.expires_from_now(std::chrono::seconds(10));
                    size_t m = boost::asio::async_read_until(socket, boost::asio::dynamic_buffer(data1), "\n", yield);

                    std::string data = data1;
                    std::cout << data << " ";
                    // boost::asio::async_write(socket, boost::asio::buffer("200"), yield);

                    if (!data.empty()) 
                    {
                        // process request
                        std::string request = data.substr(0, data.find(" "));  // method: login signup get send
                        data.erase(0, data.find(" ") + 1);
                        data.pop_back();  // body of request

                        if (request == "login")
                        {
                            if (db.check_user(data)) 
                                boost::asio::async_write(socket, boost::asio::buffer("200\n"), yield);
                            else
                                boost::asio::async_write(socket, boost::asio::buffer("404\n"), yield);
                        }

                        else if (request == "signup")
                        {
                            if (db.create_user(data)) 
                                boost::asio::async_write(socket, boost::asio::buffer("200\n"), yield);
                            else
                                boost::asio::async_write(socket, boost::asio::buffer("404\n"), yield);
                        }

                        else if (request == "get")  // sync messages
                        {
                            json raw = db.sync_messages(data);
                            std::cout << raw.dump(4) << "\n";
                            std::string dumped = raw.dump();
                            boost::asio::async_write(socket, boost::asio::buffer(dumped+"\n"), yield);
                        }

                        else if (request == "send")  // accept message
                        {   // send sender receiver text text text ...... \n
                            std::string sender = data.substr(0, data.find(" "));  
                            data.erase(0, data.find(" ") + 1);

                            std::string receiver = data.substr(0, data.find(" "));  
                            data.erase(0, data.find(" ") + 1);

                            std::cout << "send sender: |" << sender << "| receiver: |" << receiver << "| text: |" << data << "| \n";
                            
                            db.accept_message(sender, receiver, data);
                            db.print_messages();

                            boost::asio::async_write(socket, boost::asio::buffer("200\n"), yield);
                            
                        }
                    }
                       













                    // https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/overview/core/line_based.html
                    // std::string_view text{data};
                    // if (m > 0 && !text.empty() && std::isdigit(text[0], loc)) {
                    //     char str[] = "hello, ";
                    //     int n = 0;
                    //     auto [ptr, ec] = std::from_chars(
                    //         text.data(), std::to_address(text.end()), n);
                    //     if (ec == std::errc{}) {
                    //         text.remove_prefix(size_t(ptr - text.data()));
                    //         for (int i = 0; i < n; ++i)
                    //             boost::asio::async_write(
                    //                 socket, boost::asio::buffer(str), yield);
                    //     }
                    // }
                    // boost::asio::async_write(socket, boost::asio::buffer(text),
                    //                          yield);
                }
            } catch (std::exception &e) {
                socket.close();
                timer.cancel();
            }
        });

        boost::asio::spawn(strand, delegate(self, &session::timer_callback));
    }

    void timer_callback(boost::asio::yield_context yield) {
        while (socket.is_open()) {
            boost::system::error_code ignored_ec;
            timer.async_wait(yield[ignored_ec]);
            if (timer.expires_from_now() <= std::chrono::seconds(0))
                socket.close();
        }
    }

  private:
    tcp::socket socket;
    boost::asio::steady_timer timer;
    boost::asio::strand<boost::asio::io_context::executor_type> strand;
};

int main(int argc, char *argv[]) {
    try {
        unsigned short port = 1234;
        if (argc > 1)
            port = static_cast<unsigned short>(std::atoi(argv[1]));
        // if (argc != 2) {
        //     std::cerr << "Usage: echo_server <port>\n";
        //     return 1;
        // }

        boost::asio::io_context io_context;

        boost::asio::spawn(io_context, [&](boost::asio::yield_context yield) {
            tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

            while (true) {

                // for (auto i : db.users) { std::cout << i << " - ";}


                boost::system::error_code ec;
                tcp::socket socket(io_context);
                acceptor.async_accept(socket, yield[ec]);
                if (!ec) {
                    std::make_shared<session>(io_context, std::move(socket))
                        ->go();
                } else {
                    std::cerr << ec << "\n";
                }
            }
        });

        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}