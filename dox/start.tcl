#!/usr/bin/tclsh

# Command-line starter for occdoc command, use it as follows:
# tclsh> source dox/start.tcl [arguments]

source [file join [file dirname [info script]] occdoc.tcl]
occdoc {*}$::argv
