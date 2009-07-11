#include <ail/exception.hpp>

namespace ail
{
	exception::exception()
	{
	}

	exception::exception(std::string const & message):
		message(message)
	{
	}

	exception::~exception() throw ()
	{
	}

	std::string const & exception::get_message() const
	{
		return message;
	}
}
