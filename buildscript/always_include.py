#!/usr/bin/env python
from sys import argv, stderr
import argparse

def eprint(*args, **kwargs):
    print(*args, file=stderr, **kwargs)

parser = argparse.ArgumentParser(
    description="Prepends include directives into a source file"
)
parser.add_argument('source',
    metavar="<source>",
    help="The source file to manipulate"
)
parser.add_argument('-o', "--output", 
    metavar="<outfile>",
    type=str,
    help="The output to use."
)
parser.add_argument('-H', '--headers',
    nargs="+", metavar="<headers>",
    required=True, type=str,
    help="The headers to include"
)
args = parser.parse_args(argv[1:])

code = ""
with open(args.source, "r") as src:
    code = src.read()

eprint("args.output:", args.output)
eprint("args.source:", args.source)

output = args.output if args.output != None else args.source
with open(output, "w") as src:
    for header in args.headers:
        print("#include <", header, ">", sep="", file=src)
    print(code, end="", file=src)