#include "client.hpp"

#include <random>
#include <thread>
#include <memory>
#include <format>

namespace client {

    namespace sev = boost::log::trivial;

    client::client(const std::string& host, const std::string& port)
        : io_context_(),
        signals_(io_context_, SIGINT, SIGTERM),
        resolver_(io_context_),
        socket_(io_context_)
    {
		signals_.async_wait(boost::bind(&client::stop, this));

        resolver_.async_resolve(host, port,
            boost::bind(&client::handle_resolve, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::results));
    }

    void client::run()
    {
        BOOST_LOG_SEV(logger_, sev::info) << "Starting client... Use ctrl+c to stop";

		const auto thread_pool_size = 2u;

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

    void client::stop()
    {
        BOOST_LOG_SEV(logger_, sev::info) << "Got stopping signal. Stopping client...";

        io_context_.post(boost::bind(&client::handle_stop, this));
    }

    void client::handle_resolve(const boost::system::error_code& err,
        boost::asio::ip::tcp::resolver::results_type results)
    {
        if (!err)
        {
			BOOST_LOG_SEV(logger_, sev::info) << "Successfully resolved host. "
				<< "Trying to connect...";

            boost::asio::async_connect(socket_, results,
                boost::bind(&client::handle_connect, this,
                    boost::asio::placeholders::error));
        }
        else
        {
			BOOST_LOG_SEV(logger_, sev::error) << "Failed to resolve host. "
				<< "Reason: " << err.message();
        }
    }

    static unsigned generate_number(const std::pair<unsigned, unsigned>& range)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        const auto& [min, max] = range;
        std::uniform_int_distribution dis(min, max);

        return dis(gen);
    }

    void client::handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            BOOST_LOG_SEV(logger_, sev::info) << "Successfully connected";

            send_data();
        }
        else
        {
			BOOST_LOG_SEV(logger_, sev::error) << "Failed to connect "
				<< "Reason: " << err.message();
        }
    }

    void client::handle_write(const boost::system::error_code& err)
    {
        if (!err)
        {
            BOOST_LOG_SEV(logger_, sev::info) << "Successfully sent request. "
                << "Waiting for response";

            boost::asio::async_read(socket_,
                boost::asio::buffer(&data_, sizeof(data_)),
                boost::bind(&client::handle_read, this,
                    boost::asio::placeholders::error));
        }
        else
        {
			BOOST_LOG_SEV(logger_, sev::error) << "Failed to send data. "
                << "Reason: " << err.message();
        }
    }

    void client::handle_read(const boost::system::error_code& err)
    {
		if (!err)
		{
            BOOST_LOG_SEV(logger_, sev::info) << "Successfully got response. "
                << "Result: " << data_;
            
            send_data();
		}
		else
		{
			BOOST_LOG_SEV(logger_, sev::error) << "Failed to receive data. "
				<< "Reason: " << err.message();
		}
    }

	void client::handle_stop()
	{
        if (socket_.is_open())
        {
            boost::system::error_code err;
			socket_.close(err);
            
            if (err)
            {
				BOOST_LOG_SEV(logger_, sev::error) << "Failed to close socket. "
					<< "Reason: " << err.message();
            }
        }

        BOOST_LOG_SEV(logger_, sev::info) << "Client stopped.";
	}

    void client::send_data()
    {
		data_ = generate_number(data_range_);

        BOOST_LOG_SEV(logger_, sev::info) << "Generated number is " << data_
		    << ". Sending request...";

		boost::asio::async_write(socket_,
			boost::asio::buffer(&data_, sizeof(data_)),
			boost::bind(&client::handle_write, this,
				boost::asio::placeholders::error));
    }
}
