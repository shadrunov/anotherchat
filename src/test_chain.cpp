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

TEST_CASE("RUN ALL TESTS TOGETHER") {
    REQUIRE(true == true);
}

TEST_CASE("1. create first user") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        std::string username = "alice";
        REQUIRE(signup(username) == true);
    }};

    io_context.run();
    th.join();
}

TEST_CASE("2. create second user") {
    
    REQUIRE(signup("mark 123") == false);

    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        std::string username = "bob";
        REQUIRE(signup(username) == true);
    }};

    io_context.run();
    th.join();
}

TEST_CASE("3. login first user") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        std::string username = "alice";
        REQUIRE(login(username) == true);
    }};

    io_context.run();
    th.join();
}

TEST_CASE("4. login second user") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        std::string username = "bob";
        REQUIRE(login(username) == true);
    }};

    io_context.run();
    th.join();
}

TEST_CASE("5. alice sends to bob") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        std::string sender = "alice";
        std::string receiver = "bob";
        std::string text = "hello bob!";
        REQUIRE(send_reply(sender, receiver, text) == true);
    }};

    io_context.run();
    th.join();
}

TEST_CASE("6. bob sends to alice") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        std::string sender = "bob";
        std::string receiver = "alice";
        std::string text = "hello alice!";
        REQUIRE(send_reply(sender, receiver, text) == true);
    }};

    io_context.run();
    th.join();
}

TEST_CASE("7. alice gets messages") {
    
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket(io_context);
    acceptor.async_accept(socket, [&](boost::system::error_code ec) {
        REQUIRE(!ec);
        std::make_shared<session>(io_context, std::move(socket))->go();
    });

    std::thread th{[&] {
        User u;
        u.username = "alice";
        REQUIRE(get_messages(u) == json::parse(R"({"bob":{"0":{"receiver":"bob","sender":"alice","text":"hello bob!"},"1":{"receiver":"alice","sender":"bob","text":"hello alice!"}}})"));
    }};

    io_context.run();
    th.join();
}


















