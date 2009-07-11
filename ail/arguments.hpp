#ifndef AIL_ARGUMENTS_HPP
#define AIL_ARGUMENTS_HPP

#include <string>
#include <vector>
#include <ail/types.hpp>
#include <ail/exception.hpp>

namespace ail
{
	enum argument_variable_type
	{
		argument_variable_type_bool,
		argument_variable_type_string
	};

	struct argument_variable
	{
		std::string name;
		argument_variable_type type;
		void
			* address,
			* default_value;
		bool
			handled,
			required;

		argument_variable(std::string const & name, bool & variable);
		argument_variable(std::string const & name, std::string & variable);
		void set_common_properties();

		bool operator==(std::string const & input) const;

		template <class type>
			void set_default_value()
		{
			*reinterpret_cast<type *>(address) = *reinterpret_cast<type *>(default_value);
		}
	};

	typedef std::vector<argument_variable> variable_vector;
	
	class argument_controller
	{
	public:
		argument_controller(argument_variable & variable);
		argument_controller & required();
		argument_controller & default_flag(bool value);
		argument_controller & default_string(std::string const & value);

	private:
		argument_variable & variable;
	};

	class argument_parser
	{
	public:

		argument_controller & flag(std::string const & name, bool & variable);
		argument_controller & string(std::string const & name, std::string & variable);

		void parse(std::string const & input);
		void parse(int argc, char ** argv);
		void parse(string_vector & tokens);

	private:
		variable_vector variables;
	};
}

#endif
