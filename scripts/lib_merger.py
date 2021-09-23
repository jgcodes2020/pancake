#!/usr/bin/python3
from pathlib import Path
from argparse import *
import platform
import subprocess as proc
import sys


class ExistingFileAction(Action):
    def __call__(self, parser, namespace, values, opt_str=None) -> None:
        def process(v: str) -> Path:
            result = Path(v).resolve()
            if result.is_file():
                return result
            else:
                raise ArgumentTypeError(f"path_file: {v} is not a file")
        if type(values) is list:
            setattr(namespace, self.dest, [process(p) for p in values])
        else:
            setattr(namespace, self.dest, [process(values)])


class NewFileAction(Action):
    def __call__(self, parser, namespace, values, opt_str=None) -> None:
        def process(v: str) -> Path:
            result = Path.cwd().joinpath(Path(v)).resolve()
            if (result.is_dir()):
                raise ArgumentTypeError(
                    f"{v} is a directory, can't write file")
            else:
                return result
        if type(values) is list:
            setattr(namespace, self.dest, [process(v) for v in values])
        else:
            setattr(namespace, self.dest, [process(values)])


parser = ArgumentParser(description='''
Merges static libraries into one, in a cross-platform manner.

MSVC ABI:
Requires at minimum VS Build Tools if not full VS.

Itanium/GCC ABI:
Requires access to ar with .mri scripts.
''')
parser.add_argument("-o", "--output",
                    action=NewFileAction,
                    dest="output",
                    help="Output library.",
                    metavar="<output.lib>"
                    )
parser.add_argument("libs",
                    nargs='+',
                    action=ExistingFileAction,
                    help="The libraries to merge.",
                    metavar="<input.lib>"
                    )
args = parser.parse_args()
del parser


def lib_exe_merge():
    vswhere_exe = str(Path(__file__).parent.joinpath("vswhere.exe"))
    vswhere_proc = proc.run(
        [vswhere_exe, "-latest", "-products", "*",
         "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
         "Microsoft.VisualStudio.Component.Windows10SDK.19041",
         "-property", "installationPath"],
        capture_output=True, encoding="utf-8"
    )
    vswhere_proc.check_returncode()
    vs_path = Path(vswhere_proc.stdout.strip())

    lib_exe = str(vs_path.joinpath("VC\\Tools\\MSVC\\14.29.30133\\bin\\Hostx64\\x64\\lib.exe"))
    lib_cmd: list[str] = [lib_exe]
    try:
        lib_output = f"/OUT:{str(args.output[0])}"
        lib_cmd.append(lib_output)
    except AttributeError:
        pass
    lib_cmd += [str(p) for p in args.libs]
    print(f"lib_cmd: {lib_cmd}")
    lib_proc = proc.run(
        lib_cmd
    )
    sys.exit(lib_proc.returncode)

def ar_mri_merge():
    pass


if platform.system() == "Windows":
    lib_exe_merge()
elif platform.system() == "Linux":
    ar_mri_merge()
