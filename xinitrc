#!/usr/bin/env bash
xsetroot -solid "#353535"
xrandr --output VGA1 --left-of HDMI1 # set dual screen
				     # get a list of display from xrandr -q
xsetroot -cursor_name  left_ptr
exec /home/anywhere-dev01/NetBeansProjects/tiny/dist/Debug/GNU-Linux-x86/tiny
