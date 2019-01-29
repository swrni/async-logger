import os
import sys
import shutil
import subprocess

load_environment = os.path.join(os.sep, "MinGW", "load_environment.py")
exec(open(load_environment).read())

here = os.getcwd()

# Update the makefile.
shutil.copyfile(os.path.join(here, "makefile.txt"),
				os.path.join(here, "makefile"))

command = ["make", "--warn-undefined-variables", "--always-make"] + sys.argv[1:]

print("Running command: %s" % command)
subprocess.run(command)
