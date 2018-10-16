#pragma once

#include <cstddef>
#include <memory>
#include <boost/asio.hpp>
#include "command_reader.h"
#include "bulk_handler.h"
#include "command_processors.h"

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, BulkHandler& handler);

    void start();

private:
    void do_read();

private:
    CommandReader m_reader;

    static const size_t max_length = 128;
    tcp::socket m_socket;
    char m_data[max_length];
};

class server
{
public:
    explicit server(unsigned short port, size_t bulkSize);

private:
    void do_accept();

private:
    BulkHandler m_handler;
    CommandProcessor m_processor;
    CommandMultipleLog m_log;

    boost::asio::io_service m_service;
    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
};