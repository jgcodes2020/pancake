#!/usr/bin/env python

import sys
from pathlib import Path
import shutil
import subprocess
import json
import os

prevenv = os.environ.copy()

try:
    # Provide VC environment if on Windows
    if sys.platform.startswith("win"):
        bscript_dir = Path(__file__).parent
        # Run VSWhere to find VS with MSVC x64
        vswhere = subprocess.run(
            [str(bscript_dir.joinpath("vswhere.exe")), 
                "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                "-format", "json", "-utf8"
            ], capture_output=True, encoding="utf-8"
        )
        vspath = json.loads(vswhere.stdout)
        if len(vspath) == 0:
            raise RuntimeError("Download Visual Studio 2019 with Visual C++.")
        # Use VS path to locate vcvars64.bat
        vcvars_path = Path(vspath[0]["installationPath"]).joinpath("VC\\Auxiliary\\Build\\vcvars64.bat")
        # Run vcvars64.bat and print environment
        vcvars = subprocess.run(
            ["cmd", "/c", str(vcvars_path), "&&", "set"],
            capture_output=True, encoding="utf-8"
        )
        # Use dict comprehension to get final env variable
        vcenv = {var.split("=", 1)[0] : var.split("=", 1)[1] for var in vcvars.stdout.splitlines()[5:]}
        vcenv["PATH"] = vcenv["Path"]
        os.environ.update(vcenv)

    project_dir = Path(__file__).parents[1]
    if project_dir.joinpath("build").is_dir():
        print("This clears any existing build folders. Do you want to continue?")
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
    exit_code = subprocess.call(["meson", "setup", "build"], cwd=project_dir)
    if exit_code != 0:
        sys.exit(exit_code)
    print("")

    print("Done!")
finally:
    os.environ.update(prevenv)