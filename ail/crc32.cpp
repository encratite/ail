#include <ail/crc32.hpp>
#include <ail/array.hpp>

namespace ail
{
	namespace
	{
		ulong crc32_table[256];

		struct initialise_crc32_table
		{
			initialise_crc32_table()
			{
				char const polynomial_terms[] =
				{
					 0,  1,  2,  4,  5,  7,  8,
					10, 11, 12, 16, 22,	23, 26
				};

				ulong polynomial_pattern = 0;
				for(std::size_t i = 0; i < sizeof(polynomial_terms); ++i)
				{
					ulong shifts = 31 - polynomial_terms[i];
					polynomial_pattern |= 1 << shifts;
				}

				for(std::size_t i = 0; i < ail::countof(crc32_table); ++i)
				{
					ulong new_table_entry = static_cast<ulong>(i);
					for(int j = 0; j < 8; ++j)
					{
						bool first_bit_was_set = (new_table_entry & 1);
						new_table_entry >>= 1;
						if(first_bit_was_set == true)
							new_table_entry ^= polynomial_pattern;
						crc32_table[i] = new_table_entry;
					}
				}
			}
		}
			crc32_initialiser;
	}

	crc32::crc32():
		sum(0)
	{
	}

	crc32::crc32(char const * data, std::size_t size):
		sum(0)
	{
		hash(data, size);
	}

	ulong crc32::hash(char const * data, std::size_t size)
	{
		sum ^= 0xffffffff;
		while(size > 0)
		{
			ulong index = (*data ^ sum) & 0xff;
			ulong shifted_sum = sum >> 8;
			sum = crc32_table[index] ^ shifted_sum;
			--size;
			++data;
		}
		sum ^= 0xffffffff;
		return sum;
	}

	ulong crc32::checksum() const
	{
		return sum;
	}
}
