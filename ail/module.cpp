#include <ail/module.hpp>

namespace ail
{
	dynamic_module::dynamic_module():
		is_loaded(false)
	{
	}

	dynamic_module::~dynamic_module()
	{
		unload();
	}

	bool dynamic_module::load(std::string const & new_path)
	{
		module_handle = ::LoadLibrary(new_path.c_str());
		is_loaded = module_handle != 0;
		if(is_loaded)
			path = new_path;
		return is_loaded;
	}

	void dynamic_module::unload()
	{
		if(is_loaded)
		{
			::FreeLibrary(module_handle);
			is_loaded = false;
		}
	}

	bool dynamic_module::get_function(std::string const & name, void * & output)
	{
		if(!is_loaded)
			return false;

		output = ::GetProcAddress(module_handle, name.c_str());
		return output != 0;
	}
}
