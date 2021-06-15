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
#include <string>

using json = nlohmann::json;
using namespace std::literals;

int main()
{
    json messages;

    
    messages["mama"s+"mark"s] = {};

    json mes;
    mes["sender"] = "mama";
    mes["receiver"] = "mark";
    mes["text"] = "vdfkvbjv";

    int num = messages["mama"].size();
    
    messages["mama"][std::to_string(num)] = mes;


    // cond ? expr1 : expr2 это как в python было expr1 if cond else expr2


    json mes1;
    mes1["sender"] = "ivan";
    mes1["receiver"] = "mark";
    mes1["text"] = "vdfkvbjv";

    num = messages["mama"].size();
    
    messages["mama"][std::to_string(num)] = mes1;
    


    // for ()

    // for (auto e : messages) { std::cout << e << " "; }
    std::cout << messages << "\n";
    std::cout << " nnn " << messages.contains("k") << "\n";
    std::cout << messages << "\n";
    std::cout << " nnn " << messages["k"].size() << "\n";
    std::cout << messages << "\n";
    messages["k"]["0"] = {"sender", "mamaaaaaa"};
    std::cout << messages << "\n";
    messages["k"]["1"] = {"sender", "11111111"};
    std::cout << messages << "\n\n";

    json a;
    json m;
    m["sender"] = "mama";
    m["receiver"] = "papa";
    m["text"] = "vdfkvbjv";
    num = a["mama+papa"].size();
    a["mama+papa"][std::to_string(num)] = m;

    m["sender"] = "papa";
    m["receiver"] = "mama";
    m["text"] = "pppppppppp";
    num = a["mama+papa"].size();
    a["mama+papa"][std::to_string(num)] = m;

    std::cout << a << "\n";
    for (auto k : a.items()) { std::cout << k.key() << " " << k.value() << "\n"; }
    // std::cout << a.items() << "/n";
    // std::cout << a.items() << "/n";


    json result;
    std::string username = "mama";

    for (auto e : a.items())
    {
        std::string chat_id{e.key()};
        std::string person1 = chat_id.substr(0, chat_id.find("+"));
        std::string person2 = chat_id.substr(chat_id.find("+")+1, chat_id.size());

        if ((username == person1) or (username == person2))  // this user participates in chat
        {
            std::string other = (username == person1) ? person1 : person2;
            result[other] = e.value();
        }
    }

    std::cout << "results\n" << result << "\n";



    return 0;
}




// {
//     ["mama", "mark"]: {
//         "0": {
//             "senders": "mama", 
//             "reciever": "mark", 
//             "text": "hello it is the first message"
//         },
//         "1": {
//             "sender": "mark", 
//             "reciever": "mama", 
//             "text": "it is a reply"
//         }
//     },

//     "papa": {
//         "0": {
//             "sender": "mark",
//             "reciever": "papa",
//             "text": "cucou"
//         }
//     }
    
// }
