#pragma once

#include <string>

#include <ail/types.hpp>

namespace ail
{
	class stop_watch
	{
	public:
		stop_watch();
		void start();
		ullong stop();

	private:
		ullong beginning;
	};

	ulong time();
	ullong milliseconds();
	std::string timestamp();
	ullong boot_time();
}
