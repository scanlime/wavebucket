#!/bin/sh
#
# Simple driver script, piping audio in from mplayer.

set -e

make

mplayer "$@" \
    -really-quiet \
    -af channels=2,resample=44100,format=s16le \
    -vc null -vo null \
    -ao pcm:nowaveheader:fast:file=/dev/stdout \
    | ./viz
