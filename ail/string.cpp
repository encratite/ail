#include <ail/string.hpp>
#include <ail/array.hpp>

namespace ail
{
	bool is_alpha(char input)
	{
		return (input >= 'A' && input <= 'Z') || (input >= 'a' && input <= 'z');
	}

	bool is_digit(char input)
	{
		return input >= '0' && input <= '9';
	}

	bool is_oct_digit(char input)
	{
		return input >= '0' && input <= '7';
	}

	bool is_hex_digit(char input)
	{
		return is_digit(input) || (input >= 'a' && input <= 'f') || (input >= 'A' && input <= 'F');
	}

	bool is_whitespace(char input)
	{
		return input == '\t' || input == '\n' || input == '\r' || input == ' ';
	}

	char to_upper(char input)
	{
		char output = input;
		if(output >= 'a' && output <= 'z')
			output -= 32;
		return output;
	}

	char to_lower(char input)
	{
		char output = input;
		if(output >= 'A' && output <= 'Z')
			output += 32;
		return output;
	}

	std::string to_upper(std::string const & input)
	{
		std::string output = input;
		for(std::string::iterator i = output.begin(), end = output.end(); i != end; ++i)
		{
			char & current_char = *i;
			current_char = to_upper(current_char);
		}
		return output;
	}

	std::string to_lower(std::string const & input)
	{
		std::string output = input;
		for(std::string::iterator i = output.begin(), end = output.end(); i != end; ++i)
		{
			char & current_char = *i;
			current_char = to_lower(current_char);
		}
		return output;
	}

	std::string left_trim(std::string const & input)
	{
		std::string output = input;
		std::string::iterator
			begin = output.begin(),
			end = output.end();

		for(;; ++begin)
		{
			if(begin >= end)
				return "";

			if(is_whitespace(*begin) == false)
				break;
		}

		output.erase(output.begin(), begin);

		return output;
	}

	std::string right_trim(std::string const & input)
	{
		std::string output = input;
		std::string::iterator
			end = output.end();

		for(end -= 1; is_whitespace(*end); --end);

		output.erase(end + 1, output.end());

		return output;
	}

	std::string trim(std::string const & input)
	{
		std::string output = input;
		std::string::iterator
			begin = output.begin(),
			end = output.end();

		for(;; ++begin)
		{
			if(begin >= end)
				return "";

			if(is_whitespace(*begin) == false)
				break;
		}

		for(end -= 1; is_whitespace(*end); --end);

		std::size_t offset_begin = begin - output.begin();
		std::size_t offset_end = end - output.begin();
		output = output.substr(offset_begin, offset_end - offset_begin + 1);

		return output;
	}

	unsigned long count_strings(std::string const & input, std::string const & target)
	{
		if(input.empty() == true)
			return 0;

		if(target.empty() == true)
			throw exception("Empty target");

		unsigned long output = 0;

		std::size_t target_length = target.length();
		std::size_t result = input.find(target);

		while(result != std::string::npos)
		{
			++output;
			result = input.find(target, result + target_length);
		}

		return output;
	}

	std::string erase_string(std::string const & input, std::string const & target)
	{
		std::stringstream output;

		if(target.empty())
			return input;

		std::size_t
			target_length = target.length(),
			offset = input.find(target),
			last_offset = 0;

		while(offset != std::string::npos)
		{
			output << input.substr(last_offset, offset - last_offset);
			offset += target_length;
			last_offset = offset;
			offset = input.find(target, offset);
		}

		output << input.substr(last_offset);

		return output.str();
	}

	std::string replace_string(std::string const & input, std::string const & target, std::string const & replacement)
	{
		std::string output = input;

		if(target.empty() == true)
			throw exception("Empty target");

		if(target == replacement)
			return input;

		std::size_t target_length = target.length();
		std::size_t replacement_length = replacement.length();

		std::size_t result = output.find(target);

		while(result != std::string::npos)
		{
			output.replace(result, target_length, replacement);
			result = output.find(target, result);
		}

		return output;
	}

	/*
	void tokenise(std::string const & input, string_vector & output, char delimiter)
	{
		std::string token;
		std::istringstream stream(input);
		while(std::getline(stream, token, delimiter))
				output.push_back(token);
	}
	*/

	void tokenise(std::string const & input, std::string const & delimiter, string_vector & output)
	{
		std::size_t
			delimiter_length = delimiter.length(),
			left = 0,
			right = input.find(delimiter),
			count = 1;

		while(right != std::string::npos)
		{
			std::string new_token(input.substr(left, right - left));
			left = right + delimiter_length;
			right = input.find(delimiter, left);
			count++;
		}

		left = 0;
		right = input.find(delimiter);

		output.reserve(count);

		while(right != std::string::npos)
		{
			std::string new_token(input.substr(left, right - left));
			output.push_back(new_token);
			left = right + delimiter_length;
			right = input.find(delimiter, left);
		}

		output.push_back(input.substr(left));
	}

	string_vector tokenise(std::string const & input, std::string const & delimiter)
	{
		string_vector output;
		tokenise(input, delimiter, output);
		return output;
	}

	std::string join_strings(std::vector<std::string> const & input, std::string const & delimiter)
	{
		std::string output;
		std::vector<std::string>::const_iterator
			i = input.begin(),
			end = input.end();
		if(i != end)
		{
			output = *i;
			for(++i; i != end; ++i)
			{
				output += delimiter + *i;
			}
		}
		return output;
	}

	bool string_to_bool(std::string const & input, bool & output)
	{
		if(input == "true" || input == "1")
		{
			output = true;
			return true;
		}
		else if(input == "false" || input == "0")
		{
			output = false;
			return true;
		}

		return false;
	}

	std::string bool_to_string(bool input)
	{
		return input ? "true" : "false";
	}

