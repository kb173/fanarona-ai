# Fanorona AI

C++ AI for Fanorona using alpha-beta-pruning

## Installation

Run `tools/install-deps.sh` after cloning this repository.

## Multi-run

Run `tools/run-multiple.sh [number of games]`

## Linting and formatting

Run `tools/format.sh` to fix style and naming issues.

## Comment translation

Run `python3 tools/translate_comments.py [filename ...]` to translate all comments from English to German.

The Translate package is required for this: `pip3 install translate`

## Code analyzation

### valgrind

All major linux distributions will include valgrind in their repositories, if not try `apt install valgrind` or contact your local unix guru :p

Run `tools/run-valgrind.sh` for overview analyzation of unfreed memory.

If you need more details, you can run `tools/run-valgrind_verbose.sh` (but be aware this is much slower!)

### gprof

Make sure the program was build with ``-pg`` flag in c++ compiler flags.

If the program exited normally, a "gmon.out" file should have been generated,

to put this in readable form run ``tools/build-gprof-analysis.sh`` 
