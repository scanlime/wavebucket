#!/bin/sh
#
# Simple driver script, piping audio in from mplayer.

set -e

DEFAULT="$HOME/Dropbox/iTunes/iTunes Music/Owl City/Of June/03 Designer Skyline.m4a"

INPUT=$1
: ${INPUT:=$DEFAULT}

make

mplayer "$INPUT" \
    -really-quiet \
    -af channels=2,resample=44100,format=s16le \
    -vc null -vo null \
    -ao pcm:nowaveheader:fast:file=/dev/stdout \
    | ./viz
