#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


const std::string ip = "127.0.0.1";
const int port = 1234;


bool is_valid_username(std::string username);

struct User
{
    size_t state = 0;  // current position
    bool logged_in = false;
    std::string username;

    json messages;

    std::string current_chat;
    std::string current_key;
};


bool login(std::string username);


bool signup(std::string username);


json get_messages(User &user);


bool send_reply(std::string &sender, std::string &receiver, std::string &text);