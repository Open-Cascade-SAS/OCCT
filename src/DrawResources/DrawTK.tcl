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

#
# TK features for Draw
#

# reload bindings
if { [info exists tk_library] } {
    set version [split [info tclversion] "."]
    set major [lindex ${version} 0]
    set minor [lindex ${version} 1]
    if { (${major} > 8) || (${major} >= 8 && ${minor} >= 4) } {
        #source $tk_library/tk.tcl
    } else {
        source $tk_library/tk.tcl
    }
}

#fills menu "Load" with submenus
proc fillloadmenu {} {
  set drawplugpath ""
  if {[array names ::env CSF_OCCTResourcePath] != "" && "$::env(CSF_OCCTResourcePath)" != "" && [file exists $::env(CSF_OCCTResourcePath)/DrawResources/DrawPlugin]} {
    set drawplugpath "$::env(CSF_OCCTResourcePath)/DrawResources/DrawPlugin"
  } elseif {[array names ::env CASROOT] != "" && "$::env(CASROOT)" != "" && [file exists $::env(CASROOT)/src/DrawResources/DrawPlugin]} {
    set drawplugpath "$::env(CASROOT)/src/DrawResources/DrawPlugin"
  }

  set chan [open [file nativename $drawplugpath]]
  while {[gets $chan line] >= 0} {
    if {[lindex [split $line ""] 0] != "!"} {
      if {[lindex [split $line ""] 0] == ""} {continue}
        set plugname [lindex [split $line " "] 0]
        addmenu Load "pload $plugname" "pload $plugname"
    }
  }
  close $chan
}

wm geometry . +10+10
bind . <F1> {vcommands}

frame .mbar -relief raised -bd 2
pack .mbar -side top -fill x
focus .mbar

set theMenus("") ""
set Draw_MenuIndex 0

proc addmenuitem {menu options} {

    global theMenus Draw_MenuIndex
    if {![info exists theMenus($menu)]} {
	incr Draw_MenuIndex
	set m .mbar.m$Draw_MenuIndex.menu
	menubutton .mbar.m$Draw_MenuIndex -text $menu -menu $m
	pack .mbar.m$Draw_MenuIndex -side left
	menu $m
	set theMenus($menu) $m
    } else {set m $theMenus($menu)}

    eval $m add $options
}

proc addmenu {menu submenu {command ""}} {
    if {$command == ""} {set command $submenu}
    addmenuitem $menu "command -label {$submenu} -command {$command}"
}

#################################
# Menus definition
#################################

# the file menu

addmenu File "Choose Data Directory" vdatadir
addmenu File "Load Shape (restore)" vrestore
addmenu File "Load Script (source)" vsource
addmenu File Exit exit

# the Load menu
fillloadmenu

# the view menu

source [file join $dir Move.tcl]

addmenuitem Views {checkbutton -label "Extended view commands" \
    -command ShowHideExtCommands -variable ShowExtCommands -onvalue 1 -offvalue 0}
addmenuitem Views   "separator"
addmenu Views axo   {smallview AXON}
addmenu Views top   {smallview +X+Y}
addmenu Views front {smallview +X+Z}
addmenu Views left  {smallview +Y+Z}
addmenu Views 2d    {smallview -2D-}
addmenuitem Views   "separator"
addmenu Views mu4
addmenu Views av2d
addmenu Views axo
addmenu Views pers

# the display menu

addmenu Display fit   "fit; repaint"
addmenu Display 2dfit "2dfit; repaint"
addmenu Display clear
addmenu Display 2dclear

# the samples menu
addmenu Samples "View samples" vsample

# the help menu

addmenu Help "System Info" sysinfo
addmenu Help Commands vcommands
addmenu Help About about
addmenu Help "User Guide" openuserguide

#redraw help submenu in the end of menu
proc redrawhelp {} {
  global theMenus
  set m $theMenus(Help)
  destroy [string trimright $m ".menu"]
  if [info exists theMenus(Help)] {unset theMenus(Help)}
  addmenu Help "System Info" sysinfo
  addmenu Help Commands vcommands
  addmenu Help About about
  addmenu Help "User Guide" openuserguide
}

