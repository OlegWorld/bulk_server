#include "async.h"


session::session(tcp::socket socket, BulkHandler& handler)
        :   m_reader(handler),
            m_socket(std::move(socket))
{ }

void session::start() {
    do_read();
}

void session::do_read() {
    auto self(shared_from_this());
    m_socket.async_read_some(boost::asio::buffer(m_data, max_length),
                             [this, self](boost::system::error_code ec, std::size_t length)
                             {
                                 if (!ec) {
                                     m_reader.scan_input(std::string(m_data, length));
                                 }
                             });
}

server::server(unsigned short port, size_t bulkSize)
        :   m_handler(bulkSize),
            m_log(2),
            m_acceptor(m_service, tcp::endpoint(tcp::v4(), port)),
            m_socket(m_service)
{
    m_handler.subscribe(&m_processor);
    m_handler.subscribe(&m_log);

    while(!m_processor.ready());
    while(!m_log.ready());

    do_accept();

    m_service.run();
}

void server::do_accept() {
    m_acceptor.async_accept(m_socket,
                            [this](boost::system::error_code ec)
                            {
                                if (!ec)
                                {
                                    std::make_shared<session>(std::move(m_socket), m_handler)->start();
                                }

                                do_accept();
                            });
}
