#!/usr/bin/env python

from argparse import ArgumentParser, ArgumentTypeError, ArgumentError
from pathlib import Path
from textwrap import dedent
import re

def path_file(arg: str):
    result = Path(arg).resolve()
    if result.is_file():
        return result
    else:
        raise ArgumentTypeError(f"path_file:{arg} is not a file")
def path_file_new(arg: str):
    result = Path.cwd().joinpath(Path(arg)).resolve()
    if (result.is_dir()):
        raise ArgumentTypeError(f"{arg} is an existing directory, can't write file")
    else:
        return result

parser = ArgumentParser(
    description="Generates a C++ header and source file for a Pancake resource."
)
parser.add_argument("input",
    help="Input resource file to be packaged into C++",
    type=path_file
)

parser.add_argument("-H", "--out-header", 
    help="Output header to include from C++ files",
    dest="header", type=path_file_new,
    required=True
)

parser.add_argument("-C", "--out-cpp",
    help="Output C++ file containing resource definition",
    dest="source", type=path_file_new,
    required=True
)

args = parser.parse_args()
del parser

non_id_char = re.compile(r"\W")
input_size = args.input.stat().st_size
input_id = re.sub(non_id_char, "_", args.input.name.replace(".", "__"))

def ensure_usable(p: Path):
    p.parent.mkdir(exist_ok=True, parents=True)
    p.touch()

ensure_usable(Path(args.source))
ensure_usable(Path(args.header))

# gen header
with open(args.header, mode="w") as header:
    print(dedent(f"""\
    #ifndef _PANCAKE_RESOURCE_{input_id}_
    #define _PANCAKE_RESOURCE_{input_id}_
    #include "pancake/resource/rsrc_def.hpp"
    
    namespace pancake::resources {{
      extern resource<{input_size}> {input_id};
    }}
    #endif
    """), file=header, end="")

with open(args.input, mode="rb") as infile, open(args.source, mode="w") as source:
    print(dedent(f"""\
    // {args.input.name}
    #include "{args.header.name}"
    namespace pancake::resources {{
      pancake::resources::resource<{input_size}> {input_id} = {{
    """), file=source, end="")
    
    block = b""
    while (block := infile.read(16)) != b"":
        c_code = "    " + "".join([f"'\\x{byte:02x}', " for byte in block])
        print(c_code, file=source)
    print(dedent(f"""\
      }};
    }}
    """), file=source, end="")
