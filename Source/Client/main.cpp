#include <ctime>
#include <string>

#include "client.hpp"
#include "Common/logger.hpp"

int main(int argc, char* argv[])
{
	namespace sev = boost::log::trivial;

	logger::init_log("client_" + std::to_string(std::time(nullptr)));
	logger::logger main_log;

	if (argc != 3)
	{
		BOOST_LOG_SEV(main_log, sev::error) << "Invalid number of parameters. \n"
			<< "\t Usage: client.exe <host> <port_number> \n"
			<< "\t\t Example:  \"client.exe 127.0.0.1 8033\" \n";
		return 1;
	}

	try
	{
		client::client cl(argv[1], argv[2]);
		cl.run();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_SEV(main_log, sev::error) << "exception: " << e.what();
	}

	return 0;
}
