###########################
#                         #
# Version 1.3             #
# by FID & YAN            #
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

proc dftree { DDF_Browser } {

    global DFTREE_WINDOWS
    global DFTREE_LabelStyle
    global DFTREE_LabelStyle1
    global DFTREE_AttributeStyle
    global DFTREE_AttributeStyle1
    global DFTREE_AttributeStyle2

    global $DDF_Browser
##    global $DDF_Browser
    puts $DDF_Browser

    package require Tix
    #tixPref:InitFontSet:14Point

    set DFTREE_LabelStyle [tixDisplayStyle imagetext \
	    -font 9x15bold \
	    -background Bisque3 \
	    ]

    set DFTREE_LabelStyle1 [tixDisplayStyle imagetext \
	    -font 9x15bold \
	    -background Bisque3 \
	    -foreground SeaGreen2 \
	    ]

    set DFTREE_AttributeStyle [tixDisplayStyle imagetext \
	    -font 9x15 \
	    -background Bisque3 \
	    ]

    set DFTREE_AttributeStyle1 [tixDisplayStyle imagetext \
	    -font 9x15 \
	    -background Bisque3 \
	    -foreground DarkGreen \
	    ]

    set DFTREE_AttributeStyle2 [tixDisplayStyle imagetext \
	    -font 9x15 \
	    -background Bisque3 \
	    -foreground Red \
	    ]

    set w .$DDF_Browser
    toplevel $w

    set top [frame $w.thu -bd 1 -relief raised -background Bisque3]

    ################
    # Paned Window #
    ################
    set p [tixPanedWindow $top.p -orient horizontal -height 400 -width 700]
    pack $p -expand yes -fill both -padx 4 -pady 4

    DFTREE:Tree:InitTreePanel $DDF_Browser $w $p
#    DFTREE:Tree:InitTextPanel $DDF_Browser $w $p

    tixForm $top -left 2 -top 2 -right %99 -bottom %99

    return 
}

###############################################################################
#
#
proc DFTREE:Tree:InitTreePanel { DDF_Browser w p } {
    global DFTREE_WINDOWS
    global $DDF_Browser 

    ########
    # Tree #
    ########

    set p1 [$p add pane1 -expand 1 -size 700] ; $p1 config -relief flat 
    set tree  [tixTree $p1.tree \
	    -opencmd  [list DFTREE:Tree:Open $DDF_Browser $w] \
	    -options { \
	    hlist.separator "^" \
	    hlist.font 9x15bold \
	    hlist.background Bisque3 \
	    hlist.foreground Black \
	} ]
    pack $p1.tree -expand yes -fill both -padx 4 -pady 4

    # -browsecmd DFTREE:Tree:BrowseCmd
    # Cette option peut etre ajoutee a la commande tixTree,
    # mais elle declanche l'appel a la procedure a chaque clic , 
    # double-clic, voire a chaque appui puis relachement de souris!!!

    # to see different fonts: /usr/openwin/lib/X11/fonts/misc or xlsfonts?
    # 8x13 8x13bold 9x15 9x15bold
    #	    hlist.font 8x13bold
    #	    hlist.gap "15"
    #	    hlist.indent "30"

    set DFTREE_WINDOWS($w,NAV,tree)   $tree
    set DFTREE_WINDOWS($w,NAV,hlist)  [$tree subwidget hlist]
    $DFTREE_WINDOWS($w,NAV,hlist) add ^ \
	    -text $DDF_Browser \
	    -data [list $DDF_Browser DataFramework]
    $DFTREE_WINDOWS($w,NAV,tree)  setmode ^ open
}

###############################################################################
#
#
proc DFTREE:Tree:InitTextPanel { DDF_Browser w p } {
    global DFTREE_WINDOWS
    global $DDF_Browser

    ########
    # Text #
    ########
    set p2 [$p add pane2 -expand 4 -size 400] ; $p2 config -relief flat
    tixScrolledText $p2.st 
    pack $p2.st   -expand yes -fill both -padx 4 -pady 4	
    
    set DFTREE_WINDOWS($w,NAV,text)   [$p2.st subwidget text]



    $DFTREE_WINDOWS($w,NAV,text) insert end " Welcome to the QDF browser (Rev #.#)\n"
    $DFTREE_WINDOWS($w,NAV,text) insert end "--------------------------------------\n\n"
    $DFTREE_WINDOWS($w,NAV,text) insert end "This browser is an easy to use prototype made with Tix technology. We hope it will be usefull for understanding and debugging QDF.\n"
    $DFTREE_WINDOWS($w,NAV,text) insert end "\t\t\t\tFID & YAN"
}

