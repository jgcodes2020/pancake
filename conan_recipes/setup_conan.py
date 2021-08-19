#!/usr/bin/env python
import subprocess as subp
import shlex
import sys
from pathlib import Path
import conans.client.conan_api as conan

# conan inspect will error if the package doesn't exist
# the package with this user should never exist on CCI
def setup_pkg(name: str, dirname: str):
    if subp.run(
        ["conan", "inspect", name],
        stdout=subp.DEVNULL, stderr=subp.STDOUT).returncode != 0:
        print("Creating package for libdwarf/0.1.1...")
        subp.run(
            ["conan", "export", f"./{dirname}", name],
            cwd=Path(__file__).parent)

setup_pkg("libdwarf/0.1.1@jgcodes2020/stable", "libdwarf")
setup_pkg("LIEF/0.11.5@jgcodes2020/stable", "LIEF")
setup_pkg("mbedtls/2.25.0@jgcodes2020/stable", "mbedtls")