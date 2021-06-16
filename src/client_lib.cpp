#include "client_lib.hpp"
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string>

using json = nlohmann::json;
using boost::asio::ip::tcp;


bool is_valid_username(std::string username)
{
    for (char x : username)
        if ((std::isalnum(x) == 0) && (x != '_')) { return false; }  // bad character 
    return true;
}


bool login(std::string username)
{
    if (!is_valid_username(username)) return false;

    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
    const std::string msg = "login " + username + "\n";
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);

    boost::asio::streambuf rb;
    boost::asio::read_until(socket, rb, "\n");
    std::string response{boost::asio::buffer_cast<const char*>(rb.data())};

    return (response == "200\n");
}


bool signup(std::string username)
{
    if (!is_valid_username(username)) return false;

    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
    const std::string msg = "signup " + username + "\n";
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);

    boost::asio::streambuf rb;
    boost::asio::read_until(socket, rb, "\n");
    std::string response{boost::asio::buffer_cast<const char*>(rb.data())};

    return (response == "200\n");
}


json get_messages(User &user)
{
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
    const std::string msg = "get " + user.username + "\n";
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);

    boost::asio::streambuf rb;
    boost::asio::read_until(socket, rb, "\n");
    std::string response{boost::asio::buffer_cast<const char*>(rb.data())};

    return json::parse(response);
}


bool send_reply(std::string &sender, std::string &receiver, std::string &text)
{
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
    const std::string msg = "send " + 
        sender + " " +
        receiver + " " +
        text + "\n";
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);

    boost::asio::streambuf rb;
    boost::asio::read_until(socket, rb, "\n");
    std::string response{boost::asio::buffer_cast<const char*>(rb.data())};

    return (response == "200\n");
}