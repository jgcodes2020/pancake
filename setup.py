#!/usr/bin/env python

import subprocess
import sys
proc = subprocess.run(['python', 'scripts/setup.py'])
sys.exit(proc.returncode)