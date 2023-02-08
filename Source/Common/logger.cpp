#include "logger.hpp"

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace logger {

	namespace logging = boost::log;
	namespace keywords = boost::log::keywords;

	void init_log(std::string app_name)
	{
		logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
		logging::add_common_attributes();

		logging::add_console_log(std::clog, keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
		logging::add_file_log(
			keywords::file_name = "TestResults/" + app_name + "_%N.log", keywords::rotation_size = 10 * 1024 * 1024,
			keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),
			keywords::auto_flush = true, keywords::format = "[%TimeStamp%][thread_id=%ThreadID%][%Severity%]: %Message%");
	}

}
