#include "request_handler.hpp"

#include "reply.hpp"
#include "request.hpp"

namespace server {

	namespace sev = boost::log::trivial;

	request_handler::request_handler(data& server_data)
		: data_(server_data)
	{
	}

	void request_handler::handle_request(const request& req, reply& rep)
	{
		BOOST_LOG_SEV(logger_, sev::info) << "Handling request...";

		std::lock_guard guard(data_.mutex_);

		if (!data_.data_set_.contains(req.data_))
		{
			BOOST_LOG_SEV(logger_, sev::info) << "Requested number [" << req.data_
				<< "] is unique, persisting.";

			data_.data_set_.insert(req.data_);
			data_.sum_squares += req.data_ * req.data_;
		}
		else
		{
			BOOST_LOG_SEV(logger_, sev::info) << "Requested number [" << req.data_
				<< "] was already present.";
		}

		rep.data_ = data_.sum_squares / static_cast<unsigned>(data_.data_set_.size());
		BOOST_LOG_SEV(logger_, sev::info) << "Average sum squares is [" << rep.data_ << "]";
	}

}
