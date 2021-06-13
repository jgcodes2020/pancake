from argparse import ArgumentParser
from pathlib import Path
import sys

project_dir = Path(__file__).parents[1]

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
    print("configure debug")
elif args.config == "release":
    print("configure release")
else:
    print("Invalid configuration! Use either debug or release")
    sys.exit(2)