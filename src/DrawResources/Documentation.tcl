# Copyright (c) 1999-2014 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and / or modify it
# under the terms of the GNU Lesser General Public version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

#
# Documentation.tcl
#
# TCL procedures to process documentation files
#



#
# Read a file

# The array subTitles binds the subTitles for each title (or empty list)
# The array texts     binds the text for each title
#
# Titles are of the form {Title subTitle subTitle}
#

proc readFile {filename} {
    global subTitles texts
    if {! [file readable $filename]} return

    foreach ar {subTitles texts} {if [info exists $ar] {unset $ar}}
    set title  "Top"
    set stitle "Top"
    set subTitles($stitle) {}
    set level 1
    set f [open $filename r]
    while {[gets $f line] >= 0} {

	if [regexp {^[ ]*(\*+)[ ]*(.*$)} $line dummy s t] {
	    
	    # it is a new title
	    set l [string length $s]

	    # at a deepest level go down enough levels
	    while {$level < $l} {
		set up($title) $stitle
		set stitle $title
		lappend title $t
		incr level
	    }

	    # at an upper level go up enough level
	    while {$level > $l} {
		set title stitle
		set stitle $up($stitle)
		incr level -1
	    }
	    
	    # at the current level
	    lappend subTitles($stitle) $t
	    set title [concat $stitle $t]
	    set texts($title) ""

	} else {

	    # it is a line for the current title
	    lappend texts($title) $line
	}
    }
    close $f
}

#
# call on each title : titleProc title level
# call on each text  : textProc text

proc dump {titleProc textProc {title Top} {level 1}} {
    global subTitles texts
    if [info exists texts($title)] {$textProc $texts($title)}
    if [info exists subTitles($title)] {
	set l $level
	incr l
	foreach t $subTitles($title) {
	    $titleProc $t $level
	    dump $titleProc $textProc [concat $title $t] $l
	}
    }
}

# cut a text into sections
# and call on  each section  : sectionProc section text

proc sectionText {aText aSectionProc} {
    set section ""
    set text    {}
    foreach line $aText {
	
	if {[string index $line 0] == "."} {
	    
	    $aSectionProc $section $text
	    set section $line
	    set text {}
	} else {
	    lappend text $line
	}
    }
    $aSectionProc $section $text
}

