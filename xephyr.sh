#!/bin/sh

# Need to unset because of bug see README
# This is why the Xephyr command cannot be run in the Makefile
# we have to unset a exported variable which I do not know how to do/is not
# possible in a Makefile
unset XDG_SEAT

xinit ./wmisbloat -- /usr/bin/Xephyr :1

# X in Xephyr must be capitilized
# After much trial and error this does not seem to work
# Xephyr -screen "1024x650" :1
# DISPLAY=:1 ./wmisbloat
# DISPLAY=:1 xterm
