import os
import nil.build, nil.environment, nil.setup

project = 'ail'

packages = ['libboost1.40-all-dev', 'libbz2-dev', 'libbz2-1.0', 'zlib1g-dev']

nil.environment.unix_like()

nil.setup.install_packages(packages)

builder = nil.build.builder(project)

if builder.static_library():
	root = os.path.dirname(nil.environment.get_script_path())
	
	target_library_path = os.path.join(root, builder.output_directory, builder.library)
	library_path = os.path.join('/usr/local/lib', builder.library)
	
	nil.setup.symlink(target_library_path, library_path)
	
	target_include_path = os.path.join(root, project)
	include_path = os.path.join('/usr/local/include', project)

	nil.setup.symlink(target_include_path, include_path)
