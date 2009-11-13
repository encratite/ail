#include <ail/memory.hpp>
#include <ail/string.hpp>

#include <boost/foreach.hpp>

namespace ail
{
	std::size_t const bits_per_byte = 8;

	bit_reader::bit_reader(std::string const & data, bool high_to_low_order):
		offset(0),
		big_endian(true)
	{
		std::size_t
				initial_value,
				limit,
				direction;

		if(high_to_low_order)
		{
			initial_value = bits_per_byte - 1;
			limit = -1;
			direction = -1;
		}
		else
		{
			initial_value = 0;
			limit = bits_per_byte;
			direction = 1;
		}

		BOOST_FOREACH(char i, data)
		{
			for(std::size_t bit = initial_value; bit != limit; bit += direction)
				bit_stream.push_back(static_cast<char>((i >> bit) & 1));
		}
	}

	ulong bit_reader::read(std::size_t bit_count)
	{
		std::size_t new_offset = offset + bit_count;
		if(new_offset > bit_stream.size())
			throw exception("Reading bit stream data out of bounds");
		std::string binary_string = bit_stream.substr(offset, bit_count);
		ulong output = binary_string_to_number(binary_string);
		offset = new_offset;
		return output;
	}

	ulong bit_reader::read_bytes(std::size_t byte_count)
	{
		return read(bits_per_byte * byte_count);
	}

	bool bit_reader::read_bool()
	{
		return read(1) == 1;
	}

	std::size_t bit_reader::get_offset() const
	{
		return offset;
	}

	void bit_reader::set_offset(std::size_t new_offset)
	{
		if(new_offset > bit_stream.size())
			throw exception("New bit stream offset is out of bounds");
		offset = new_offset;
	}

	std::string bit_reader::string(std::size_t length, std::size_t character_size)
	{
		std::string output;
		for(std::size_t i = 0; i < length; i++)
			output.push_back(static_cast<char>(read(character_size)));
		return output;
	}

	ulong read_big_endian(char const * input, std::size_t size, std::size_t offset)
	{
		unsigned char const * unsigned_input = reinterpret_cast<unsigned char const *>(input + offset);
		ulong output = 0;
		if(size)
		{
			for(unsigned char const * i = unsigned_input, * end = unsigned_input + size; i < end; ++i)
			{
				output <<= bits_per_byte;
				output |= *i;
			}
		}
		return output;
	}

	ulong read_little_endian(char const * input, std::size_t size, std::size_t offset)
	{
		unsigned char const * unsigned_input = reinterpret_cast<unsigned char const *>(input + offset);
		ulong output = 0;
		if(size)
		{
			for(unsigned char const * i = unsigned_input + size - 1, * end = unsigned_input; i >= end; --i)
			{
				output <<= bits_per_byte;
				output |= static_cast<ulong>(*i);
			}
		}
		return output;
	}

	ulong read_big_endian_bits(char const * data, std::size_t bit_count, std::size_t offset)
	{
		ulong output = 0;
		for(std::size_t i = 0; i < bit_count; i++)
		{
			output <<= 1;
			output |= get_bit(data, offset + i);
		}
		return output;
	}

	ulong read_little_endian_bits(char const * data, std::size_t bit_count, std::size_t offset)
	{
		ulong output = 0;
		for(std::size_t i = 0; i < bit_count; i++)
			output |= get_bit(data, offset + i) << i;
		return output;
	}

	void write_big_endian(ulong input, char * output, std::size_t size)
	{
		if(size)
		{
			for(char * i = output + size - 1; i >= output; --i)
			{
				*i = (char)input;
				input >>= bits_per_byte;
			}
		}
	}

	void write_little_endian(ulong input, char * output, std::size_t size)
	{
		if(size)
		{
			for(char * i = output, * end = output + size; i < end; ++i)
			{
				*i = (char)input;
				input >>= bits_per_byte;
			}
		}
	}

	std::string big_endian_string(ulong input, std::size_t size)
	{
		char * buffer = new char[size];
		write_big_endian(input, buffer, size);
		std::string output = std::string(buffer, size);
		delete buffer;
		return output;
	}

	std::string little_endian_string(ulong input, std::size_t size)
	{
		char * buffer = new char[size];
		write_little_endian(input, buffer, size);
		std::string output = std::string(buffer, size);
		delete buffer;
		return output;
	}

	ulong get_bit(char const * data, std::size_t offset)
	{
		std::size_t byte = offset / bits_per_byte;
		std::size_t bit = offset % bits_per_byte;
		return (data[byte] >> bit) & 1;
	}
}
