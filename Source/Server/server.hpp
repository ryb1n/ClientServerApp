#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "data.hpp"
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"
#include "Common/logger.hpp"

namespace server {

	class server
		: private boost::noncopyable
	{
	public:
		explicit server(const std::string& address, unsigned port,
			unsigned dump_period);

		void run();
		void stop();

	private:
		void handle_accept(const boost::system::error_code& err);
		void handle_stop();

		std::unordered_set<unsigned> get_data_for_dump();
		void dump_data(const boost::system::error_code& err);

		boost::asio::io_context io_context_;
		boost::asio::signal_set signals_;
		boost::asio::ip::tcp::acceptor acceptor_;

		connection_manager connection_manager_;
		request_handler request_handler_;

		connection_ptr new_connection_;

		data data_;

		unsigned dump_period_ = 0u;
		boost::asio::deadline_timer timer_;

		logger::logger logger_;

	};

}
