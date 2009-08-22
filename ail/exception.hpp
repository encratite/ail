#pragma once

#include <stdexcept>

namespace ail
{
	class exception: public std::exception
	{
	public:
		exception();
		exception(std::string const & message);
		~exception() throw ();

		std::string const & get_message() const;

	private:
		std::string message;
	};
}
