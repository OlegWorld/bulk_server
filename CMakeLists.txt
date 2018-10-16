cmake_minimum_required(VERSION 3.9)
project(bulk_server)

find_package(GTest REQUIRED)
find_package (Threads REQUIRED)
find_package(Boost 1.58 REQUIRED COMPONENTS system)

add_executable(bulk_server main.cpp async.cpp async.h
        command_reader.cpp command_reader.h
        command_processors.cpp command_processors.h
        command_bulk.cpp command_bulk.h
        thread_utility.h abstract_observer.h bulk_handler.h bulk_handler.cpp)

add_executable(bulk_server_test test.cpp async.cpp async.h
        command_reader.cpp command_reader.h
        command_processors.cpp command_processors.h
        command_bulk.cpp command_bulk.h
        thread_utility.h abstract_observer.h bulk_handler.h bulk_handler.cpp)

target_include_directories(bulk_server_test PRIVATE ${GTEST_INCLUDE_DIRS})

set_target_properties(bulk_server PROPERTIES   CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra")

set_target_properties(bulk_server_test PROPERTIES CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON)

target_link_libraries(bulk_server ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} stdc++fs)

target_link_libraries(bulk_server_test ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} stdc++fs)

add_test(test bulk_server_test)

enable_testing()

#install(TARGETS async LIBRARY DESTINATION lib)
#
#set(CPACK_GENERATOR DEB)
#
#set(CPACK_PACKAGE_VERSION_MAJOR "0")
#set(CPACK_PACKAGE_VERSION_MINOR "1")
#set(CPACK_PACKAGE_VERSION_PATCH "$ENV{TRAVIS_BUILD_NUMBER}")
#
#set(CPACK_PACKAGE_CONTACT osmironov@gmail.com)
#
#include(CPack)