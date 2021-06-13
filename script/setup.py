#!/usr/bin/env python

import sys
from pathlib import Path
import shutil
import subprocess

project_dir = Path(__file__).parents[1]

print("This deletes any existing build folders. Do you want to continue?")
confirm = input("[type \"Yes.\" (without quotes) to continue] ")
if confirm != "Yes.":
    sys.exit(2)

print("Clearing unpacked wraps...")
subproj_dir = project_dir.joinpath("subprojects")
for d in subproj_dir.iterdir():
    if d.is_dir() and (not d.name == "packagefiles"):
        shutil.rmtree(d)

print("Setting up build...")
shutil.rmtree(project_dir.joinpath("build"), ignore_errors=True)
exit_code = subprocess.call(["meson", "setup", "build"])
if exit_code != 0:
    sys.exit(exit_code)
print("")

print("Done!")