#
# wish tools to browse tests and results
#
package require Tk

####################################################
# source batch tcl 
####################################################
#
#  sets the following environment variables
#  
#  WBCONTAINER
#  WBROOT
#  STATION
#
source $env(DRAWHOME)/InitEnvironment.tcl

source $env(DRAWHOME)/Tests.tcl

####################################################
# executable to display images
####################################################

set imageExec $env(WBROOT)/$env(STATION)/bin/TestImage
#set imageExec [ wokinfo -p testexec:TestImage ] 

####################################################
# Colors and title
####################################################

option add *Background grey
option add *Foreground black
wm  title . "Modeling Team Test Browser"
wm geometry . +460+10

####################################################
# Les Boutons
####################################################

frame .top

frame .b

set nextCommand "nextTest"
proc InteractiveNext {} {

    # process the next button
    # remove display of image for rapidity

    global nextCommand withImage
    set oldwith $withImage
    set withImage 0
    $nextCommand
    set withImage $oldwith
    displayImage
    focus .
}

button .b.init  -text "init"          -command init
button .b.clear -text "clear"         -command clearResult
button .b.copy  -text "Copy"          -command copyMaster
button .b.exit  -text "exit"          -command {if $withImage switchImage; exit}
button .b.view  -text "View"          -command viewTest
button .b.run   -text "Run"           -command runTest
button .b.send   -text "Send"         -command sendTest
button .b.next  -text "Next"          -command InteractiveNext
button .b.nul   -text "next UL"       -command nextUL
button .b.nfunc -text "next Function" -command nextFunction

radiobutton .b.withtest -anchor w -relief flat \
	-text "Test " -variable nextCommand -value "nextTest"
radiobutton .b.withres  -anchor w -relief flat \
	-text "With result" -variable nextCommand -value "nextWithResult"
radiobutton .b.witherr  -anchor w -relief flat \
	-text "With error" -variable nextCommand -value "nextWithError"

pack    .b.init .b.clear .b.copy  .b.run .b.send .b.view \
	.b.next .b.withtest .b.withres .b.witherr \
	.b.nul .b.nfunc .b.exit \
	-side top -fill x

pack .b -side right -anchor n -in .top

####################################################
# Les directory test resultat et master
# les entry
####################################################

frame  .root
label  .root.name  -text "Root of tests : " -width 20 -anchor w
entry  .root.val  -relief sunken -textvariable testRoot -width 40
pack .root.name .root.val -side left

frame  .res
label  .res.name    -text "Result directory : " -width 20 -anchor w
entry  .res.val -relief sunken -textvariable resultRoot -width 40
pack .res.name .res.val -side left

frame .master
label .master.name -text "Master directory : " -width 20 -anchor w
entry .master.val  -relief sunken -textvariable masterRoot -width 40
pack .master.name .master.val -side left

frame .exec
label .exec.name -text "Executable : " -width 20 -anchor w
entry .exec.val  -relief sunken -textvariable theExec -width 40
pack .exec.name .exec.val -side left

frame .sendto
label .sendto.name -text "Send to : " -width 20 -anchor w
entry .sendto.val  -relief sunken -textvariable theDraw -width 40
pack .sendto.name .sendto.val -side left

# enforce update when return
bind .root.val <Return> "set theTest $theTest"
bind .res.val <Return> "set theTest $theTest"
bind .master.val <Return> "set theTest $theTest"
bind .exec.val <Return> "set theTest $theTest"

pack .root .res .master .exec .sendto -side top -in .top

####################################################
# Les list box UL Function Test
####################################################

listbox .uls -relief sunken -borderwidth 2 \
	-yscrollcommand ".suls set"
scrollbar .suls -relief sunken  -command ".uls yview"

listbox .funcs -relief sunken -borderwidth 2 \
-yscrollcommand ".sfuncs set"
scrollbar .sfuncs -relief sunken  -command ".funcs yview"

listbox .tests -relief sunken -borderwidth 2 \
-yscrollcommand ".stests set"
scrollbar .stests -relief sunken  -command ".tests yview"

bind .uls <Double-Button-1> {
    set theUL [selection get]
}

bind .funcs <Double-Button-1> {
    set theFunction [selection get]
}

bind .tests <Double-Button-1> {
    set theTest [selection get]
}

# procedures de trace des variables listes
# mettent a jour la list box correspondante

