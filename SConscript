import nil.directory, nil.environment, sys, os

library = 'ail'

is_windows = nil.environment.is_windows()

if is_windows:
	boost = ARGUMENTS.get('boost')
	bzip2 = ARGUMENTS.get('bzip2')
	zlib = ARGUMENTS.get('zlib')

	if boost == None:

		print 'This library requires boost (www.boost.org) so you will have to specify the path to the boost library in the scons arguments:'
		print 'scons boost=<boost directory>'
		print 'The bzip2 (www.bzip.org) and the zlib (www.zlib.net) components of this library are optional and will require you to specify their locations the same way:'
		print 'scons boost=<boost directory> bzip2=<bzip2 directory> zlib=<zlib directory>'
		sys.exit(1)
		
	optional_files = [
		(bzip2, 'bzip2.cpp'),
		(zlib, 'zlib.cpp')
	]

	defines = {
		'_WIN32_WINNT': '0x0501',
		'_CRT_SECURE_NO_WARNINGS': '1',
		'_SCL_SECURE_NO_WARNINGS': '1',
		'BOOST_LIB_DIAGNOSTIC': 1
	}

	flags = [
		'/EHsc'
	]

	linker_flags = [
		'/NOLOGO',
		'/LTCG'
	]

	dependencies = [boost, bzip2, zlib]
	
else:
	optional_files = []
	defines = []
	flags = []
	linker_flags = []
	dependencies = []

relative_source_directory = os.path.join('..', library)

source_files = map(lambda path: os.path.basename(path), nil.directory.get_files_by_extension(relative_source_directory, 'cpp'))
if len(source_files) == 0:
	print 'No targets. CWD: %s' % os.getcwd()
	sys.exit(1)
	
print '%d targets:' % len(source_files)

for source_file in source_files:
	print source_file
	
for path, file in optional_files:
	if path == None:
		source_files.remove(file)
		pass

include_directories = ['..'] + filter(lambda path: path != None, dependencies)

cpus = int(os.environ.get('NUMBER_OF_PROCESSORS', 1))

thread_string = 'thread'
if cpus > 1:
	thread_string += 's'
print 'Compiling project with %d %s' % (cpus, thread_string)

environment = Environment(CPPPATH = include_directories, CPPDEFINES = defines, CCFLAGS = flags)
environment.Append(LINKFLAGS = linker_flags)
environment.SetOption('num_jobs', cpus)
environment.StaticLibrary(library, source_files)
