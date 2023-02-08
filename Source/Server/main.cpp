#include <boost/lexical_cast/try_lexical_convert.hpp>

#include "server.hpp"
#include "Common/logger.hpp"

int main(int argc, char* argv[])
{
	namespace sev = boost::log::trivial;

	logger::init_log("server");
	logger::logger main_log;

	if (argc != 4)
	{
		BOOST_LOG_SEV(main_log, sev::error) << "Invalid number of parameters. \n"
			<< "\t Usage: server.exe <address> <port_number> <dump_period> \n"
			<< "\t\t example: \"server.exe 0.0.0.0 8033 30\"";
		return 1;
	}

	unsigned port_number = 0;
	if (!boost::conversion::try_lexical_convert(argv[2], port_number))
	{
		BOOST_LOG_SEV(main_log, sev::error) << "failed to parse port_number";
		return 1;
	}

	unsigned dump_period = 0;
	if (!boost::conversion::try_lexical_convert(argv[3], dump_period))
	{
		BOOST_LOG_SEV(main_log, sev::error) << "failed to parse dump_period";
		return 1;
	}

	try
	{
		server::server s(argv[1], port_number, dump_period);
		s.run();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_SEV(main_log, sev::error) << "Exception: " << e.what();
	}

	return 0;
}