trace variable listUL w  listULProcW
proc listULProcW {name element op} {
    global listUL
    .uls delete 0 [.uls size]
    foreach f $listUL {.uls insert end $f}
}

trace variable listFunction w  listFunctionProcW
proc listFunctionProcW {name element op} {
    global listFunction
    .funcs delete 0 [.funcs size]
    foreach f $listFunction {.funcs insert end $f}
}

trace variable listTest w  listTestProcW
proc listTestProcW {name element op} {
    global listTest
    .tests delete 0 [.tests size]
    foreach f $listTest {.tests insert end $f}
}

#presentation 

pack .uls .suls .funcs .sfuncs .tests .stests -side left -fill both -in .top

####################################################
# the current UL Function Test, associated buttons
####################################################

frame .current

label .current.ul   -textvariable theUL
label .current.func -textvariable theFunction
label .current.test -textvariable theTest

pack .current.ul .current.func .current.test -side left

####################################################
# le statut
####################################################

frame .status
label .status.h -text "${theStatusHeader} : "
label .status.s -textvariable theStatusLine -anchor w

# menu des fichiers xwd

set withImage 0
set hasImage "No Image"
set nbXWD "0 images  "

label  .status.xwd  -textvariable nbXWD
button .status.vxwd -text "no Display" -command switchImage
bind . <d> switchImage

proc switchImage {} {
    global withImage imageProcess
    if $withImage {
	set withImage 0
	.status.vxwd configure -text "no Display"
	catch {exec kill $imageProcess}
	set imageProcess 0
    } else {
	set withImage 1
	.status.vxwd configure -text "Display"
	displayImage
    }
}

trace variable theXWDFiles w theXWDFilesProcW

proc theXWDFilesProcW {name element op} {
    global nbXWD theXWDFiles
    set nbXWD "[llength $theXWDFiles] images  "
    displayImage
}

set imageProcess 0

