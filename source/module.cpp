#include <ail/module.hpp>

#ifndef AIL_WINDOWS
#include <dlfcn.h>
#endif

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
#ifdef AIL_WINDOWS
		module_handle = ::LoadLibrary(new_path.c_str());
#else
		module_handle = ::dlopen(new_path.c_str(), RTLD_LAZY);
#endif

		is_loaded = module_handle != 0;

		if(is_loaded)
			path = new_path;
		return is_loaded;
	}

	void dynamic_module::unload()
	{
		if(is_loaded)
		{
#ifdef AIL_WINDOWS
			::FreeLibrary(module_handle);
#else
			::dlclose(module_handle);
#endif
			is_loaded = false;
		}
	}

	bool dynamic_module::get_function(std::string const & name, void * & output)
	{
		if(!is_loaded)
			return false;

#ifdef AIL_WINDOWS
		output = ::GetProcAddress(module_handle, name.c_str());
		return output != 0;
#else
		boost::mutex::scoped_lock scoped_lock(mutex);
		output = ::dlsym(module_handle, name.c_str());
		return dlerror() == 0;
#endif
	}
}
