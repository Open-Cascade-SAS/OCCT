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

wm geometry . +10+10

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
    addmenuitem $menu "command -label $submenu -command {$command}"
}

#################################
# Menus definition
#################################

# the file menu

addmenu File datadir vdatadir
addmenu File restore vrestore
addmenu File source  vsource
addmenu File exit    

# the view menu

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


#################################
# Modal dialog box
# add OK, help,  cancel buttons  
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


####################################
# Modal get file
#  select a file and launch a command
#  - file is the original value
#  - okproc is the OK procedure, 
#    it will be called with the filename
#  - title is the box title
#  - filter is called on each subfile
#  - Buttons are added in the dialbox, if none it is created
####################################

proc retyes {file} {return 1}

proc getfile {file okproc title {filter "retyes"} {box ""}} {

    if {$box == ""} {
	set box ".s"
	toplevel .s
    }
    wm title $box $title

    # The text entry at the top
    frame  $box.d
    entry  $box.d.e -relief sunken -width 40
    $box.d.e insert end $file
    button $box.d.s -text scan -command "filescan $filter $box"
    pack   $box.d.e -side left
    pack   $box.d.s -side right
    pack   $box.d   -side top

    # The list box with the files
    frame $box.f
    listbox $box.f.l -relief sunken -yscrollcommand "$box.f.s set"
    scrollbar $box.f.s -relief sunken -command "$box.f.l yview"
    pack $box.f.l $box.f.s -side left -fill y
    pack $box.f -side top

    filescan $filter $box

    bind $box.f.l <Double-Button-1> "fileclick $box $filter $okproc"
    
    modaldialog $box [concat $okproc " \[" $box.d.e "get\]"]
}

# when double click
proc fileclick {box filter okproc} {
    filescan $filter $box [selection get]
    set f [$box.d.e get]
    if {! [file isdirectory $f]} {
	destroy $box
	$okproc $f
    }
}

proc filescan {filter box {subfile ""}} {

    set s [$box.d.e get]
    if {$s == "."} {set s [pwd]/}

    $box.d.e delete 0 end
    if {$subfile != ""} {
	if {$subfile == ".."} {
	    set s [file dirname [file dirname $s]]/
	} else {
	    set s [file dirname $s]/$subfile
	}
    }
    $box.d.e insert end $s

    # list directories
    $box.f.l delete 0 end
    $box.f.l insert end ".."
    if [file isdirectory $s] {
	set d $s
	if {![string match */ $s]} {append s "/"}
    } else {
	set d [file dirname $s]
    }
    foreach f [glob -nocomplain $d/*] {
	if [$filter $f] {
	    set x [file tail $f]
	    if [file isdirectory $f] {append x "/"}
	    $box.f.l insert end $x
	}
    }
}


#################################
# File menu procedures
#################################

#
# dialog box for datadir
#

proc isdir {f} {return [file isdirectory $f]}

proc sdatadir {d} {
    global Draw_DataDir
    set Draw_DataDir $d
}

proc vdatadir {} {
    global Draw_DataDir
    toplevel .s
    frame .s.t
    button .s.t.d -text data -command {
	.s.d.e delete 0 end
	.s.d.e insert end $env(WBCONTAINER)/data/
	filescan isdir .s
    }
    pack .s.t.d -side left
    pack .s.t -side top
    getfile $Draw_DataDir sdatadir "Data Directory" isdir .s
}

proc notild {f} {return [expr ! [string match *~ $f]]}

proc rresto {f} {
    if {! [file isdirectory $f]} {
	uplevel \#0 "brestore $f [file tail $f]"
	repaint
    }
}

proc vrestore {} {
    global Draw_DataDir 
    getfile $Draw_DataDir rresto "Restore" notild
}


proc ssour {f} {
    global Draw_Source
    set Draw_Source $f
    if {! [file isdirectory $f]} {
	uplevel \#0 "source $f"
    }
}

set Draw_Source [pwd]
proc vsource {} {
    global Draw_Source 
    getfile $Draw_Source  ssour "Source" notild
}
