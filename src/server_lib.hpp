/**
 * \file
 * \brief File defines chat server
**/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <nlohmann/json.hpp>


class DataBase {
  /**
   * stores and process messages and users
  **/
  public:
    /**
    Checks if user exists on server
    \param username given username
    \return true if account exists, false otherwise
    **/
    bool check_user(std::string &username);

    /**
    Creates user on server
    \param username new username
    \return true if created, false if existed already
    **/
    bool create_user(std::string &username);

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
    auto sync_messages(std::string &username) -> nlohmann::json;

    /**
    Accepts message from client, calculates id and saves to database
    \param sender username of current user
    \param receiver username of recipient
    \param text string with encrypted text of the message
    \return true if successful
    **/
    bool accept_message(std::string &sender, std::string &receiver, std::string &text);

    /**
     * prints out messages
    **/
    void print_messages();

  private:
    std::vector<std::string> users;  ///< vector with users
    nlohmann::json messages;  ///< json to store all the messages
};



template <typename Class, typename Function>
auto delegate(std::shared_ptr<Class> ptr, Function fun) {
    // return [ptr = std::move(ptr), fun]() {
    return [ ptr, fun ]<typename... Args>(Args && ... arg) {
        return (ptr.get()->*fun)(std::forward<Args>(arg)...);
    };
}



class session : public std::enable_shared_from_this<session> {
  public:
    explicit session(boost::asio::io_context &io_context, boost::asio::ip::tcp::socket t_socket)
        : socket(std::move(t_socket)), timer(io_context),
          strand(io_context.get_executor()) {}

    void go();

    void timer_callback(boost::asio::yield_context yield) {
        while (socket.is_open()) {
            boost::system::error_code ignored_ec;
            timer.async_wait(yield[ignored_ec]);
            if (timer.expires_from_now() <= std::chrono::seconds(0))
                socket.close();
        }
    }

  private:
    boost::asio::ip::tcp::socket socket;
    boost::asio::steady_timer timer;
    boost::asio::strand<boost::asio::io_context::executor_type> strand;
};

