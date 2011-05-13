#include <ail/file.hpp>
#include <ail/string.hpp>
#include <ail/environment.hpp>

#include <boost/filesystem.hpp>

namespace ail
{
	file::file():
		file_descriptor(0)
	{
	}

	file::file(std::string const & name, open_mode_type mode):
		file_descriptor(0)
	{
		open(name, mode);
	}

	file::~file()
	{
		close();
	}

	bool file::open()
	{
		return file_descriptor != 0;
	}

	bool file::open(std::string const & name, open_mode_type mode)
	{
		close();

		std::string fopen_mode;

		switch(mode)
		{
			case open_mode_read:
				fopen_mode = "r";
				break;

			case open_mode_read_write:
				fopen_mode = "r+";
				break;

			case open_mode_write_truncate:
				fopen_mode = "w";
				break;

			case open_mode_read_write_truncate:
				fopen_mode = "w+";
				break;
		}

		fopen_mode += "b";

		file_descriptor = std::fopen(name.c_str(), fopen_mode.c_str());

		return file_descriptor != 0;
	}

	bool file::open_read_only(std::string const & name)
	{
		return open(name, open_mode_read);
	}

	bool file::open_create(std::string const & name)
	{
		return open(name, open_mode_read_write_truncate);
	}

	std::string file::read(std::size_t size)
	{
		if(size == 0)
			return "";

		char * buffer = new char[size];
		read(buffer, size);
		std::string output;
		output.assign(buffer, size);
		delete[] buffer;
		return output;
	}

	void file::read(char * output, std::size_t size)
	{
		if(file_descriptor == 0)
			throw exception("Trying to read from an unopened file");

		std::size_t result = std::fread(output, 1, static_cast<int>(size), file_descriptor);
		if(result == 0)
		{
			int fail = std::ferror(file_descriptor);
			if(fail == 0)
				throw exception("An unknown error occured while attempting to read from a file");
		}
	}

	void file::write(std::string const & input)
	{
		write(input.c_str(), input.length());
	}

	void file::write(char const * input, std::size_t size)
	{
		if(file_descriptor == 0)
			throw exception("Trying to write to an unopened file");

		std::size_t result = std::fwrite(input, 1, size, file_descriptor);
		if(result < size)
			throw exception("Failed to write to file");
	}

	void file::close()
	{
		if(file_descriptor != 0)
		{
			std::fclose(file_descriptor);
			file_descriptor = 0;
		}
	}

	std::size_t file::get_size()
	{
		if(file_descriptor == 0)
			throw exception("Cannot get the size of an unopened file");

		fpos_t position;
		int fail = std::fgetpos(file_descriptor, &position);
		if(fail != 0)
			throw exception("Failed to retrieve file pointer");
		seek_end();
		std::size_t output = static_cast<std::size_t>(std::ftell(file_descriptor));
		fail = std::fsetpos(file_descriptor, &position);
		if(fail != 0)
			throw exception("Failed to set file pointer");
		return output;
	}

	std::size_t file::get_file_pointer()
	{
		if(file_descriptor == 0)
			throw exception("Trying to get the file pointer of an unopened file");

		long position = std::ftell(file_descriptor);
		if(position == -1)
			throw exception("Failed to retrieve file pointer");
		return static_cast<std::size_t>(position);
	}

	void file::set_file_pointer(std::size_t offset)
	{
		if(file_descriptor == 0)
			throw exception("Trying to set the file pointer of an unopened file");

		int fail = std::fseek(file_descriptor, static_cast<long>(offset), SEEK_SET);
		if(fail != 0)
			throw exception("Failed to set file pointer");
	}

	void file::seek_end()
	{
		if(file_descriptor == 0)
			throw exception("Trying to seek the end of an unopened file");

		int fail = std::fseek(file_descriptor, 0, SEEK_END);
		if(fail != 0)
			throw exception("Failed to seek end of file");
	}

	bool read_file(std::string const & file_name, std::string & output)
	{
		file file_object;
		bool success = file_object.open_read_only(file_name);
		if(!success)
			return false;
		std::size_t file_size = file_object.get_size();
		output = file_object.read(file_size);
		return true;
	}

	bool write_file(std::string const & file_name, std::string const & input)
	{
		file file_object;
		bool success = file_object.open_create(file_name);
		if(success == false)
			return false;
		file_object.write(input);
		return true;
	}
	
	bool append_to_file(std::string const & file_name, std::string const & input)
	{
		file file_object;
		if(
			!file_object.open(file_name) &&
			!file_object.open_create(file_name)
		)
			return false;
		file_object.seek_end();
		file_object.write(input);
		return true;
	}

	bool read_lines(std::string const & file_name, std::vector<std::string> & output)
	{
		std::string input;
		bool result = read_file(file_name, input);
		if(result == false)
			return false;
		input = erase_string(input, "\r");
		output = tokenise(input, "\n");
		return true;
	}

	bool create_directory(std::string const & path)
	{
		try
		{
			return boost::filesystem::create_directory(path);
		}
		catch(boost::exception &)
		{
			throw ail::exception("Unable to create directory");
		}
	}

	bool read_directory(std::string const & directory, string_vector & files, string_vector & directories)
	{
		try
		{
			for(boost::filesystem::directory_iterator i(directory), end; i != end; i++)
			{
				std::string const & path = i->path().string();
				if(boost::filesystem::is_directory(i->status()))
					directories.push_back(path);
				else
					files.push_back(path);
			}
		}
		catch(...)
		{
			return false;
		}
		return true;
	}

	bool rename_file(std::string const & target, std::string const & new_name)
	{
		try
		{
			boost::filesystem::rename(target, new_name);
		}
		catch(...)
		{
			return false;
		}
		return true;
	}

	bool remove_file(std::string const & path)
	{
		try
		{
			return boost::filesystem::remove(path);
		}
		catch(...)
		{
			return false;
		}
	}

	std::string join_paths(std::string const & left, std::string const & right)
	{
#ifdef AIL_WINDOWS
		std::string const path_char = "\\";
#else
		std::string const path_char = "/";
#endif
		std::string output = left + path_char + right;
		std::string const target = path_char + path_char;
		for(std::size_t i = 0; i < 2; i++)
			output = replace_string(output, target, path_char);
		return output;
	}

	bool retrieve_extension(std::string const & path, std::string & output)
	{
		std::size_t offset = path.rfind('.');
		if(offset == std::string::npos)
			return false;
		output = path.substr(offset + 1);
		return true;
	}

	bool file_exists(std::string const & path)
	{
		file file_object;
		return file_object.open(path, file::open_mode_read);
	}
}
