# Copyright (c) 1999-2012 OPEN CASCADE SAS
#
# The content of this file is subject to the Open CASCADE Technology Public
# License Version 6.5 (the "License"). You may not use the content of this file
# except in compliance with the License. Please obtain a copy of the License
# at http://www.opencascade.org and read it completely before using this file.
#
# The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
# main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
#
# The Original Code and all software distributed under the License is
# distributed on an "AS IS" basis, without warranty of any kind, and the
# Initial Developer hereby disclaims all such warranties, including without
# limitation, any warranties of merchantability, fitness for a particular
# purpose or non-infringement. Please see the License for the specific terms
# and conditions governing the rights and limitations under the License.

#
# tclsh tools to browse tests
#
# xab : 22-Mar-96  rajout de floatDifference pour eviter des diffs
#                  en fonction de la tolerance DiffTestTolerance
# pmn : 25-Sept-96 masterRoot sans reference explicite a gti 
#                 (pour une utilisation Aixoise)
# xab : 12-Nov-996 nouveau trap dans les comptes rendus
#                  INV par checkshape ou checktopshape
# xab : 22/11/96  : on a encore besoin des 3 variables d'environement
#                   WBCONTAINER
#                   WBROOT
#                   STATION
# xab : 10-Mar-97 : string compare au lieu de != dans les diffs
#
####################################################
# Error list
####################################################

set nomatch "ZXWYWXZ"
set theErrors {
    { nomatch                   "NOLOGFILE" "no result for the test"}
    { nomatch                   "OK       " "no error and no difference"}
    { nomatch                   "NO       " "no error and no master file to compare"}
    { nomatch                   "NOEND    " "abnormal termination"}
    { "An exception was caught" "CATCH    " "an exception was caught"}
    { "segv"                    "SEGV     " "segmentation violation"}
    { "ERROR"                   "ERROR    " "user generated error"}
    { "NYI"                     "NYI      " "This test is not yet implemented"}
    { "Cpu Limit Exceeded"      "CPULIM   " "CPU limit exceeded"}
    { "BRepCheck_"              "INV      " "invalid result"}    
}

####################################################
# Variables
####################################################
set DiffTestTolerance 1.0e-6

set theStation $env(STATION)

if {$theStation=="wnt"} {
    set ExeMachinePath  drv\\DFLT\\$env(STATION)
    set exePath         $env(WBROOT)\\prod
#    set testRoot        $env(WBCONTAINER)\\test
    set testRoot        $env(WBCONTAINER)
    set resultRoot      $env(WBROOT)\\work\\result$env(STATION)
    set masterRoot      $env(WBROOT)\\work\\master
} else {
    set ExeMachinePath  drv/DFLT/$env(STATION)
    set ExePath         $env(WBROOT)/$env(STATION)/bin
    set testRoot        $env(WBCONTAINER)
    set resultRoot      $env(WBROOT)/work/result$env(STATION)
    set masterRoot      $env(WBROOT)/work/master
}
puts "$testRoot  et        $env(WBCONTAINER) "
#
## customize default values, if "setenv" has been set by AQ Team  -plb/13feb97-

foreach el1 [array names env] {
    if       { [ string compare $el1 "DRAW_TEST_ROOT" ] == 0 }   {
	set testRoot   $env($el1)
    } elseif { [ string compare $el1 "DRAW_TEST_RESULT" ] == 0 } {
	set resultRoot $env($el1)
    } elseif { [ string compare $el1 "DRAW_TEST_MASTER" ] == 0 } {
	set masterRoot $env($el1)
    }
}


set theUL       ""
set theFunction ""
set theTest     ""
set theExec     ""
set themode     ""

# name of Draw process to send
set theDraw     "Draw"
set FILTRE      ""
set DIFF        ""
set RESULT      ""
set PREVIOUS    ""
set TEMP        ""

set listUL       {}
set listFunction {}
set listTest     {}

set theLog            ""
set theMasterLog      ""
set theXWDFiles       {}
set theMasterXWDFiles {}

set theStatusHeader "Result    LOG  XWD"
set theStatus       ""
set theStatusLine   ""
set theErrorLines   {}
set theDiffs        {}


####################################################
# List from a directory
####################################################

# match what is not a test
set testRegExp \
	"^(Applet_GridTest|modeles|scripts|data|bin|help|begin.*|begin_wnt|end.*|grid|executables|readme|image|save|read.me|photo.*|.*~|#.*#)$"

