#!/usr/bin/python3
from argparse import *

parser = ArgumentParser(description='''
Merges static libraries into one, in a cross-platform manner.

MSVC ABI:
Requires at minimum VS Build Tools if not full VS.

Itanium/GCC ABI:
Requires access to ar with .mri scripts.
''')