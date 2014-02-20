# Copyright (c) 1999-2014 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

###########################
#                         #
# Version 1.0             #
# by FID                  #
#                         #
###########################
#
# Open         : double-clic or +
# Close        : double-clic or -
# Refresh tree : close top level and re-open
#
# Attributes:
# white     : interoperable
# white + c : non interoperable
# X         : X Reference
# Red       : not yet commited in transaction #0
# 
#

proc __update { args } {
}

global TreeBrowser

proc treebrowser { TreeBrowser } {

    global TREE_WINDOWS
    global TREE_LabelStyle
    global TREE_LabelStyle1
    global TREE_AttributeStyle
    global TREE_AttributeStyle1
    global TREE_AttributeStyle2

    global $TreeBrowser

    package require Tix
    tixPref:InitFontSet:14Point

    set TREE_LabelStyle [tixDisplayStyle imagetext \
	    -font 9x15bold \
	    -background Bisque3 \
	    ]

    set TREE_LabelStyle1 [tixDisplayStyle imagetext \
	    -font 9x15bold \
	    -background Bisque3 \
	    -foreground SeaGreen2 \
	    ]

    set TREE_AttributeStyle [tixDisplayStyle imagetext \
	    -font 9x15 \
	    -background Bisque3 \
	    ]

    set TREE_AttributeStyle1 [tixDisplayStyle imagetext \
	    -font 9x15 \
	    -background Bisque3 \
	    -foreground Green \
	    ]

    set TREE_AttributeStyle2 [tixDisplayStyle imagetext \
	    -font 9x15 \
	    -background Bisque3 \
	    -foreground Red \
	    ]

    set w .$TreeBrowser
    toplevel $w

    set top [frame $w.thu -bd 1 -relief raised -background Bisque3]

    ################
    # Paned Window #
    ################
    set p [tixPanedWindow $top.p -orient horizontal -height 400 -width 700]
    pack $p -expand yes -fill both -padx 4 -pady 4

    TREE:Tree:InitTreePanel $TreeBrowser $w $p
    #TREE:Tree:InitTextPanel $TreeBrowser $w $p

    tixForm $top -left 2 -top 2 -right %99 -bottom %99

    return 
}

###############################################################################
#
#
proc TREE:Tree:InitTreePanel { TreeBrowser w p } {
    global TREE_WINDOWS
    global $TreeBrowser 

    ########
    # Tree #
    ########

    set p1 [$p add pane1 -expand 1 -size 700] ; $p1 config -relief flat 
    set tree  [tixTree $p1.tree \
	    -opencmd  [list TREE:Tree:Open $TreeBrowser $w] \
	    -options { \
	    hlist.separator "^" \
	    hlist.font 9x15bold \
	    hlist.background Bisque3 \
	    hlist.foreground Black \
	} ]
    pack $p1.tree -expand yes -fill both -padx 4 -pady 4

    # -browsecmd TREE:Tree:BrowseCmd
    # Cette option peut etre ajoutee a la commande tixTree,
    # mais elle declanche l'appel a la procedure a chaque clic , 
    # double-clic, voire a chaque appui puis relachement de souris!!!

    # to see different fonts: /usr/openwin/lib/X11/fonts/misc or xlsfonts?
    # 8x13 8x13bold 9x15 9x15bold
    #	    hlist.font 8x13bold
    #	    hlist.gap "15"
    #	    hlist.indent "30"

    set TREE_WINDOWS($w,NAV,tree)   $tree
    set TREE_WINDOWS($w,NAV,hlist)  [$tree subwidget hlist]
    $TREE_WINDOWS($w,NAV,hlist) add ^ \
	    -text $TreeBrowser \
	    -data [list $TreeBrowser Root]
    $TREE_WINDOWS($w,NAV,tree)  setmode ^ open
}

###############################################################################
#
#
proc TREE:Tree:InitTextPanel { TreeBrowser w p } {
    global TREE_WINDOWS
    global $TreeBrowser

    ########
    # Text #
    ########
    set p2 [$p add pane2 -expand 4 -size 400] ; $p2 config -relief flat
    tixScrolledText $p2.st 
    pack $p2.st   -expand yes -fill both -padx 4 -pady 4	
    
    set TREE_WINDOWS($w,NAV,text)   [$p2.st subwidget text]



    $TREE_WINDOWS($w,NAV,text) insert end " Welcome to the QDF browser (Rev #.#)\n"
    $TREE_WINDOWS($w,NAV,text) insert end "--------------------------------------\n\n"
    $TREE_WINDOWS($w,NAV,text) insert end "This browser is an easy to use prototype made with Tix technology. We hope it will be usefull for understanding and debugging QDF.\n"
    $TREE_WINDOWS($w,NAV,text) insert end "\t\t\t\tFID & YAN"
}

