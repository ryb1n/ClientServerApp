#pragma once

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

namespace logger {

	void init_log(std::string app_name);

	class logger :
		public boost::log::sources::severity_logger<boost::log::trivial::severity_level>
	{
	public:
		logger() = default;
	};

}