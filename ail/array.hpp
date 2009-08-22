#pragma once

#include <cstddef>

namespace ail
{
	template <typename type, std::size_t size>
		std::size_t countof(type (&array)[size])
	{
		return size;
	}
}