###############################################################################
#
#
proc TREE:Tree:BrowseCmd { dir } {
    puts "Hello $dir !"
}
###############################################################################
# Se positionne sur l'entry pere et update les fenetres.
#    
proc FCTREE:Tree:Up { w } {
    global TREE_WINDOWS
    global TREE_GLOBALS
    #puts "TREE:Tree:Up"

    if { [set here [$TREE_WINDOWS($w,NAV,hlist) info anchor]] != {} } {
	if { [set up [$TREE_WINDOWS($w,NAV,hlist) info parent $here]] != {} } {
	    TREE:Tree:ShowUp $w $up
	    set TREE_GLOBALS(CWD) [lindex [$TREE_WINDOWS($w,NAV,hlist) info data $up] 0]
	} 
    } 
    return
}
###############################################################################
# Se positionne sur l'entry up sans update (History) 
#    
proc TREE:Tree:ShowUp { w dir } {
    global TREE_WINDOWS
    #puts "TREE:Tree:ShowUp"

    $TREE_WINDOWS($w,NAV,hlist) anchor clear
    $TREE_WINDOWS($w,NAV,hlist) anchor set $dir
    $TREE_WINDOWS($w,NAV,hlist) selection clear
    $TREE_WINDOWS($w,NAV,hlist) selection set $dir
    $TREE_WINDOWS($w,NAV,hlist) see $dir
    return
}
###############################################################################
#
#
proc TREE:Tree:Open { TreeBrowser w dir} {
    global TREE_WINDOWS
    global $TreeBrowser
    #puts "TREE:Tree:Open"

    if {$dir == "^"} {
	# This is root
	if {[$TREE_WINDOWS($w,NAV,hlist) info children $dir] != {}} {
	    # The root branch already exists in hlist.
	    # Clear all its children to force the tree to be updated.
	    foreach kid [$TREE_WINDOWS($w,NAV,hlist) info children $dir] {
		$TREE_WINDOWS($w,NAV,hlist) delete entry $kid
	    }
	}
    }
    
    if {[$TREE_WINDOWS($w,NAV,hlist) info children $dir] != {}} {
	# The branch exists in hlist.
	foreach kid [$TREE_WINDOWS($w,NAV,hlist) info children $dir] {
	    $TREE_WINDOWS($w,NAV,hlist) show entry $kid
	}
	set data [$TREE_WINDOWS($w,NAV,hlist) info data $dir]
	#set loc  [lindex $data 0]
    } else {
	# The branch is unknown.
	tixBusy $w on
	update
	TREE:Tree:Fill $TreeBrowser $w $dir
	tixBusy $w off
    }
    return
}
###############################################################################
#
#
proc TREE:Tree:Fill { TreeBrowser w  dir } {
    global TREE_WINDOWS
    global TREE_GLOBALS
    global $TreeBrowser
    #puts "TREE:Tree:Fill"
    
    set data [$TREE_WINDOWS($w,NAV,hlist) info data $dir]
    #set loc  [lindex $data 0]
    set type [lindex $data 1]

    #puts "====================="
    #puts "Type $type"
    #puts "Window $w"
    #puts "Loc $loc"
    #puts "Dir $dir"
    #puts "====================="

    switch -glob $type {
	
	Root {
	    TREE:Tree:UpdateRoot $TreeBrowser $w $dir
	}
	
	Node {
	    set lab [lindex $data 0]
	    TREE:Tree:UpdateNode $TreeBrowser $w $dir $lab
	}
	
	terminal {
	    TREE:Tree:terminal $TreeBrowser $w $dir
	}
	
	default {
	    puts "type non reconnu"
	}
    }
    return
}
###############################################################################
# ici dir = ^
#
proc TREE:Tree:UpdateRoot  { TreeBrowser w dir } {
    global TREE_WINDOWS
    global $TreeBrowser
    global TREE_AttributeStyle
    global TREE_AttributeStyle1
    #puts "TREE:Tree:UpdateRoot"

    foreach nodeItem [split [OpenNode $TreeBrowser ""] "\\" ] {

	TREE:Tree:DecodeNodeItem $TreeBrowser $w $dir $nodeItem

    }

    return
}
###############################################################################
# 
#
proc TREE:Tree:UpdateNode  { TreeBrowser w dir lab} {
    global TREE_WINDOWS
    global $TreeBrowser
    global TREE_AttributeStyle
    global TREE_AttributeStyle1
    #puts "TREE:Tree:UpdateNode"

    foreach nodeItem [split [OpenNode $TreeBrowser $lab ] "\\" ] {

	TREE:Tree:DecodeNodeItem $TreeBrowser $w $dir $nodeItem

    }
    return
}
###############################################################################
# item:
# "LabelEntry "Name" DynamicType Executable|Forgotten Failed|Success First|Null [ LabelFather]"
proc TREE:Tree:DecodeNodeItem { TreeBrowser w dir nodeItem} {
    global TREE_WINDOWS
    global $TreeBrowser
    global TREE_AttributeStyle
    global TREE_AttributeStyle1
    global TREE_AttributeStyle2
    #puts "TREE:Tree:DecodeNodeItem"

    set litm {}
    set standardimage  [tix getimage file]
    set forgottenimage [tix getimage maximize]
    set image $standardimage

    # Information first split
    set tmplist [split $nodeItem " "]
    set labentry    [lindex $tmplist 0]
    set name        [lindex $tmplist 1]
    set type        [lindex $tmplist 2]
    #set exec        [lindex $tmplist 3]
    #set state       [lindex $tmplist 4]
    set children    [lindex $tmplist 3]
    set father      [lindex $tmplist 4]
    set first       [lindex $tmplist 5]
    set next        [lindex $tmplist 6]
    set previous    [lindex $tmplist 7]

    #puts "labentry : $labentry"
    #puts "name : $name"
    #puts "type : $type"
    #puts "state : $state"
    #puts "exec : $exec"
    #puts "children : $children"
    #puts "father : $father"
    #puts "first : $first"
    #puts "next : $next"
    #puts "previous : $previous"

    # Label entry , name & Dynamic type.
    set ll [expr [string length $name] -2]
    if {$ll > 0} {
	set textname "$labentry [string range $name 1 $ll] $type"
    } else {
	set textname "$labentry $type"
    }

    set locstyle $TREE_AttributeStyle

    # Executable/Forgotten?
    #if {$exec == "Forgotten"} {
	#set textname "$textname $exec"
	#set locstyle $TREE_AttributeStyle1
	#set image $forgottenimage
    #}

    # Failed/Success analysis.
    #if {$state == "Failed"} {
	#set textname "$textname $state"
	#set locstyle $TREE_AttributeStyle2
    #}

    # Father?
    if {$father != "Null"} {set textname "$textname Father=$father"}
    # First?
    if {$first != "Null"} {set textname "$textname First=$first"}
    # Next?
    if {$next != "Null"} {set textname "$textname Next=$next"}
    # Previous?
    if {$previous != "Null"} {set textname "$textname Previous=$previous"}

    $TREE_WINDOWS($w,NAV,hlist) add ${dir}^${labentry} \
	    -itemtype imagetext \
	    -text $textname \
	    -image $image \
	    -style $locstyle \
	    -data  [list ${labentry} Node]
    if {$children == "First"} {
	$TREE_WINDOWS($w,NAV,tree) setmode ${dir}^${labentry} open
    }
    lappend litm [list $labentry $image]

    return
}
###############################################################################
#
#
proc TREE:Tree:terminal { TreeBrowser w dir } {
    global TREE_WINDOWS
    global $TreeBrowser
    return
}
###############################################################################
# 
#
proc TREE:Tree:DisplayAttribute { TreeBrowser w dir } {
    global TREE_WINDOWS
    global $TreeBrowser

    return
}
###############################################################################
# imprime tout ce qu'il y a dans hli ( Hlist )
#
proc wokDBG { {root {}} } {
    global TREE_GLOBALS
    global TREE_WINDOWS
    set w $TREE_GLOBALS(toplevel)
    set hli $TREE_WINDOWS($w,NAV,hlist)
    foreach c [$hli info children $root] {
	puts "$c : data <[$hli info data $c]>"
	wokDBG $c
    }
    return
}