###############################################################################
#
#
proc DFTREE:Tree:BrowseCmd { dir } {
    puts "Hello $dir !"
}
###############################################################################
# Se positionne sur l'entry pere et update les fenetres.
#    
proc DFTREE:Tree:Up { w } {
    global DFTREE_WINDOWS
    global DFTREE_GLOBALS
    #puts "DFTREE:Tree:Up"

    if { [set here [$DFTREE_WINDOWS($w,NAV,hlist) info anchor]] != {} } {
	if { [set up [$DFTREE_WINDOWS($w,NAV,hlist) info parent $here]] != {} } {
	    DFTREE:Tree:ShowUp $w $up
	    set DFTREE_GLOBALS(CWD) [lindex [$DFTREE_WINDOWS($w,NAV,hlist) info data $up] 0]
	} 
    } 
    return
}
###############################################################################
# Se positionne sur l'entry up sans update (History) 
#    
proc DFTREE:Tree:ShowUp { w dir } {
    global DFTREE_WINDOWS
    #puts "DFTREE:Tree:ShowUp"

    $DFTREE_WINDOWS($w,NAV,hlist) anchor clear
    $DFTREE_WINDOWS($w,NAV,hlist) anchor set $dir
    $DFTREE_WINDOWS($w,NAV,hlist) selection clear
    $DFTREE_WINDOWS($w,NAV,hlist) selection set $dir
    $DFTREE_WINDOWS($w,NAV,hlist) see $dir
    return
}
###############################################################################
#
#
proc DFTREE:Tree:Open { DDF_Browser w dir} {
    global DFTREE_WINDOWS
    global $DDF_Browser
    #puts "DFTREE:Tree:Open"

    if {$dir == "^"} {
	# This is root
	if {[$DFTREE_WINDOWS($w,NAV,hlist) info children $dir] != {}} {
	    # The root branch already exists in hlist.
	    # Clear all its children to force the tree to be updated.
	    foreach kid [$DFTREE_WINDOWS($w,NAV,hlist) info children $dir] {
		$DFTREE_WINDOWS($w,NAV,hlist) delete entry $kid
	    }
	}
    }
    
    if {[$DFTREE_WINDOWS($w,NAV,hlist) info children $dir] != {}} {
	# The branch exists in hlist.
	foreach kid [$DFTREE_WINDOWS($w,NAV,hlist) info children $dir] {
	    $DFTREE_WINDOWS($w,NAV,hlist) show entry $kid
	}
	set data [$DFTREE_WINDOWS($w,NAV,hlist) info data $dir]
	set loc  [lindex $data 0]
    } else {
	# The branch is unknown.
	tixBusy $w on
	update
	DFTREE:Tree:Fill $DDF_Browser $w $dir
	tixBusy $w off
    }
    return
}
###############################################################################
#
#
proc DFTREE:Tree:Fill { DDF_Browser w  dir } {
    global DFTREE_WINDOWS
    global DFTREE_GLOBALS
    global $DDF_Browser
    #puts "DFTREE:Tree:Fill"
    
    set data [$DFTREE_WINDOWS($w,NAV,hlist) info data $dir]
    set loc  [lindex $data 0]
    set type [lindex $data 1]

    #puts "====================="
    #puts "Type $type"
    #puts "Window $w"
    #puts "Loc $loc"
    #puts "Dir $dir"
    switch -glob $type {
	
	DataFramework {
	    DFTREE:Tree:UpdateDataFramework $DDF_Browser $w $loc $dir
	}
	
	Label { 
	    DFTREE:Tree:UpdateLabel $DDF_Browser $w $loc $dir
	}
	
	AttributeList { 
	    set lab [lindex $data 2]
	    DFTREE:Tree:UpdateAttributeList $DDF_Browser $w $loc $dir $lab
	}
	
	Attribute { 
	    DFTREE:Tree:UpdateAttribute $DDF_Browser $w $loc $dir
	}

	terminal {
	    DFTREE:Tree:terminal $DDF_Browser $w $loc $dir
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
proc DFTREE:Tree:UpdateDataFramework  { DDF_Browser w loc dir } {
    global DFTREE_WINDOWS
    global $DDF_Browser
    global DFTREE_LabelStyle
    global DFTREE_LabelStyle1

    set litm {}

    foreach fullname [split [DFOpenLabel $DDF_Browser ""] "\\" ] {
	FCTREE:Tree:DecodeLabelItem $DDF_Browser $w $loc $dir $fullname
    }

    return
}
###############################################################################
# $loc is a label entry, "1.3.2" for exemple.
#
proc DFTREE:Tree:UpdateLabel  { DDF_Browser w loc dir } {
    global DFTREE_WINDOWS
    global $DDF_Browser
    global DFTREE_LabelStyle
    global DFTREE_LabelStyle1
    global DFTREE_AttributeStyle
    global DFTREE_AttributeStyle1

    set litm {}
    foreach fullname [split [DFOpenLabel $DDF_Browser $loc ] "\\" ] {
	FCTREE:Tree:DecodeLabelItem $DDF_Browser $w $loc $dir $fullname
    }
    return
}
###############################################################################
#
#
proc DFTREE:Tree:UpdateAttributeList { DDF_Browser w loc dir lab} {
    global DFTREE_WINDOWS
    global $DDF_Browser
    global DFTREE_AttributeStyle
    global DFTREE_AttributeStyle1

    set litm {}
    set standardimage [tix getimage srcfile]
    set otherimage    [tix getimage file]
    set xrefimage     [tix getimage cross]

    # abv: index attributes
    set num 0
    set attributes [split [DFOpenAttributeList $DDF_Browser $lab ] "\\" ]
    set iattributes {}
    foreach fullname $attributes {
	set num [expr $num + 1]
	lappend fullname $num
	lappend iattributes $fullname
    }
    foreach fullname [lsort $iattributes] {

	# Information first split
	set tmplist [split $fullname " "]
	set name        [lindex $tmplist 0]
	set transaction [lindex $tmplist 1]
	set valid       [lindex $tmplist 2]
	set forgotten   [lindex $tmplist 3]
	set backuped    [lindex $tmplist 4]
	set maybeopen   [lindex $tmplist 5]

	# Package analysis to determine the icon type.
	set pk [lindex [split $name _] 0]
	set image $otherimage
	if {$pk == "TDataStd" || $pk == "TNaming"} {set image $standardimage}
	if {$pk == "TXRef"} {set image $xrefimage}

	# Name analysis to suppress the map address.
	set shortlist [split $name "#"]
	set shortname [lindex $shortlist 0]
	set index     [lindex $shortlist 1]

	set textname "$shortname"

#	if { [llength $tmplist] >5 } { set textname [lindex $tmplist 5] }
	set textname "$textname [DFGetAttributeValue $DDF_Browser $lab [lindex $tmplist 6]]"

	# Transaction analysis
	if {$transaction == "0"} {
	    set locstyle $DFTREE_AttributeStyle
	} else {
#	    set textname "$textname T=$transaction"
	    set locstyle $DFTREE_AttributeStyle1
	}

	# Valid?
	if {$valid == "NotValid"} {set textname "$textname $valid"}

	# Forgotten?
	if {$forgotten == "Forgotten"} {set textname "$textname $forgotten"}

	# Backuped?
	if {$backuped == "Backuped"} {set textname "$textname $backuped"}


	$DFTREE_WINDOWS($w,NAV,hlist) add ${dir}^${name} \
		-itemtype imagetext \
		-text $textname \
		-image $image \
		-style $locstyle \
		-data  [list ${index} Attribute]
	if {$maybeopen == "1"} {
	    $DFTREE_WINDOWS($w,NAV,tree) setmode ${dir}^${name} open
	}
	lappend litm [list $name $image]
    }
    return
}
###############################################################################
# $loc is always the attribute index
#
proc DFTREE:Tree:UpdateAttribute { DDF_Browser w loc dir } {
    global DFTREE_WINDOWS
    global $DDF_Browser
    global DFTREE_AttributeStyle
    global DFTREE_AttributeStyle1
    global DFTREE_AttributeStyle2

    set litm {}
    set image [tix getimage minimize]
    set tmplist [split [DFOpenAttribute $DDF_Browser $loc ] "\\"]

    # Failed or not?
    if {[lindex $tmplist 0] == "Failed"} {
	set locstyle $DFTREE_AttributeStyle2
    } else {
	set locstyle $DFTREE_AttributeStyle
    }

    foreach name $tmplist {
	$DFTREE_WINDOWS($w,NAV,hlist) add ${dir}^${name} \
		-itemtype imagetext \
		-text $name \
		-image $image \
		-style $locstyle \
		-data  [list ${loc}:${name} terminal] \
		-state disabled
	#$DFTREE_WINDOWS($w,NAV,tree) setmode ${dir}^${name} open
	lappend litm [list $name $image]
    }
    return
}
###############################################################################
# item:
# "Entry Name=LeNomSIlExiste Modified|NotModified 0|1"
proc FCTREE:Tree:DecodeLabelItem { DDF_Browser w loc dir labelItem} {
    global DFTREE_WINDOWS
    global $DDF_Browser
    global DFTREE_LabelStyle
    global DFTREE_LabelStyle1
    global DFTREE_AttributeStyle
    global DFTREE_AttributeStyle1

    set image [tix getimage folder]

    set tmplist [split $labelItem " " ]
    set labentry  [lindex $tmplist 0]
    set name      [lindex $tmplist 1]
    set modified  [lindex $tmplist 2]
    set maybeopen [lindex $tmplist 3]

    set textname "$labentry"

    if {$labentry == "AttributeList"} {
	# Attribute List
	# --------------

	set modified  [lindex $tmplist 1]

	# Modified or not?
	if {$modified == "Modified"} {
	    set textname "$textname $modified"
	    set locstyle $DFTREE_AttributeStyle1
	} else {
	    set locstyle $DFTREE_AttributeStyle
	}

	set image [tix getimage textfile]
	$DFTREE_WINDOWS($w,NAV,hlist) add ${dir}^$labentry \
		-itemtype imagetext \
		-text $textname \
		-image $image \
		-style $locstyle \
		-data  [list $labentry AttributeList $loc]
	$DFTREE_WINDOWS($w,NAV,tree) setmode ${dir}^$labentry open
	lappend litm [list $labentry $image]
	set image [tix getimage folder]

    } else {
	# Sub-label(s)
	# ------------

	# Name?
	set ll [expr [string length $name] -2]
	if {$ll > 0} {
	    set textname "$textname [string range $name 1 $ll]"
	}

	# Modified or not?
	if {$modified == "Modified"} {
	    set textname "$textname $modified"
	    set locstyle $DFTREE_LabelStyle1
	} else {
	    set locstyle $DFTREE_LabelStyle
	}

	$DFTREE_WINDOWS($w,NAV,hlist) add ${dir}^$labentry \
		-itemtype imagetext \
		-text $textname \
		-image $image \
		-style $locstyle \
		-data  [list $labentry Label]
	if {$maybeopen == "1"} {
	    $DFTREE_WINDOWS($w,NAV,tree) setmode ${dir}^$labentry open
	}
	lappend litm [list $labentry $image]
    }
}
###############################################################################
#
#
proc DFTREE:Tree:terminal { DDF_Browser w loc dir } {
    global DFTREE_WINDOWS
    global $DDF_Browser
    return
}
###############################################################################
# loc est une adresse de label?
#
proc DFTREE:Tree:DisplayAttribute { DDF_Browser w loc dir } {
    global DFTREE_WINDOWS
    global $DDF_Browser

    return
}
###############################################################################
# imprime tout ce qu'il y a dans hli ( Hlist )
#
proc wokDBG { {root {}} } {
    global DFTREE_GLOBALS
    global DFTREE_WINDOWS
    set w $DFTREE_GLOBALS(toplevel)
    set hli $DFTREE_WINDOWS($w,NAV,hlist)
    foreach c [$hli info children $root] {
	puts "$c : data <[$hli info data $c]>"
	wokDBG $c
    }
    return
}

proc DFGetAttributeValue {browser lab index} {
    global $browser; # necessary for DRAW command to see the browser
#puts "$lab $index"
#    set names [split $dir ^]
#    set len [expr [llength $names] -1]
#    set lab [lindex $names [expr $len - 2]]
#    set num [lindex $names $len]
#    regexp {browser_(.*)} $browser whole doc
#    XSAttributeValue browser_D a b
    if {[catch "XAttributeValue $browser $lab $index" ret]} {
	return ""
    }
    if {"$ret" == ""} { return "" }
    return "\[$ret\]"
}

