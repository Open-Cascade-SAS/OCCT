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
# Version 1.4             #
# by SZV                  #
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

  global env
  global DFTREE_WINDOWS
  global DFTREE_GLOBALS
  global $DDF_Browser
    
  puts $DDF_Browser

  package require Tk

  ## Create images
  set DFTREE_GLOBALS(ImageLabel)     [DFOpenImage dfb_folder.gif]
  set DFTREE_GLOBALS(ImageAttrOther) [DFOpenImage dfb_attribute.gif]
  set DFTREE_GLOBALS(ImageAttrNS)    [DFOpenImage dfb_attribns.gif]

##    set DFTREE_LabelStyle [tixDisplayStyle imagetext \
##	    -font 9x15bold \
##	    -background Bisque3 \
##	    ]

##    set DFTREE_LabelStyle1 [tixDisplayStyle imagetext \
##	    -font 9x15bold \
##	    -background Bisque3 \
##	    -foreground SeaGreen2 \
##	    ]

##    set DFTREE_AttributeStyle [tixDisplayStyle imagetext \
##	    -font 9x15 \
##	    -background Bisque3 \
##	    ]

##    set DFTREE_AttributeStyle1 [tixDisplayStyle imagetext \
##	    -font 9x15 \
##	    -background Bisque3 \
##	    -foreground DarkGreen \
##	    ]

##    set DFTREE_AttributeStyle2 [tixDisplayStyle imagetext \
##	    -font 9x15 \
##	    -background Bisque3 \
##	    -foreground Red \
##	    ]

  set w .$DDF_Browser
  toplevel $w -height 400 -width 700 -background bisque3

  ########
  # Tree #
  ########

  set tree1 [ttk::treeview $w.tree -show tree]
  $tree1 tag bind Label <<TreeviewOpen>> [list DFTREE:Tree:Open $DDF_Browser $w]
  $tree1 tag configure Label -font 9x15bold -foreground DarkGreen
  #$tree1 tag configure Attribute -font 9x15 -background bisque3
  #$tree1 tag configure AttributeList -font 9x15 -background bisque3
  pack $tree1 -expand yes -fill both -padx 4 -pady 4

  # to see different fonts: /usr/openwin/lib/X11/fonts/misc or xlsfonts?
  # 8x13 8x13bold 9x15 9x15bold
  #	    hlist.font 8x13bold
  #	    hlist.gap "15"
  #	    hlist.indent "30"

  set DFTREE_WINDOWS($w,tree) $tree1
  set DFTREE_WINDOWS($w,root) [$tree1 insert {} end \
                                 -text $DDF_Browser \
                                 -values "" \
                                 -tags Label]

  # Here we need to open first node!
  
  return
}

###############################################################################
#
#
proc DFTREE:Tree:Open { DDF_Browser w } {

  global DFTREE_WINDOWS
  global DFTREE_GLOBALS
  global $DDF_Browser

  set df_tree $DFTREE_WINDOWS($w,tree)
  set df_node [$df_tree focus]

  #if {$df_node == $DFTREE_WINDOWS($w,root)} {
	# This is root
    set chdlist [$df_tree children $df_node]
    if {$chdlist != {}} {
      # The root branch already exists in hlist.
      # Clear all its children to force the tree to be updated.
      $df_tree delete $chdlist
    }
  #}

  #update
  DFTREE:Tree:Fill $DDF_Browser $df_tree $df_node
  return
}

###############################################################################
#
#
proc DFTREE:Tree:Fill { DDF_Browser df_tree df_node } {

  global DFTREE_GLOBALS
  global $DDF_Browser

  set loc  [lindex [$df_tree item $df_node -values] 0]
  set type [lindex [$df_tree item $df_node -tags] 0]

  switch -glob $type {

    Label {
      DFTREE:Tree:UpdateLabel $DDF_Browser $df_tree $df_node $loc
    }

    #AttributeList { 
    #  DFTREE:Tree:UpdateAttributeList $DDF_Browser $df_tree $df_node $loc
    #}

    default {
    }
  }
  return
}

###############################################################################
# $df_entry is a label entry, "1:3:2" for example, or "" for root.
#
proc DFTREE:Tree:UpdateLabel  { DDF_Browser df_tree df_node df_entry } {

  global DFTREE_GLOBALS
  global $DDF_Browser

  foreach fullname [split [DFOpenLabel $DDF_Browser $df_entry] "\\" ] {
    FCTREE:Tree:DecodeLabelItem $DDF_Browser $df_tree $df_node $df_entry $fullname
  }
  return
}

