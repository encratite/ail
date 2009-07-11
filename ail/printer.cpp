#include <ail/printer.hpp>

#include <iostream>

#include <boost/thread/mutex.hpp>

namespace
{
	boost::mutex console_mutex;
}

namespace ail
{

	void print(std::string const & input)
	{
		raw_print(input + "\n");
	}

	void raw_print(std::string const & input)
	{
		boost::mutex::scoped_lock scoped_lock(console_mutex);
		std::cout << input;
	}

}