	bool wildcard_match(std::string const & target, std::string const & wildcard_string)
	{
		std::string::const_iterator target_iterator = target.begin(), target_end = target.end();
		for(std::string::const_iterator i = wildcard_string.begin(), end = wildcard_string.end(); i != end;	++i, ++target_iterator)
		{
			char current_char = *i;
			switch(current_char)
			{
				case '*':
				{
					++i;
					if(i == end)
						return true;
					char next_char = *i;
					while(true)
					{
						if(target_iterator == target_end)
							return false;
						if(*target_iterator == next_char)
							break;
						++target_iterator;
					}
					break;
				}

				case '?':
				{
					if(target_iterator == target_end)
						return false;
					break;
				}

				default:
				{
					if(target_iterator == target_end)
						return false;
					if(current_char != *target_iterator)
						return false;
					break;
				}
			}
		}
		return (target_iterator == target_end);
	}

	std::string consolify(std::string const & input)
	{
		std::string output = input;
		output = replace_string(output, "\b", "\\b");
		output = replace_string(output, "\t", "\\t");
		output = replace_string(output, "\n", "\\n");
		output = replace_string(output, "\v", "\\v");
		output = replace_string(output, "\f", "\\f");
		output = replace_string(output, "\r", "\\r");
		output = replace_string(output, "\x07", "\\x07");
		return output;
	}

	std::size_t locate_string(std::string const & input, std::string const & target, long occurence, std::size_t offset)
	{
		for(; occurence > 0; occurence--)
		{
			offset = input.find(target, offset + 1);
			if(offset == std::string::npos)
				break;
		}
		return offset;
	}

	bool extract_string(std::string const & input, std::string const & left_target, std::string const & right_target, std::string & output, long occurence)
	{
		std::size_t left_offset = locate_string(input, left_target, occurence, 0);
		if(left_offset == std::string::npos)
			return false;
		left_offset += left_target.length();
		std::size_t right_offset = input.find(right_target, left_offset);
		if(right_offset == std::string::npos)
			return false;
		output = input.substr(left_offset, right_offset - left_offset);
		return true;
	}

	string_vector extract_strings(std::string const & input, std::string const & left_target, std::string const & right_target)
	{
		string_vector output;
		std::size_t last_offset = 0;
		while(true)
		{
			std::size_t left_offset = input.find(left_target, last_offset);
			if(left_offset == std::string::npos)
				break;
			left_offset += left_target.length();
			std::size_t right_offset = input.find(right_target, left_offset);
			if(right_offset == std::string::npos)
				break;
			std::string element = input.substr(left_offset, right_offset - left_offset);
			output.push_back(element);
			last_offset = right_offset + right_target.length();
		}
		return output;
	}

	bool split_string(std::string const & input, std::string const & delimiter, std::string & left_string, std::string & right_string)
	{
		std::size_t offset = input.find(delimiter);
		if(offset == std::string::npos)
		{
			left_string = input;
			right_string.clear();
			return false;
		}
		left_string = input.substr(0, offset);
		std::size_t right_offset = offset + delimiter.length();
		right_string = input.substr(right_offset);
		return true;
	}

	std::string get_size_string(ullong size)
	{
		std::string const size_strings[] =
		{
			"byte",
			"KiB",
			"MiB",
			"GiB",
			"TiB"
		};

		double adjusted_size = static_cast<double>(size);
		ullong const base = 1024;
		double const divisor = static_cast<double>(base);
		std::size_t size_index = 0;
		for(std::size_t maximum = ail::countof(size_strings) - 1; adjusted_size > divisor && size_index < maximum; adjusted_size /= base, size_index++);

		std::stringstream stream;
		stream << std::fixed;

		if(size_index > 0)
		{
			stream.precision(2);
			stream << adjusted_size << " " << size_strings[size_index];
			return stream.str();
		}
		else
		{
			stream << size << " " << size_strings[size_index]; 
			std::string output = stream.str();
			if(size != 1)
				output += "s";
			return output;
		}
	}

	ulong binary_string_to_number(std::string const & input, bool big_endian)
	{
		ulong output = 0;

		std::size_t
			initial_value,
			limit,
			direction;

		if(big_endian)
		{
			initial_value = 0;
			limit = input.size();
			direction = 1;
		}
		else
		{
			initial_value = input.size() - 1;
			limit = -1;
			direction = -1;
		}

		for(std::size_t i = initial_value; i != limit; i += direction)
		{
			output <<= 1;
			output |= input[i] & 1;
		}

		return output;
	}

	std::string hex_string(ullong input, std::size_t bytes, bool prefix = true)
	{
		std::stringstream output;
		if(prefix)
			output << "0x";
		output.width(2 * bytes);
		output.fill('0');
		output << std::hex << input;
		return output.str();
	}

	std::string hex_string_8(ulong input)
	{
		return hex_string(input, 1);
	}

	std::string hex_string_16(ulong input)
	{
		return hex_string(input, 2);
	}

	std::string hex_string_32(ulong input)
	{
		return hex_string(input, 4);
	}

	std::string hex_string_64(ullong input)
	{
		return hex_string(input, 8);
	}

	std::string hex_string(std::string const & input)
	{
		uchar const * data = reinterpret_cast<uchar const *>(input.c_str());
		std::string output;
		bool first = true;
		for(std::size_t i = 0, end = input.size(); i < end; i++)
		{
			if(first)
				first = false;
			else
				output += " ";
			output += hex_string(data[i], 1, false);
		}
		return output;
	}
}

std::string operator+(char left, std::string const & right)
{
	std::string string;
	string += left;
	return string + right;
}

std::string operator+(char const * left, std::string const & right)
{
	return std::string(left) + right;
}
