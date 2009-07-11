#include <zlib.h>

#include <ail/zlib.hpp>

namespace ail
{
	void inflate_data(char const * input, std::size_t size, bool is_gzip, std::string & output)
	{
		char buffer[16 * 1024];

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
			result = ::inflateInit2(&stream, 16 + MAX_WBITS);
		else
			result = ::inflateInit(&stream);

		if(result != Z_OK)
			throw exception("zlib: Initialisation error");

		while(true)
		{
			std::string error_string;

			stream.next_out = reinterpret_cast<Bytef *>(buffer);
			stream.avail_out = static_cast<uInt>(sizeof(buffer));

			int result = ::inflate(&stream, Z_SYNC_FLUSH);
			switch(result)
			{
				case Z_OK:
				case Z_STREAM_END:
					output.append(buffer, static_cast<uInt>(sizeof(buffer)) - stream.avail_out);
					break;

				case Z_BUF_ERROR:
					error_string = "zlib: Buffer error";
					break;

				case Z_MEM_ERROR:
					error_string = "zlib: Insufficient memory";
					break;

				case Z_STREAM_ERROR:
					error_string = "zlib: Stream error";
					break;

				case Z_NEED_DICT:
					error_string = "zlib: Need dictionary";
					break;

				case Z_DATA_ERROR:
					error_string = "zlib: Invalid input";
					break;

				default:
					error_string = "zlib: Unknown inflation error occured";
					break;
			}

			if(!error_string.empty())
			{
				::inflateEnd(&stream);
				throw exception(error_string);
			}

			if(result == Z_STREAM_END)
				break;
		}

		::inflateEnd(&stream);
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
