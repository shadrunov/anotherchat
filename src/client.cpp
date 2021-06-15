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
#include <fmt/core.h>

#include <cstdlib>
#include <map>

#include "encrypt.h"


using boost::asio::ip::tcp;
using json = nlohmann::json;

const std::string ip = "127.0.0.1";
const int port = 1234;


struct User
{
    size_t state = 0;  // current position
    bool logged_in = false;
    std::string username;

    json messages;

    std::string current_chat;
    std::string current_key;


};

bool is_valid_username(std::string username)
{
    for (char x : username)
    {
        if ((std::isalnum(x) == 0) && (x != '_')) { return false; }  // bad character 
    }
    return true;
}


bool login(std::string username)
{
    if (!is_valid_username(username)) return false;

    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
    const string msg = "login " + username + "\n";
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);

    boost::asio::streambuf rb;
    boost::asio::read_until(socket, rb, "\n");
    std::string response{boost::asio::buffer_cast<const char*>(rb.data())};
    // const char* response = boost::asio::buffer_cast<const char*>(rb.data());
    // std::cout << response << " ";

    return (response == "200\n");
}


bool signup(std::string username)
{
    if (!is_valid_username(username)) return false;

    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
    const string msg = "signup " + username + "\n";
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);

    boost::asio::streambuf rb;
    boost::asio::read_until(socket, rb, "\n");
    std::string response{boost::asio::buffer_cast<const char*>(rb.data())};
    // std::cout << response << " ";

    return (response == "200\n");
}


json get_messages(User &user)
{
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
    const string msg = "get " + user.username + "\n";
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);

    boost::asio::streambuf rb;
    boost::asio::read_until(socket, rb, "\n");
    std::string response{boost::asio::buffer_cast<const char*>(rb.data())};
    // std::cout << response << " ";

    return json::parse(response);

}


    // socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
//     boost::asio::streambuf receive_buffer;
//     std::cout << "line 66" << std::endl;
//     boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
//     std::cout << "line 68" << std::endl;
//     std::cin >> username;
//     if ( error && error != boost::asio::error::eof ) {
//         std::cout << "receive failed: " << error.message() << std::endl;
//     }
//     else {
//         const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
//         std::cout << data << std::endl;
//     }
//     return true;
// }

int send_reply(std::string &sender, std::string &receiver, std::string &text)
{
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
    const string msg = "send " + 
        sender + " " +
        receiver + " " +
        text + "\n";
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);

    boost::asio::streambuf rb;
    boost::asio::read_until(socket, rb, "\n");
    std::string response{boost::asio::buffer_cast<const char*>(rb.data())};
    // std::cout << response << " ";
    // послать на сервер сообщение

    return (response == "200\n");
}


