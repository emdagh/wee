#!/usr/bin/env python

import sys, os, subprocess

from os import *
from os.path import isfile, join

pathIn = sys.argv[1:]
extOut = sys.argv[-1]

files = [f for f in pathIn]

cwd = os.path.dirname(os.path.abspath(__file__))

_ = [subprocess.call([join(cwd, 'convert'), f, os.path.splitext(f)[0] + extOut]) for f in files[:-1]]


'''for f in files:
    print f
    print os.path.splitext(f)[0] + extOut
    print join(cwd, 'convert')
    subprocess.call([join(cwd, "convert"), f, os.path.splitext(f)[0]+extOut])
    exit()

print files
print extOut;
'''

