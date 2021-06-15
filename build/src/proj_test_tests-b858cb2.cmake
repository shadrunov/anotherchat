add_test( [==[Something works]==] /home/alex/Documents/asio-example-main/build/bin/proj_test [==[--test-case=Something works]==])
set_tests_properties( [==[Something works]==] PROPERTIES WORKING_DIRECTORY /home/alex/Documents/asio-example-main/build/src)
set( proj_test_TESTS [==[Something works]==])
