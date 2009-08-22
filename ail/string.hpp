#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <ios>
#include <cstring>

#include <ail/exception.hpp>
#include <ail/types.hpp>

namespace ail
{
	bool is_alpha(char input);
	bool is_digit(char input);
	bool is_oct_digit(char input);
	bool is_hex_digit(char input);
	bool is_whitespace(char input);

	char to_upper(char input);
	char to_lower(char input);

	std::string to_upper(std::string const & input);
	std::string to_lower(std::string const & input);

	std::string left_trim(std::string const & input);
	std::string right_trim(std::string const & input);
	std::string trim(std::string const & input);

	unsigned long count_strings(std::string const & input, std::string const & target);
	std::string erase_string(std::string const & input, std::string const & target);
	std::string replace_string(std::string const & input, std::string const & target, std::string const & replacement);

	void tokenise(std::string const & input, std::string const & delimiter, string_vector & output);
	string_vector tokenise(std::string const & input, std::string const & delimiter);

	std::string join_strings(std::vector<std::string> const & input, std::string const & delimiter);

	bool string_to_bool(std::string const & input, bool & output);
	std::string bool_to_string(bool input);

	bool wildcard_match(std::string const & target, std::string const & wildcard_string);

	std::string get_size_string(ullong size);

	std::string consolify(std::string const & input);

	template <class number_type>
		number_type string_to_number(std::string const & input, std::ios_base::fmtflags base = std::ios_base::dec)
	{
		number_type output;
		std::stringstream stream(input);
		stream.setf(base, std::ios_base::basefield);
		if((stream >> output) == 0)
			throw exception("String to number conversion failed");
		return output;
	}

	template <class number_type>
		bool string_to_number(std::string const & input, number_type & output, std::ios_base::fmtflags base = std::ios_base::dec)
	{
		std::stringstream stream(input);
		stream.setf(base, std::ios_base::basefield);
		if((stream >> output) == 0)
			return false;
		return true;
	}

	template <class number_type>
		std::string number_to_string(number_type input, std::ios_base::fmtflags base = std::ios_base::dec)
	{
		std::string output;
		std::stringstream stream;
		stream.setf(base, std::ios_base::basefield);
		stream << input;
		stream >> output;
		return output;
	}

	std::size_t locate_string(std::string const & input, std::string const & target, long occurence, std::size_t offset = 0);
	bool extract_string(std::string const & input, std::string const & left_target, std::string const & right_target, std::string & output, long occurence = 1);
	string_vector extract_strings(std::string const & input, std::string const & left_target, std::string const & right_target);
	bool split_string(std::string const & input, std::string const & delimiter, std::string & left_string, std::string & right_string);

	ulong binary_string_to_number(std::string const & input, bool big_endian = true);

	std::string hex_string_8(ulong input);
	std::string hex_string_16(ulong input);
	std::string hex_string_32(ulong input);
	std::string hex_string_64(ullong input);
	std::string hex_string(std::string const & input);
}

std::string operator+(char left, std::string const & right);
std::string operator+(char const * left, std::string const & right);
