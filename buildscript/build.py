from argparse import ArgumentParser
from pathlib import Path
import subprocess
import sys; import os
import json
import shutil

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
    parser.add_argument("--wrap-reload", "-W", 
        action="store_true",
        dest="wrap_reload",
        help="Reloads subproject wraps."
    )
    parser.add_argument("--use-vs", "-V",
        action="store_true",
        dest="use_vs",
        help="Uses the Visual Studio backend."
    )
    parser.add_argument("--no-compile", "-c",
        action="store_false",
        dest="do_compile",
        help="Don't compile the file."
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
        vswhere.check_returncode()
        vspath = json.loads(vswhere.stdout)
        # Use VS path to locate vcvars64.bat
        vcvars_path = Path(vspath[0]["installationPath"]).joinpath("VC\\Auxiliary\\Build\\vcvars64.bat")
        # Run vcvars64.bat and print environment
        print("Initializing vcvars64 environment...")
        vcvars = subprocess.run(
            ["cmd", "/c", str(vcvars_path), "&&", "set"],
            capture_output=True, encoding="utf-8"
        )
        vcvars.check_returncode()
        # Use dict comprehension to get final env variable
        vcenv = {var.split("=", 1)[0] : var.split("=", 1)[1] for var in vcvars.stdout.splitlines()[5:]}
        vcenv["PATH"] = vcenv["Path"]
        os.environ.update(vcenv)
    
    # Clear loaded wraps
    if args.wrap_reload:
        print("Clearing loaded wraps...")
        for f in project_dir.joinpath("subprojects").iterdir():
            if f.is_dir() and f.name != "packagefiles":
                shutil.rmtree(f);
    # Load existing config
    introspect = subprocess.run(
        ["meson", "introspect", "--buildoptions"],
        cwd=build_dir, encoding="utf-8", capture_output=True
    )
    current_opts = json.loads(introspect.stdout)
    opt_buildtype = next(filter(lambda obj: obj["name"] == "buildtype", current_opts))
    opt_backend = next(filter(lambda obj: obj["name"] == "backend", current_opts))
    # Create config command
    print("Configuring Meson...")
    backend = "vs" if args.use_vs else "ninja"
    config = args.config
    
    compile_cmd = ["meson", "configure"]
    if (opt_backend["value"] != backend):
        compile_cmd.append(f"--backend={backend}")
    if (opt_buildtype["value"] != config):
        compile_cmd.append(f"--buildtype={config}")
    # Check if config is needed and run
    if (len(compile_cmd) > 2):
        subprocess.run(
            compile_cmd,
            cwd=build_dir
        ).check_returncode()
    
    if args.do_compile:
        print("Running Meson build...")
        subprocess.run(
            ["meson", "compile"],
            cwd=build_dir
        ).check_returncode()
finally:
    os.environ.update(prevenv)