proc displayImage {} {
    global withImage
    if {! $withImage} return
    global imageExec imageProcess
    global theXWDFiles resultRoot masterRoot theUL theFunction 
    if $imageProcess {catch {exec kill $imageProcess}}

    foreach f [glob -nocomplain /tmp/*.xwd] {catch {exec rm -f $f}}
    set r {}
    foreach h $theXWDFiles {
	set g [file rootname [file tail $h]]
	set f $resultRoot/$theUL/$theFunction/$g.Z
	if [file readable $f] {
	    catch {exec cp $f /tmp/r$g.Z}
	    catch {exec uncompress /tmp/r$g.Z}
	    lappend r /tmp/r$g
	} else {lappend r "XXXX"}
	set f $masterRoot/$theUL/$theFunction/$g.Z
	if [file readable $f] {
	    catch {exec cp $f /tmp/m$g.Z}
	    catch {exec uncompress /tmp/m$g.Z}
	    lappend r /tmp/m$g
	} else {lappend r "XXXX"}
    }
    set imageProcess [eval exec $imageExec $r & ]
}

pack .status.h .status.s -side left
pack .status.vxwd .status.xwd -side right

####################################################
# Les resultats et les master
####################################################

frame .log
text .log.result -relief raised -bd 2 -width 38 -yscrollcommand ".log.sresult set"
scrollbar .log.sresult -relief sunken -command ".log.result yview"
text .log.master -relief raised -bd 2 -width 38 -yscrollcommand ".log.smaster set"
scrollbar .log.smaster -relief sunken -command ".log.master yview"

# trace the files names
proc loadFile {file text} {
    $text delete 1.0 end
    if {$file == ""} return
    if [file isdirectory $file] return
    if [file readable $file] {
	set f [open $file]
	while {![eof $f]} {
	    $text insert end [read $f 1000]
	}
	close $f
    }
}

trace variable theLog w theLogProc
proc theLogProc {name element op} {
    global theLog 
    loadFile $theLog .log.result
}

trace variable theErrorLines w theErrorLinesProc
proc theErrorLinesProc {name element op} {
    global theErrorLines
    # tag the errors if there are
    .log.result tag delete error
    foreach l $theErrorLines {
	.log.result tag add error $l.0 $l.end
	.log.result tag configure error -background orange
    }
}

trace variable theDiffs w theDiffsProc
proc theDiffsProc {name element op} {
    global theDiffs
    # tag the differences if there are
    .log.result tag delete diffs
    .log.master tag delete diffs
    if [llength $theDiffs] {
	foreach l $theDiffs {
	    .log.result tag add diffs ${l}.0 ${l}.end
	    .log.master tag add diffs ${l}.0 ${l}.end
	}
	.log.result tag configure diffs -background lightblue
	.log.master tag configure diffs -background lightblue
    }
}

trace variable theMasterLog w theMasterLogProc
proc theMasterLogProc {name element op} {
    global theMasterLog
    loadFile $theMasterLog .log.master
}

pack .log.result .log.sresult .log.master .log.smaster -side left -fill y

####################################################
# panel to display the test
####################################################

set hasTestPanel 0
set withBeginEnd 0
set beginFunction ""
set endFunction   ""
set beginUL       ""
set endUL         ""

proc viewTest {} {
    global hasTestPanel 
    if {! $hasTestPanel} {
	set hasTestPanel 1
	toplevel .panel
	wm geometry .panel +10+610

	frame .panel.b
	button .panel.b.quit -text Quit \
		-command {destroy .panel; set hasTestPanel 0}
	bind .panel <q> {destroy .panel; set hasTestPanel 0}

	button .panel.b.bfunc -textvariable beginFunction \
		-command {loadFile $testRoot/$theUL/$theFunction/begin .panel.t.text}
	button .panel.b.efunc -textvariable endFunction \
		-command {loadFile $testRoot/$theUL/$theFunction/end .panel.t.text}
	button .panel.b.bul -textvariable beginUL \
		-command {loadFile $testRoot/$theUL/begin .panel.t.text}
	button .panel.b.eul -textvariable endUL \
		-command {loadFile $testRoot/$theUL/end .panel.t.text}
	button .panel.b.test -textvariable theTest \
		-command {loadFile $testRoot/$theUL/$theFunction/$theTest .panel.t.text}
	button .panel.b.next -text Next -command {$nextCommand; focus .panel}
	bind .panel <n> {$nextCommand}

	button .panel.b.concat -text "Concat " -command {
	    if { $withBeginEnd} { 
		set withBeginEnd 0
	    } else { 
		set withBeginEnd 1
	    } 
	    displayTest
	}
	button .panel.b.send   -text "Send"         -command sendTest

	pack .panel.b.quit .panel.b.next .panel.b.concat .panel.b.send \
		-side top -fill x 
	pack .panel.b.eul .panel.b.efunc .panel.b.test .panel.b.bfunc \
		.panel.b.bul \
		-side bottom -fill x 

	frame .panel.t
	text .panel.t.text -relief raised -width 65 -bd 2 \
		-yscrollcommand ".panel.t.scroll set"
	scrollbar .panel.t.scroll -relief sunken -command ".panel.t.text yview"
	pack .panel.t.text .panel.t.scroll -side left -fill both

	pack .panel.t .panel.b -side left -fill both

    } else {
	destroy .panel
	set hasTestPanel 0	
    }
    displayTest
}

proc displayTest {} {
    global hasTestPanel
    global withBeginEnd
    if {! $hasTestPanel} return
    global testRoot theUL theFunction theTest
    global beginFunction endFunction beginUL endUL
    if { ! $withBeginEnd} {
	loadFile $testRoot/$theUL/$theFunction/$theTest .panel.t.text
    } else {
	global testRoot resultRoot theExec theLog theDraw
	global theUL theFunction theTest
	set f /tmp/theTest[pid]
	set ff [open $f w]
	puts $ff "set testroot $testRoot"
	puts $ff "set testinfos(resultRoot)  $resultRoot"
	puts $ff "set testinfos(theUL)       $theUL"
	puts $ff "set testinfos(theFunction) $theFunction"
	puts $ff "set testinfos(theTest)     $theTest"
	close $ff
	catch {exec cat -s \
		$testRoot/$theUL/begin \
		$testRoot/$theUL/$theFunction/begin \
		$testRoot/$theUL/$theFunction/$theTest \
		$testRoot/$theUL/$theFunction/end \
		$testRoot/$theUL/end  >> $f}
	loadFile $f .panel.t.text
    }
    set beginFunction "$theFunction begin"
    set endFunction   "$theFunction end"
    set beginUL "$theUL begin"
    set endUL   "$theUL end"
    wm title .panel "$theUL $theFunction $theTest"
}

trace variable theTest w theTestProcW
proc theTestProcW {name element op} {
    displayTest
}

####################################################
# presentation generale
####################################################

pack .top .current .status .log -side top -fill x

init
