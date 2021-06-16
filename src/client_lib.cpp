/**
 * \file
 * \brief File implements functions from client_lib.hpp
**/

#include "client_lib.hpp"
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string>

using json = nlohmann::json;
using boost::asio::ip::tcp;

/**
Checks if given username is correct (consists only from digits, letters and "_")
\param username given username
\return true if username is valid, false otherwise
**/
bool is_valid_username(std::string username)
{
    for (char x : username)
        if ((std::isalnum(x) == 0) && (x != '_')) { return false; }  // bad character 
    return true;
}

/**
Checks if given username is correct and exists on server
\param username username of the user
\return true if username is valid and exists on server, false otherwise
**/
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

/**
Checks if given username is correct and then creates new user on server
\param username username of new user
\return true if username new user creates, false if not or if username is not correct
**/
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

/**
Downloads messages addressed to current user from server
\param user instance of User class
\return json with messages
\code json
get bob
{
    "alice": {
        "0": {
            "receiver": "alice",
            "sender": "bob",
            "text": "q2ZJuWt="
        },
        "1": {
            "receiver": "alice",
            "sender": "bob",
            "text": "riY="
        }
    },
    "mike": {
        "0": {
            "receiver": "bob",
            "sender": "mike",
            "text": "rh5Ggq3GhzUXe2QxvU=="
        }
    }
}
\endcode
**/
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

/**
Sends message to server
\param sender username of current user
\param receiver username of recipient
\param text string with encrypted text of the message
\return true if successful
**/
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

