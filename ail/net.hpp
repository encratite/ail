#pragma once

#include <ail/types.hpp>

#include <boost/asio.hpp>

namespace ail
{
	void extract_data(boost::asio::streambuf & stream_buffer, std::string & buffer);
	std::string extract_data(boost::asio::streambuf & stream_buffer);

	std::string convert_ipv4(ulong input);
	bool convert_ipv4(std::string const & input, ulong & output);
	ulong convert_ipv4(std::string const & input);

	std::string convert_ipv6(std::string const & input);
}
