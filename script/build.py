from argparse import ArgumentParser
from pathlib import Path
import subprocess
import sys

project_dir = Path(__file__).parents[1]
build_dir = project_dir.joinpath("build")

parser = ArgumentParser(
    description="Configures and builds the project."
)
parser.add_argument("config",
    help="Should either be \"debug\" or \"release\".",
    default="debug"
)

args = parser.parse_args()
del parser

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