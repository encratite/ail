#ifndef AIL_ARRAY_HPP
#define AIL_ARRAY_HPP

#include <cstddef>

namespace ail
{
	template <typename type, std::size_t size>
		std::size_t countof(type (&array)[size])
	{
		return size;
	}
}
#endif
