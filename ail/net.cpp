#include <ail/net.hpp>
#include <ail/string.hpp>

#include <sstream>

//#include <boost/format.hpp>
#include <boost/foreach.hpp>

namespace ail
{
	void extract_data(boost::asio::streambuf & stream_buffer, std::string & buffer)
	{
		std::size_t size = stream_buffer.in_avail();
		char * char_buffer = new char[size];
		stream_buffer.sgetn(char_buffer, size);
		buffer.append(char_buffer, size);
		delete[] char_buffer;
	}

	std::string extract_data(boost::asio::streambuf & stream_buffer)
	{
		std::string output;
		extract_data(stream_buffer, output);
		return output;
	}

	std::string convert_ipv4(ulong input)
	{
		std::stringstream output;
		bool first = true;
		for(long i = 3; i >= 0; i--)
		{
			if(first)
				first = false;
			else
				output << ".";
			output << ((input >> (i * 8)) & 0xff);
		}
		return output.str();
	}

	bool convert_ipv4(std::string const & input, ulong & output)
	{
		std::size_t const token_count = 4;

		string_vector tokens = tokenise(input, ".");
		if(tokens.size() != token_count)
			return false;

		output = 0;

		for(std::size_t i = 0; i < token_count; i++)
		{
			ulong value;
			if(!string_to_number<ulong>(tokens[i], value))
				return false;
			if(value < 0 || value > 0xff)
				return false;
			output <<= 8;
			output |= value;
		}

		return true;
	}

	std::string get_ipv6_unit(std::string const & input)
	{
		if(input[0] == 0 && input[1] == 0)
			return "";
		std::stringstream output;
		BOOST_FOREACH(char i, input)
		{
			output.width(2);
			output.fill('0');
			output << std::hex;
			output << static_cast<ulong>(static_cast<unsigned char>(i));
		}
		return output.str();
	}

	std::string convert_ipv6(std::string const & input)
	{
		if(input.size() != 16)
			throw exception("Invalid IPv6 address input");
		bool first = true;
		std::string output;
		for(std::size_t i = 0; i < input.size(); i += 4)
		{
			std::string unit = input.substr(i, 4);
			if(first)
				first = false;
			else
				output += ":";
			output += get_ipv6_unit(unit);
		}
		return output;
	}

	ulong convert_ipv4(std::string const & input)
	{
		ulong output;
		if(!convert_ipv4(input, output))
			throw exception("Failed to convert IPv4 address");
		return output;
	}
}
