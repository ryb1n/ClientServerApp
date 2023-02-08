#pragma once 

#include <boost/noncopyable.hpp>

#include "data.hpp"
#include "common/logger.hpp"

namespace server {

	struct reply;
	struct request;

	class request_handler
		: private boost::noncopyable
	{
	public:
		explicit request_handler(data& server_data);

		void handle_request(const request& req, reply& rep);

	private:
		data& data_;

		logger::logger logger_;

	};

}
