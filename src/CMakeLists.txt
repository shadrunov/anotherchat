cmake_minimum_required(VERSION 3.10.0)

add_executable(proj_test test.cpp lib.cpp)
# ^ Взять test.cpp и lib.cpp, собрать вместе, назвать proj_test (proj_test.exe)
target_link_libraries(proj_test PUBLIC doctest::doctest default)
# ^ Использовать в proj_test библиотеки doctest::doctest и default
doctest_discover_tests(proj_test)
# ^ Прочитать из proj_test список тестов

add_executable(proj main.cpp lib.cpp)
target_link_libraries(proj PUBLIC doctest::doctest default Boost::system Boost::coroutine CONAN_PKG::fmt CONAN_PKG::nlohmann_json)
# fix error on Windows
target_link_directories(proj PUBLIC "${Boost_LIBRARY_DIRS}")

add_executable(client client.cpp)
target_link_libraries(client PUBLIC easy-encryption doctest::doctest default Boost::system Boost::coroutine CONAN_PKG::fmt CONAN_PKG::nlohmann_json)

add_executable(j j.cpp)
target_link_libraries(j PUBLIC easy-encryption doctest::doctest default Boost::system Boost::coroutine CONAN_PKG::fmt CONAN_PKG::nlohmann_json)