###############################################################################
#
#
proc DFTREE:Tree:UpdateAttributeList { DDF_Browser df_tree df_node df_entry} {

  global DFTREE_GLOBALS
  global $DDF_Browser

  set image_other $DFTREE_GLOBALS(ImageAttrOther)
  set image_ns    $DFTREE_GLOBALS(ImageAttrNS)
  #set xrefimage   $DFTREE_GLOBALS(ImageAttrOther)

  # abv: index attributes
  set num 0
  set attributes [split [DFOpenAttributeList $DDF_Browser $df_entry ] "\\" ]
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

	# Name analysis to suppress the map address.
	set shortlist [split $name "#"]
	set shortname [lindex $shortlist 0]
	set index     [lindex $shortlist 1]

	# Package analysis to determine the icon type.
	#set pk [lindex [split $name _] 0]
	set node_img $image_other
	#if {$pk == "TDataStd" || $pk == "TNaming"} {set node_img $standardimage}
	#if {$pk == "TXRef"} {set node_img $xrefimage}
    if {$shortname == "TNaming_NamedShape"} {set node_img $image_ns}

	set textname "$shortname"

#	if { [llength $tmplist] >5 } { set textname [lindex $tmplist 5] }
    set textname "$textname [DFGetAttributeValue $DDF_Browser $df_entry [lindex $tmplist 6]]"

	# Transaction analysis
	if {$transaction == "0"} {
#	    set locstyle $DFTREE_AttributeStyle
	} else {
#	    set textname "$textname T=$transaction"
#	    set locstyle $DFTREE_AttributeStyle1
	}

	# Valid?
	if {$valid == "NotValid"} {set textname "$textname $valid"}

	# Forgotten?
	if {$forgotten == "Forgotten"} {set textname "$textname $forgotten"}

	# Backuped?
	if {$backuped == "Backuped"} {set textname "$textname $backuped"}

    set df_new [$df_tree insert $df_node end \
                  -text $textname -image $node_img -tags Attribute]

    if {$maybeopen == "1"} {
      $df_tree item $df_new -open true
      DFTREE:Tree:UpdateAttribute $DDF_Browser $df_tree $df_new $index
    }
  }
}

###############################################################################
# $loc is always the attribute index
#
proc DFTREE:Tree:UpdateAttribute { DDF_Browser df_tree df_node a_index } {

  global DFTREE_GLOBALS
  global $DDF_Browser

  set tmplist [split [DFOpenAttribute $DDF_Browser $a_index ] "\\"]

  # Failed or not?
  if {[lindex $tmplist 0] == "Failed"} {
##	set locstyle $DFTREE_AttributeStyle2
  } else {
##	set locstyle $DFTREE_AttributeStyle
  }

  foreach name $tmplist {
    $df_tree insert $df_node end -text $name -tags Terminal
  }
  return
}

###############################################################################
# item:
# "Entry Name=TheNameIfExists Modified|NotModified 0|1"
proc FCTREE:Tree:DecodeLabelItem { DDF_Browser df_tree df_node df_entry labelItem} {

  global DFTREE_GLOBALS
  global $DDF_Browser

  set tmplist [split $labelItem " " ]
  set labname [lindex $tmplist 0]

  set textname "$labname"

  if {$labname == "AttributeList"} {
  
    # Attribute List
	# --------------

	#set modified  [lindex $tmplist 1]

	# Modified or not?
	#if {$modified == "Modified"} {
    #  set textname "$textname $modified"
##      set locstyle $DFTREE_AttributeStyle1
	#} else {
##      set locstyle $DFTREE_AttributeStyle
	#}

    #set df_new [$df_tree insert $df_node end \
    #              -text $textname \
    #              -image $DFTREE_GLOBALS(ImageAttrList) \
    #              -tags AttributeList]

    #$df_tree item $df_new -open true

    DFTREE:Tree:UpdateAttributeList $DDF_Browser $df_tree $df_node $df_entry
    
  } else {

    # Sub-label(s)
    # ------------

    set name      [lindex $tmplist 1]
    set modified  [lindex $tmplist 2]
    set maybeopen [lindex $tmplist 3]

    # Name?
    set ll [expr [string length $name] -2]
    if {$ll > 0} {
      set textname "$textname [string range $name 1 $ll]"
	}

	# Modified or not?
	if {$modified == "Modified"} {
      set textname "$textname $modified"
    }

    set df_new [$df_tree insert $df_node end \
                  -text $textname \
                  -image $DFTREE_GLOBALS(ImageLabel) \
                  -values $labname \
                  -tags Label]

    if {$maybeopen == "1"} {
      $df_tree item $df_new -open true
    }

    DFTREE:Tree:UpdateLabel $DDF_Browser $df_tree $df_new $labname
  }
}

###############################################################################
#
#
proc DFGetAttributeValue { DDF_Browser lab index } {

    global $DDF_Browser; # necessary for DRAW command to see the browser

    if {[catch "XAttributeValue $DDF_Browser $lab $index" ret]} {
	return ""
    }
    if {"$ret" == ""} { return "" }
    return "\[$ret\]"
}

###############################################################################
#
#
proc DFOpenImage { img } {

  global env

  if {[catch "image create photo -file $env(DRAWHOME)/$img" ret]} {
    return ""
  }
  return $ret
}
