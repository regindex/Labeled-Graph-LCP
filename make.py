#!/usr/bin/env python3

import sys, subprocess, os

arg = sys.argv[1]

if(arg == "install"):
	print("###### Installing sdsl-lite...")
	os.chdir("external/sdsl-lite")
	subprocess.call("mkdir install_dir".split())
	subprocess.call("./install.sh install_dir".split())
	os.chdir("../../")
	print("###### Installing Wheeler-Automaton-LCS...")
	subprocess.call("mkdir build".split())
	os.chdir("build")
	subprocess.call("cmake ..".split())
	subprocess.call("make".split())
if(arg == "clean"):
	print("###### Cleaning sdsl-lite install...")
	subprocess.call("rm -r external/sdsl-lite/install_dir".split())
	print("###### Cleaning Wheeler-Automaton-LCS install...")
	subprocess.call("rm -r build".split())
if(arg == "update"):
	print("###### Updating Wheeler-Automaton-LCS install...")
	subprocess.call("make -C build".split())