#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "Common/logger.hpp"

namespace client {

    class client
    {
    public:
        explicit client(const std::string& host, const std::string& port);

        void run();
    private:
		void stop();

        void handle_resolve(const boost::system::error_code& err,
            boost::asio::ip::tcp::resolver::results_type results);
        void handle_connect(const boost::system::error_code& err);
        void handle_write(const boost::system::error_code& err);
        void handle_read(const boost::system::error_code& err);
		void handle_stop();

        void send_data();
           
        boost::asio::io_context io_context_;
        boost::asio::signal_set signals_;
        boost::asio::ip::tcp::resolver resolver_;
        boost::asio::ip::tcp::socket socket_;

        unsigned data_ = 0u;
        static constexpr auto data_range_ = std::pair(0u, 1023u);

        logger::logger logger_;

    };

}
