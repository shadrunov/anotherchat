#include "catch.hpp"
#include "server_lib.hpp"
#include "client_lib.hpp"
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <sstream>
#include <string>

using boost::asio::ip::tcp;
using json = nlohmann::json;

TEST_CASE("Server: database user creation") {
    DataBase db1;
    std::string username = "mama";
    db1.create_user(username);
    REQUIRE(db1.check_user(username) == true);
}

TEST_CASE("Server: database check user") {
    DataBase db1;
    std::string username = "mama";
    REQUIRE(db1.check_user(username) == false);
}

TEST_CASE("Server: database message") {
    DataBase db1;
    std::string sender = "mama";
    std::string receiver = "mark";
    std::string text1 = "hello there";
    std::string text2 = "nice to meet you";

    std::string username = "mama";

    db1.accept_message(sender, receiver, text1);
    json reply = json::parse(R"({"mark":{"0":{"receiver":"mark","sender":"mama","text":"hello there"}}})");
    REQUIRE(db1.sync_messages(username) == reply);

    db1.accept_message(receiver, sender, text2);
    REQUIRE(db1.sync_messages(username)["mark"].size() == 2);
}


TEST_CASE("Server: login non-existed user") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 0));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        tcp::iostream stream(acceptor.local_endpoint());
        std::string request = "login mama\n";
        stream << request << std::flush;
        std::string s;
        std::getline(stream, s);
        REQUIRE(s == R"(404)");
    }};
    io_context.run();
    th.join();
}

TEST_CASE("Server: create user") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 0));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        tcp::iostream stream(acceptor.local_endpoint());
        std::string request = "signup mark123\n";
        stream << request << std::flush;
        std::string s;
        std::getline(stream, s);
        REQUIRE(s == R"(200)");
    }};

    io_context.run();
    th.join();
}

TEST_CASE("Server: get messages") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 0));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        tcp::iostream stream(acceptor.local_endpoint());
        std::string request = "get mark123\n";
        stream << request << std::flush;
        std::string s;
        std::getline(stream, s);
        REQUIRE(s == R"(null)");
    }};

    io_context.run();
    th.join();
}

TEST_CASE("Server: send message") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 0));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        tcp::iostream stream(acceptor.local_endpoint());
        std::string request = "send mark mike hello!! how are you?\n";
        stream << request << std::flush;
        std::string s;
        std::getline(stream, s);
        REQUIRE(s == R"(200)");
    }};

    io_context.run();
    th.join();
}

TEST_CASE("Client: is valid username") {
    
    REQUIRE(is_valid_username("mama") == true);
    REQUIRE(is_valid_username("mark 123") == false);
    REQUIRE(is_valid_username("mark+1") == false);
    REQUIRE(is_valid_username("maike.123") == false);
    REQUIRE(is_valid_username("john_smith") == true);
    REQUIRE(is_valid_username("mr_holmes and dr_watson") == false);

}


TEST_CASE("Client: create user") {
    
    REQUIRE(signup("mark 123") == false);

    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        std::string username = "mama";
        REQUIRE(signup(username) == true);
    }};

    io_context.run();
    th.join();
}

TEST_CASE("Client: login user") {
    
    REQUIRE(login("mark 123") == false);
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        std::string username = "mark";
        REQUIRE(login(username) == false);
    }};

    io_context.run();
    th.join();
}

TEST_CASE("Client: get messages") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        User u;
        u.username = "mama";
        REQUIRE(get_messages(u) == nullptr);
    }};

    io_context.run();
    th.join();
}

TEST_CASE("Client: send messages") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        std::string sender = "mama";
        std::string receiver = "mark";
        std::string text = "hello there";
        REQUIRE(send_reply(sender, receiver, text) == true);
    }};

    io_context.run();
    th.join();
}