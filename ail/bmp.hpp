#ifndef AIL_BMP_HPP
#define AIL_BMP_HPP

#include <string>

#include <ail/exception.hpp>

namespace ail
{
	class bmp
	{
	public:
		bmp();
		bmp(std::size_t width, std::size_t height);
		~bmp();
		void release();
		void initialise(std::size_t new_width, std::size_t new_height);
		void set_pixel(std::size_t x, std::size_t y, long value);
		bool write(std::string const & file_name);
		std::size_t process_image_size(std::size_t width, std::size_t height);
		
	private:
		std::size_t
			width,
			height,
			bytes_per_line,
			image_size;

		char * buffer;

		void write_header();
	};
}

#endif AIL_BMP_HPP
