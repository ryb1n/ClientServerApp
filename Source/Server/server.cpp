#include "server.hpp"

#include <filesystem>
#include <fstream>
#include <thread>
#include <boost/asio/signal_set.hpp>
#include <boost/bind/bind.hpp>

namespace server {

	namespace sev = boost::log::trivial;
	namespace ip = boost::asio::ip;

	server::server(const std::string& address, unsigned port, unsigned dump_period)
		: io_context_(),
		signals_(io_context_, SIGINT, SIGTERM),
		acceptor_(io_context_),
		connection_manager_(),
		new_connection_(new connection(io_context_, connection_manager_, request_handler_)),
		request_handler_(data_),
		dump_period_(dump_period),
		timer_(io_context_, boost::posix_time::seconds(dump_period_))
	{
		BOOST_LOG_SEV(logger_, sev::trace) << "Starting waiting termination signals...";
		signals_.async_wait(boost::bind(&server::stop, this));
		BOOST_LOG_SEV(logger_, sev::trace) << "Waiting termination signals started";

		BOOST_LOG_SEV(logger_, sev::trace) << "Starting dumping data timer...";
		timer_.async_wait(boost::bind(&server::dump_data, this, boost::asio::placeholders::error));
		BOOST_LOG_SEV(logger_, sev::trace) << "Dumping data timer started";

		ip::tcp::endpoint endpoint(ip::make_address_v4(address), port);
		acceptor_.open(endpoint.protocol());
		acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
		acceptor_.bind(endpoint);
		acceptor_.listen();
		BOOST_LOG_SEV(logger_, sev::trace) << "Starting listening [" << address << ":" << port << "]...";
		acceptor_.async_accept(new_connection_->socket(),
			boost::bind(&server::handle_accept, this, boost::asio::placeholders::error));
	}

	void server::run()
	{
		BOOST_LOG_SEV(logger_, sev::info) << "Starting server... Use ctrl+c to stop";

		constexpr auto thread_pool_size = 3u;

		std::vector<std::shared_ptr<std::thread>> threads;
		for (auto i = 0u; i < thread_pool_size; ++i)
		{
			std::shared_ptr<std::thread> thread(new std::thread(
				boost::bind(&boost::asio::io_context::run, &io_context_)));
			threads.push_back(thread);
		}

		for (auto i = 0u; i < threads.size(); ++i)
			threads[i]->join();
	}

	void server::stop()
	{
		BOOST_LOG_SEV(logger_, sev::info) << "Got stopping signal. Stopping server...";

		boost::asio::post(io_context_, boost::bind(&server::handle_stop, this));
	}

	void server::handle_accept(const boost::system::error_code& err)
	{
		if (!err)
		{
			BOOST_LOG_SEV(logger_, sev::info) << "Got new connection: "
				<< new_connection_->socket().remote_endpoint();

			connection_manager_.start(new_connection_);
			new_connection_.reset(new connection(io_context_, connection_manager_, request_handler_));
			acceptor_.async_accept(new_connection_->socket(),
				boost::bind(&server::handle_accept, this, boost::asio::placeholders::error));
		}
		else
		{
			BOOST_LOG_SEV(logger_, sev::error) << "Failed to accept new connection. "
				<< "Reason: " << err.message();
		}
	}

	void server::handle_stop()
	{
		acceptor_.close();
		connection_manager_.stop_all();
		timer_.cancel();
	}

	static std::string generate_dump_file_path()
	{
		return "TestResults/dump_" + std::to_string(std::time(nullptr)) + ".bin";
	}

	std::unordered_set<unsigned> server::get_data_for_dump()
	{
		BOOST_LOG_SEV(logger_, sev::info) << "Getting copy of data for dump...";

		std::lock_guard guard(data_.mutex_);

		return data_.data_set_;
	}

	void server::dump_data(const boost::system::error_code& err)
	{
		if (!err)
		{
			BOOST_LOG_SEV(logger_, sev::info) << "Dump data timer triggered";

			const auto file_name = generate_dump_file_path();
			std::ofstream file(file_name, std::ios::out | std::ios::binary);
			if (file.is_open())
			{
				BOOST_LOG_SEV(logger_, sev::info) << "Dump file successfully opened";

				const auto data = get_data_for_dump();

				const auto num_elements = data.size();
				file.write(reinterpret_cast<const char*>(&num_elements), sizeof(num_elements));

				for (auto&& elem : data)
					file.write(reinterpret_cast<const char*>(&elem), sizeof(elem));

				BOOST_LOG_SEV(logger_, sev::info) << "Data successfully dumped to the file";
			}
			else
			{
				BOOST_LOG_SEV(logger_, sev::error) << "Failed to create dump file " << file_name;
			}

			BOOST_LOG_SEV(logger_, sev::info) << "Extending dumping data timer for "
				<< dump_period_ << " seconds";
			timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(dump_period_));
			timer_.async_wait(boost::bind(&server::dump_data, this, boost::asio::placeholders::error));
		}
		else
		{
			BOOST_LOG_SEV(logger_, sev::error) << "Dumping data timer failed. "
				<< "Reason: " << err.message();
		}
	}

}
