#include <ail/bmp.hpp>

#include <cstring>

#include <ail/memory.hpp>
#include <ail/file.hpp>

namespace ail
{
	namespace
	{
		std::size_t const
			bmp_header_size = 54,
			bytes_per_pixel = 3, //24-bit
			bmp_padding = 4,
			dword_size = 4;
	}

	bmp::bmp():
		buffer(0)
	{
	}

	bmp::bmp(std::size_t width, std::size_t height):
		buffer(0)
	{
		initialise(width, height);
	}

	bmp::~bmp()
	{
		release();
	}

	void bmp::initialise(std::size_t new_width, std::size_t new_height)
	{
		release();
		width = new_width;
		height = new_height;
		process_image_size(width, height);
		buffer = new char[image_size];
		write_header();
	}

	std::size_t bmp::process_image_size(std::size_t width, std::size_t height)
	{
		bytes_per_line = width * bytes_per_pixel;
		std::size_t remainder = bytes_per_line % bmp_padding;
		if(remainder != 0)
			bytes_per_line += bmp_padding - remainder;
		image_size = bmp_header_size + height * bytes_per_line;
		return image_size;
	}

	void bmp::set_pixel(std::size_t x, std::size_t y, long value)
	{
		if(x < 0 || x >= width)
			throw exception("Invalid x coordinate");
		if(y < 0 || y >= height)
			throw exception("Invalid y coordinate");
		std::size_t offset = bmp_header_size + (height - y) * bytes_per_line + x * bytes_per_pixel;
		write_little_endian(static_cast<ulong>(value), buffer + offset, bytes_per_pixel);
	}

	bool bmp::write(std::string const & file_name)
	{
		file bmp_file;
		if(!bmp_file.open_create(file_name))
			return false;
		bmp_file.write(buffer, image_size);
		bmp_file.close();
		return true;
	}

	void bmp::write_header()
	{
		char const * data =
			"\x42\x4D\xF6\x00\x00\x00\x00\x00\x00\x00\x36\x00\x00\x00\x28\x00"
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x18\x00\x00\x00"
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
			"\x00\x00\x00\x00\x00\x00";

		std::memcpy(buffer, data, bmp_header_size);
		write_little_endian(static_cast<ulong>(width), buffer + 18, dword_size);
		write_little_endian(static_cast<ulong>(height), buffer + 22, dword_size);
		write_little_endian(static_cast<ulong>(image_size - bmp_header_size), buffer + 34, dword_size);
	}

	void bmp::release()
	{
		if(buffer != 0)
		{
			delete buffer;
			buffer = 0;
		}
	}
}
