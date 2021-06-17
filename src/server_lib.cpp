/**
 * \file
 * \brief File implements functions from server_lib.hpp
**/

#include "server_lib.hpp"
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using boost::asio::ip::tcp;

DataBase db;  ///< stores messages and users

/**
Checks if user exists on server
\param username given username
\return true if account exists, false otherwise
**/
bool DataBase::check_user(std::string &username)
{
    return (std::find(this->users.begin(), this->users.end(), username) != this->users.end());
}

/**
Creates user on server
\param username new username
\return true if created, false if existed already
**/
bool DataBase::create_user(std::string &username)
{
    if (std::find(this->users.begin(), this->users.end(), username) != this->users.end())
        return false;
    this->users.emplace_back(username);
    return true;
}

/**
Compiles json with messages addressed to particular user and sends to client
\param username username
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
auto DataBase::sync_messages(std::string &username) -> json
{
    json result;

    for (auto e : messages.items())
    {
        std::string chat_id{e.key()};
        std::string person1 = chat_id.substr(0, chat_id.find("+"));
        std::string person2 = chat_id.substr(chat_id.find("+") + 1, chat_id.size());

        if ((username == person1) or (username == person2)) // this user participates in chat
        {
            std::string other = (username == person2) ? person1 : person2;
            result[other] = e.value();
        }
    }
    return result;
}

/**
Accepts message from client, calculates id and saves to database
\param sender username of current user
\param receiver username of recipient
\param text string with encrypted text of the message
\return true if successful
**/
bool DataBase::accept_message(std::string &sender, std::string &receiver, std::string &text)
{
    // generate chat_id
    std::string chat_id = (sender < receiver) ? sender + "+" + receiver : receiver + "+" + sender;

    // generate number
    int message_id = messages[chat_id].size();

    // save message to db
    messages[chat_id][std::to_string(message_id)] = {
        {"sender", sender},
        {"receiver", receiver},
        {"text", text}};
    return true;
}

/**
 * prints out messages
**/
void DataBase::print_messages() { std::cout << "\n messages: \n"
                                            << messages.dump(4) << "\n"; }

/**
Function processes incoming tcp requests and sends a response to the client
**/
void session::go() {

    auto self(shared_from_this());
    boost::asio::spawn(strand, [this, self](boost::asio::yield_context yield) {
        try {
            std::locale loc{""};
            for (;;) { // while (true)
                std::string data1;
                timer.expires_from_now(std::chrono::seconds(10));
                boost::asio::async_read_until(socket, boost::asio::dynamic_buffer(data1), "\n", yield);
                // end of example

                std::string data = data1;
                std::cout << data << " ";

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
            // start of the example
            }
        } catch (std::exception &e) {
            socket.close();
            timer.cancel();
        }
    });

    boost::asio::spawn(strand, delegate(self, &session::timer_callback));
}