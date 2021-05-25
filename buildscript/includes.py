import sys
from pathlib import Path

def eprint(*args, **kwargs):
    print(*args, file = sys.stderr, flush = True, **kwargs)

includes = []
with open(Path(sys.argv[1]).joinpath('conanbuildinfo.txt')) as binfo:
    eprint("conan info opened")
    line = ""
    while (line := binfo.readline()) != "[includedirs]\n":
        pass
    eprint("found include dirs")
    while (line := binfo.readline()) != "\n":
        print(line, end="")
    