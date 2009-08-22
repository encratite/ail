#pragma once

#include <ail/exception.hpp>

namespace ail
{
	void deflate(char const * input, std::size_t size, std::string & output);
	void compress_gzip(char const * input, std::size_t size, std::string & output);

	void deflate(std::string const & input, std::string & output);
	void compress_gzip(std::string const & input, std::string & output);

	void inflate(char const * input, std::size_t size, std::string & output);
	void inflate(std::string const & input, std::string & output);
	
	void decompress_gzip(char const * input, std::size_t size, std::string & output);
	void decompress_gzip(std::string const & input, std::string & output);
}
