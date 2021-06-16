#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <nlohmann/json.hpp>


class DataBase {
  public:
    bool check_user(std::string &username);

    bool create_user(std::string &username);

    auto sync_messages(std::string &username) -> nlohmann::json;

    bool accept_message(std::string &sender, std::string &receiver, std::string &text);

    void print_messages();

  private:
    std::vector<std::string> users;
    nlohmann::json messages;

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

