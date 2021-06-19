from pathlib import Path
import os
import json

cc = Path(os.environ["MESON_BUILD_ROOT"]).joinpath("compile_commands.json")
exist = "exists" if cc.is_file() else "does not exist"