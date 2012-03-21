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
#
#  procedures pour automatiser la fabrication de gti
#
#
puts " *****  loading  build_list   ***** "
###############################################################################
#
#
proc build_list { factory ilot workbench  exelist toolkitlist   packagelist } {
#
#
###############################################################################
# fabrique la liste des executables, toolkits et packages
# pour un workbench donne
#

    upvar $exelist      EXECUTABLE_LIST
    upvar $toolkitlist  TOOLKIT_LIST
    upvar $packagelist  PACKAGE_LIST
    wokcd [concat $factory:$ilot:$workbench]
    wokprofile -d
    set UD_LIST [ w_info -l ]
    set PACKAGE_LIST ""
    set EXECUTABLE_LIST ""
    set TOOLKIT_LIST ""
    for { set i 0 } { $i <= [expr [ llength $UD_LIST ] - 1 ]} { incr i } {
	set p [lindex $UD_LIST $i]
	set typ [ uinfo -t $p ]
	if { $typ == "executable" } {
	    lappend EXECUTABLE_LIST $p
	} elseif { $typ == "toolkit" } {
	    lappend TOOLKIT_LIST $p
	} elseif { $typ == "package" || $typ == "nocdlpack" } {
	    lappend PACKAGE_LIST $p
	}
    }
}
puts " *****  loading   extract_from_cdl ***** "
###############################################################################
#
#
proc extract_from_cdl { factory ilot workbench } {
#
#
###############################################################################
    build_list $factory $ilot $workbench  TYPE_OF_PACKAGE_LIST(1) TYPE_OF_PACKAGE_LIST(2) TYPE_OF_PACKAGE_LIST(3) 
    puts " *********** Extraction commencing at "
    puts "[exec date]"
    for { set i 0 } { $i <= [expr [ llength $TYPE_OF_PACKAGE_LIST(3) ] - 1 ]} { incr i } {
	    set PACKAGE [lindex $TYPE_OF_PACKAGE_LIST(3) $i]
	    wokcd $PACKAGE
	    set LOCAL_UD [ wokcd ] 

            
	    set ERROR_FILE [ wokinfo -p admfile:cdlcompile.log $LOCAL_UD ] 
            set ERROR_DIRECTORY [ file dirname $ERROR_FILE ] 
	    puts " $ERROR_FILE "
#
#  attention : verifier que la directory existe 
#
	    if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		set FILE_DESCRIPTOR [ open $ERROR_FILE w ] 
#
#   on initialise avec un chaine null le fichier
#
		puts $FILE_DESCRIPTOR ""
#
#   on redirige les erreurs avec un dispatcheur
#
		msgsetcmd dispatch_compile_message $FILE_DESCRIPTOR
	    }
	    catch { umake -fe obj.inc } 
#           puts "umake -fe obj.inc"
	    if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		close $FILE_DESCRIPTOR
		msgunsetcmd
            }
	
    }
    puts " ********** Extraction ending at " 
    puts " [exec date]"
    msclear
}
puts " *****  loading  force_build_only_obj *****  "
###############################################################################
#
#
proc force_build_only_obj { factory ilot workbench } {
#
#
###############################################################################
    build_list $factory $ilot $workbench  TYPE_OF_PACKAGE_LIST(1) TYPE_OF_PACKAGE_LIST(2) TYPE_OF_PACKAGE_LIST(3) 
    puts " *********   Obj commencing at "
    puts " [ exec date ] "
    for { set i 0 } { $i <= [expr [ llength $TYPE_OF_PACKAGE_LIST(3) ] - 1 ]} { incr i } {
	set p [lindex $TYPE_OF_PACKAGE_LIST(3) $i]
	wokcd $p
	set LOCAL_UD [ wokcd ] 
	set ERROR_FILE [ wokinfo -p stadmfile:objcompile.log $LOCAL_UD ]
        set ERROR_DIRECTORY [ file dirname $ERROR_FILE ] 
	puts " $ERROR_FILE "
#
#  attention : verifier que la directory existe 
#
	if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		set FILE_DESCRIPTOR [ open $ERROR_FILE w ] 
#
#   on initialise avec un chaine null le fichier
#
		puts $FILE_DESCRIPTOR ""
#
#   on redirige les erreurs avec un dispatcheur
#
		msgsetcmd dispatch_compile_message $FILE_DESCRIPTOR
	    } 
	catch { umake -of obj } 
#       puts " umake -o obj  $p "
	if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		close $FILE_DESCRIPTOR
		msgunsetcmd 
	}

	
    }
    for { set j 1 } { $j <= 2 } { incr j } {
	for { set i 0 } { $i <= [expr [ llength $TYPE_OF_PACKAGE_LIST($j) ] - 1 ]} { incr i } {
	    set p [lindex $TYPE_OF_PACKAGE_LIST($j) $i]
	    wokcd $p
	    set LOCAL_UD [ wokcd ] 
	    set ERROR_FILE [ wokinfo -p stadmfile:objcompile.log $LOCAL_UD ]
            set ERROR_DIRECTORY [ file dirname $ERROR_FILE ] 
	    puts " $ERROR_FILE "
#
#  attention : verifier que la directory existe 
#
	    if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		set FILE_DESCRIPTOR [ open $ERROR_FILE w ] 
#
#   on initialise avec un chaine null le fichier
#
		puts $FILE_DESCRIPTOR ""
#
#   on redirige les erreurs avec un dispatcheur
#
		msgsetcmd dispatch_compile_message $FILE_DESCRIPTOR
	    } 
	    
	    catch { umake -f  } 
#           puts " umake $p "
	    if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		close $FILE_DESCRIPTOR
		msgunsetcmd 
	    }
	}
    }

    puts " *********   Obj ending at "
    puts " [ exec date ] " 
    msclear
}
puts " *****  loading  build_only_obj ****** "
###############################################################################
#
#
proc build_only_obj { factory ilot workbench } {
#
#
###############################################################################

    build_list $factory $ilot $workbench  TYPE_OF_PACKAGE_LIST(1) TYPE_OF_PACKAGE_LIST(2) TYPE_OF_PACKAGE_LIST(3) 
    puts " *********   Obj commencing at "
    puts " [ exec date ] "
    for { set i 0 } { $i <= [expr [ llength $TYPE_OF_PACKAGE_LIST(3) ] - 1 ]} { incr i } {
	set p [lindex $TYPE_OF_PACKAGE_LIST(3) $i]
	wokcd $p
	set LOCAL_UD [ wokcd ] 
	set ERROR_FILE [ wokinfo -p stadmfile:objcompile.log $LOCAL_UD ]
        set ERROR_DIRECTORY [ file dirname $ERROR_FILE ] 
	puts " $ERROR_FILE "
#
#  attention : verifier que la directory existe 
#
	if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		set FILE_DESCRIPTOR [ open $ERROR_FILE w ] 
#
#   on initialise avec un chaine null le fichier
#
		puts $FILE_DESCRIPTOR ""
#
#   on redirige les erreurs avec un dispatcheur
#
		msgsetcmd dispatch_compile_message $FILE_DESCRIPTOR
	    } 
	catch { umake -o obj } 
#       puts " umake -o obj  $p "
	if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		close $FILE_DESCRIPTOR
		msgunsetcmd 
	}

	
    }
    for { set j 1 } { $j <= 2 } { incr j } {
	for { set i 0 } { $i <= [expr [ llength $TYPE_OF_PACKAGE_LIST($j) ] - 1 ]} { incr i } {
	    set p [lindex $TYPE_OF_PACKAGE_LIST($j) $i]
	    wokcd $p
	    set LOCAL_UD [ wokcd ] 
	    set ERROR_FILE [ wokinfo -p stadmfile:objcompile.log $LOCAL_UD ]
            set ERROR_DIRECTORY [ file dirname $ERROR_FILE ] 
	    puts " $ERROR_FILE "
#
#  attention : verifier que la directory existe 
#
	    if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		set FILE_DESCRIPTOR [ open $ERROR_FILE w ] 
#
#   on initialise avec un chaine null le fichier
#
		puts $FILE_DESCRIPTOR ""
#
#   on redirige les erreurs avec un dispatcheur
#
		msgsetcmd dispatch_compile_message $FILE_DESCRIPTOR
	    } 
	    
	    catch { umake  } 
#           puts " umake $p "
	    if { [ file isdirectory  $ERROR_DIRECTORY ] } {
		close $FILE_DESCRIPTOR
		msgunsetcmd 
	    }
	}
    }

    puts " *********   Obj ending at "
    puts " [ exec date ] " 
    msclear
}
###############################################################################
#
#
proc dispatch_compile_message { code message file } {
#
#
###############################################################################
    puts " $message "
    if { $code == "E" } {
	    puts $file " $message "
     } 
#
#  E les erreurs
#       
#
#  V verbose 
#
#
#  I les infos
#
#
#  W les warnings
#
    
     
}
puts " *****  loading  build_obj_log *****  "
###############################################################################
#
#
proc build_obj_log { factory ilot workbench } {
#
#
###############################################################################
    puts " Erreurs dans les compiles obj :"
    build_list $factory $ilot $workbench  TYPE_OF_PACKAGE_LIST(1) TYPE_OF_PACKAGE_LIST(2) TYPE_OF_PACKAGE_LIST(3) 
    for { set j 2 } { $j <= 3 } { incr j } {
	for { set i 0 } { $i <= [expr [ llength $TYPE_OF_PACKAGE_LIST($j) ] - 1 ]} { incr i } {
	    set p [lindex $TYPE_OF_PACKAGE_LIST($j) $i]
	    wokcd $p
	    set LOCAL_UD [ wokcd ] 
	    set ERROR_FILE [ wokinfo -p stadmfile:objcompile.log $LOCAL_UD ] 
	    if { [ file exists $ERROR_FILE ] } {
		set FILE_DESCRIPTOR [ open $ERROR_FILE r ]
		set ERROR_HAPPENED 0 
		while { [ gets $FILE_DESCRIPTOR LINE ] >= 0 && $ERROR_HAPPENED == 0  } {
		    if { [ regexp Error $LINE ] } {
   		        puts "$LOCAL_UD"
			set ERROR_HAPPENED 1
		    }
		}
		close $FILE_DESCRIPTOR
	    }
        }
    }
}
puts " *****  loading   build_cdl_log ******  "
###############################################################################
#
#
proc build_cdl_log { factory ilot workbench } {
#
#
###############################################################################
    puts "Erreurs dans les compiles cdl :"
    build_list $factory $ilot $workbench  TYPE_OF_PACKAGE_LIST(1) TYPE_OF_PACKAGE_LIST(2) TYPE_OF_PACKAGE_LIST(3) 
    for { set i 0 } { $i <= [expr [ llength $TYPE_OF_PACKAGE_LIST(3) ] - 1 ]} { incr i } {
	    set p [lindex $TYPE_OF_PACKAGE_LIST(3) $i]
	    wokcd $p
	    set LOCAL_UD [ wokcd ] 
	    set ERROR_FILE [ wokinfo -p admfile:cdlcompile.log $LOCAL_UD ] 
	    if { [ file exists $ERROR_FILE ] } {
		set FILE_DESCRIPTOR [ open $ERROR_FILE r ] 
		set ERROR_HAPPENED  0
		while { [ gets $FILE_DESCRIPTOR LINE ] >= 0 && $ERROR_HAPPENED == 0} {
		    if { [ regexp Error $LINE ] } {
			puts "$LOCAL_UD "
			set ERROR_HAPPENED 1 
		    }
		}
		close $FILE_DESCRIPTOR
	    }
   }
}




