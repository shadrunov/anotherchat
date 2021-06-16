cmake_minimum_required(VERSION 3.10.0)

add_executable(server server.cpp server_lib.cpp)
target_link_libraries(server PUBLIC doctest::doctest default Boost::system Boost::coroutine CONAN_PKG::fmt CONAN_PKG::nlohmann_json)
target_link_directories(server PUBLIC "${Boost_LIBRARY_DIRS}")

add_executable(client client.cpp client_lib.cpp)
target_link_libraries(client PUBLIC easy-encryption doctest::doctest default Boost::system Boost::coroutine CONAN_PKG::fmt CONAN_PKG::nlohmann_json)

# add_executable(j j.cpp)
# target_link_libraries(j PUBLIC easy-encryption doctest::doctest default Boost::system Boost::coroutine CONAN_PKG::fmt CONAN_PKG::nlohmann_json)

add_executable(proj_test test.cpp client_lib.cpp server_lib.cpp)
target_link_libraries(proj_test PUBLIC easy-encryption doctest::doctest default Boost::system Boost::coroutine CONAN_PKG::fmt CONAN_PKG::nlohmann_json)
doctest_discover_tests(proj_test)

add_executable(test_chain test_chain.cpp client_lib.cpp server_lib.cpp)
target_link_libraries(test_chain PUBLIC easy-encryption doctest::doctest default Boost::system Boost::coroutine CONAN_PKG::fmt CONAN_PKG::nlohmann_json)
doctest_discover_tests(test_chain)