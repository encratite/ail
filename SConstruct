import os, shutil

source = 'ail'
script = 'SConscript'
temporary = os.path.join(source, script)

shutil.copyfile(script, temporary)
SConscript(temporary, variant_dir = 'build', duplicate = 0)
os.unlink(temporary)