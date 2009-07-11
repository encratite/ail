#include <algorithm>
#include <ail/arguments.hpp>
#include <ail/string.hpp>
#include <boost/foreach.hpp>

namespace ail
{
	argument_variable::argument_variable(std::string const & name, bool & variable):
		name(name),
		type(argument_variable_type_bool),
		address(&variable)
	{
		set_common_properties();
	}

	argument_variable::argument_variable(std::string const & name, std::string & variable):
		name(name),
		type(argument_variable_type_string),
		address(&variable)
	{
		set_common_properties();
	}

	void argument_variable::set_common_properties()
	{
		default_value = 0;
		handled = false;
		required = false;
	}

	bool argument_variable::operator==(std::string const & input) const
	{
		return name == input;
	}

	argument_controller::argument_controller(argument_variable & _variable):
		variable(_variable)
	{
	}

	argument_controller & argument_controller::required()
	{
		variable.required = true;
		return *this;
	}

	argument_controller & argument_controller::default_flag(bool value)
	{
		variable.default_value = new bool(value);
		return *this;
	}

	argument_controller & argument_controller::default_string(std::string const & value)
	{
		variable.default_value = new std::string(value);
		return *this;
	}

	argument_controller & argument_parser::flag(std::string const & name, bool & variable)
	{
		argument_variable new_variable(name, variable);
		variables.push_back(new_variable);
		return * new argument_controller(*(variables.end() - 1));
	}

	argument_controller & argument_parser::string(std::string const & name, std::string & variable)
	{
		argument_variable new_variable(name, variable);
		variables.push_back(new_variable);
		return * new argument_controller(*(variables.end() - 1));
	}

	void argument_parser::parse(std::string const & input)
	{
		string_vector tokens = tokenise(input, " ");
		parse(tokens);
	}

	void argument_parser::parse(int argc, char ** argv)
	{
		string_vector tokens;
		for(int i = 0; i < argc; i++)
			tokens.push_back(argv[i]);
		parse(tokens);
	}

	void argument_parser::parse(string_vector & tokens)
	{
		for(std::size_t i = 0, end = tokens.size(); i < end; i++)
		{
			std::string const & token = tokens[i];
			if(token.empty())
				continue;

			if(token[0] == '-')
			{
				std::string name = token.substr(1);
				variable_vector::iterator iterator = std::find(variables.begin(), variables.end(), name);
				if(iterator == variables.end())
					continue;
				argument_variable & variable = *iterator;
				i++;
				if(i == end)
					throw exception("Variable \"" + name + "\" is lacking an argument");
				std::string argument = tokens[i];
				switch(variable.type)
				{
					case argument_variable_type_bool:
					{
						bool value;
						if(!string_to_bool(argument, value))
							throw exception("Invalid boolean value specified for variable \"" + name + "\"");
						*reinterpret_cast<bool *>(variable.address) = value;
						break;
					}

					case argument_variable_type_string:
						*reinterpret_cast<std::string *>(variable.address) = argument;
						break;

					default:
						throw exception("Unknown variable type encountered in argument parser");
						break;
				}
				variable.handled = true;
			}
		}

		BOOST_FOREACH(argument_variable & variable, variables)
		{
			if(!variable.handled)
			{
				if(variable.required)
					throw exception("Required variable \"" + variable.name + "\" has not been specified");

				else if(variable.default_value)
				{
					switch(variable.type)
					{
						case argument_variable_type_bool:
							variable.set_default_value<bool>();
							break;

						case argument_variable_type_string:
							variable.set_default_value<std::string>();
							break;
					}
				}
			}
		}
	}
}
