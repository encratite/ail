#ifndef AIL_ZLIB_HPP
#define AIL_ZLIB_HPP

#include <ail/exception.hpp>

namespace ail
{
	void inflate(char const * input, std::size_t size, std::string & output);
	void inflate(std::string const & input, std::string & output);
	
	void decompress_gzip(char const * input, std::size_t size, std::string & output);
	void decompress_gzip(std::string const & input, std::string & output);
}

#endif
