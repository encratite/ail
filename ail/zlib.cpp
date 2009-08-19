#include <iostream>
#include <zlib.h>
#include <ail/zlib.hpp>

namespace ail
{
	namespace
	{
		std::size_t const buffer_size = 16 * 1024;
		int gzip_window_bits = 16 + MAX_WBITS;
	}

	std::string get_zlib_error_string(int error)
	{
		switch(error)
		{
			case Z_BUF_ERROR:
				return "zlib: Buffer error";

			case Z_MEM_ERROR:
				return "zlib: Insufficient memory";

			case Z_STREAM_ERROR:
				return "zlib: Stream error";

			case Z_NEED_DICT:
				return "zlib: Need dictionary";

			case Z_DATA_ERROR:
				return "zlib: Invalid input";

			default:
				return "zlib: Unknown inflation error occured";
		}
	}

	void deflate_data(char const * input, std::size_t size, bool is_gzip, std::string & output)
	{
		char buffer[buffer_size];

		z_stream stream;

		stream.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(input));
		stream.avail_in = static_cast<uInt>(size);
		stream.next_out = reinterpret_cast<Bytef *>(buffer);
		stream.avail_out = static_cast<uInt>(sizeof(buffer));
		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;

		int result;
		if(is_gzip)
			result = ::deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, gzip_window_bits, 8, Z_DEFAULT_STRATEGY);
		else
			result = ::deflateInit(&stream, Z_BEST_COMPRESSION);

		if(result != Z_OK)
			throw exception("zlib: Deflate initialisation error");

		while(true)
		{
			stream.next_out = reinterpret_cast<Bytef *>(buffer);
			stream.avail_out = static_cast<uInt>(sizeof(buffer));

			int result = ::deflate(&stream, Z_SYNC_FLUSH);
			if(result == Z_OK)
				output.append(buffer, static_cast<uInt>(sizeof(buffer)) - stream.avail_out);
			else
			{
				::deflateEnd(&stream);
				throw exception(get_zlib_error_string(result));
			}

			if(stream.total_in == size)
				break;
		}

		::deflateEnd(&stream);
	}

	void inflate_data(char const * input, std::size_t size, bool is_gzip, std::string & output)
	{
		char buffer[buffer_size];

		z_stream stream;

		stream.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(input));
		stream.avail_in = static_cast<uInt>(size);
		stream.next_out = reinterpret_cast<Bytef *>(buffer);
		stream.avail_out = static_cast<uInt>(sizeof(buffer));
		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;

		int result;

		if(is_gzip)
			result = ::inflateInit2(&stream, gzip_window_bits);
		else
			result = ::inflateInit(&stream);

		if(result != Z_OK)
			throw exception("zlib: Inflate initialisation error");

		while(true)
		{
			stream.next_out = reinterpret_cast<Bytef *>(buffer);
			stream.avail_out = static_cast<uInt>(sizeof(buffer));

			int result = ::inflate(&stream, Z_SYNC_FLUSH);
			switch(result)
			{
				case Z_OK:
				case Z_STREAM_END:
					output.append(buffer, static_cast<uInt>(sizeof(buffer)) - stream.avail_out);
					break;

				default:
					::inflateEnd(&stream);
					throw exception(get_zlib_error_string(result));
			}

			if(stream.total_in == size)
				break;
		}

		::inflateEnd(&stream);
	}

	void deflate(char const * input, std::size_t size, std::string & output)
	{
		deflate_data(input, size, false, output);
	}

	void compress_gzip(char const * input, std::size_t size, std::string & output)
	{
		deflate_data(input, size, true, output);
	}

	void deflate(std::string const & input, std::string & output)
	{
		deflate(input.c_str(), input.size(), output);
	}

	void compress_gzip(std::string const & input, std::string & output)
	{
		compress_gzip(input.c_str(), input.size(), output);
	}

	void inflate(char const * input, std::size_t size, std::string & output)
	{
		inflate_data(input, size, false, output);
	}

	void decompress_gzip(char const * input, std::size_t size, std::string & output)
	{
		inflate_data(input, size, true, output);
	}

	void inflate(std::string const & input, std::string & output)
	{
		inflate(input.c_str(), input.size(), output);
	}

	void decompress_gzip(std::string const & input, std::string & output)
	{
		decompress_gzip(input.c_str(), input.size(), output);
	}
}