void process_step(User &user)
{   
    system("clear");
    std::string code;

    if (user.state == 0)  // welcome
    {
        system("clear");

        if (user.logged_in)
        {
            fmt::print("Hi {}! \n\nSelect an option below:\n", user.username);
            fmt::print(" s  |  Sync\n");
            fmt::print(" c  |  Chat List\n");
            // fmt::print(" n  |  Start New Chat\n");
        } else {
            fmt::print("Welcome to Chat client! \n\nSelect an option below:\n");
            fmt::print(" u  |  Sign Up\n");
            fmt::print(" l  |  Log In\n");
        }
        fmt::print("\ncommand> ");
        std::cin >> code;

        if ((code == std::string{'s'}) && (user.logged_in))
        {
            // start sync
            user.state = 3;
        }
        if ((code == std::string{'c'}) && (user.logged_in))
        {
            // chat list
            user.state = 4;
        }
        // if ((code == std::string{'n'}) && (user.logged_in))
        // {
        //     // start new chat
        //     user.state = 7;
        // }

        if ((code == std::string{'u'}) && (!user.logged_in))
        {
            // sign up
            user.state = 1;
        }
        if ((code == std::string{'l'}) && (!user.logged_in))
        {
            // log in
            user.state = 2;
        }
    } 
    
    else if (user.state == 1)  // sign up
    {
        fmt::print("Sign Up \n\n");

        while (!user.logged_in)  // ещё не зарегестрировался
        {
            fmt::print("Enter username (or b to go back): ");
            std::getline(std::cin >> std::ws, code);
            if (code == std::string{'b'}) { user.state = 0; break;}

            if (!signup(code))
                fmt::print("Username is incorrect or is taken already.\n");
            else {  // signed up
                    user.username = code;
                    user.state = 0;
                    user.logged_in = true;
            }
        }
    } 
    
    else if (user.state == 2)  // log in
    {
        fmt::print("Log in \n\n");
        while (!user.logged_in)  // ещё не вошёл
        {
            fmt::print("Enter username (or b to go back): ");
            std::getline(std::cin >> std::ws, code);
            if (code == std::string{'b'}) { user.state = 0; break;}

            if (!login(code))
                fmt::print("Username is incorrect or does not exist yet. If so, please, register an account.\n");
            else {  // log in
                    user.username = code;
                    user.state = 0;
                    user.logged_in = true;
            }
        }
    } 
    
    else if (user.state == 3)  // sync
    {
        fmt::print("Sync messages... \n\n");

        user.messages = get_messages(user);
        user.state = 0;
        fmt::print("Synced {} chats.\n\n", user.messages.size());
    }

    else if (user.state == 4)  // chat list
    {
        std::vector<std::string> senders;
        for (auto m : user.messages.items())
        {
            senders.push_back(m.key());
            // std::cout << m << " ";
        }

        auto print_messages{
            [&](){
                system("clear");
                fmt::print("Chat List \n\nSelect from the list below: \n\n");
                if (user.messages.empty()) fmt::print("Empty. Try to sync messages first. \n\n");
                else
                {
                    // print out messages
                    for (int i = 0; i < senders.size(); i++) 
                        fmt::print(" {} | {} \n", std::to_string(i), senders[i]);
                }
            }
        };

        
        print_messages();
        fmt::print("\ncommand (b to go back, n new chat)> ");

        int n = -1;

        while ((n < 0) || (n >= senders.size()))
        {
            std::getline(std::cin >> std::ws, code);
            if (code == std::string{'b'}) { user.state = 0; break; }
            if (code == std::string{'n'}) { user.state = 7; break; }
            try
                { n = std::stoi(code); }
            catch(const std::exception& e) {}

            if ((n < 0) || (n > senders.size()))
            {
                // number is incorrect
                print_messages();
                fmt::print("\nPlease, specify correct number or b to go back! ");
                fmt::print("\ncommand (b to go back)> ");
            }
            else
            {
                // process number
                user.current_chat = senders[n];
                user.state = 5;
            }
        }
    }

    else if (user.state == 5)  // enter key
    {
        fmt::print("Decrypt messages\n\nEnter the key from {} or b to go back: ", user.current_chat);
        std::getline(std::cin >> std::ws, code);
        if (code == std::string{'b'}) user.state = 0;
        else 
        {
            user.current_key = code;
            user.state = 6;
        }
    } 

    else if (user.state == 6)  // show chat
    {
        fmt::print("Chat with {}\n\n", user.current_chat);
        json this_chat = user.messages[user.current_chat];
        
        for (int i = 0; i < this_chat.size(); i++)
        {
            // go through messages
            json message = this_chat[std::to_string(i)];
            std::string sender{message["sender"]};
            std::string enc_text{message["text"]};

            fmt::print("{} : {}\n", sender, decrypt(enc_text, user.current_key));
        }

        while (code != std::string{'b'})  // reply
        {
            fmt::print("\nreply (or b)> ");
            std::getline(std::cin >> std::ws, code);
            std::string reply = encrypt(code, user.current_key);
            if (code == std::string{'b'}) break;
            if (send_reply(user.username, user.current_chat, reply)) 
                fmt::print("Sent sucessfully. \n");
            else { 
                fmt::print("Cannot send. \n\nenter b to go back>"); 
                std::getline(std::cin >> std::ws, code);
                break;
            }
        }
        user.state = 4;
    }
    
    else if (user.state == 7)  // start new chat
    {
        fmt::print("Start New Chat\n\n");
        while (user.state == 7)
        { 
            fmt::print("Enter username: ");
            std::getline(std::cin >> std::ws, code);
            if (code == std::string{'b'}) { user.state = 0; break; }
            if (login(code)){  // проверяем что existed
                user.current_chat = code;
                user.state = 5;
            }
        }
    }
}

int main() 
{

    User user;

    for (;;) { process_step(user); }
    
    return 0;
}

    
    // fmt::print("Don't {}\n", "panic");
    // std::cin >> port;
    // fmt::print("Don't {}\n", port);
    //
    // 
    // json j = {
    //         {"mama", {
    //             {"0", {
    //                 {"sender", "mama"},
    //                 {"receiver", "mark"},
    //                 {"text", "hello there"}
    //             }}
    //         }}
    //     };
    // 
    // for (auto e : j.items())
    // {
    //     std::cout << e.key() << " ";
    //     std::cout << e << " ";
    //     std::cout << e.value() << " ";
    // }
// 
    // std::string msg = "mama goes to work";
    // std::string key = "qqq";
    // std::string cr = encrypt(msg, key);