#pragma once

#include <string>
#include <vector>

#include <cstdio>

#include <ail/exception.hpp>
#include <ail/types.hpp>

namespace ail
{
	class file
	{
	public:

		enum open_mode_type
		{
			open_mode_read,
			open_mode_read_write,
			open_mode_write_truncate,
			open_mode_read_write_truncate
		};

		file();
		file(std::string const & name, open_mode_type mode = open_mode_read_write);
		~file();

		bool open();
		bool open(std::string const & name, open_mode_type mode = open_mode_read_write);
		bool open_read_only(std::string const & name);
		bool open_create(std::string const & name);

		std::string read(std::size_t size);
		void read(char * output, std::size_t size);
		void write(std::string const & input);
		void write(char const * input, std::size_t size);
		void close();

		std::size_t get_size();
		std::size_t get_file_pointer();
		void set_file_pointer(std::size_t offset);
		void seek_end();

		template <typename type>
			void read_type(type * output, std::size_t count)
		{
			read(reinterpret_cast<char *>(output), count * sizeof(type));
		}

	private:
		FILE * file_descriptor;
	};

	bool read_file(std::string const & file_name, std::string & output);
	bool write_file(std::string const & file_name, std::string const & input);
	bool append_to_file(std::string const & file_name, std::string const & input);
	bool read_lines(std::string const & file_name, std::vector<std::string> & output);

	bool create_directory(std::string const & path);
	bool read_directory(std::string const & directory, string_vector & files, string_vector & directories);
	bool rename_file(std::string const & target, std::string const & new_name);
	bool remove_file(std::string const & path);

	std::string join_paths(std::string const & left, std::string const & right);
	bool retrieve_extension(std::string const & path, std::string & output);

	bool file_exists(std::string const & path);
}
