#ifndef AIL_CRC32_HPP
#define AIL_CRC32_HPP

#include <cstddef>
#include <ail/types.hpp>

namespace ail
{
	class crc32
	{
	public:
		crc32();
		crc32(char const * data, std::size_t size);
		ulong hash(char const * data, std::size_t size);
		ulong checksum() const;

	private:
		ulong sum;
	};
}

#endif
