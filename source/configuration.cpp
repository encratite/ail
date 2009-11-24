#include <ail/configuration.hpp>

namespace ail
{
	configuration::configuration()
	{
	}

	configuration::configuration(std::string const & file_name)
	{
		load(file_name);
	}

	bool configuration::load(std::string const & new_file_name)
	{
		file_name = new_file_name;

		values.clear();

		std::string input;
		bool success = read_file(file_name, input);
		if(success == false)
		{
			return false;
		}

		std::vector<std::string> lines = tokenise(input, "\n");
		for(std::vector<std::string>::iterator i = lines.begin(), end = lines.end(); i != end; ++i)
		{
			std::string line = trim(*i);

			bool is_empty = line.empty();

			if(
				(is_empty == true)
					||
				(
					(is_empty == false)
						&&
					(line[0] == '#')
				)
			)
			{
				continue;
			}

			std::size_t offset = line.find('=');
			if(offset == std::string::npos)
			{
				continue;
			}

			std::string
				variable = right_trim(line.substr(0, offset)),
				value = left_trim(line.substr(offset + 1));

			if(
				(value.length() >= 2)
					&&
				(value[0] == '"')
					&&
				(*(value.end() - 1) == '"')
			)
			{
				value.erase(value.begin());
				value.erase(value.end() - 1);
			}

			values[variable] = value;
		}

		return true;
	}

	bool configuration::read_string(std::string const & variable_name, std::string & output)
	{
		std::map<std::string, std::string>::iterator search = values.find(variable_name);
		if(search == values.end())
			return false;

		output = search->second;
		return true;
	}

	std::string configuration::string(std::string const & variable_name)
	{
		std::string output;
		bool success = read_string(variable_name, output);
		if(!success)
			throw exception("Unable to find string value \"" + variable_name + "\" in \"" + file_name + "\"");
		return output;
	}

	std::string configuration::string(std::string const & variable_name, std::string const & default_value)
	{
		std::string output;
		bool success = read_string(variable_name, output);
		if(!success)
			return default_value;

		return output;
	}

	bool configuration::boolean(std::string const & variable_name)
	{
		word output;
		bool success = read_number<word>(variable_name, output);
		if(!success)
			throw exception("Unable to find bool value \"" + variable_name + "\" in \"" + file_name + "\"");
		return output != 0;
	}

	bool configuration::boolean(std::string const & variable_name, bool default_value)
	{
		word output;
		bool success = read_number<word>(variable_name, output);
		if(success == false)
			return default_value;
		return output != 0;
	}
}
