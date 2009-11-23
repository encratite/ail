import os
import nil.build, nil.environment, nil.setup

project = 'ail'

packages = ['libboost1.40-all-dev', 'libbz2-dev', 'libbz2-1.0', 'zlib1g-dev']

nil.environment.unix_like()

nil.setup.install_packages(packages)

builder = nil.build.builder(project)

if builder.static_library():
	nil.setup.include(project)
	nil.setup.library(builder.library)
