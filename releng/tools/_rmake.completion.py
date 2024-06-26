#!/usr/bin/env python3

import sys
import os
from common import *

flags = [
    "--all",
    "--list",
    "--doc",
    "--help",
    "--test",
    "--tidy"
    ]

word = int(sys.argv[1])
words = sys.argv[3:]
test = False

if len(words) > 0 and words[0].startswith("--"):
    if word == 1:
        candidates = [ s for s in flags if s.startswith(words[0]) ]
        if len(candidates) > 0:
            print("\n".join(candidates))
    if words[0] not in ["--test", "--tidy"]:
        sys.exit(0)
    else:
        test = words[0] == "--test"

candidates = []
if test and word == 2:
    candidates += ["--all"]
if word == 1:
    candidates += flags
if "--tidy" in words:
    for origin, dirs, files in os.walk(root):
        for f in files:
            if os.path.splitext(f)[1] not in [".c", ".cpp"]:
                continue
            path = os.path.join(origin, f)
            path = os.path.relpath(path, root)
            if path.startswith("build"):
                continue
            candidates.append(path)


if test:
    targets = availableTestTargets()
else:
    targets = availableTargets()

candidates += [t.target for t in targets]
candidates += [t.path for t in targets]

suites = configuredSuites()
candidates += suites
if not test:
    candidates += ["clean-" + s for s in suites]
    candidates += ["clean"]


if word >= 1 and word - 1 < len(words):
    candidates = [ s for s in candidates if s.startswith(words[word - 1]) ]
candidates = set(candidates).difference(words)

if len(candidates) > 0:
    print("\n".join(candidates))