#################################
# Modal dialog box
# add OK, help, cancel buttons
#################################

proc modaldialog {box okproc {helpproc ""} {cancelproc ""}} {
    wm geometry $box +10+60
    button $box.ok -text ok -command "$okproc ; destroy $box"
    pack $box.ok -side left
    button $box.ko -text Cancel -command "$cancelproc ; destroy $box"
    pack $box.ko -side right
    if {$helpproc != ""} {
	button $box.help -text Help -command $helpproc
	pack $box.help -side right
    }
    grab set $box
}


#################################
# File menu procedures
#################################

##############################
#
# dialbox command arg1 val1 arg2 val2 ...
#
##############################

proc dialbox args {
    set com [lindex $args 0]

    toplevel .d
    wm title .d $com

    # com will be the command
    set com "eval $com"

    # create entries for the arguments
    set n [llength $args]

    for {set i 1} {$i < $n} {incr i 2} {

	frame .d.a$i
	label .d.a$i.l -text [lindex $args $i]
	entry .d.a$i.e -relief sunken
	.d.a$i.e insert end [lindex $args [expr $i+1]]
	pack .d.a$i.l -side left
	pack .d.a$i.e -side right
	pack .d.a$i -side top -fill x

	append com { [} ".d.a$i.e get" {]}
    }
    append com ";repaint"

    modaldialog .d $com "help [lindex $args 0]"
	}
proc sdatadir {d} {
  global Draw_DataDir
  set Draw_DataDir $d
}

proc vdatadir {} {
  global Draw_DataDir
  sdatadir [tk_chooseDirectory -title "Data Directory" -initialdir $Draw_DataDir]
}

proc rresto {f} {
  if {[file exists $f]} {
    if {! [file isdirectory $f]} {
      puts "restore $f [file tail $f]"
      uplevel \#0 "restore $f [file tail $f]"
      repaint
    }
  }
}

proc vrestore {} {
  global Draw_DataDir
  rresto [tk_getOpenFile -title "Load Shape (restore)" -filetypes {{{BREP} {.brep}}} -initialdir $Draw_DataDir]
}


proc ssour {f} {
  global Draw_Source
  if {[file exists $f]} {
    set Draw_Source $f
    if {! [file isdirectory $f]} {
      puts "source $f [file tail $f]"
      uplevel \#0 "source $f"
    }
  }
}

set Draw_Source [pwd]
proc vsource {} {
  global Draw_Source
  ssour [tk_getOpenFile -title "Load Script (source)" -filetypes {{{All Files} *}} -initialdir Draw_Source]
}

