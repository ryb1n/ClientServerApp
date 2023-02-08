#include "connection.hpp"

#include <boost/bind/bind.hpp>

#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace server {

	namespace sev = boost::log::trivial;

	connection::connection(boost::asio::io_context& io_context,
		connection_manager& conn_manager, request_handler& handler)
		: socket_(io_context),
		connection_manager_(conn_manager),
		request_handler_(handler)
	{
	}

	boost::asio::ip::tcp::socket& connection::socket()
	{
		return socket_;
	}

	void connection::start()
	{
		BOOST_LOG_SEV(logger_, sev::info) << "Waiting for receive data...";

		socket_.async_read_some(boost::asio::buffer(&request_.data_, sizeof(request_.data_)),
			boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void connection::stop()
	{
		socket_.close();
	}

	void connection::handle_read(const boost::system::error_code& err,
		std::size_t bytes_transferred)
	{
		if (!err)
		{
			BOOST_LOG_SEV(logger_, sev::info) << "Data successfully received: " << request_.data_;

			request_handler_.handle_request(request_, reply_);

			boost::asio::async_write(socket_,
				boost::asio::buffer(&reply_.data_, sizeof(reply_.data_)),
				boost::bind(&connection::handle_write, shared_from_this(),
					boost::asio::placeholders::error));
		}
		else if (err != boost::asio::error::operation_aborted)
		{
			BOOST_LOG_SEV(logger_, sev::error) << "Failed to receive data. "
				<< "Reason: " << err.message();

			connection_manager_.stop(shared_from_this());
		}
	}

	void connection::handle_write(const boost::system::error_code& err)
	{
		if (!err)
		{
			BOOST_LOG_SEV(logger_, sev::info) << "Data successfully sent: " << request_.data_;

			start();
		}
		else if (err != boost::asio::error::operation_aborted)
		{
			BOOST_LOG_SEV(logger_, sev::error) << "Failed to sent data. "
				<< "Reason: " << err.message();

			connection_manager_.stop(shared_from_this());
		}
	}

}
