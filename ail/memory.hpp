#pragma once

#include <string>
#include <cstddef>
#include <ail/types.hpp>
#include <ail/exception.hpp>

namespace ail
{
	extern std::size_t const bits_per_byte;

	class bit_reader
	{
	public:
		bit_reader(std::string const & data, bool high_to_low_order = true);
		ulong read(std::size_t bit_count);
		ulong read_bytes(std::size_t byte_count);
		bool read_bool();
		std::size_t get_offset() const;
		void set_offset(std::size_t new_offset);
		std::string string(std::size_t length, std::size_t character_size = 8);

		bool big_endian;

	private:
		std::string bit_stream;
		std::size_t offset;
	};

	ulong read_big_endian(char const * input, std::size_t byte_count, std::size_t offset = 0);
	ulong read_little_endian(char const * input, std::size_t byte_count, std::size_t offset = 0);

	ulong read_little_endian_bits(char const * data, std::size_t bit_count, std::size_t offset = 0);
	ulong read_big_endian_bits(char const * data, std::size_t bit_count, std::size_t offset = 0);

	void write_big_endian(ulong input, char * output, std::size_t size);
	void write_little_endian(ulong input, char * output, std::size_t size);

	std::string big_endian_string(ulong input, std::size_t size);
	std::string little_endian_string(ulong input, std::size_t size);

	ulong get_bit(char const * data, std::size_t offset);
}
