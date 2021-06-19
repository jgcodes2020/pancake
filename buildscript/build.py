from argparse import ArgumentParser
from pathlib import Path
import subprocess
import sys; import os
import json

prevenv = os.environ.copy()
try:
    project_dir = Path(__file__).parents[1]
    build_dir = project_dir.joinpath("build")
    bscript_dir = Path(__file__).parent

    parser = ArgumentParser(
        description="Configures and builds the project."
    )
    parser.add_argument("config",
        help="Should either be \"debug\" or \"release\".",
        default="debug"
    )

    args = parser.parse_args()
    del parser

    subp_args = {
        "cwd": build_dir
    }
    # Provide VC++ environment if on Windows
    if sys.platform.startswith("win"):
        bscript_dir = Path(__file__).parent
        # Run VSWhere to find VS
        print("Finding Visual Studio...")
        vswhere = subprocess.run(
            [str(bscript_dir.joinpath("vswhere.exe")), 
                "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                "-format", "json", "-utf8"
            ], capture_output=True, encoding="utf-8"
        )
        vspath = json.loads(vswhere.stdout)
        # Use VS path to locate vcvars64.bat
        vcvars_path = Path(vspath[0]["installationPath"]).joinpath("VC\\Auxiliary\\Build\\vcvars64.bat")
        # Run vcvars64.bat and print environment
        print("Initializing vcvars64 environment...")
        vcvars = subprocess.run(
            ["cmd", "/c", str(vcvars_path), "&&", "set"],
            capture_output=True, encoding="utf-8"
        )
        # Use dict comprehension to get final env variable
        vcenv = {var.split("=", 1)[0] : var.split("=", 1)[1] for var in vcvars.stdout.splitlines()[5:]}
        vcenv["PATH"] = vcenv["Path"]
        os.environ.update(vcenv)

    if args.config == "debug":
        print("Configuring debug")
        subprocess.call(
            ["meson", "configure", "--buildtype=debug"
                # Add additional options to Meson here
            ],
            cwd=build_dir
        )
    elif args.config == "release":
        print("Configuring release")
        subprocess.call(
            ["meson", "configure", "--buildtype=release"
                # Add additional options to Meson here
            ],
            cwd=build_dir
        )
        
    else:
        print("Invalid configuration! Use either debug or release")
        sys.exit(2)

    subprocess.call(
        ["meson", "compile"],
        cwd=build_dir
    )
finally:
    os.environ.update(prevenv)