proc mkListTests root {
    global testRegExp  Wok theStation env
    set l {}

    if {$theStation=="wnt"} {
	foreach f [exec $env(WOKHOME)\\lib\\wnt\\ls.exe  $root] {
	    set f [file tail $f]
	    if [regexp $testRegExp $f] continue
		lappend l $f
	}
    } else {
        foreach f [glob -nocomplain $root/*] {
	    set f [file tail $f]
	    if [regexp $testRegExp $f] continue
	    lappend l $f
	}
    }
    return [lsort $l]
}

####################################################
# Junky Heuristic to compare numbers in tests 
####################################################

proc floatDifference { line1 line2 } {
    global DiffTestTolerance 
    set has_number 0
    for { set ii 1 } { $ii <= 3 } { incr ii } {
	for { set jj 1 } { $jj <= 3 } { incr jj } {
	    set number($ii,$jj) 0.0e0 
	}
    }
    if { [regexp { ([0-9]+\.[0-9]+e-([0-9]+))|([0-9]+\.[0-9]*)} $line1  number(1,1) number(1,2) number(1,3) ] } {
	if { [regexp { ([0-9]+\.[0-9]+e-([0-9]+))|([0-9]+\.[0-9]*)} $line2 number(2,1) number(2,2)  number(2,3) ] } {
	    for { set ii 1 } { $ii <= 3 } { incr ii } {
		if { $number(1,$ii) != "" && $number(2,$ii) != "" } {
		    if { [ regexp {\.} $number(1,$ii) ] && [ regexp {\.} $number(2,$ii) ] } {
			set has_number 1 
			set diff [ expr $number(1,$ii) - $number(2,$ii) ]
			set diff [ expr abs($diff) ] 
			if { $diff > $DiffTestTolerance } {
			    return 1
			
			}
		    }
		}
	    }
			
	}
    }

#
# scabreux mais donne de bon resultat
#
    if { $has_number } {
	return 0
    } else {
	return 1 
    }
	    
}

####################################################
# compute the current test status
####################################################

proc computeStatus {} {
    global testRoot resultRoot masterRoot
    global theUL theFunction theTest
    global theLog theMasterLog theXWDFiles theMasterXWDFiles
    global theErrors theStatus theStatusLine theErrorLines theDiffs
    
    set hasresult [file readable $theLog]
    if {! $hasresult} {
	set theStatus     "-"
	set theStatusLine "NOLOGFILE"
	set theErrorLines {}
	set theDiffs      {}
	return
    }
    set hasmaster [file readable $theMasterLog]

    # analyse the log file and compare to the master
    set curline    0
    set error      ""
    set errorlines {} 
    set diffs      {}
    set completed  0

    set f [open $theLog]
    if $hasmaster {set g [open $theMasterLog]}
    set moremaster $hasmaster


    while {[gets $f line] >= 0} {
	incr curline
	# difference with master
	if {$moremaster} {
	    if {[gets $g mline] >= 0} {
		# compare the two lines
		if { [ string compare $line $mline ] } {
		    if { [ floatDifference  $mline $line  ] } {
			lappend diffs $curline
                    }
		}
	    } else {
		# the master file is finished
		set moremaster 0
		lappend ldiff $curline
	    }
	}

	# search for errors
	foreach err $theErrors {
	    if [regexp [lindex $err 0] $line] {
		if {[llength $errorlines] == 0} {set error [lindex $err 1]}
		lappend errorlines $curline
	    }
	}

	# check for end of test
	if [regexp "TEST COMPLETED" $line] {
	    set completed 1
	}
    }

    close $f
    if $hasmaster {close $g}

    set status "*"
    set statusline $error
    if {$error == ""} {
	if {! $completed} {
	    set statusline "NOEND    "
	} else {
	    set statusline "OK       "
	    set status " "
	}
    }

    if {$hasmaster} {
	if [llength $diffs] {
	    append statusline " DIFF"
	    set status "*"
	} else {
	    append statusline " OK  "
	}
    } else {
	    append statusline " NO  "
    }
    
    # xwd files

    if [llength $theXWDFiles] {
	# here we should compare the XWD files
	if [llength $theMasterXWDFiles] {
	    append statusline " OK    "
	} else {
	    append statusline " NO    "
	}
    } else {
	    append statusline " NOXWD "
    }
    
    # set results
    
    set theStatus     $status
    set theStatusLine $statusline
    set theErrorLines $errorlines
    set theDiffs      $diffs
}

####################################################
# trace on variables to update the lists
####################################################

trace variable theUL w theULProc 
proc theULProc {name element op} {
    global theUL testRoot listFunction
    global theStation

    # Met a jour la liste des fonctions
    if {$theUL == ""} {
	set listFunction {}
    } else {
	set listFunction [mkListTests $testRoot/$theUL]
    }
}

trace variable theFunction w theFunctionProc
proc theFunctionProc {name element op} {
    global testRoot theUL theFunction listTest
    global theStation

    # Met a jour la liste des tests
    if {$theFunction == ""} {
	set listTest {}
    } else {
	set listTest [mkListTests $testRoot/$theUL/$theFunction]
    }
}

trace variable theTest w theTestProc
proc theTestProc {name element op} {
    global resultRoot masterRoot theUL theFunction theTest
    global theLog theMasterLog theXWDFiles theMasterXWDFiles
    global theStation

    if {$theStation=="wnt"} {
	
	# update the result variables
	set theLog       $resultRoot\\$theUL\\$theFunction\\$theTest.log
	set theMasterLog $masterRoot\\$theUL\\$theFunction\\$theTest.log
 
	set theXWDFiles \
		[glob -nocomplain \
		$resultRoot\\$theUL\\$theFunction\\${theTest}.*.xwd.Z]
	
	set theMasterXWDFiles \
		[glob -nocomplain \
		$masterRoot\\$theUL\\$theFunction\\${theTest}.*.xwd.Z]
    } else {
	# update the result variables
	set theLog       $resultRoot/$theUL/$theFunction/$theTest.log
	set theMasterLog $masterRoot/$theUL/$theFunction/$theTest.log
 
	set theXWDFiles \
		[glob -nocomplain \
		$resultRoot/$theUL/$theFunction/${theTest}.*.xwd.Z]
	
	set theMasterXWDFiles \
		[glob -nocomplain \
		$masterRoot/$theUL/$theFunction/${theTest}.*.xwd.Z]
    }	

    # update the status
    computeStatus
}

trace variable listUL w  listULProc
proc listULProc {name element op} {
    global listUL theUL
    if [llength $listUL] {
	set theUL [lindex $listUL 0]
    } else {
	set theUL ""
    }
}

trace variable listFunction w  listFunctionProc
proc listFunctionProc {name element op} {
    global listFunction theFunction
    if [llength $listFunction] {
	set theFunction [lindex $listFunction 0]
    } else {
	set theFunction ""
    }
}

trace variable listTest w  listTestProc
proc listTestProc {name element op} {
    global listTest theTest
    if [llength $listTest] {
	set theTest [lindex $listTest 0]
    } else {
	set theTest ""
    }
}

####################################################
# procedure to change test
####################################################

proc nextUL {} {
    global listUL theUL
    set l [llength $listUL]
    if ($l) {
	set i [lsearch $listUL $theUL]
	incr i
	if {$i < $l} {
	    set theUL [lindex $listUL $i]
	    return
	}
    }
    set theUL ""
}

proc nextFunction {} {
    global listFunction theFunction
    set l [llength $listFunction]
    if ($l) {
	set i [lsearch $listFunction $theFunction]
	incr i
	if {$i < $l} {
	    set theFunction [lindex $listFunction $i]
	    return
	}
    }
    nextUL
}

proc nextTest {} {
    global listTest theTest
    set l [llength $listTest]
    if ($l) {
	set i [lsearch $listTest $theTest]
	incr i
	if {$i < $l} {
	    set theTest [lindex $listTest $i]
	    return
	}
    }
    nextFunction
}

proc nextWithResult {} {
    global theUL theTest theStatus
    while 1 { 
	nextTest
	if {$theUL == ""} break
	if {$theTest == ""} continue
	if {$theStatus == "-"} continue
	break
    }
}

proc nextWithError {} {
    global theStatus theUL
    while 1 {
	nextWithResult
	if {$theUL == ""} break
	if {$theStatus == "*"} break
    }
}

####################################################
# run the current Test
####################################################

proc runTest {} {
    global themode theDraw Drawid 
    global RESULT PREVIOUS TEMP DIFF FILTRE
    global testRoot resultRoot theExec theLog
    global theUL theFunction theTest
    global ExePath  ExeMachinePath
    global theStation theTmp theDrawHome theTmp1
    global cmdexec protect
    global testRootNT env
    
    
    puts "  theUL $theUL theFunction $theFunction test  $theTest "
    puts "thest == $theStation"
    
    if {$theTest == ""} return
    
    set exe $theExec
    
    if {$exe == "" } {

	if {$theStation=="wnt"} { 
	    set exe "${ExePath}\\T${theUL}\\${ExeMachinePath}\\T${theUL}"
	} else {
	    set exe "${ExePath}/T${theUL}"
	}
    }
    
    if {![file exists $resultRoot]} {
	if {$theStation=="wnt"} {
	    set stat1 [catch {file mkdir $resultRoot} iscreated ]
	    #	    [exec $cmdexec $resultRoot] 
	} else {
	    catch {exec mkdir $resultRoot}
	    catch {exec chmod 777 $resultRoot}
	}
    }
    
    cd $resultRoot
    
    if {![file isdirectory $resultRoot/$theUL]} {
	if { $theStation == "wnt" }  {
	    set statpop [catch {file mkdir $resultRoot/$theUL} erreurfile ]
	    if { $statpop != 0 } { puts "erreurfile == $erreurfile " }
	    #	    [exec $cmdexec $protect $theUL]
	} else {
	    catch {exec mkdir $resultRoot/$theUL}
	    catch {exec chmod 777 $resultRoot/$theUL}
	}
    }
    
    cd $resultRoot/$theUL
    
    if {![file isdirectory $resultRoot/$theUL/$theFunction]} {
	if {$theStation=="wnt"} {
            set stat1 [catch {file mkdir $resultRoot/$theUL/$theFunction} iscreated ]
	    if {$stat1 != 0 } { puts "iscreated == $iscreated " }
	    #		[exec $cmdexec $protect $theUL]
	} else {
	    catch {exec mkdir $resultRoot/$theUL/$theFunction}
	    catch {exec chmod 777 $resultRoot/$theUL/$theFunction}
	}
    }
    cd $resultRoot/$theUL/$theFunction
    
    # run the test
    if {$theStation=="wnt"} {
	catch {file delete $theLog}
    } else {
	catch {exec rm -f $theLog}
    }
    
    foreach f [glob -nocomplain ${theTest}.*.xwd.Z] {catch {exec $cmdexec $del $f}}
   

    if {$theStation=="wnt"} {
	set f  $resultRoot/$theUL/$theFunction/${theTest}[pid]
	set f1 $resultRoot/$theUL/$theFunction/${theTest}[pid]
    	set ff [open $f1 w]
    } else {
	set f /tmp/theTest[pid]
	set ff [open $f w]
    }
    ## send current infos in "begin" script for customize by AQ Team  -plb/14feb97-
    puts $ff "set testinfos(resultRoot)  $resultRoot"
    puts $ff "set testinfos(theUL)       $theUL"
    puts $ff "set testinfos(theFunction) $theFunction"
    puts $ff "set testinfos(theTest)     $theTest"
    if {$themode == "samedraw"} {
	puts $ff "cd $resultRoot/$theUL/$theFunction"
    }
    
    close $ff

    puts "testRoot $testRoot"

    cd $resultRoot/$theUL/$theFunction

    if {$theStation=="wnt"} {
	set fff  $resultRoot/$theUL/$theFunction/${theTest}[pid]-pop
	set fpop [open $fff w]
	for_file line $f1                                    { puts $fpop $line }
	for_file line $testRoot/begin                        { puts $fpop $line }
	if {[file exist  $testRoot/$theUL/begin_wnt  ] } { 
	    for_file line $testRoot/$theUL/begin_wnt             { puts $fpop $line }
	}
	if {[file exist  $testRoot/$theUL/$theFunction/begin_wnt]} {
	    for_file line $testRoot/$theUL/$theFunction/begin_wnt { puts $fpop $line }
	} else {
	    if {[file exist  $testRoot/$theUL/$theFunction/begin]} {
		for_file line $testRoot/$theUL/$theFunction/begin { puts $fpop $line }
	    }
	}	
	for_file line $testRoot/$theUL/$theFunction/$theTest { puts $fpop $line }
	if { [ file exists $testRoot/$theUL/end ] } {
	    for_file line $testRoot/$theUL/end                   { puts $fpop $line }
	}
	if { [ file exists $testRoot/end ] } {
	    for_file line $testRoot/end                          { puts $fpop $line }
	}
	puts $fpop "exit"
	if {[file exist $resultRoot/$theUL/$theFunction/${theTest}[pid] ] } {
	    file delete $resultRoot/$theUL/$theFunction/${theTest}[pid]
	}
	close $fpop
	file copy $fff $f1
	file delet $fff
    } else {
	catch {exec cat -s \
		   $testRoot/begin \
		   $testRoot/$theUL/begin \
		   $testRoot/$theUL/$theFunction/begin \
		   $testRoot/$theUL/$theFunction/$theTest \
		   $testRoot/$theUL/$theFunction/end \
		   $testRoot/$theUL/end \
		   $testRoot/end >> $f}
	
	set testroot $testRoot
    }
    if {$theStation=="wnt"} {
	set f3 $resultRoot/$theUL/$theFunction/${theTest}.log
	if { [file exists $f3] } { file delete $f3 } 
	puts "l'executable == $exe"
	puts "testRoot == $testRoot"
	#puts "input  == $f1"
	#puts "output == $f3"
	catch { exec $exe -f $f1 -o $f3 } popstatus
	puts $popstatus
	
    } else {
	if {$themode != "samedraw" } {
	    catch { exec $exe -f $f >& ${theTest}.log }
	} else {
	    # the draw est il toujours valide ?
	    set myList [winfo interps]
	    set DrawExists "0"
	    for {set i 0} { $i < [expr [llength $myList]] } { incr i } {
		set p [lindex $myList $i]
		if {$p == $theDraw} {set DrawExists "1"}
	    }
	    if { $DrawExists == "0"} {
		puts "Pas de Draw, on en lance un !!"
	    }
	    while { $DrawExists == "0"} {
		set myList [winfo interps]
		set mylen [llength $myList]
		while { [catch { exec $theExec -l >& $RESULT & } message] } {
		    puts $message
		}
		set Drawid $message
		# On espere avoir lance un draw, on recupere son nom
		# comme on peut. La methode utilisee est extremement
		# plantatoire, il faudrait pouvoir trouver le numero
		# du process !?!
		set i 0
		while { $myList == [winfo interps] && [expr $i] < 20 } {
		    sleep 1
		    puts "1s d attente"
		    incr i
		}
		if {  [expr $i] < 20 } {
		    sleep 2
		    puts "2s d attente"
		    set newList [winfo interps]
		    set newlen [llength $newList]
		    set theDraw ""
		    puts $myList
		    puts $newList
		    for {set i 0} { $i < [expr $newlen] && !$DrawExists } { incr i } {
			set DrawExists 1
			set theDraw [lindex $newList $i]
			for {set j 0} { $j < [expr $mylen]} {incr j} {
			    set p [lindex $myList $j]
			    if { $p == $theDraw} {
				set DrawExists 0
			    }
			}
		    }
		}
		if { $DrawExists } {
		    puts "nouvelle appli : $theDraw"
		} else {
		    puts "echec creation nouveau Draw, on recommence"
		    set theDraw ""
		    if { [catch { exec kill -9 $Drawid } mes] } { 
			puts $mes 
		    } else {
			puts "le process etait bien la, mais le Draw n est pas venu!?!"
		    }
		}
	    }
	    
	    exec cp $RESULT $PREVIOUS
	    
	    if { [catch {send $theDraw "source $f"} mes]} {
		puts $mes
		puts "on tue le Draw"
		set theDraw ""
		if { [catch { exec kill -9 $Drawid } mes] } { puts $mes }
	    }

	    catch {exec $DIFF $PREVIOUS $RESULT > $TEMP}
	    catch {exec cat $TEMP | $FILTRE >> ${theTest}.log}
	}
	
    }
    

    if {$theStation=="wnt"} {
	catch {file delete $f1} 
    } else {
        catch {exec rm -f $f}
    }
    
    if { $theStation == "wnt" } {
    	catch {exec $cmdexec attrib ${theTest}.log}
    } else {
	catch { exec chmod 666 ${theTest}.log }
    }
    
    
    # process photos    
    foreach f [glob -nocomplain photo*] {
	set g $theTest.[string range $f 5 end].xwd
	puts "Processing $g"
	if {$theStation=="wnt"} { 
	    catch {exec $cmdexec $move $f $g}
	} else {
	    catch { exec mv -f $f $g }
	}
	catch {exec compress $g}
	if {$theStation=="wnt"} { 
	    catch { exec $cmdexec attrib $g.Z }
	} else {
	    catch { exec chmod 666 $g.Z }
	}
    }
    
    #enforce update
    set theTest $theTest
}

####################################################
# send the current test 
# to the process $theDraw
####################################################

proc sendTest {} {
    global testRoot resultRoot theExec theLog theDraw
    global theUL theFunction theTest
    global theStation
    
    if {$theTest == ""} return
    
    puts "Sending $theUL $theFunction $theTest to $theDraw"
    if {$theStation=="wnt"} {
	set f  $resultRoot\\\\$theUL\\\\$theFunction\\\\${theTest}[pid]
	set f1 $resultRoot\\\\$theUL\\\\$theFunction\\\\${theTest}[pid]
	set f2 $resultRoot\\\\$theUL\\\\$theFunction\\\\${theTest}.log
    	set ff [open $f1 w]
    	set gg [open $f2 w]
	#set ff2 [open $f2 w]
	puts $ff "set testroot $testRootNT"
    } else {
	set f /tmp/theTest[pid]
	set ff [open $f w]
	puts $ff "set testroot $testRoot"
    }
    
## send current infos in "begin" script for customize by AQ Team  -plb/14feb97-
    puts $ff "set testinfos(resultRoot)  $resultRoot"
    puts $ff "set testinfos(theUL)       $theUL"
    puts $ff "set testinfos(theFunction) $theFunction"
    puts $ff "set testinfos(theTest)     $theTest"
    close $ff

    if {$theStation=="wnt"} {
	set stat1 [catch {exec cmd /C copy /A \ $f1 + $testRoot\\begin + $testRoot\\$theUL\\begin_wnt + $testRoot\\$theUL\\$theFunction\\begin + $testRoot\\$theUL\\$theFunction\\$theTest + $testRoot\\$theUL\\$theFunction\\end + $testRoot\\$theUL\\end + $testRoot\\end + $f1} myerro]
	send $theDraw "source $f1"
    } else {
	catch {exec cat -s \
		   $testRoot/$theUL/begin \
		   $testRoot/$theUL/$theFunction/begin \
		   $testRoot/$theUL/$theFunction/$theTest \
		   $testRoot/$theUL/$theFunction/end \
		   $testRoot/$theUL/end  >> $f}
	send $theDraw "source $f"
    }
}

####################################################
# clear result, copy master
####################################################

proc clearResult {} {
    global theTest theLog theXWDFiles
    catch {exec rm -f $theLog}
    foreach f  $theXWDFiles {catch {exec rm -f $f}}

    #enforce update
    set theTest $theTest
}

proc copyMaster {} {

    global theLog theXWDFiles
    global theMasterLog theMasterXWDFiles
    global masterRoot theUL theFunction theTest

    catch {exec rm -f $theMasterLog}
    foreach f $theMasterXWDFiles {catch {exec rm -f  $f}}

    if {![file isdirectory $masterRoot/$theUL]} {
	if {$theStation=="wnt"} {
		cd $masterRoot
		file mkdir $theUL
		#catch {exec $cmdexec mkdir $masterRoot\\$theUL}
		#catch {exec $cmdexec $protect $masterRoot\\$theUL}
	} else {
	    catch {exec mkdir $masterRoot/$theUL}
	    catch {exec chmod 777 $masterRoot/$theUL}
	}
    }

    if {![file isdirectory $masterRoot/$theUL/$theFunction]} {
	if {$theStation=="wnt"} {
		cd $theUL
		file mkdir $theFunction
		#catch {exec $cmdexec mkdir $masterRoot\\$theUL\\$theFunction}
		#catch {exec $cmdexec $protect $masterRoot\\$theUL\\$theFunction}
	} else {
	    catch {exec mkdir $masterRoot/$theUL/$theFunction}
	    catch {exec chmod 777 $masterRoot/$theUL/$theFunction}
	}
    }

    if {$theStation=="wnt"} {set d $masterRoot\\$theUL\\$theFunction
} else {set d $masterRoot/$theUL/$theFunction}
    if [file readable $theLog] {catch {exec cp $theLog $d}}
    foreach f $theXWDFiles {catch {exec cp $f $d}}

    # enforce update
    set theTest $theTest
}

####################################################
# Make the initial list of UL
####################################################

proc init {} {
    global listUL testRoot
    set listUL [mkListTests $testRoot]
}
