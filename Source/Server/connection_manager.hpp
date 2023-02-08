#pragma once

#include <set>
#include <boost/noncopyable.hpp>

#include "connection.hpp"

namespace server {

	class connection_manager
		: private boost::noncopyable
	{
	public:
		void start(connection_ptr conn);
		void stop(connection_ptr conn);
		void stop_all();

	private:
		std::set<connection_ptr> connections_;
	};

}