#Creates a "Samples" window
proc vsamples {} {
  #create list {{category} {title} {filename}}
  set alistofthree ""

  set samplespath ""
  if { [array names ::env CSF_OCCTSamplesPath] != "" && "$::env(CSF_OCCTSamplesPath)" != "" && [file exists $::env(CSF_OCCTSamplesPath)/tcl/]} {
    set samplespath "$::env(CSF_OCCTSamplesPath)/tcl/"
  } elseif { [array names ::env CASROOT] != "" && "$::env(CASROOT)" != "" && [file exists $::env(CASROOT)/samples/tcl/]} {
    set samplespath "$::env(CASROOT)/samples/tcl/"
  }

  foreach fname [glob -path "${samplespath}" *.tcl] {

    set chan [open $fname]
    set istitlefound 0
    while {[gets $chan line] >= 0} {
      if {[lindex [split $line " "] 0] == "#Category:"} {
        set acategory [string trim [string trimleft $line "#Category: "]]
      }
      if {[lindex [split $line " "] 0] == "#Title:"} {
        set atitle [string trim [string trimleft $line "#Title: "]]
        lappend alistofthree $acategory $atitle $fname
        incr istitlefound
        break
      }
    }
    close $chan
    if {$istitlefound == 0} {
    lappend alistofthree Other "[lindex [split $fname \\] end]" $fname
    }
  }
  #create window
  toplevel .samples
  wm title .samples "Samples"
  wm geometry .samples +0+0
  wm minsize .samples 800 600
  frame .samples.right
  frame .samples.left
  frame .samples.right.textframe
  frame .samples.right.botframe
  ttk::treeview .samples.left.tree -selectmode browse -yscrollcommand {.samples.left.treescroll set}
  pack .samples.left.tree -fill both -expand 1 -side left
  .samples.left.tree column #0 -minwidth 200
  .samples.left.tree heading #0 -text "Samples"
  pack .samples.right -side right -fill both -expand 1 -padx 10 -pady 10
  pack .samples.left -side left -padx 10 -pady 10 -fill both
  pack .samples.right.textframe -side top -fill both -expand 1
  pack .samples.right.botframe -side bottom -fill both -expand 1
  text .samples.right.textframe.text -yscrollcommand {.samples.right.textframe.scroll set} -xscrollcommand {.samples.right.botframe.scrollx set} -wrap none -width 40 -height 32
  pack .samples.right.textframe.text -fill both -side left -expand 1
  .samples.right.textframe.text delete 0.0 end
  .samples.right.textframe.text configure -state disabled
  set i 1
  foreach {acat title fnam} $alistofthree {
    if [.samples.left.tree exists $acat] {
      .samples.left.tree insert $acat end -id $title -text $title -tags "selected$i"
      .samples.left.tree tag bind selected$i <1> "fillsampletext {$fnam}"
      incr i
      continue
    } else {
      .samples.left.tree insert {} end -id $acat -text $acat
      .samples.left.tree insert $acat end -id $title -text $title -tags "selected$i"
      .samples.left.tree tag bind selected$i <1> "fillsampletext {$fnam}"
      incr i
    }
  }
  scrollbar .samples.right.textframe.scroll -command {.samples.right.textframe.text yview}
  scrollbar .samples.left.treescroll -command {.samples.left.tree yview}
  scrollbar .samples.right.botframe.scrollx -command {.samples.right.textframe.text xview} -orient horizontal
  pack .samples.right.textframe.scroll -side right -fill y
  pack .samples.right.botframe.scrollx -side top -fill x
  pack .samples.left.treescroll -side right -fill y
  button .samples.right.botframe.button -text "Run sample" -state disabled
  pack .samples.right.botframe.button -fill none -pady 10
}

#Fills the textbox in "Samples" window
proc fillsampletext {fname} {
  .samples.right.botframe.button configure -state normal -command "lower .samples;catch {vclose ALL};catch {vremove -all}; catch {vclear}; source {$fname}"
  .samples.right.textframe.text configure -state normal
  .samples.right.textframe.text delete 0.0 end
  set chan [open "$fname"]
    while {[gets $chan line] >= 0} {
    .samples.right.textframe.text insert end "$line\n"
    }
  close $chan
  .samples.right.textframe.text configure -state disabled
}

