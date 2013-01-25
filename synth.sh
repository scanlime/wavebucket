#!/bin/sh
#
# Uses sox to synthesize audio for input.
#
# For example:
#   ./synth.sh 1000 sin %-1 gain -20
#

set -e

make

sox -r 44100 -c 2 -b 16 -s -n -t raw - synth "$@" | ./viz 
