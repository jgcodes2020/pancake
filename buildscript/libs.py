import sys
from pathlib import Path

def eprint(*args, **kwargs):
    print(*args, file = sys.stderr, flush = True, **kwargs)

lib_dirs = []
with open(Path(sys.argv[1]).joinpath('conanbuildinfo.txt')) as binfo:
    eprint("conan info opened")
    line = ""
    while (line := binfo.readline()) != "[libdirs]\n":
        pass
    eprint("found static lib dirs")
    while (line := binfo.readline()) != "\n":
        stripped = line.strip()
        contents = list(Path(stripped).resolve().iterdir())
        for lib in contents:
            print("{dir}\t{name}".format(dir=stripped, name=lib.stem.split(".", 1)[0]))