#Creates a "Commands help" window
proc vcommands {} {
  global Draw_Groups Find_Button_Click_Count Entry_Cache
  set Find_Button_Click_Count 0
  set Entry_Cache ""
  toplevel .commands
  focus .commands
  wm minsize .commands 800 600
  wm title .commands "Commands help"
  wm geometry .commands +0+0
  frame .commands.t
  frame .commands.left
  ttk::treeview .commands.left.tree -selectmode browse -yscrollcommand {.commands.left.treescroll set}
  .commands.left.tree column #0 -width 300
  .commands.left.tree heading #0 -text "Help treeview"
  pack .commands.left.tree -expand 1 -fill both -side left
  pack .commands.t -side right -fill both -expand 1 -padx 10 -pady 10
  pack .commands.left -side left -fill both -padx 10 -pady 10
  pack [frame .commands.t.top] -side top -fill x -padx 10 -pady 10
  text .commands.t.text -yscrollcommand {.commands.t.scroll set} -width 40
  .commands.t.text delete 0.0 end
  pack .commands.t.text -fill both -side left -expand 1
  .commands.t.text configure -state disabled
  pack [entry .commands.t.top.e  -width 20] -side left
  pack [button .commands.t.top.findcom -text "Find command" -command vhelpsearch] -side left -padx 10
  pack [button .commands.t.top.textfind -text "Find in text" -command "vhelptextsearch; incr Find_Button_Click_Count"] -side left
  set i 1
  set j 100
  set newgroupinx 0
  foreach h [lsort [array names Draw_Groups]] {
  .commands.left.tree insert {} end -id $i -text $h -tags "info$i"
  .commands.left.tree tag bind info$i <1> "vcomhelp {$h}"
  set newgroupinx $j
    foreach f [lsort $Draw_Groups($h)] {
      .commands.left.tree insert $i end -id $j -text $f  -tags "selected$j"
      .commands.left.tree tag bind selected$j <1> "vcomhelp {$h} $j $newgroupinx"
       incr j
    }
   incr i
  }
  scrollbar .commands.t.scroll -command {.commands.t.text yview}
  scrollbar .commands.left.treescroll -command {.commands.left.tree yview}
  pack .commands.t.scroll -side right -fill y
  pack .commands.left.treescroll -side right -fill y -expand 1
  #hotkeys
  bind .commands.t.top.e <Return> {vhelpsearch}
  bind .commands <Control-f> {focus .commands.t.top.e}
  bind .commands <Control-F> {focus .commands.t.top.e}
  bind .commands <Escape> {destroy .commands}
  }

############################################################
# Fills the textbox in "Commands help" window
# $h -group of commands to display
# $selindex - index of selected item in the treeview
# $startindex - index of item int the treeview to start from
############################################################
proc vcomhelp {h {selindex -1} {startindex 0}} {
  global Draw_Helps Draw_Groups
  set highlighted false
  .commands.t.text configure -state normal
  .commands.t.text delete 1.0 end
  foreach f [lsort $Draw_Groups($h)] {
    if {$startindex == $selindex} {
      .commands.t.text insert end "$f : $Draw_Helps($f)\n\n" "highlightline"
      incr startindex
      set highlighted true
      continue
    }
    .commands.t.text insert end "$f : $Draw_Helps($f)\n\n"
    incr startindex
  }
  .commands.t.text tag configure highlightline -background yellow -relief raised
  .commands.t.text configure -state disabled
  if {$highlighted == true} {.commands.t.text see highlightline.last}
}

#Creates a "About" window
proc about {} {
  toplevel .about
  focus .about
  wm resizable .about 0 0
  wm title .about "About"
  set screenheight [expr {int([winfo screenheight .]*0.5-200)}]
  set screenwidth [expr {int([winfo screenwidth .]*0.5-200)}]
  wm geometry .about 400x200+$screenwidth+$screenheight

  set logopath ""
  if {[array names ::env CSF_OCCTResourcePath] != "" && "$::env(CSF_OCCTResourcePath)" != "" && [file exists $::env(CSF_OCCTResourcePath)/DrawResources/OCC_logo.png]} {
    set logopath "$::env(CSF_OCCTResourcePath)/DrawResources/OCC_logo.png"
  } elseif {[array names ::env CASROOT] != "" && "$::env(CASROOT)" != "" && [file exists $::env(CASROOT)/src/DrawResources/OCC_logo.png]} {
    set logopath "$::env(CASROOT)/src/DrawResources/OCC_logo.png"
  }

  image create photo occlogo -file $logopath -format png
  frame .about.logo -bg red
  frame .about.links -bg blue
  frame .about.copyright
  pack .about.logo -side top -fill both
  pack .about.links -fill both
  pack .about.copyright -side top -fill both
  label .about.logo.img -image occlogo
  pack .about.logo.img -fill both
  text .about.links.text -bg lightgray -fg blue -height 1 -width 10
  .about.links.text insert end "http://www.opencascade.com/" "link1"
  .about.links.text tag bind link1 <1> "_launchBrowser http://www.opencascade.com/"
  .about.links.text tag bind link1 <Enter> ".about.links.text configure -cursor hand2"
  .about.links.text tag bind link1 <Leave> ".about.links.text configure -cursor arrow"
  .about.links.text tag configure link1 -underline true -justify center
  pack .about.links.text -fill both
  label .about.copyright.text -text "Copyright (c) 1999-2014 OPEN CASCADE SAS"
  button .about.button -text "OK" -command "destroy .about"
  pack .about.button -padx 10 -pady 10
  pack .about.copyright.text
  .about.links.text configure -state disabled
  grab .about
  bind .about <Return> {destroy .about}
}

