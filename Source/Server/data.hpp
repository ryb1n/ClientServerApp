#pragma once

#include <mutex>
#include <unordered_set>
#include <boost/noncopyable.hpp>

namespace server {

	struct data
		: private boost::noncopyable
	{
		data() { data_set_.reserve(1024); }

		mutable std::mutex mutex_;
		std::unordered_set<unsigned> data_set_;
		unsigned sum_squares = 0u;

	};

}
