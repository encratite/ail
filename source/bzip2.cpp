#include <ail/bzip2.hpp>

namespace ail
{
	bzip2_reader::bzip2_reader():
		bz_file(0)
	{
	}

	bzip2_reader::bzip2_reader(std::string const & file_name)
	{
		open(file_name);
	}

	bzip2_reader::~bzip2_reader()
	{
		if(bz_file != 0)
		{
			int bz_error;
			::fclose(file);
			::BZ2_bzReadClose(&bz_error, bz_file);
		}
	}

	bool bzip2_reader::open(std::string const & file_name)
	{
		int bz_error;
		file = ::fopen(file_name.c_str(), "rb");
		if(file == 0)
			return false;
		bz_file = ::BZ2_bzReadOpen(&bz_error, file, 0, 0, 0, 0);
		if(bz_file == 0)
		{
			::fclose(file);
			::BZ2_bzReadClose(&bz_error, bz_file);
			return false;
		}
		return true;
	}

	bool bzip2_reader::read(std::string & buffer)
	{
		std::size_t const buffer_size = 16 * 1024;
		char in_buffer[buffer_size];
		int bz_error;
		int bytes_read = ::BZ2_bzRead(&bz_error, bz_file, in_buffer, static_cast<int>(buffer_size));
		if(bz_error != BZ_OK)
			return false;
		std::string new_data;
		new_data.assign(in_buffer, static_cast<std::size_t>(bytes_read));
		buffer += new_data;
		return true;
	}

	bool decompress_bzip2(std::string const & file_name, std::string & output)
	{
		bzip2_reader reader;
		if(!reader.open(file_name))
			return false;
		while(reader.read(output));
		return true;
	}
}