#Executes files and hyperlinks
proc launchBrowser url {
  global tcl_platform

  if {$tcl_platform(platform) eq "windows"} {
    set command [list {*}[auto_execok start] {}]
  } elseif {$tcl_platform(os) eq "Darwin"} {
      set command [list open]
    } else {
        set command [list xdg-open]
    }
  exec {*}$command $url &
}

#Safe execution of files and hyperlinks
proc _launchBrowser {url} {
  if [catch {launchBrowser $url} err] {
    tk_messageBox -icon error -message "error '$err' with '$command'"
  }
}
################################################################
# This procedure tries to open an userguide on Draw Harness in pdf format
# If there is no a such one, then tries to open it in html format
# Else opens a site with this guide
################################################################
proc openuserguide {} {
  if { [array names ::env CSF_OCCTDocPath] != "" && "$::env(CSF_OCCTDocPath)" != "" && [file exists $::env(CSF_OCCTDocPath)/pdf/user_guides/occt_test_harness.pdf]} {
    _launchBrowser $::env(CSF_OCCTDocPath)/pdf/user_guides/occt_test_harness.pdf
  } elseif {  [array names ::env CSF_OCCTDocPath] != "" && "$::env(CSF_OCCTDocPath)" != "" && [file exists $::env(CSF_OCCTDocPath)/overview/html/occt_user_guides__test_harness.html]} {
    _launchBrowser $::env(CSF_OCCTDocPath)/overview/html/occt_user_guides__test_harness.html
  } elseif { [array names ::env CASROOT] != "" && "$::env(CASROOT)" != "" && [file exists $::env(CASROOT)/doc/pdf/user_guides/occt_test_harness.pdf]} {
    _launchBrowser $::env(CASROOT)/doc/pdf/user_guides/occt_test_harness.pdf
  } elseif {  [array names ::env CASROOT] != "" && "$::env(CASROOT)" != "" && [file exists $::env(CASROOT)/doc/overview/html/occt_user_guides__test_harness.html]} {
    _launchBrowser $::env(CASROOT)/doc/overview/html/occt_user_guides__test_harness.html
  } else {
    launchBrowser {http://dev.opencascade.org/doc/overview/html/occt_user_guides__test_harness.html}
  }
}

#Search through commands and display the result
proc vhelpsearch {} {
  global Draw_Groups Entry_Cache
  set searchstring [.commands.t.top.e get]
  set i 1
  set j 100
  set newgroupinx 0
  set isfound 0
  foreach h [lsort [array names Draw_Groups]] {
  set newgroupinx $j
    foreach f [lsort $Draw_Groups($h)] {
      if {$f == $searchstring} {
        incr isfound
        .commands.left.tree see  $j
        .commands.left.tree selection set $j
        vcomhelp $h $j $newgroupinx
        break
      }
    incr j
    }
   incr i
  }
  if {$isfound == 0} {
    errorhelp "No help found for '$searchstring'!"
  } else {set Entry_Cache ""}
}

#Displays an error window with $errstring inside
proc errorhelp {errstring} {
    toplevel .errorhelp
    focus .errorhelp
    wm resizable .errorhelp 0 0
    wm title .errorhelp "Error"
    set screenheight [expr {int([winfo screenheight .]*0.5-200)}]
    set screenwidth [expr {int([winfo screenwidth .]*0.5-200)}]
    wm geometry .errorhelp +$screenwidth+$screenheight
    text .errorhelp.t -width 40 -height 5
    .errorhelp.t insert end $errstring
    button .errorhelp.button -text "OK" -command "destroy .errorhelp"
    pack .errorhelp.t
    .errorhelp.t configure -state disabled
    pack .errorhelp.button -padx 10 -pady 10
    bind .errorhelp <Return> {destroy .errorhelp}
    grab .errorhelp
}

#Search through text of help and display the result
proc vhelptextsearch {} {
  global Draw_Helps Draw_Groups Find_Button_Click_Count Entry_Cache End_of_Search
  set searchstring [.commands.t.top.e get]
  if {$Entry_Cache != $searchstring} {
    set Find_Button_Click_Count 0
    set End_of_Search 0
    set Entry_Cache $searchstring
  }
  if {$End_of_Search} {
    errorhelp "No more '$searchstring' found!"
    return
  }
  .commands.t.text configure -state normal
  .commands.t.text delete 0.0 end
  set i 0
  set isfound 0
  foreach h [lsort [array names Draw_Groups]] {
    foreach f [lsort $Draw_Groups($h)] {
      if [string match *$searchstring* $Draw_Helps($f)] {
        incr i
        if {$i > $Find_Button_Click_Count+1} {incr isfound; break}
        .commands.t.text insert end "$f : "
        foreach line [list $Draw_Helps($f)] {
          foreach word [split $line " "] {
            if [string match *$searchstring* $word] {
              .commands.t.text insert end "$word" "highlightword"
              .commands.t.text insert end " "
              continue
            }
            .commands.t.text insert end "$word "
          }
        }
      .commands.t.text insert end \n\n
      }
    }
  }
  if {!$isfound} {
    incr End_of_Search
  }
  .commands.t.text tag configure highlightword -background yellow -relief raised
  .commands.t.text see end
}

#Create a "System information" window
proc sysinfo {} {
  toplevel .info
  wm title .info "System information"
  wm resizable .info 0 0
  pack [frame .info.top] -side top -fill both -padx 5 -pady 10
  pack [frame .info.bot] -side bottom -fill both -padx 5 -pady 10
  pack [frame .info.top.left] -side left -fill both -padx 5 -pady 10
  pack [frame .info.top.mid] -side left -fill both -padx 5 -pady 10
  pack [frame .info.top.right] -side left -fill both -padx 5 -pady 10
  pack [label .info.top.left.label -text "OCCT build configuration "]
  pack [label .info.top.mid.label -text "Memory info"]
  pack [label .info.top.right.label -text "OpenGL info"]
  pack [text .info.top.left.text -width 50 -height 20]
  pack [text .info.top.mid.text -width 50 -height 20]
  pack [text .info.top.right.text -width 50 -height 20]
  pack [button .info.bot.button -text "Update" -command rescaninfo]
  pack [button .info.bot.close -text "Close" -command "destroy .info"] -pady 10
  rescaninfo
}

#Updates information in "System information" window
proc rescaninfo {} {
  .info.top.left.text configure -state normal
  .info.top.mid.text configure -state normal
  .info.top.right.text configure -state normal
  .info.top.left.text delete 0.0 end
  .info.top.mid.text delete 0.0 end
  .info.top.right.text delete 0.0 end
  .info.top.left.text insert end [dversion]
  .info.top.mid.text insert end [meminfo]
  set glinfo ""
  if [catch {vglinfo} err] {
    if {$err == ""} {
      .info.top.right.text insert end "No active view. Please call vinit."
    } else {
        .info.top.right.text insert end "VISUALIZATION is not loaded. Please call pload VISUALIZATION"
      }
  } else {
      .info.top.right.text insert end [vglinfo]
  }
  .info.top.left.text configure -state disabled
  .info.top.mid.text configure -state disabled
  .info.top.right.text configure -state disabled
}
