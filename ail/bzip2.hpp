#pragma once

#include <bzlib.h>
#include <cstdio>
#include <string>

namespace ail
{
	class bzip2_reader
	{
	public:
		bzip2_reader();
		bzip2_reader(std::string const & file_name);
		~bzip2_reader();

		bool open(std::string const & file_name);
		bool read(std::string & buffer);

	private:
		::bz_stream stream;
		::FILE * file;
		::BZFILE * bz_file;
	};

	bool decompress_bzip2(std::string const & file_name, std::string & output);
}
