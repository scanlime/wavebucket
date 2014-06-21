#!/bin/sh
#
# Simple driver script, piping audio in from the microphone.

set -e

make

rec --buffer 256 -r 44100 -b 16 -s -t raw - | ./viz
