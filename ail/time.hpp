#ifndef AIL_TIME_HPP
#define AIL_TIME_HPP

#include <string>

#include <ail/types.hpp>

namespace ail
{
	ulong time();
	ullong milliseconds();
	std::string timestamp();
	ullong boot_time();
}

#endif
