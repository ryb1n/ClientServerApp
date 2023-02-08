#pragma once

#include <memory>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "Common/logger.hpp"

namespace server {

	class connection_manager;

	class connection
		: public std::enable_shared_from_this<connection>
		, private boost::noncopyable
	{
	public:
		explicit connection(boost::asio::io_context& io_context,
			connection_manager& conn_manager, request_handler& handler);

		boost::asio::ip::tcp::socket& socket();

		void start();
		void stop();

	private:
		void handle_read(const boost::system::error_code& err, std::size_t bytes_transferred);
		void handle_write(const boost::system::error_code& err);

		boost::asio::ip::tcp::socket socket_;

		connection_manager& connection_manager_;
		request_handler& request_handler_;

		request request_;
		reply reply_;

		logger::logger logger_;

	};

	using connection_ptr = std::shared_ptr<connection>;

}
