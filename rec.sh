#!/bin/sh
#
# Simple driver script, piping audio in from the microphone.

set -e

make

rec -r 44100 -b 16 -s -t raw - | ./viz
