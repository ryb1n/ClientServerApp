#include "connection_manager.hpp"

#include <ranges>

namespace server {

	void connection_manager::start(connection_ptr conn)
	{
		connections_.insert(conn);
		conn->start();
	}

	void connection_manager::stop(connection_ptr conn)
	{
		connections_.erase(conn);
		conn->stop();
	}

	void connection_manager::stop_all()
	{
		std::ranges::for_each(connections_, std::mem_fn(&connection::stop));
		connections_.clear();
	}

}
