/**
 * \file
 * \brief File defines functions and a structure for chat client
**/

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


const std::string ip = "127.0.0.1";
const int port = 1234;

/**
    Checks if given username is correct (consists only from digits, letters and "_")
    \param username given username
    \return true if username is valid, false otherwise
**/
bool is_valid_username(std::string username);


struct User
{
    /**
     * stores data of current user
    **/
    size_t state = 0;  ///< current position of user (current screen)
    bool logged_in = false;  ///< if user is logged in
    std::string username;  ///< username of the user

    json messages;  ///< stores messages downloaded from server

    std::string current_chat;  ///< username of current chat
    std::string current_key;  ///< crypto key of current chat
};

/**
    Checks if given username is correct and exists on server
    \param username username of the user
    \return true if username is valid and exists on server, false otherwise
**/
bool login(std::string username);

/**
    Checks if given username is correct and then creates new user on server
    \param username username of new user
    \return true if username new user creates, false if not or if username is not correct
**/
bool signup(std::string username);

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
json get_messages(User &user);

/**
    Sends message to server
    \param sender username of current user
    \param receiver username of recipient
    \param text string with encrypted text of the message
    \return true if successful
**/
bool send_reply(std::string &sender, std::string &receiver, std::string &text);