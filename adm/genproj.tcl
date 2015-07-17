# =======================================================================
# This script generates project files for different IDEs:
#      "vc7" "vc8" "vc9" "vc10" "vc11" "vc12" "cbp" "amk"
#
# Example:
#      genproj -path=D:/occt -target=vc10
# =======================================================================

set path ""
set fBranch ""
switch -exact -- "$tcl_platform(platform)" {
    "windows"   {set targetStation "wnt"}
    "unix"      {set targetStation "lin"}
    #"macos"   {set targetStation "wnt"}
}

proc _get_options { platform type branch } {
  global path
  set res ""
  if {[file exists "$path/adm/CMPLRS"]} {
    set fd [open "$path/adm/CMPLRS" rb]
    set opts [split [read $fd] "\n"]
    close $fd
    foreach line $opts {
      if {[regexp "^${platform} ${type} ${branch} (.+)$" $line dummy res]} {
        while {[regexp {\(([^\(\)]+) ([^\(\)]+) ([^\(\)]+)\)(.+)} $res dummy p t b oldres]} {
          set res "[_get_options $p $t $b] $oldres"
        }
      }
    }
  }
  return $res
}

proc _get_type { name } {
  global path
  if {[file exists "$path/adm/UDLIST"]} {
    set fd [open "$path/adm/UDLIST" rb]
    set UDLIST [split [read $fd] "\n"]
    close $fd
    foreach uitem $UDLIST {
      set line [split $uitem]
      if {[lindex $line 1] == "$name"} {
        return [lindex $line 0]
      }
    }
  }
  return ""
}

proc _get_used_files { pk {inc true} {src true} } {
  global path
  set type [_get_type $pk]
  set lret {}
  set pk_path  "$path/src/$pk"
  set FILES_path "$path/src/$pk/FILES"
  set FILES {}
  if {[file exists $FILES_path]} {
    set fd [open $FILES_path rb]
    set FILES [split [read $fd] "\n"]
    close $fd
  }
  set FILES [lsearch -inline -all -not -exact $FILES ""]

  set index -1
  foreach line $FILES {
    incr index
    if {$inc && ([regexp {([^:\s]*\.[hgl]xx)$} $line dummy name] || [regexp {([^:\s]*\.h)$} $line dummy name]) && [file exists $pk_path/$name]} {
      lappend lret "pubinclude $name $pk_path/$name"
      continue
    }
    if {[regexp {:} $line]} {
      regexp {[^:]*:+([^\s]*)} $line dummy line
    }
    regexp {([^\s]*)} $line dummy line
    if {$src && [file exists $pk_path/$line]} {
      lappend lret "source $line $pk_path/$line"
    }
  }
  return $lret
}

# Wrapper-function to generate VS project files
proc genproj { args } {
  global path targetStation
  set aSupportedTargets { "vc7" "vc8" "vc9" "vc10" "vc11" "vc12" "cbp" "amk" }
  set anArgs $args

  # Setting default IDE.
  set anTarget ""
  switch -exact -- "$targetStation" {
    "wnt"   {set anTarget "$::env(VCVER)"}
    "lin"   {set anTarget "amk"}
    "mac"   {set anTarget "cbp"}
  }

  set isTargetDefault true

  if { [set anIndex [lsearch -nocase $anArgs -target=*]] != -1 } {
    regsub -nocase "\\-target=" [lindex $anArgs $anIndex] "" anTarget
    set anArgs [removeAllOccurrencesOf -target=* $anArgs]
    set isTargetDefault false
  }

  if { [set anIndex [lsearch -nocase $anArgs -path=*]] != -1} {
    regsub -nocase "\\-path=" [lindex $anArgs $anIndex] "" path
    set anArgs [removeAllOccurrencesOf -path=* $anArgs]
    puts "Starting work with \"$path\""
  }

  if { [llength $anArgs] == 0 && $isTargetDefault == true } {
    puts "the default \'$anTarget\' target has been applied"
  }

  set isHelpRequire false
  if { [lsearch -nocase $anArgs -h] != -1} {
    set anArgs [removeAllOccurrencesOf -h $anArgs]
    set isHelpRequire true
  }

  if {$path == ""} {
    set isHelpRequire true
  }

  if { [lsearch -nocase $aSupportedTargets $anTarget] == -1} {
    puts "the \'$anTarget\' is wrong TARGET"
    set isHelpRequire true
  }

  if {[llength $anArgs] > 0} {
    set isHelpRequire true

    foreach anArg $anArgs {
      puts "genproj: unrecognized option \'$anArg\'"
    }
  }

  if {  $isHelpRequire == true } {
    puts "usage: genproj \[ -target=<TARGET> \] \[ -path=<PATH> \]

    PATH: 
      path to the project

    TARGET:
      vc8   -  Visual Studio 2005
      vc9   -  Visual Studio 2008
      vc10  -  Visual Studio 2010
      vc11  -  Visual Studio 2012
      vc12  -  Visual Studio 2013
      cbp   -  CodeBlocks
      amk   -  AutoMake"
      return
  }

  if {!$isTargetDefault} {
    puts "the \'$anTarget\' target has been applied"
  }
  
  if {"$anTarget" == "amk"} {
    set targetStation "lin"
  }
  set anAdmPath "$path/adm"

  OS:MKPRC "$anAdmPath" "$anTarget"

  genprojbat "$anAdmPath" "$anTarget"
}

proc genprojbat {thePath theIDE} {
  global path
  
  set anOsIncPath "$path/src/OS"
  set anOsRootPath "$path"

  set aTargetPlatform "lin"
  if { "$::tcl_platform(platform)" == "windows" } {
    set aTargetPlatform "wnt"
  }
  
  if {[regexp {(vc)[0-9]*$} $theIDE] == 1} {
    set aTargetPlatform wnt
  } elseif {"$theIDE" == "amk"} {
    set aTargetPlatform lin
  }

  set aTargetPlatformExt sh
  if { "$aTargetPlatform" == "wnt" } {
    set aTargetPlatformExt bat
  }

  set aBox [file normalize "$thePath/.."]

  if { "$aTargetPlatform" != "wnt" } {
    file copy -force -- "$path/adm/templates/config.h" "$aBox/src/config.h"
  }

  if {"$theIDE" != "cmake"} {
    set anEnvTmplFile [open "$path/adm/templates/env.${aTargetPlatformExt}" "r"]
    set anEnvTmpl [read $anEnvTmplFile]
    close $anEnvTmplFile

    set aCasRoot ""
    if { [file normalize "$anOsRootPath"] != "$aBox" } {
      set aCasRoot [relativePath "$aBox" "$anOsRootPath"]
    }
    set anOsIncPath [relativePath "$aBox" "$anOsRootPath"]

    regsub -all -- {__CASROOT__}   $anEnvTmpl "$aCasRoot" anEnvTmpl

    set anEnvFile [open "$aBox/env.${aTargetPlatformExt}" "w"]
    puts $anEnvFile $anEnvTmpl
    close $anEnvFile

    file copy -force -- "$path/adm/templates/draw.${aTargetPlatformExt}" "$aBox/draw.${aTargetPlatformExt}"
  }

  if {[regexp {(vc)[0-9]*$} $theIDE] == 1} {
    file copy -force -- "$path/adm/templates/msvc.bat" "$aBox/msvc.bat"
  } else {
    switch -exact -- "$theIDE" {
      "cbp"   { file copy -force -- "$path/adm/templates/codeblocks.sh" "$aBox/codeblocks.sh" }
    }
  }
}

###### MSVC #############################################################33
proc removeAllOccurrencesOf { theObject theList } {
  set aSortIndices [lsort -decreasing [lsearch -all -nocase $theList $theObject]]
  foreach anIndex $aSortIndices {
    set theList [lreplace $theList $anIndex $anIndex]
  }
  return $theList
}

proc OS:mkdir { d } {
    global tcl_version
    if ![file exists $d] {
	if { "$tcl_version" == "7.5" } {
	    mkdir -path $d
	} else {
	    file mkdir $d
	}
	if [file exists $d] {
	    return $d
	} else {
	    return {}
	}
    } else {
	return $d
    }
}

# Entry function to generate project files and solutions for IDE
proc OS:MKPRC { {theOutDir {}} {theIDE ""} } {
  global path targetStation
  set aSupportedIDE { "vc7" "vc8" "vc9" "vc10" "vc11" "vc12" "cbp" "amk" }

  if { [lsearch $aSupportedIDE $theIDE] < 0 } {
    puts stderr "WOK does not support generation of project files for the selected IDE: $theIDE\nSupported IDEs: [join ${aSupportedIDE} " "]"
    return
  }
  
  set anOutRoot $theOutDir
  if { $anOutRoot == "" } {
    error "Error : \"theOutDir\" is not initialized"
  }

  # Create output directory
  set aWokStation "$targetStation"

  if { [lsearch -exact {vc7 vc8 vc9 vc10 vc11 vc12} $theIDE] != -1 } {
    set aWokStation "msvc"
  }

  set anOutDir "${anOutRoot}/${aWokStation}/${theIDE}"

  # read map of already generated GUIDs
  set aGuidsFilePath [file join $anOutDir "wok_${theIDE}_guids.txt"]
  if [file exists "$aGuidsFilePath"] {
    set aFileIn [open "$aGuidsFilePath" r]
    set aFileDataRaw [read $aFileIn]
    close $aFileIn
    set aFileData [split $aFileDataRaw "\n"]
    foreach aLine $aFileData {
      set aLineSplt [split $aLine "="]
      if { [llength $aLineSplt] == 2 } {
        set ::THE_GUIDS_LIST([lindex $aLineSplt 0]) [lindex $aLineSplt 1]
      }
    }
  }

  # make list of modules and platforms
  set aModules [OS:init]

  # generate one solution for all projects if complete OS or VAS is processed
  set anAllSolution "OCCT"

  OS:mkdir $anOutDir
  if { ![file exists $anOutDir] } {
    puts stderr "Error: Could not create output directory \"$anOutDir\""
    return
  }

  # create the out dir if it does not exist
  if (![file isdirectory $path/inc]) {
    puts "$path/inc folder does not exists and will be created"
    wokUtils:FILES:mkdir $path/inc
  }

  # collect all required header files
  puts "Collecting required header files into $path/inc ..."
  osutils:collectinc $aModules $path/inc $targetStation

  # Generating project files for the selected IDE
  switch -exact -- "$theIDE" {
    "vc7"   -
    "vc8"   -
    "vc9"   -
    "vc10"   -
    "vc11"   -
    "vc12"  { OS:MKVC  $anOutDir $aModules $anAllSolution $theIDE }
    "cbp"   { OS:MKCBP $anOutDir $aModules $anAllSolution }
    "amk"   { OS:MKAMK $anOutDir $aModules "adm/${aWokStation}/${theIDE}"}
  }
  # generate config.txt file
  #if { ${anAllSolution} == "Products" && "$targetStation" == "wnt" } {
  #  osutils:mkCollectScript "collect_binary.cfg" "$anOutRoot/../" ${theIDE} $::env(ARCH) "release"
  #}

  # Store generated GUIDs map
  set anOutFile [open "$aGuidsFilePath" "w"]
  fconfigure $anOutFile -translation lf
  foreach aKey [array names ::THE_GUIDS_LIST] {
    set aValue $::THE_GUIDS_LIST($aKey)
    puts $anOutFile "${aKey}=${aValue}"
  }
  close $anOutFile
}

# Function to generate Visual Studio solution and project files
proc OS:MKVC { theOutDir {theModules {}} {theAllSolution ""} {theVcVer "vc8"} } {

  puts stderr "Generating VS project files for $theVcVer"

  # generate projects for toolkits and separate solution for each module
  foreach aModule $theModules {
    OS:vcsolution $theVcVer $aModule $aModule $theOutDir ::THE_GUIDS_LIST
    OS:vcproj     $theVcVer $aModule          $theOutDir ::THE_GUIDS_LIST
  }

  # generate single solution "OCCT" containing projects from all modules
  if { "$theAllSolution" != "" } {
    OS:vcsolution $theVcVer $theAllSolution $theModules $theOutDir ::THE_GUIDS_LIST
  }

  puts "The Visual Studio solution and project files are stored in the $theOutDir directory"
}

proc OS:init {{os {}}} {
    global path
    global env
    global tcl_platform
    
    set askplat $os
    if { "$os" == "" } {
      set os $tcl_platform(os)
    }

    ;# Load list of OCCT modules and their definitions
    source "$path/src/OS/Modules.tcl"
    set Modules {}
    foreach module [OS:Modules] {
        set f "$path/src/OS/${module}.tcl"
        if [file exists $f] {
            source $f
            lappend Modules $module
        } else {
            puts stderr "Definition file for module $module is not found in unit OS"
        }
    }

    # Load list of products and their definitions
#    set Products [woklocate -p VAS:source:Products.tcl]
    #if { "$Products" != "" } {
	#source "$Products"
	#foreach product [VAS:Products] {
	    #set f [woklocate -p VAS:source:${product}.tcl]
	    #if [file exists $f] {
		#source $f
	    #} else {
		#puts stderr "Definition file for product $product is not found in unit VAS"
	    #}
	#}
    #}

    return $Modules
}

# topological sort. returns a list {  {a h} {b g} {c f} {c h} {d i}  } => { d a b c i g f h }
proc wokUtils:EASY:tsort { listofpairs } {
    foreach x $listofpairs {
	set e1 [lindex $x 0]
	set e2 [lindex $x 1]
	if ![info exists pcnt($e1)] {
	    set pcnt($e1) 0
	}
	if ![ info exists pcnt($e2)] {
	    set pcnt($e2) 1
	} else {
	    incr pcnt($e2)
	}
	if ![info exists scnt($e1)] {
	    set scnt($e1) 1
	} else {
	    incr scnt($e1)
	}
	set l {}
	if [info exists slist($e1)] {
	    set l $slist($e1)
	}
	lappend l $e2
	set slist($e1) $l
    }
    set nodecnt 0
    set back 0
    foreach node [array names pcnt] {
	incr nodecnt
	if { $pcnt($node) == 0 } {
	    incr back
	    set q($back) $node
	}
	if ![info exists scnt($node)] {
	    set scnt($node) 0
	}
    }
    set res {}
    for {set front 1} { $front <= $back } { incr front } {
	lappend res [set node $q($front)]
	for {set i 1} {$i <= $scnt($node) } { incr i } {
	    set ll $slist($node)
	    set j [expr {$i - 1}]
	    set u [expr { $pcnt([lindex $ll $j]) - 1 }]
	    if { [set pcnt([lindex $ll $j]) $u] == 0 } {
		incr back
		set q($back) [lindex $ll $j]
	    }
	}
    }
    if { $back != $nodecnt } {
	puts stderr "input contains a cycle"
	return {}
    } else {
	return $res
    }
}

proc wokUtils:LIST:Purge { l } {
    set r {}
     foreach e $l {
	 if ![info exist tab($e)] {
	     lappend r $e
	     set tab($e) {}
	 } 
     }
     return $r
}

# Read file pointed to by path
# 1. sort = 1 tri 
# 2. trim = 1 plusieurs blancs => 1 seul blanc
# 3. purge= not yet implemented.
# 4. emptl= dont process blank lines
proc wokUtils:FILES:FileToList { path {sort 0} {trim 0} {purge 0} {emptl 1} } {
    if ![ catch { set id [ open $path r ] } ] {
	set l  {}
	while {[gets $id line] >= 0 } {
	    if { $trim } {
		regsub -all {[ ]+} $line " " line
	    }
	    if { $emptl } {
		if { [string length ${line}] != 0 } {
		    lappend l $line
		}
	    } else {
		lappend l $line
	    }
	}
	close $id
	if { $sort } {
	    return [lsort $l]
	} else {
	    return $l
	}
    } else {
	return {}
    }
}

# retorn the list of executables in module.
proc OS:executable { module } {
    set lret {}
    foreach XXX  [${module}:ressources] {
	if { "[lindex $XXX 1]" == "x" } {
	    lappend lret [lindex $XXX 2]
	}
    }
    return $lret
}

# Topological sort of toolkits in tklm
proc osutils:tk:sort { tklm } {
  set tkby2 {}
  foreach tkloc $tklm {
    set lprg [wokUtils:LIST:Purge [osutils:tk:close $tkloc]]
    foreach tkx  $lprg {
      if { [lsearch $tklm $tkx] != -1 } {
        lappend tkby2 [list $tkx $tkloc]
      } else {
        lappend tkby2 [list $tkloc {}]
      }
    }
  }
  set lret {}
  foreach e [wokUtils:EASY:tsort $tkby2] {
    if { $e != {} } {
      lappend lret $e
    }
  }
  return $lret
}

#  close dependencies of ltk. (full wok pathes of toolkits)
# The CURRENT WOK LOCATION MUST contains ALL TOOLKITS required.
# (locate not performed.)
proc osutils:tk:close { ltk } {
  set result {}
  set recurse {}
  foreach dir $ltk {
    set ids [LibToLink $dir]
    set eated [osutils:tk:eatpk $ids]
    set result [concat $result $eated]
    set ids [LibToLink $dir]
    set result [concat $result $ids]

    foreach file $eated {
      set kds "$path/src/$file/EXTERNLIB"
      if { [osutils:tk:eatpk $kds] !=  {} } {
        lappend recurse $file
      }
    }
  }
  if { $recurse != {} } {
    set result [concat $result [osutils:tk:close $recurse]]
  }
  return $result
}

proc osutils:tk:eatpk { EXTERNLIB  } {
  set l [wokUtils:FILES:FileToList $EXTERNLIB]
  set lret  {}
  foreach str $l {
    if ![regexp -- {(CSF_[^ ]*)} $str csf] {
      lappend lret $str
    }
  }
  return $lret
}
# Define libraries to link using only EXTERNLIB file

proc LibToLink {theTKit} {
  global path
  regexp {^.*:([^:]+)$} $theTKit dummy theTKit
  set type [_get_type $theTKit]
  if {$type != "t" && $type != "x"} {
    return
  }
  set aToolkits {}
  set anExtLibList [osutils:tk:eatpk "$path/src/$theTKit/EXTERNLIB"]
  foreach anExtLib $anExtLibList {
    set aFullPath [LocateRecur $anExtLib]
    if { "$aFullPath" != "" && [_get_type $anExtLib] == "t" } {
      lappend aToolkits $anExtLib
    }
  }
  return $aToolkits
}
# Search unit recursively

proc LocateRecur {theName} {
  global path
  set theNamePath "$path/src/$theName"
  if {[file isdirectory $theNamePath]} {
    return $theNamePath
  }
  return ""
}

proc OS:genGUID { {theIDE "vc"} } {
  if { "$theIDE" == "vc" } {
    set p1 "[format %07X [expr { int(rand() * 268435456) }]][format %X [expr { int(rand() * 16) }]]"
    set p2 "[format %04X [expr { int(rand() * 6536) }]]"
    set p3 "[format %04X [expr { int(rand() * 6536) }]]"
    set p4 "[format %04X [expr { int(rand() * 6536) }]]"
    set p5 "[format %06X [expr { int(rand() * 16777216) }]][format %06X [expr { int(rand() * 16777216) }]]"
    return "{$p1-$p2-$p3-$p4-$p5}"
  } else {
    set p1 "[format %04X [expr { int(rand() * 6536) }]]"
    set p2 "[format %04X [expr { int(rand() * 6536) }]]"
    set p3 "[format %04X [expr { int(rand() * 6536) }]]"
    set p4 "[format %04X [expr { int(rand() * 6536) }]]"
    set p5 "[format %04X [expr { int(rand() * 6536) }]]"
    set p6 "[format %04X [expr { int(rand() * 6536) }]]"
    return "$p1$p2$p3$p4$p5$p6"
  }
}

# collect all include file that required for theModules in theOutDir
proc osutils:collectinc {theModules theIncPath theTargetStation} {
  global path

  set aCasRoot [file normalize $path]
  set anIncPath [file normalize $theIncPath]

  if {![file isdirectory $aCasRoot]} {
    puts "OCCT directory is not defined correctly: $aCasRoot"
    return
  }

  set anUsedToolKits {}
  foreach aModule $theModules {
    foreach aToolKit [${aModule}:toolkits] {
      lappend anUsedToolKits $aToolKit

      foreach aDependency [LibToLink $aToolKit] {
        lappend anUsedToolKits $aDependency
      }
    }
    foreach anExecutable [OS:executable ${aModule}] {
      lappend anUsedToolKits $anExecutable

      foreach aDependency [LibToLink $anExecutable] {
        lappend anUsedToolKits $aDependency
      }
    }
  }
  lsort -unique $anUsedToolKits

  set anUnits {}
  foreach anUsedToolKit $anUsedToolKits {
    set anUnits [concat $anUnits [osutils:tk:units $anUsedToolKit]]
  }
  lsort -unique $anUnits

  if { [info exists ::env(SHORTCUT_HEADERS)] && 
       $::env(SHORTCUT_HEADERS) == "true" } {
    # template preparation
    if { ![file exists $aCasRoot/adm/templates/header.in] } {
      puts "template file does not exist: $aCasRoot/adm/templates/header.in"
      return
    }
    set aHeaderTmpl [wokUtils:FILES:FileToString $aCasRoot/adm/templates/header.in]

    # relative anIncPath in connection with aCasRoot/src
    set aFromBuildIncToSrcPath [relativePath "$anIncPath" "$aCasRoot/src"]

    # create and copy short-cut header files
    foreach anUnit $anUnits {
      set aHFiles [glob -nocomplain -dir $aCasRoot/src/$anUnit "*.h"]
      foreach aHeaderFile [concat [glob -nocomplain -dir $aCasRoot/src/$anUnit "*.\[hgl\]xx"] $aHFiles] {
        set aHeaderFileName [file tail $aHeaderFile]

        regsub -all -- {@OCCT_HEADER_FILE@} $aHeaderTmpl "$aFromBuildIncToSrcPath/$anUnit/$aHeaderFileName" aShortCutHeaderFileContent

        if {[file exists "$theIncPath/$aHeaderFileName"] && [file readable "$theIncPath/$aHeaderFileName"]} {
          set fp [open "$theIncPath/$aHeaderFileName" r]
          set aHeaderContent [read $fp]
          close $fp

          # minus eof
          set aHeaderLenght  [expr [string length $aHeaderContent] - 1]

          if {$aHeaderLenght == [string length $aShortCutHeaderFileContent]} {
            # remove eof from string
            set aHeaderContent [string range $aHeaderContent 0 [expr $aHeaderLenght - 1]]

            if {[string compare $aShortCutHeaderFileContent $aHeaderContent] == 0} {
              continue
            }
          }
        }

        set aShortCutHeaderFile [open "$theIncPath/$aHeaderFileName" "w"]
        fconfigure $aShortCutHeaderFile -translation lf
        puts $aShortCutHeaderFile $aShortCutHeaderFileContent
        close $aShortCutHeaderFile
      }
    }  
  } else {
    set nbcopied 0
    foreach anUnit $anUnits {
      set aHFiles [glob -nocomplain -dir $aCasRoot/src/$anUnit "*.h"]
      foreach aHeaderFile [concat [glob -nocomplain -dir $aCasRoot/src/$anUnit "*.\[hgl\]xx"] $aHFiles] {
        set aHeaderFileName [file tail $aHeaderFile]

        # copy file only if target does not exist or is older than original
        set torig [file mtime $aHeaderFile]
        if { ! [file isfile $anIncPath/$aHeaderFileName] } {
          set tcopy 0
        } else {
          set tcopy [file mtime $anIncPath/$aHeaderFileName]
        }
        if { $tcopy < $torig } {
          incr nbcopied
          file copy -force $aHeaderFile $anIncPath/$aHeaderFileName
        } elseif { $tcopy != $torig } {
          puts "Warning: file $anIncPath/$aHeaderFileName is newer than $aHeaderFile, not changed!"
        }
      }
    }
    puts "Info: $nbcopied files updated"
  }
}

# Generate header for VS solution file
proc osutils:vcsolution:header { vcversion } {
  if { "$vcversion" == "vc7" } {
    append var \
      "Microsoft Visual Studio Solution File, Format Version 8.00\n"
  } elseif { "$vcversion" == "vc8" } {
    append var \
      "Microsoft Visual Studio Solution File, Format Version 9.00\n" \
      "# Visual Studio 2005\n"
  } elseif { "$vcversion" == "vc9" } {
    append var \
      "Microsoft Visual Studio Solution File, Format Version 10.00\n" \
      "# Visual Studio 2008\n"
  } elseif { "$vcversion" == "vc10" } {
    append var \
      "Microsoft Visual Studio Solution File, Format Version 11.00\n" \
      "# Visual Studio 2010\n"
  } elseif { "$vcversion" == "vc11" } {
    append var \
      "Microsoft Visual Studio Solution File, Format Version 12.00\n" \
      "# Visual Studio 2012\n"
  } elseif { "$vcversion" == "vc12" } {
    append var \
      "Microsoft Visual Studio Solution File, Format Version 13.00\n" \
      "# Visual Studio 2013\n"
  } else {
    puts stderr "Error: Visual Studio version $vcversion is not supported by this function!"
  }
  return $var
}
# Returns extension (without dot) for project files of given version of VC

proc osutils:vcproj:ext { vcversion } {
  if { "$vcversion" == "vc7" || "$vcversion" == "vc8" || "$vcversion" == "vc9" } {
    return "vcproj"
  } elseif { "$vcversion" == "vc10" || "$vcversion" == "vc11" || "$vcversion" == "vc12" } {
    return "vcxproj"
  } else {
    puts stderr "Error: Visual Studio version $vc is not supported by this function!"
  }
}
# Generate start of configuration section of VS solution file

proc osutils:vcsolution:config:begin { vcversion } {
  if { "$vcversion" == "vc7" } {
    append var \
      "Global\n" \
      "\tGlobalSection(SolutionConfiguration) = preSolution\n" \
      "\t\tDebug = Debug\n" \
      "\t\tRelease = Release\n" \
      "\tEndGlobalSection\n" \
      "\tGlobalSection(ProjectConfiguration) = postSolution\n"
  } elseif { "$vcversion" == "vc8" || "$vcversion" == "vc9" ||
             "$vcversion" == "vc10" || "$vcversion" == "vc11" ||
             "$vcversion" == "vc12" } {
    append var \
      "Global\n" \
      "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n" \
      "\t\tDebug|Win32 = Debug|Win32\n" \
      "\t\tRelease|Win32 = Release|Win32\n" \
      "\t\tDebug|x64 = Debug|x64\n" \
      "\t\tRelease|x64 = Release|x64\n" \
      "\tEndGlobalSection\n" \
      "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n"
  } else {
    puts stderr "Error: Visual Studio version $vcversion is not supported by this function!"
  }
  return $var
}
# Generate part of configuration section of VS solution file describing one project

proc osutils:vcsolution:config:project { vcversion guid } {
  if { "$vcversion" == "vc7" } {
    append var \
      "\t\t$guid.Debug.ActiveCfg = Debug|Win32\n" \
      "\t\t$guid.Debug.Build.0 = Debug|Win32\n" \
      "\t\t$guid.Release.ActiveCfg = Release|Win32\n" \
      "\t\t$guid.Release.Build.0 = Release|Win32\n"
  } elseif { "$vcversion" == "vc8" || "$vcversion" == "vc9" ||
             "$vcversion" == "vc10" || "$vcversion" == "vc11" ||
             "$vcversion" == "vc12" } {
    append var \
      "\t\t$guid.Debug|Win32.ActiveCfg = Debug|Win32\n" \
      "\t\t$guid.Debug|Win32.Build.0 = Debug|Win32\n" \
      "\t\t$guid.Release|Win32.ActiveCfg = Release|Win32\n" \
      "\t\t$guid.Release|Win32.Build.0 = Release|Win32\n" \
      "\t\t$guid.Debug|x64.ActiveCfg = Debug|x64\n" \
      "\t\t$guid.Debug|x64.Build.0 = Debug|x64\n" \
      "\t\t$guid.Release|x64.ActiveCfg = Release|x64\n" \
      "\t\t$guid.Release|x64.Build.0 = Release|x64\n"
  } else {
    puts stderr "Error: Visual Studio version $vcversion is not supported by this function!"
  }
  return $var
}
# Generate start of configuration section of VS solution file

proc osutils:vcsolution:config:end { vcversion } {
  if { "$vcversion" == "vc7" } {
    append var \
      "\tEndGlobalSection\n" \
      "\tGlobalSection(ExtensibilityGlobals) = postSolution\n" \
      "\tEndGlobalSection\n" \
      "\tGlobalSection(ExtensibilityAddIns) = postSolution\n" \
      "\tEndGlobalSection\n"
  } elseif { "$vcversion" == "vc8" || "$vcversion" == "vc9" ||
             "$vcversion" == "vc10" || "$vcversion" == "vc11" ||
             "$vcversion" == "vc12" } {
    append var \
      "\tEndGlobalSection\n" \
      "\tGlobalSection(SolutionProperties) = preSolution\n" \
      "\t\tHideSolutionNode = FALSE\n" \
      "\tEndGlobalSection\n"
  } else {
    puts stderr "Error: Visual Studio version $vcversion is not supported by this function!"
  }
  return $var
}
# generate Visual Studio solution file
# if module is empty, generates one solution for all known modules

proc OS:vcsolution { theVcVer theSolName theModules theOutDir theGuidsMap } {
  global path
  upvar $theGuidsMap aGuidsMap

  # collect list of projects to be created
  set aProjects {}
  set aDependencies {}
  foreach aModule $theModules {
    # toolkits
    foreach aToolKit [osutils:tk:sort [${aModule}:toolkits]] {
      lappend aProjects $aToolKit
      lappend aProjectsInModule($aModule) $aToolKit
      lappend aDependencies [LibToLink $aToolKit]
    }
    # executables, assume one project per cxx file...
    foreach aUnit [OS:executable ${aModule}] {
      set aUnitLoc $aUnit
      set src_files [_get_used_files $aUnit false]
      set aSrcFiles {}
      foreach s $src_files {
        regexp {source ([^\s]+)} $s dummy name
        lappend aSrcFiles $name
      }
      foreach aSrcFile $aSrcFiles {
        set aFileExtension [file extension $aSrcFile]
        if { $aFileExtension == ".cxx" } {
          set aPrjName [file rootname $aSrcFile]
          lappend aProjects $aPrjName
          lappend aProjectsInModule($aModule) $aPrjName
          if {[file isdirectory $path/src/$aUnitLoc]} {
            lappend aDependencies [LibToLinkX $aUnitLoc [file rootname $aSrcFile]]
          } else {
            lappend aDependencies {}
          }
        }
      }
    }
  }

# generate GUIDs for projects (unless already known)
  foreach aProject $aProjects {
    if { ! [info exists aGuidsMap($aProject)] } {
      set aGuidsMap($aProject) [OS:genGUID]
    }
  }

  # generate solution file
#  puts "Generating Visual Studio ($theVcVer) solution file for $theSolName ($aProjects)"
  append aFileBuff [osutils:vcsolution:header $theVcVer]

  # GUID identifying group projects in Visual Studio
  set VC_GROUP_GUID "{2150E333-8FDC-42A3-9474-1A3956D46DE8}"

  # generate group projects -- one per module
  if { "$theVcVer" != "vc7" && [llength "$theModules"] > 1 } {
    foreach aModule $theModules {
      if { ! [info exists aGuidsMap(_$aModule)] } {
        set aGuidsMap(_$aModule) [OS:genGUID]
      }
      set aGuid $aGuidsMap(_$aModule)
      append aFileBuff "Project(\"${VC_GROUP_GUID}\") = \"$aModule\", \"$aModule\", \"$aGuid\"\nEndProject\n"
    }
  }

  # extension of project files
  set aProjExt [osutils:vcproj:ext $theVcVer]

  # GUID identifying C++ projects in Visual Studio
  set VC_CPP_GUID "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}"

  # generate normal projects
  set aProjsNb [llength $aProjects]
  for {set aProjId 0} {$aProjId < $aProjsNb} {incr aProjId} {
    set aProj [lindex $aProjects $aProjId]
    set aGuid $aGuidsMap($aProj)
    append aFileBuff "Project(\"${VC_CPP_GUID}\") = \"$aProj\", \"$aProj.${aProjExt}\", \"$aGuid\"\n"
    # write projects dependencies information (vc7 to vc9)
    set aDepGuids ""
    foreach aDepLib [lindex $aDependencies $aProjId] {
      if { $aDepLib != $aProj && [lsearch $aProjects $aDepLib] != "-1" } {
        set depGUID $aGuidsMap($aDepLib)
        append aDepGuids "\t\t$depGUID = $depGUID\n"
      }
    }
    if { "$aDepGuids" != "" } {
      append aFileBuff "\tProjectSection(ProjectDependencies) = postProject\n"
      append aFileBuff "$aDepGuids"
      append aFileBuff "\tEndProjectSection\n"
    }
    append aFileBuff "EndProject\n"
  }

  # generate configuration section
  append aFileBuff [osutils:vcsolution:config:begin $theVcVer]
  foreach aProj $aProjects {
    append aFileBuff [osutils:vcsolution:config:project $theVcVer $aGuidsMap($aProj)]
  }
  append aFileBuff [osutils:vcsolution:config:end $theVcVer]

  # write information of grouping of projects by module
  if { "$theVcVer" != "vc7" && [llength "$theModules"] > 1 } {
    append aFileBuff "	GlobalSection(NestedProjects) = preSolution\n"
    foreach aModule $theModules {
      if { ! [info exists aProjectsInModule($aModule)] } { continue }
      foreach aProject $aProjectsInModule($aModule) {
        append aFileBuff "		$aGuidsMap($aProject) = $aGuidsMap(_$aModule)\n"
      }
    }
    append aFileBuff "	EndGlobalSection\n"
  }

  # final word (footer)
  append aFileBuff "EndGlobal"

  # write solution
  set aFile [open [set fdsw [file join $theOutDir ${theSolName}.sln]] w]
  fconfigure $aFile -translation crlf
  puts $aFile $aFileBuff
  close $aFile
  return [file join $theOutDir ${theSolName}.sln]
}
# Generate Visual Studio projects for specified version

proc OS:vcproj { theVcVer theModules theOutDir theGuidsMap } {
  upvar $theGuidsMap aGuidsMap

  set aProjectFiles {}

  foreach aModule $theModules {
    foreach aToolKit [${aModule}:toolkits] {
      lappend aProjectFiles [osutils:vcproj  $theVcVer $theOutDir $aToolKit     aGuidsMap]
    }
    foreach anExecutable [OS:executable ${aModule}] {
      lappend aProjectFiles [osutils:vcprojx $theVcVer $theOutDir $anExecutable aGuidsMap]
    }
  }
  return $aProjectFiles
}
# generate template name and load it for given version of Visual Studio and platform

proc osutils:vcproj:readtemplate {vc isexec} {
  set ext $vc
  set what "$vc"
  if { $isexec } {
    set ext "${ext}x"
    set what "$what executable"
  }
  return [osutils:readtemplate $ext "MS VC++ project ($what)"]
}

proc osutils:readtemplate {ext what} {
  global env
  global path
  set loc "$path/adm/templates/template.$ext"
  return [wokUtils:FILES:FileToString $loc]
}
# Read a file in a string as is.

proc wokUtils:FILES:FileToString { fin } {
    if { [catch { set in [ open $fin r ] } errin] == 0 } {
	set strin [read $in [file size $fin]]
	close $in
	return $strin
    } else {
	return {}
    }
}
# List extensions of compilable files in OCCT

proc osutils:compilable { } {
  global targetStation
  set aWokStation "$targetStation"
  if { "$aWokStation" == "mac" } {
    return [list .c .cxx .cpp .mm]
  }
  return [list .c .cxx .cpp]
}

proc osutils:commonUsedTK { theToolKit } {
  global path
  set anUsedToolKits [list]
  set aDepToolkits [LibToLink $theToolKit]
  foreach tkx $aDepToolkits {
    if {[_get_type $tkx] == "t"} {
      lappend anUsedToolKits "${tkx}"
    }
  }
  return $anUsedToolKits
}
# Return the list of name *CSF_ in a EXTERNLIB description of a toolkit

proc osutils:tk:hascsf { EXTERNLIB } {
  set l [wokUtils:FILES:FileToList $EXTERNLIB]
  set lret  {STLPort}
  foreach str $l {
    if [regexp -- {(CSF_[^ ]*)} $str csf] {
      lappend lret $csf
    }
  }
  return $lret
}

proc osutils:csfList { theOS  theCsfMap } {
  upvar $theCsfMap aCsfMap

  unset theCsfMap

  if { "$theOS" == "wnt" } {
    # -- WinAPI libraries
    set aCsfMap(CSF_kernel32)   "kernel32.lib"
    set aCsfMap(CSF_advapi32)   "advapi32.lib"
    set aCsfMap(CSF_gdi32)      "gdi32.lib"
    set aCsfMap(CSF_user32)     "user32.lib"
    set aCsfMap(CSF_glu32)      "glu32.lib"
    set aCsfMap(CSF_opengl32)   "opengl32.lib"
    set aCsfMap(CSF_wsock32)    "wsock32.lib"
    set aCsfMap(CSF_netapi32)   "netapi32.lib"
    set aCsfMap(CSF_AviLibs)    "ws2_32.lib vfw32.lib"
    set aCsfMap(CSF_OpenGlLibs) "opengl32.lib glu32.lib"

    # -- 3rd-parties precompiled libraries
    # Note: Tcl library name depends on version and is chosen by #pragma
    set aCsfMap(CSF_QT)         "QtCore4.lib QtGui4.lib"

    #-- VTK
    set aCsfMap(CSF_VTK)         [osutils:vtkCsf "wnt"]

  } else {

    #-- Tcl/Tk configuration
    set aCsfMap(CSF_TclLibs)    "tcl8.6"
    set aCsfMap(CSF_TclTkLibs)  "X11 tk8.6"

    if { "$theOS" == "lin" } {
      set aCsfMap(CSF_ThreadLibs) "pthread rt"
      set aCsfMap(CSF_OpenGlLibs) "GLU GL"

    } elseif { "$theOS" == "mac" } {
      set aCsfMap(CSF_objc)     "objc"

      # frameworks
      set aCsfMap(CSF_Appkit)     "Appkit"
      set aCsfMap(CSF_IOKit)      "IOKit"
      set aCsfMap(CSF_OpenGlLibs) "OpenGL"
      set aCsfMap(CSF_TclLibs)    "Tcl"
      set aCsfMap(CSF_TclTkLibs)  "Tk"
    }

    set aCsfMap(CSF_XwLibs)     "X11 Xext Xmu Xi"
    set aCsfMap(CSF_MotifLibs)  "X11"

    # variable is required for support for OCCT version that use fgtl
    #-- FTGL (font renderer for OpenGL)
    set aCsfMap(CSF_FTGL)       "ftgl"

    #-- FreeType
    set aCsfMap(CSF_FREETYPE)   "freetype"

    #-- optional 3rd-parties
    #-- TBB
    set aCsfMap(CSF_TBB)            "tbb tbbmalloc"

    #-- FreeImage
    set aCsfMap(CSF_FreeImagePlus)  "freeimage"

    #-- GL2PS
    set aCsfMap(CSF_GL2PS)          "gl2ps"

    #-- VTK
    set aCsfMap(CSF_VTK)         [osutils:vtkCsf "unix"]
  }
}

# Returns string of library dependencies for generation of Visual Studio project or make lists.
proc osutils:vtkCsf {{theOS ""}} {
  set aVtkVer "6.1"

  set aLibSuffix ""
  set aPathSplitter ":"
  
  if {"$theOS" == "wnt"} {
    set aPathSplitter ";"
    set aLibSuffix ".lib"
  }

  set anOptIncs [split $::env(CSF_OPT_INC) "$aPathSplitter"]
  foreach anIncItem $anOptIncs {
    if {[regexp -- "vtk-(.*)$" [file tail $anIncItem] dummy aFoundVtkVer]} {
      set aVtkVer $aFoundVtkVer
    }
  }

  set aLibArray [list vtkCommonCore vtkCommonDataModel vtkCommonExecutionModel vtkCommonMath vtkCommonTransforms vtkRenderingCore \
                      vtkRenderingOpenGL  vtkFiltersGeneral vtkIOCore vtkIOImage vtkImagingCore vtkInteractionStyle]

  # Additional suffices for the libraries
  set anIdx 0
  foreach anItem $aLibArray {
    lset aLibArray $anIdx $anItem-$aVtkVer$aLibSuffix
    incr anIdx
  }

  return [join $aLibArray " "]
}

proc osutils:usedOsLibs { theToolKit theOS } {
  global path
  set aUsedLibs [list]

  osutils:csfList $theOS anOsCsfList

  foreach element [osutils:tk:hascsf "$path/src/${theToolKit}/EXTERNLIB"] {
    # test if variable is not setted - continue
    if ![info exists anOsCsfList($element)] {
      continue
    }

    foreach aLib [split "$anOsCsfList($element)"] {
      if { [lsearch $aUsedLibs $aLib] == "-1"} {
        lappend aUsedLibs $aLib
      }
    }
  }

  return $aUsedLibs
}

# Returns liste of UD in a toolkit. tkloc is a full path wok.
proc osutils:tk:units { tkloc } {
  global path
  set l {}
  set PACKAGES "$path/src/$tkloc/PACKAGES"
  foreach u [wokUtils:FILES:FileToList $PACKAGES] {
    if {[file isdirectory "$path/src/$u"]} {
      lappend l $u
    }
  }
  if { $l == {} } {
    ;#puts stderr "Warning. No devunit included in $tkloc"
  }
  return $l
}

proc osutils:justwnt { listloc } {
  # ImageUtility is required for support for old (<6.5.4) versions of OCCT
  set goaway [list Xdps Xw  ImageUtility WOKUnix]
  return [osutils:juststation $goaway $listloc]
}

# remove from listloc OpenCascade units indesirables on NT
proc osutils:juststation {goaway listloc} {
  global path
  set lret {}
  foreach u $listloc {
    if {([file isdirectory "$path/src/$u"] && [lsearch $goaway $u] == -1 )
     || (![file isdirectory "$path/src/$u"] && [lsearch $goaway $u] == -1 ) } {
      lappend lret $u
    }
  }
  return $lret
}

# intersect3 - perform the intersecting of two lists, returning a list containing three lists.
# The first list is everything in the first list that wasn't in the second,
# the second list contains the intersection of the two lists, the third list contains everything
# in the second list that wasn't in the first.
proc osutils:intersect3 {list1 list2} {
  set la1(0) {} ; unset la1(0)
  set lai(0) {} ; unset lai(0)
  set la2(0) {} ; unset la2(0)
  foreach v $list1 {
    set la1($v) {}
  }
  foreach v $list2 {
    set la2($v) {}
  }
  foreach elem [concat $list1 $list2] {
    if {[info exists la1($elem)] && [info exists la2($elem)]} {
      unset la1($elem)
      unset la2($elem)
      set lai($elem) {}
    }
  }
  list [lsort [array names la1]] [lsort [array names lai]] [lsort [array names la2]]
}

# Prepare relative path
proc relativePath {thePathFrom thePathTo} {
  if { [file isdirectory "$thePathFrom"] == 0 } {
    return ""
  }

  set aPathFrom [file normalize "$thePathFrom"]
  set aPathTo   [file normalize "$thePathTo"]

  set aCutedPathFrom "${aPathFrom}/dummy"
  set aRelatedDeepPath ""

  while { "$aCutedPathFrom" != [file normalize "$aCutedPathFrom/.."] } {
    set aCutedPathFrom [file normalize "$aCutedPathFrom/.."]
    # does aPathTo contain aCutedPathFrom?
    regsub -all $aCutedPathFrom $aPathTo "" aPathFromAfterCut
    if { "$aPathFromAfterCut" != "$aPathTo" } { # if so
      if { "$aCutedPathFrom" == "$aPathFrom" } { # just go higher, for example, ./somefolder/someotherfolder
        set aPathTo ".${aPathTo}"
      } elseif { "$aCutedPathFrom" == "$aPathTo" } { # remove the last "/"
        set aRelatedDeepPath [string replace $aRelatedDeepPath end end ""]
      }
      regsub -all $aCutedPathFrom $aPathTo $aRelatedDeepPath aPathToAfterCut
      regsub -all "//" $aPathToAfterCut "/" aPathToAfterCut
      return $aPathToAfterCut
    }
    set aRelatedDeepPath "$aRelatedDeepPath../"

  }

  return $thePathTo
}

proc wokUtils:EASY:bs1 { s } {
    regsub -all {/} $s {\\} r
    return $r
}

# Returs for a full path the liste of n last directory part
# n = 1 => tail
# n = 2 => dir/file.c
# n = 3 => sdir/dir/file.c
# etc..
proc wokUtils:FILES:wtail { f n } {
    set ll [expr [llength [set lif [file split $f]]] -$n]
    return [join [lrange $lif $ll end] /]
}

# Generate entry for one source file in Visual Studio 10 project file
proc osutils:vcxproj:file { vcversion file params } {
  append text "    <ClCompile Include=\"..\\..\\..\\[wokUtils:EASY:bs1 [wokUtils:FILES:wtail $file 3]]\">\n"
  if { $params != "" } {
    append text "      <AdditionalOptions Condition=\"\'\$(Configuration)|\$(Platform)\'==\'Debug|Win32\'\">[string trim ${params}]  %(AdditionalOptions)</AdditionalOptions>\n"
  }

  if { $params != "" } {
    append text "      <AdditionalOptions Condition=\"\'\$(Configuration)|\$(Platform)\'==\'Release|Win32\'\">[string trim ${params}]  %(AdditionalOptions)</AdditionalOptions>\n"
  }

  if { $params != "" } {
    append text "      <AdditionalOptions Condition=\"\'\$(Configuration)|\$(Platform)\'==\'Debug|x64\'\">[string trim ${params}]  %(AdditionalOptions)</AdditionalOptions>\n"
  }

  if { $params != "" } {
    append text "      <AdditionalOptions Condition=\"\'\$(Configuration)|\$(Platform)\'==\'Release|x64\'\">[string trim ${params}]  %(AdditionalOptions)</AdditionalOptions>\n"
  }

  append text "    </ClCompile>\n"
  return $text
}

# Generate Visual Studio 2010 project filters file
proc osutils:vcxproj:filters { dir proj theFilesMap } {
  upvar $theFilesMap aFilesMap

  # header
  append text "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
  append text "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"

  # list of "filters" (units)
  append text "  <ItemGroup>\n"
  append text "    <Filter Include=\"Source files\">\n"
  append text "      <UniqueIdentifier>[OS:genGUID]</UniqueIdentifier>\n"
  append text "    </Filter>\n"
  foreach unit $aFilesMap(units) {
    append text "    <Filter Include=\"Source files\\${unit}\">\n"
    append text "      <UniqueIdentifier>[OS:genGUID]</UniqueIdentifier>\n"
    append text "    </Filter>\n"
  }
  append text "  </ItemGroup>\n"

  # list of files
  append text "  <ItemGroup>\n"
  foreach unit $aFilesMap(units) {
    foreach file $aFilesMap($unit) {
      append text "    <ClCompile Include=\"..\\..\\..\\[wokUtils:EASY:bs1 [wokUtils:FILES:wtail $file 3]]\">\n"
      append text "      <Filter>Source files\\${unit}</Filter>\n"
      append text "    </ClCompile>\n"
    }
  }
  append text "  </ItemGroup>\n"

  # end
  append text "</Project>"

  # write file
  set fp [open [set fvcproj [file join $dir ${proj}.vcxproj.filters]] w]
  fconfigure $fp -translation crlf
  puts $fp $text
  close $fp

  return ${proj}.vcxproj.filters
}

# Generate Visual Studio 2011 project filters file
proc osutils:vcx1proj:filters { dir proj theFilesMap } {
  upvar $theFilesMap aFilesMap

  # header
  append text "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
  append text "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"

  # list of "filters" (units)
  append text "  <ItemGroup>\n"
  append text "    <Filter Include=\"Source files\">\n"
  append text "      <UniqueIdentifier>[OS:genGUID]</UniqueIdentifier>\n"
  append text "    </Filter>\n"
  foreach unit $aFilesMap(units) {
    append text "    <Filter Include=\"Source files\\${unit}\">\n"
    append text "      <UniqueIdentifier>[OS:genGUID]</UniqueIdentifier>\n"
    append text "    </Filter>\n"
  }
  append text "  </ItemGroup>\n"

  # list of files
  append text "  <ItemGroup>\n"
  foreach unit $aFilesMap(units) {
    foreach file $aFilesMap($unit) {
      append text "    <ClCompile Include=\"..\\..\\..\\[wokUtils:EASY:bs1 [wokUtils:FILES:wtail $file 3]]\">\n"
      append text "      <Filter>Source files\\${unit}</Filter>\n"
      append text "    </ClCompile>\n"
    }
  }
  append text "  </ItemGroup>\n"

  append text "  <ItemGroup>\n"
  append text "    <ResourceCompile Include=\"${proj}.rc\" />"
  append text "  </ItemGroup>\n"

  # end
  append text "</Project>"

  # write file
  set fp [open [set fvcproj [file join $dir ${proj}.vcxproj.filters]] w]
  fconfigure $fp -translation crlf
  puts $fp $text
  close $fp

  return ${proj}.vcxproj.filters
}

# Generate RC file content for ToolKit from template
proc osutils:readtemplate:rc {theOutDir theToolKit} {
  global path
  set aLoc "$path/adm/templates/template_dll.rc"
  set aBody [wokUtils:FILES:FileToString $aLoc]
  regsub -all -- {__TKNAM__} $aBody $theToolKit aBody

  set aFile [open "${theOutDir}/${theToolKit}.rc" "w"]
  fconfigure $aFile -translation lf
  puts $aFile $aBody
  close $aFile
  return "${theOutDir}/${theToolKit}.rc"
}

# Generate Visual Studio project file for ToolKit
proc osutils:vcproj { theVcVer theOutDir theToolKit theGuidsMap {theProjTmpl {} } } {
  if { $theProjTmpl == {} } {set theProjTmpl [osutils:vcproj:readtemplate $theVcVer 0]}

  set l_compilable [osutils:compilable]
  regsub -all -- {__TKNAM__} $theProjTmpl $theToolKit theProjTmpl

  upvar $theGuidsMap aGuidsMap
  if { ! [info exists aGuidsMap($theToolKit)] } {
    set aGuidsMap($theToolKit) [OS:genGUID]
  }
  regsub -all -- {__PROJECT_GUID__} $theProjTmpl $aGuidsMap($theToolKit) theProjTmpl

  set aCommonUsedTK [list]
  foreach tkx [osutils:commonUsedTK  $theToolKit] {
    lappend aCommonUsedTK "${tkx}.lib"
  }

  set aUsedToolKits [concat $aCommonUsedTK [osutils:usedOsLibs $theToolKit "wnt"]]

  # correct names of referred third-party libraries that are named with suffix
  # depending on VC version
  regsub -all -- {vc[0-9]+} $aUsedToolKits $theVcVer aUsedToolKits

  # and put this list to project file
  #puts "$theToolKit requires  $aUsedToolKits"
  if { "$theVcVer" == "vc10" || "$theVcVer" == "vc11" || "$theVcVer" == "vc12" } {
    set aUsedToolKits [join $aUsedToolKits {;}]
  }
  regsub -all -- {__TKDEP__} $theProjTmpl $aUsedToolKits theProjTmpl

  set anIncPaths "..\\..\\..\\inc"
  set aTKDefines ""
  set aFilesSection ""
  set aVcFilesX(units) ""
  set listloc [osutils:tk:units $theToolKit]
  set resultloc [osutils:justwnt $listloc]
  if [array exists written] { unset written }
  #puts "\t1 [wokparam -v %CMPLRS_CXX_Options [w_info -f]] father"
  #puts "\t2 [wokparam -v %CMPLRS_CXX_Options] branch"
  #puts "\t1 [wokparam -v %CMPLRS_C_Options [w_info -f]] father"
  #puts "\t2 [wokparam -v %CMPLRS_C_Options] branch"
  set fxloparamfcxx [lindex [osutils:intersect3 [_get_options wnt cmplrs_cxx f] [_get_options wnt cmplrs_cxx b]] 2]
  set fxloparamfc   [lindex [osutils:intersect3 [_get_options wnt cmplrs_c f] [_get_options wnt cmplrs_c b]] 2]
  set fxloparam ""
  foreach fxlo $resultloc {
    set xlo $fxlo
    set aSrcFiles [osutils:tk:files $xlo osutils:compilable 0]
	set fxlo_cmplrs_options_cxx [_get_options wnt cmplrs_cxx $fxlo]
    if {$fxlo_cmplrs_options_cxx == ""} {
      set fxlo_cmplrs_options_cxx [_get_options wnt cmplrs_cxx b]
    }
	set fxlo_cmplrs_options_c [_get_options wnt cmplrs_c $fxlo]
    if {$fxlo_cmplrs_options_c == ""} {
      set fxlo_cmplrs_options_c [_get_options wnt cmplrs_c b]
    }
    set fxloparam "$fxloparam [lindex [osutils:intersect3 [_get_options wnt cmplrs_cxx b] $fxlo_cmplrs_options_cxx] 2]"
    set fxloparam "$fxloparam [lindex [osutils:intersect3 [_get_options wnt cmplrs_c b] $fxlo_cmplrs_options_c] 2]"
	#puts "\t3 [wokparam -v %CMPLRS_CXX_Options] branch CXX "
	#puts "\t4 [wokparam -v %CMPLRS_CXX_Options $fxlo] $fxlo  CXX"
	#puts "\t5 [wokparam -v %CMPLRS_C_Options] branch C"
	#puts "\t6 [wokparam -v %CMPLRS_C_Options   $fxlo] $fxlo  C"
    set needparam ""
    foreach partopt $fxloparam {
      if {[string first "-I" $partopt] == "0"} {
        # this is an additional includes search path
        continue
      }
      set needparam "$needparam $partopt"
    }

    # Format of projects in vc10 and vc11 is different from vc7-9
    if { "$theVcVer" == "vc10" || "$theVcVer" == "vc11" || "$theVcVer" == "vc12" } {
      foreach aSrcFile [lsort $aSrcFiles] {
        if { ![info exists written([file tail $aSrcFile])] } {
          set written([file tail $aSrcFile]) 1
          append aFilesSection [osutils:vcxproj:file $theVcVer $aSrcFile $needparam]
        } else {
          puts "Warning : in vcproj more than one occurences for [file tail $aSrcFile]"
        }
        if { ! [info exists aVcFilesX($xlo)] } { lappend aVcFilesX(units) $xlo }
        lappend aVcFilesX($xlo) $aSrcFile
      }
    } else {
      append aFilesSection "\t\t\t<Filter\n"
      append aFilesSection "\t\t\t\tName=\"${xlo}\"\n"
      append aFilesSection "\t\t\t\t>\n"
      foreach aSrcFile [lsort $aSrcFiles] {
        if { ![info exists written([file tail $aSrcFile])] } {
          set written([file tail $aSrcFile]) 1
          append aFilesSection [osutils:vcproj:file $theVcVer $aSrcFile $needparam]
        } else {
          puts "Warning : in vcproj more than one occurences for [file tail $aSrcFile]"
        }
      }
      append aFilesSection "\t\t\t</Filter>\n"
    }

    # macros
    append aTKDefines ";__${xlo}_DLL"
    # common includes
#    append anIncPaths ";..\\..\\..\\src\\${xlo}"
  }

  regsub -all -- {__TKINC__}  $theProjTmpl $anIncPaths theProjTmpl
  regsub -all -- {__TKDEFS__} $theProjTmpl $aTKDefines theProjTmpl
  regsub -all -- {__FILES__}  $theProjTmpl $aFilesSection theProjTmpl

  # write file
  set aFile [open [set aVcFiles [file join $theOutDir ${theToolKit}.[osutils:vcproj:ext $theVcVer]]] w]
  fconfigure $aFile -translation crlf
  puts $aFile $theProjTmpl
  close $aFile

  # write filters file for vc10 and vc11
  if { "$theVcVer" == "vc10" } {
    lappend aVcFiles [osutils:vcxproj:filters $theOutDir $theToolKit aVcFilesX]
  } elseif { "$theVcVer" == "vc11" || "$theVcVer" == "vc12" } {
    lappend aVcFiles [osutils:vcx1proj:filters $theOutDir $theToolKit aVcFilesX]
  }

  # write resource file
  lappend aVcFiles [osutils:readtemplate:rc $theOutDir $theToolKit]

  return $aVcFiles
}

# for a unit returns a map containing all its file in the current
# workbench
# local = 1 only local files
proc osutils:tk:loadunit { loc map } {
  #puts $loc
  upvar $map TLOC
  catch { unset TLOC }
  set lfiles [_get_used_files $loc]
  foreach f $lfiles {
    #puts "\t$f"
    set t [lindex $f 0]
    set p [lindex $f 2]
    if [info exists TLOC($t)] {
      set l $TLOC($t)
      lappend l $p
      set TLOC($t) $l
    } else {
      set TLOC($t) $p
    }
  }
  return
}

# Returns the list of all compilable files name in a toolkit, or devunit of any type
# Call unit filter on units name to accept or reject a unit
# Tfiles lists for each unit the type of file that can be compiled.
proc osutils:tk:files { tkloc  {l_compilable {} } {justail 1} {unitfilter {}} } {
  global path
  set Tfiles(source,nocdlpack)     {source pubinclude}
  set Tfiles(source,toolkit)       {}
  set Tfiles(source,executable)    {source pubinclude}
  set listloc [concat [osutils:tk:units $tkloc] $tkloc]
  #puts " listloc = $listloc"
  if { $l_compilable == {} } {
    set l_comp [list .c .cxx .cpp]
  } else {
    set l_comp [$l_compilable]
  }
  if { $unitfilter == {} } {
    set resultloc $listloc
  } else {
    set resultloc [$unitfilter $listloc]
  }
  set lret {}
  foreach loc $resultloc {
    set utyp [_get_type $loc]
    #puts "\"$utyp\" \"$loc\""
    switch $utyp {
         "t" { set utyp "toolkit" }
         "n" { set utyp "nocdlpack" }
         "x" { set utyp "executable" }
    }
    if [array exists map] { unset map }
    osutils:tk:loadunit $loc map
    #puts " loc = $loc === > [array names map]"
    set LType $Tfiles(source,${utyp})
    foreach typ [array names map] {
      if { [lsearch $LType $typ] == -1 } {
        unset map($typ)
      }
    }
    foreach type [array names map] {
      #puts $type
      foreach f $map($type) {
        #puts $f
        if { [lsearch $l_comp [file extension $f]] != -1 } {
          if { $justail == 1 } {
            if {$type == "source"} {
              if {[lsearch $lret "@top_srcdir@/src/$loc/[file tail $f]"] == -1} {
                lappend lret @top_srcdir@/src/$loc/[file tail $f]
              }
            }
          } else {
            lappend lret $f
          }
        }
      }
    }
  }
  return $lret
}

# Generate Visual Studio project file for executable
proc osutils:vcprojx { theVcVer theOutDir theToolKit theGuidsMap {theProjTmpl {} } } {
  global path
  set aVcFiles {}
  foreach f [osutils:tk:files $theToolKit osutils:compilable 0] {
    if { $theProjTmpl == {} } {
      set aProjTmpl [osutils:vcproj:readtemplate $theVcVer 1]
    } else {
      set aProjTmpl $theProjTmpl
    }
    set aProjName [file rootname [file tail $f]]
    set l_compilable [osutils:compilable]
    regsub -all -- {__XQTNAM__} $aProjTmpl $aProjName aProjTmpl

    upvar $theGuidsMap aGuidsMap
    if { ! [info exists aGuidsMap($aProjName)] } {
      set aGuidsMap($aProjName) [OS:genGUID]
    }
    regsub -all -- {__PROJECT_GUID__} $aProjTmpl $aGuidsMap($aProjName) aProjTmpl

    set aCommonUsedTK [list]
    foreach tkx [osutils:commonUsedTK  $theToolKit] {
      lappend aCommonUsedTK "${tkx}.lib"
    }

    set aUsedToolKits [concat $aCommonUsedTK [osutils:usedOsLibs $theToolKit "wnt"]]

    # correct names of referred third-party libraries that are named with suffix
    # depending on VC version
    regsub -all -- {vc[0-9]+} $aUsedToolKits $theVcVer aUsedToolKits

#    puts "$aProjName requires  $aUsedToolKits"
    if { "$theVcVer" == "vc10" || "$theVcVer" == "vc11" || "$theVcVer" == "vc12" } {
      set aUsedToolKits [join $aUsedToolKits {;}]
    }
    regsub -all -- {__TKDEP__} $aProjTmpl $aUsedToolKits aProjTmpl

    set aFilesSection ""
    set aVcFilesX(units) ""

    if { ![info exists written([file tail $f])] } {
      set written([file tail $f]) 1

      if { "$theVcVer" == "vc10" || "$theVcVer" == "vc11" || "$theVcVer" == "vc12" } {
        append aFilesSection [osutils:vcxproj:file $theVcVer $f ""]
        if { ! [info exists aVcFilesX($theToolKit)] } { lappend aVcFilesX(units) $theToolKit }
        lappend aVcFilesX($theToolKit) $f
      } else {
        append aFilesSection "\t\t\t<Filter\n"
        append aFilesSection "\t\t\t\tName=\"$theToolKit\"\n"
        append aFilesSection "\t\t\t\t>\n"
        append aFilesSection [osutils:vcproj:file $theVcVer $f ""]
        append aFilesSection "\t\t\t</Filter>"
      }
    } else {
      puts "Warning : in vcproj there are than one occurences for [file tail $f]"
    }
    #puts "$aProjTmpl $aFilesSection"
    set aTKDefines ";__${theToolKit}_DLL"
    set anIncPaths "..\\..\\..\\inc"
    regsub -all -- {__TKINC__}  $aProjTmpl $anIncPaths    aProjTmpl
    regsub -all -- {__TKDEFS__} $aProjTmpl $aTKDefines    aProjTmpl
    regsub -all -- {__FILES__}  $aProjTmpl $aFilesSection aProjTmpl
    regsub -all -- {__CONF__}   $aProjTmpl Application    aProjTmpl

    regsub -all -- {__XQTEXT__} $aProjTmpl "exe" aProjTmpl

    set aFile [open [set aVcFilePath [file join $theOutDir ${aProjName}.[osutils:vcproj:ext $theVcVer]]] w]
    fconfigure $aFile -translation crlf
    puts $aFile $aProjTmpl
    close $aFile

    set aCommonSettingsFile "$aVcFilePath.user"
    lappend aVcFiles $aVcFilePath

    # write filters file for vc10
    if { "$theVcVer" == "vc10" || "$theVcVer" == "vc11" || "$theVcVer" == "vc12" } {
      lappend aVcFiles [osutils:vcxproj:filters $theOutDir $aProjName aVcFilesX]
    }

    set aCommonSettingsFileTmpl ""
    if { "$theVcVer" == "vc9" } {
      set aCommonSettingsFileTmpl "$path/adm/templates/vcproj.user.vc9x"
    } elseif { "$theVcVer" == "vc10" } {
      set aCommonSettingsFileTmpl "$path/adm/templates/vcxproj.user.vc10x"
    } elseif { "$theVcVer" == "vc11" } {
      set aCommonSettingsFileTmpl "$path/adm/templates/vcxproj.user.vc11x"
    } elseif { "$theVcVer" == "vc12" } {
      set aCommonSettingsFileTmpl "$path/adm/templates/vcxproj.user.vc12x"
    }
    if { "$aCommonSettingsFileTmpl" != "" } {
      file copy -force -- "$aCommonSettingsFileTmpl" "$aCommonSettingsFile"
      lappend aVcFiles "$aCommonSettingsFile"
    }
  }
  return $aVcFiles
}

# Generate entry for one source file in Visual Studio 7 - 9 project file
proc osutils:vcproj:file { theVcVer theFile theOptions } {
  append aText "\t\t\t\t<File\n"
  append aText "\t\t\t\t\tRelativePath=\"..\\..\\..\\[wokUtils:EASY:bs1 [wokUtils:FILES:wtail $theFile 3]]\">\n"
  if { $theOptions == "" } {
    append aText "\t\t\t\t</File>\n"
    return $aText
  }

  append aText "\t\t\t\t\t<FileConfiguration\n"
  append aText "\t\t\t\t\t\tName=\"Release\|Win32\">\n"
  append aText "\t\t\t\t\t\t<Tool\n"
  append aText "\t\t\t\t\t\t\tName=\"VCCLCompilerTool\"\n"
  append aText "\t\t\t\t\t\t\tAdditionalOptions=\""
  foreach aParam $theOptions {
    append aText "$aParam "
  }
  append aText "\"\n"
  append aText "\t\t\t\t\t\t/>\n"
  append aText "\t\t\t\t\t</FileConfiguration>\n"

  append aText "\t\t\t\t\t<FileConfiguration\n"
  append aText "\t\t\t\t\t\tName=\"Debug\|Win32\">\n"
  append aText "\t\t\t\t\t\t<Tool\n"
  append aText "\t\t\t\t\t\t\tName=\"VCCLCompilerTool\"\n"
  append aText "\t\t\t\t\t\t\tAdditionalOptions=\""
  foreach aParam $theOptions {
    append aText "$aParam "
  }
  append aText "\"\n"
  append aText "\t\t\t\t\t\t/>\n"
  append aText "\t\t\t\t\t</FileConfiguration>\n"
  if { "$theVcVer" == "vc7" } {
    append aText "\t\t\t\t</File>\n"
    return $aText
  }

  append aText "\t\t\t\t\t<FileConfiguration\n"
  append aText "\t\t\t\t\t\tName=\"Release\|x64\">\n"
  append aText "\t\t\t\t\t\t<Tool\n"
  append aText "\t\t\t\t\t\t\tName=\"VCCLCompilerTool\"\n"
  append aText "\t\t\t\t\t\t\tAdditionalOptions=\""
  foreach aParam $theOptions {
    append aText "$aParam "
  }
  append aText "\"\n"
  append aText "\t\t\t\t\t\t/>\n"
  append aText "\t\t\t\t\t</FileConfiguration>\n"

  append aText "\t\t\t\t\t<FileConfiguration\n"
  append aText "\t\t\t\t\t\tName=\"Debug\|x64\">\n"
  append aText "\t\t\t\t\t\t<Tool\n"
  append aText "\t\t\t\t\t\t\tName=\"VCCLCompilerTool\"\n"
  append aText "\t\t\t\t\t\t\tAdditionalOptions=\""
  foreach aParam $theOptions {
    append aText "$aParam "
  }
  append aText "\"\n"
  append aText "\t\t\t\t\t\t/>\n"
  append aText "\t\t\t\t\t</FileConfiguration>\n"

  append aText "\t\t\t\t</File>\n"
  return $aText
}

### AUTOMAKE ###############################################################################
proc OS:MKAMK { theOutDir {theModules {}} theSubPath} {
  global path
  wokUtils:FILES:mkdir $theOutDir

  foreach aModule $theModules {
    foreach aToolKit [$aModule:toolkits] {
      puts " toolkit: $aToolKit ==> $path/src/$aToolKit/EXTERNLIB"
      wokUtils:FILES:rmdir $theOutDir/$aToolKit
      wokUtils:FILES:mkdir $theOutDir/$aToolKit
      osutils:tk:mkam $theOutDir/$aToolKit $aToolKit
    }
    foreach anExecutable [OS:executable $aModule] {
      wokUtils:FILES:rmdir $theOutDir/$anExecutable
      wokUtils:FILES:mkdir $theOutDir/$anExecutable
      osutils:tk:mkamx $theOutDir/$anExecutable $anExecutable
    }
  }
  osutils:am:adm $theOutDir $theModules
  osutils:am:root $path $theSubPath $theModules

  puts "The automake files are stored in the $theOutDir directory"
}

proc wokUtils:FILES:rmdir { d } {
    global env
    global tcl_platform tcl_version  
    regsub -all {\.[^.]*} $tcl_version "" major
    if { $major == 8 } {
	file delete -force $d
    } else {
	if { "$tcl_platform(platform)" == "unix" } {
	    catch { exec rm -rf $d}
	} else {
	    
	}
    }
    return 
}

proc wokUtils:FILES:mkdir { d } {
    global tcl_version
    regsub -all {\.[^.]*} $tcl_version "" major
    if { $major == 8 } {
	file mkdir $d
    } else {
	if ![file exists $d] {
	    if { "[info command mkdir]" == "mkdir" } {
		mkdir -path $d
	    } else {
		puts stderr "wokUtils:FILES:mkdir : Error unable to find a mkdir command."
	    }
	}
    }
    if [file exists $d] {
	return $d
    } else {
	return {}
    }
}

# "Nice letter: %s" { a b c } => {Nice letter: %a}  {Nice letter: %b} ..
# as a string without backslash
proc wokUtils:EASY:FmtSimple1 { fmt l {backslh 1} } {
    foreach e $l {
	if { $backslh } {
	    append str [format $fmt $e] "\n"
	} else {
	    append str [format $fmt $e]
	}
    }
    return $str
}

# edit_last is performed ONCE fmt has been applied.
proc wokUtils:EASY:FmtString2 { fmt l {yes_for_last 0} {edit_last {}} } {
    set ldeb [lrange $l 0 [expr [llength $l] -2]]
    set last [lrange $l end end]
    foreach e $ldeb {
	append str [format $fmt $e $e] " \\" "\n"
    }

    if {$edit_last != {} } {
	set slast [$edit_last [format $fmt $last $last]]
    } else {
	set slast [format $fmt $last $last]
    }

    if { $yes_for_last } {
	append str $slast " \\" "\n" 
    } else {
	append str $slast "\n"
    }

    return $str
}

# { a.x b.c c.v } => { a b c}
proc wokUtils:LIST:sanspoint { l } {
    set rr {}
    foreach x $l {
	lappend rr [file root $x]
    }
    return $rr
}

# remove from listloc OpenCascade units indesirables on Unix
proc osutils:justunix { listloc } {
  if { "$::tcl_platform(os)" == "Darwin" && "$::MACOSX_USE_GLX" != "true" } {
    set goaway [list Xw WNT]
  } else {
    set goaway [list WNT]
  }
  return [osutils:juststation $goaway $listloc]
}

# remove ":" from last item of dependencies list in target VPATH of Makefile.am
proc osutils:am:__VPATH__lastoccur { str } {
  if { [regsub {:$} $str "" u] != 0 } {
    return $u
  }
}

#  ((((((((((((( Formats in Makefile.am )))))))))))))
# Used to replace the string __VPATH__ in Makefile.am
# l is the list of the units in a toolkit.
proc osutils:am:__VPATH__ { l } {
  set fmt "@top_srcdir@/src/%s:"
  return [wokUtils:EASY:FmtString2 $fmt $l 0 osutils:am:__VPATH__lastoccur]
}

# Used to replace the string __INCLUDES__ in Makefile.am
# l is the list of packages in a toolkit.
proc osutils:am:__INCLUDES__ { l } {
  set fmt "-I@top_srcdir@/src/%s"
  return [wokUtils:EASY:FmtString2 $fmt $l]
}

# Used to replace the string __LIBADD__ in Makefile.am
# l is the toolkit closure list of a toolkit.
proc osutils:am:__LIBADD__ { theIncToolkits {final 0} } {
  global path
  global fBranch
  set aFatherModules ""
  set aCurrentWorkBench [file tail $path]
  if { $fBranch != "" } {
    set fd [open $fBranch/adm/UDLIST rb]
    set fileContent [split [read $fd] "\n"]
    close $fd
    set ftoolkits [lsearch -all -inline $fileContent "t *"]
    foreach ft $ftoolkits {
      set aFatherModules "$aFatherModules [string range $ft 2 end]"
    }
  }
  set aLibString ""
  foreach aIncToolkit $theIncToolkits {
    if { [lsearch [split $aFatherModules " "] $aIncToolkit] != -1} {
      append aLibString " \\\n-l$aIncToolkit"
    } else {
      append aLibString " \\\n../$aIncToolkit/lib$aIncToolkit.la"
    }
  }
  return $aLibString
}

# Used to replace the string __SOURCES__ in Makefile.am
# l is the list of all compilable files in a toolkit.
proc osutils:am:__SOURCES__ { l } {
  set fmt "%s"
  return [wokUtils:EASY:FmtString1 $fmt $l]
}

proc osutils:am:__CXXFLAG__ { l } {
  set fmt "%s"
  return [wokUtils:EASY:FmtString1 $fmt [osutils:am:PkCXXOption $l]]
}

proc osutils:am:PkCXXOption { ppk } {
  global path
  #puts "\t 1 [lindex [wokparam -e  %CMPLRS_CXX_Options [wokcd]] 0]"
  set CXXCOMMON [_get_options lin cmplrs_cxx b]
  #puts "\t 2 [wokparam -v %CMPLRS_CXX_Options [w_info -f]]"
  #puts "\t 3 [wokparam -v %CMPLRS_CXX_Options]"
  set FoundFlag "[lindex [osutils:intersect3 [split [_get_options lin cmplrs_cxx f]] [split [_get_options lin cmplrs_cxx b]] ] 2]"
  foreach pk $ppk {
    #puts $pk
	if {![file isdirectory $path/src/$pk]} {
	  continue
	}
    set src_files [_get_used_files $pk false]
    set only_src_files {}
    foreach s $src_files { 
      regexp {source ([^\s]+)} $s dummy name
      lappend only_src_files $name
    }
    if {[lsearch $only_src_files ${pk}_CMPLRS.edl] != "-1"} {
      set pk_cmplrs_cxx [_get_options lin cmplrs_cxx $pk]
      if {$pk_cmplrs_cxx == ""} {
	    set pk_cmplrs_cxx [_get_options lin cmplrs_cxx b]
	  }
      set CXXStr  $pk_cmplrs_cxx
	  #puts "\t 4 [wokparam -e %CMPLRS_CXX_Options [woklocate -u $pk]] $pk"
      set LastIndex [expr {[string length $CXXCOMMON ] - 1}]
      if {[string equal $CXXCOMMON [string range $CXXStr 0 $LastIndex]]} {
        set CXXOption " "
      } else {
        set CXXOption [string range $CXXStr 0 [expr {[string last $CXXCOMMON $CXXStr] - 1}]]
      }
      if {$CXXOption != " " && $CXXOption != "" && $CXXOption != "  " && $CXXOption != "   "} {
        set FoundList [split $CXXOption " "]
        foreach elem $FoundList {
          if {$elem != ""} {
            if {[string first "-I" $elem] == "-1"  } {
              if {[string first $elem $FoundFlag] == "-1"} {
                set FoundFlag "$FoundFlag $elem"
              }
            }
          }
        }
      }
    }
  }
  return $FoundFlag
}

# Create in dir the Makefile.am associated with toolkit tkloc.
# Returns the full path of the created file.
proc osutils:tk:mkam { dir tkloc } {
  global path
  set pkgs $path/src/${tkloc}/PACKAGES
  if { ![file exists $pkgs] } {
    puts stderr "osutils:tk:mkam : Error. File PACKAGES not found for toolkit $tkloc."
    return {}
  }

  set tmplat [osutils:readtemplate mam "Makefile.am"]
  set lpkgs  [osutils:justunix [wokUtils:FILES:FileToList $pkgs]]
  set close  [wokUtils:LIST:Purge [osutils:tk:close $tkloc]]
  set lsrc   [lsort [osutils:tk:files $tkloc osutils:compilable 1 osutils:justunix]]
  set lobj   [wokUtils:LIST:sanspoint $lsrc]

  set lcsf   [osutils:tk:hascsf $path/src/${tkloc}/EXTERNLIB]

  set final 0
  set externinc ""
  set externlib ""
  if { $lcsf != {} } {
    set final 1
    set fmtinc "\$(%s_INCLUDES) "
    set fmtlib "\$(%s_LIB) "
    set externinc [wokUtils:EASY:FmtSimple1 $fmtinc $lcsf 0]
    set externlib [wokUtils:EASY:FmtSimple1 $fmtlib $lcsf 0]
  }

  regsub -all -- {__TKNAM__} $tmplat $tkloc tmplat
  set vpath [osutils:am:__VPATH__ $lpkgs]
  regsub -all -- {__VPATH__} $tmplat $vpath tmplat
  set inclu [osutils:am:__INCLUDES__ $lpkgs]
  regsub -all -- {__INCLUDES__} $tmplat $inclu tmplat
  if { $close != {} } {
    set libadd [osutils:am:__LIBADD__ $close $final]
  } else {
    set libadd ""
  }
  regsub -all -- {__LIBADD__} $tmplat $libadd tmplat
  set source [osutils:am:__SOURCES__ $lsrc]
  regsub -all -- {__SOURCES__} $tmplat $source tmplat
  regsub -all -- {__EXTERNINC__} $tmplat $externinc tmplat
  set CXXFl [osutils:am:__CXXFLAG__ $lpkgs]
  regsub -all -- {__CXXFLAG__} $tmplat $CXXFl tmplat
  set CFl [osutils:am:__CFLAG__ $lpkgs]
  regsub -all -- {__CFLAG__} $tmplat $CFl tmplat

  regsub -all -- {__EXTERNLIB__} $tmplat $externlib tmplat

  wokUtils:FILES:StringToFile $tmplat [set fmam [file join $dir Makefile.am]]
  return [list $fmam]
}

# Write a string in a file
proc wokUtils:FILES:StringToFile { str path } {
    if { [catch { set out [ open $path w ] } errout] == 0 } {
	puts -nonewline $out $str
	close $out
	return 1
    } else {
	return {}
    }
}

# Create in dir the Makefile.am associated with toolkit tkloc.
# Returns the full path of the created file.
proc osutils:tk:mkamx { dir tkloc } {
  global path
  set src_files [_get_used_files $tkloc false]
  set only_src_files {}
  foreach s $src_files { 
    regexp {source ([^\s]+)} $s dummy name
    lappend only_src_files $name
  }
  if { [lsearch $only_src_files ${tkloc}_WOKSteps.edl] != "-1"} {
    set pkgs "$path/src/${tkloc}/EXTERNLIB"
    if { $pkgs == {} } {
      puts stderr "osutils:tk:mkamx : Error. File EXTERNLIB not found for executable $tkloc."
      #return {}
    }
    set tmplat [osutils:readtemplate mamx "Makefile.am (executable)"]
    set close  [wokUtils:LIST:Purge [osutils:tk:close $tkloc]]
    set lsrc   [lsort [osutils:tk:files $tkloc osutils:compilable 1 osutils:justunix]]
    set lobj   [wokUtils:LIST:sanspoint $lsrc]
    set CXXList {}
    foreach SourceFile $only_src_files {
      if {[file extension $SourceFile] == ".cxx"} {
        lappend CXXList [file rootname $SourceFile]
      }
    }
    set pkgs [LibToLinkX $tkloc [lindex $CXXList 0]]
    set lpkgs  [osutils:justunix [wokUtils:FILES:FileToList $pkgs]]
    puts "pkgs $pkgs"
    #set lcsf   [osutils:tk:hascsf [woklocate -p ${tkloc}:source:EXTERNLIB [wokcd]]]

    set lcsf {}
    foreach tk $pkgs {
      foreach element [osutils:tk:hascsf "$path/src/${tk}/EXTERNLIB"] {
        if {[lsearch $lcsf $element] == "-1"} {
          set lcsf [concat $lcsf $element]
        }
      }
    }
    set final 0
    set externinc ""
    set externlib ""
    if { $lcsf != {} } {
      set final 1
      set fmtinc "\$(%s_INCLUDES) "
      set fmtlib "\$(%s_LIB) "
      set externinc [wokUtils:EASY:FmtSimple1 $fmtinc $lcsf 0]
      set externlib [wokUtils:EASY:FmtSimple1 $fmtlib $lcsf 0]
    }
    regsub -all -- {__XQTNAM__} $tmplat $tkloc tmplat
    set tmplat "$tmplat \nlib_LTLIBRARIES="
    foreach entity $CXXList {
      set tmplat "$tmplat lib${entity}.la"
    }
    set tmplat "$tmplat\n"
    set inclu [osutils:am:__INCLUDES__ $lpkgs]
    regsub -all -- {__INCLUDES__} $tmplat $inclu tmplat
    if { $pkgs != {} } {
      set libadd [osutils:am:__LIBADD__ $pkgs $final]
    } else {
      set libadd ""
    }
    regsub -all -- {__LIBADD__} $tmplat $libadd tmplat
    set source [osutils:am:__SOURCES__ $CXXList]
    regsub -all -- {__SOURCES__} $tmplat $source tmplat
    regsub -all -- {__EXTERNINC__} $tmplat $externinc tmplat
    foreach entity $CXXList {
      set tmplat "$tmplat lib${entity}_la_SOURCES = @top_srcdir@/src/${tkloc}/${entity}.cxx \n"
    }
    foreach entity $CXXList {
      set tmplat "$tmplat lib${entity}_la_LIBADD = $libadd $externlib \n"
    }
    wokUtils:FILES:StringToFile $tmplat [set fmam [file join $dir Makefile.am]]

    unset tmplat

    return [list $fmam]

  } else {
    set pkgs "$path/src/${tkloc}/EXTERNLIB"
    if { $pkgs == {} } {
      puts stderr "osutils:tk:mkamx : Error. File EXTERNLIB not found for executable $tkloc."
      #return {}
    }
    set tmplat [osutils:readtemplate mamx "Makefile.am (executable)"]
    set close  [wokUtils:LIST:Purge [osutils:tk:close $tkloc]]
    set lsrc   [lsort [osutils:tk:files $tkloc osutils:compilable 1 osutils:justunix]]
    set lobj   [wokUtils:LIST:sanspoint $lsrc]
    set CXXList {}
    foreach SourceFile $only_src_files {
      if {[file extension $SourceFile] == ".cxx"} {
        lappend CXXList [file rootname $SourceFile]
      }
    }
    set pkgs [LibToLinkX $tkloc [lindex $CXXList 0]]
    set lpkgs  [osutils:justunix [wokUtils:FILES:FileToList $pkgs]]
    set lcsf   [osutils:tk:hascsf "$path/src/${tkloc}/EXTERNLIB"]

    set lcsf {}
    foreach tk $pkgs {
      foreach element [osutils:tk:hascsf "$path/src/${tk}/EXTERNLIB"] {
        if {[lsearch $lcsf $element] == "-1"} {
          set lcsf [concat $lcsf $element]
        }
      }
    }
    set final 0
    set externinc ""
    set externlib ""
    if { $lcsf != {} } {
      set final 1
      set fmtinc "\$(%s_INCLUDES) "
      set fmtlib "\$(%s_LIB) "
      set externinc [wokUtils:EASY:FmtSimple1 $fmtinc $lcsf 0]
      set externlib [wokUtils:EASY:FmtSimple1 $fmtlib $lcsf 0]
    }
    regsub -all -- {__XQTNAM__} $tmplat $tkloc tmplat
    set tmplat "$tmplat \nbin_PROGRAMS="
    foreach entity $CXXList {
      set tmplat "${tmplat} ${entity}"
    }

    set tmplat "${tmplat}\n"
    set inclu [osutils:am:__INCLUDES__ $lpkgs]
    regsub -all -- {__INCLUDES__} $tmplat $inclu tmplat
    if { $pkgs != {} } {
      set libadd [osutils:am:__LIBADD__ $pkgs $final]
    } else {
      set libadd ""
    }
    set source [osutils:am:__SOURCES__ $CXXList]
    regsub -all -- {__SOURCES__} $tmplat $source tmplat
    regsub -all -- {__EXTERNINC__} $tmplat $externinc tmplat
    foreach entity $CXXList {
      set tmplat "$tmplat ${entity}_SOURCES = @top_srcdir@/src/${tkloc}/${entity}.cxx \n"
    }
    foreach entity $CXXList {
      set tmplat "$tmplat ${entity}_LDADD = $libadd $externlib \n"
    }
    wokUtils:FILES:StringToFile $tmplat [set fmam [file join $dir Makefile.am]]

    return [list $fmam]
  }
}

# Create in dir the Makefile.am in $dir directory.
# Returns the full path of the created file.
proc osutils:am:adm { dir {lesmodules {}} } {
  set amstring "srcdir = @srcdir@\n\n"
  set subdirs "SUBDIRS ="
  set vpath "VPATH = @srcdir@ ${dir}: "
  set make ""
  set phony ".PHONY:"
  foreach theModule $lesmodules {
    set units [osutils:tk:sort [$theModule:toolkits]]
    set units [concat $units [OS:executable $theModule]]
    append amstring "${theModule}_PKGS ="
    append vpath "\\\n"
    foreach unit $units {
      append amstring " ${unit}"
      append vpath "${dir}/${unit}: "
    }
    set up ${theModule}
    if { [info procs ${theModule}:alias] != "" } {
      set up [${theModule}:alias]
    }
    set up [string toupper ${up}]
    append amstring "\n\nif ENABLE_${up}\n"
    append amstring "  ${theModule}_DIRS = \$(${theModule}_PKGS)\n"
    append amstring "else\n"
    append amstring "  ${theModule}_DIRS = \n"
    append amstring "endif\n\n"
    append subdirs " \$(${theModule}_DIRS)"
    append make "${theModule}:\n"
    append make "\tfor d in \$(${theModule}_PKGS); do \\\n"
    append make "\t\tcd \$\$d; \$(MAKE) \$(AM_MAKEFLAGS) lib\$\$d.la; cd ..; \\\n"
    append make "\tdone\n\n"
    append phony " ${theModule}"
  }
  append amstring "$subdirs\n\n"
  append amstring "$vpath\n\n"
  append amstring $make
  append amstring $phony
  wokUtils:FILES:StringToFile $amstring [set fmam [file join $dir Makefile.am]]
  return [list $fmam]
}

# retourne la liste de dependances de module.
proc OS:lsdep { m } {
    set res {}
    set l [${m}:depends]
    if { $l != {} } {
	set res [concat $res $l]
	foreach h $l {
	    set res [concat $res [OS:lsdep ${h}]]
	}
    }
    return $res
}

# Create in dir the Makefile.am and configure.ac in CASROOT directory.
# Returns the full path of the created file.
proc osutils:am:root { dir theSubPath {lesmodules {}} } {
  global path
  set amstring "srcdir = @srcdir@\n\n"
  append amstring "SUBDIRS = ${theSubPath}\n\n"
  append amstring "VPATH = @srcdir@ @top_srcdir@/${theSubPath}: @top_srcdir@/${theSubPath}:\n\n"

  set phony ".PHONY:"

  set acstring [osutils:readtemplate ac "Makefile.am"]
  set enablestr ""
  set confstr ""
  set condstr ""
  set repstr ""
  set acconfstr ""

  set exelocal "install-exec-local:\n"
  append exelocal "\t"
  append exelocal {$(INSTALL) -d $(prefix)/$(platform)}
  append exelocal "\n"
  foreach d {bin lib} {
    append exelocal "\t"
    append exelocal "if \[ -e \$(prefix)/${d} -a ! -e \$(prefix)/\$(platform)/${d} \]; then \\\n"
    append exelocal "\t\tcd \$(prefix)/\$(platform) && ln -s ../${d} ${d}; \\\n"
    append exelocal "\tfi\n"
  }
  append exelocal "\t"
  append exelocal {buildd=`pwd`; cd $(top_srcdir); sourced=`pwd`; cd $(prefix); installd=`pwd`; cd $$buildd;}
  append exelocal " \\\n"
  append exelocal "\t"
  append exelocal {if [ "$$installd" != "$$sourced" ]; then}
  append exelocal " \\\n"
  append exelocal "\t\t"
  append exelocal {$(INSTALL) -d $(prefix)/src;}
  append exelocal " \\\n"
  append exelocal "\t\t"
  append exelocal {cp -frL $(top_srcdir)/src $(prefix);}
  append exelocal " \\\n"
  append exelocal "\t\t"
  append exelocal {cp -frL $$buildd/config.h $(prefix);}
  append exelocal " \\\n"
  append exelocal "\t\tfor d in "

  foreach theModule $lesmodules {
    append amstring "${theModule}_PKGS ="
    foreach r [${theModule}:ressources] {
      if { "[lindex $r 1]" == "r" } {
	append amstring " [lindex $r 2]"
      }
    }
    set up ${theModule}
    if { [info procs ${theModule}:alias] != "" } {
      set up [${theModule}:alias]
    }
    set up [string toupper ${up}]
    set lower ${theModule}
    if { [info procs ${theModule}:alias] != "" } {
      set lower [${theModule}:alias]
    }
    set lower [string tolower ${lower}]

    append amstring "\n\nif ENABLE_${up}\n"
    append amstring "  ${theModule}_DIRS = \$(${theModule}_PKGS)\n"
    append amstring "else\n"
    append amstring "  ${theModule}_DIRS = \n"
    append amstring "endif\n\n"
    append amstring "${theModule}:\n"
    append amstring "\tcd \$(top_builddir)/${theSubPath} && \$(MAKE) \$(AM_MAKEFLAGS) ${theModule}\n\n"
    append phony " ${theModule}"

    append exelocal " \$(${theModule}_DIRS)"

    append enablestr "AC_ARG_ENABLE(\[${lower}\],\n"
    append enablestr "  \[AS_HELP_STRING(\[--disable-${lower}\],\[Disable ${theModule} components\])\],\n"
    append enablestr "  \[ENABLE_${up}=\${enableval}\],\[ENABLE_${up}=yes\])\n"

    set deplist [OS:lsdep ${theModule}]
    set acdeplist {}
    if { [info procs ${theModule}:acdepends] != "" } {
      set acdeplist [${theModule}:acdepends]
    }

    if { [llength $deplist] > 0 || [llength $acdeplist] > 0} {
      append confstr "if test \"xyes\" = \"x\$ENABLE_${up}\"; then\n"
    } else {
      append confstr "if test \"xyes\" != \"x\$ENABLE_${up}\"; then\n"
    }
    foreach dep $deplist {
      set dup ${dep}
      if { [info procs ${dep}:alias] != "" } {
	set dup [${dep}:alias]
      }
      set dup [string toupper ${dup}]
      append confstr "  if test \"xyes\" = \"x\$ENABLE_${up}\" -a \"xyes\" != \"x\$ENABLE_${dup}\"; then\n"
      append confstr "    AC_MSG_NOTICE(\[Disabling ${theModule}: not building ${dep} component\])\n"
      append confstr "    DISABLE_${up}_REASON=\"(${dep} component disabled)\"\n"
      append confstr "    ENABLE_${up}=no\n"
      append confstr "  fi\n"
    }
    foreach dep $acdeplist {
      append confstr "  if test \"xyes\" = \"x\$ENABLE_${up}\" -a \"xyes\" != \"x\$HAVE_${dep}\"; then\n"
      append confstr "    AC_MSG_NOTICE(\[Disabling ${theModule}: ${dep} not found\])\n"
      append confstr "    DISABLE_${up}_REASON=\"(${dep} not found)\"\n"
      append confstr "    ENABLE_${up}=no\n"
      append confstr "  fi\n"
    }
    if { [llength $deplist] > 0 || [llength $acdeplist] > 0 } {
      append confstr "else\n"
    }
    append confstr "  DISABLE_${up}_REASON=\"(Disabled)\"\n"
    append confstr "fi\n"

    append condstr "AM_CONDITIONAL(\[ENABLE_${up}\], \[test \"xyes\" = \"x\$ENABLE_${up}\"\])\n"
    append repstr [format "echo \"%-*s  \$ENABLE_${up} \$DISABLE_${up}_REASON\"" 26 ${theModule}]
    append repstr "\n"

    set units [$theModule:toolkits]
    set units [concat $units [OS:executable $theModule]]
    foreach unit $units {
      append acconfstr "${theSubPath}/${unit}/Makefile \\\n"
    }
  }

  append exelocal "; do \\\n"
  append exelocal "\t\t\t"
  append exelocal {$(INSTALL) -d $(prefix)/src/$$d;}
  append exelocal " \\\n"
  append exelocal "\t\t\t"
  append exelocal {cp -frL $(top_srcdir)/src/$$d $(prefix)/src;}
  append exelocal " \\\n"
  append exelocal "\t\tdone; \\\n"
  append exelocal "\tfi\n"
  append exelocal "\t"
  append exelocal {if [ -e $(prefix)/src/config.h ]; then}
  append exelocal " \\\n"
  append exelocal "\t\t"
  append exelocal {unlink $(prefix)/src/config.h;}
  append exelocal " \\\n"
  append exelocal "\tfi\n"
  append exelocal "\t"
  append exelocal {cd $(prefix)/src && ln -s ../config.h config.h}
  append exelocal "\n"
  append exelocal "\t"
  append exelocal {cd $(top_srcdir) && cp *.sh $(prefix)}
  append exelocal "\n"
  append exelocal "\n"

  append amstring $exelocal
  append amstring $phony

  regsub -all -- {__ENABLEMODULES__} $acstring $enablestr acstring
  regsub -all -- {__CONFMODULES__} $acstring $confstr acstring
  regsub -all -- {__CONDMODULES__} $acstring $condstr acstring
  regsub -all -- {__REPMODULES__} $acstring $repstr acstring
  regsub -all -- {__ACCONFMODULES__} $acstring $acconfstr acstring

  wokUtils:FILES:StringToFile $amstring [set fmam [file join $dir Makefile.am]]
  wokUtils:FILES:StringToFile $acstring [set fmam [file join $dir configure.ac]]
  file copy -force -- [file join $path/adm/templates build_configure] [file join $dir build_configure]
  file copy -force -- [file join $path/adm/templates acinclude.m4] [file join $dir acinclude.m4]
  file copy -force -- [file join $path/adm/templates custom.sh.in] [file join $dir custom.sh.in]
  return [list $fmam]
}

proc wokUtils:EASY:FmtString1 { fmt l {yes_for_last 0} {edit_last {}} } {
    set ldeb [lrange $l 0 [expr [llength $l] -2]]
    set last [lrange $l end end]
    foreach e $ldeb {
	append str [format $fmt $e] " \\" "\n"
    }

    if {$edit_last != {} } {
	set slast [$edit_last [format $fmt $last]]
    } else {
	set slast [format $fmt $last]
    }

    if { $yes_for_last } {
	append str $slast " \\" "\n" 
    } else {
	append str $slast "\n"
    }
    return $str
}

# Used to replace the string __CFLAG__ in Makefile.am
# l is the list of all compilable files in a toolkit.
proc osutils:am:__CFLAG__ { l } {
  set fmt "%s"
  return [wokUtils:EASY:FmtString1 $fmt [osutils:am:PkCOption $l]]
}

proc osutils:am:PkCOption { ppk } {
  global path
  #puts "\t\t $ppk"
  #puts "\t 5 [lindex [wokparam -e  %CMPLRS_C_Options [wokcd]] 0]"
  set CCOMMON [_get_options lin cmplrs_c b]
  #puts "\t 6 [lindex [wokparam -v %CMPLRS_C_Options [w_info -f]] 0]"
  set FoundFlag "[lindex [osutils:intersect3 [split [_get_options lin cmplrs_c f]] [split [_get_options lin cmplrs_c b]]] 2]"
  foreach pk $ppk {
    if {![file isdirectory $path/src/$pk]} {
	  continue
	}
	set src_files [_get_used_files $pk false]
    set only_src_files {}
    foreach s $src_files {
      regexp {source ([^\s]+)} $s dummy name
      lappend only_src_files $name
    }
    if {[lsearch $src_files ${pk}_CMPLRS.edl] != "-1"} {
	  #puts "\t 7 [lindex [wokparam -e %CMPLRS_C_Options [woklocate -u $pk]] 0] $pk"
      set aPkList   [split "[_get_options lin cmplrs_c $pk]" " "]
      set aCcomList [split "$CCOMMON" " "]

      foreach aPkItem $aPkList {
        if { [lsearch aCcomList $aPkItem] != -1 } {
          if {[string first "-I" $aPkItem] == "-1"  } {
            set FoundFlag "$FoundFlag $aPkItem"
          }
        }
      }
    }
  }
  return $FoundFlag
}

proc osutils:tksrcfiles { theUnits  theRelatedPath {theCompatible {}} } {
  set aTKSrcFiles [list]

  if [array exists written] { unset written }
  foreach anUnit $theUnits {
    set xlo       $anUnit
    set aSrcFiles [osutils:tk:files $xlo osutils:compilable 0]

    if { $theCompatible != {} } {
      set aSrcFiles [osutils:tk:files $xlo $theCompatible 0]
    }

    foreach aSrcFile [lsort $aSrcFiles] {
      if { ![info exists written([file tail $aSrcFile])] } {
        set written([file tail $aSrcFile]) 1
        lappend aTKSrcFiles "${theRelatedPath}/[wokUtils:FILES:wtail $aSrcFile 3]"
      } else {
        puts "Warning : more than one occurences for [file tail $aSrcFile]"
      }
    }
  }

  return $aTKSrcFiles
}

proc osutils:mm_compilable { } {
  return [list .mm]
}

proc osutils:tkdefs { theUnits } {
  set aTKDefines [list]

  foreach anUnit $theUnits {
    lappend aTKDefines "__${anUnit}_DLL"
  }

  return $aTKDefines
}

proc osutils:fileGroupName { theSrcFile } {
  set path [file dirname [file normalize ${theSrcFile}]]
  regsub -all [file normalize "${path}/.."] ${path} "" aGroupName

  return $aGroupName
}


####### CODEBLOCK ###################################################################
# Function to generate Code Blocks workspace and project files
proc OS:MKCBP { theOutDir {theModules {}} {theAllSolution ""} } {
  puts stderr "Generating project files for Code Blocks"

  # Generate projects for toolkits and separate workspace for each module
  foreach aModule $theModules {
    OS:cworkspace $aModule $aModule $theOutDir
    OS:cbp        $aModule          $theOutDir
  }

  # Generate single workspace "OCCT" containing projects from all modules
  if { "$theAllSolution" != "" } {
    OS:cworkspace $theAllSolution $theModules $theOutDir
  }

  puts "The Code Blocks workspace and project files are stored in the $theOutDir directory"
}

# Generate Code Blocks projects
proc OS:cbp { theModules theOutDir } {
  set aProjectFiles {}
  foreach aModule $theModules {
    foreach aToolKit [${aModule}:toolkits] {
      lappend aProjectFiles [osutils:cbptk $theOutDir $aToolKit ]
    }
    foreach anExecutable [OS:executable ${aModule}] {
      lappend aProjectFiles [osutils:cbpx  $theOutDir $anExecutable]
    }
  }
  return $aProjectFiles
}

# Generate Code::Blocks project file for ToolKit
proc osutils:cbptk { theOutDir theToolKit } {
  set aUsedToolKits [list]
  set aFrameworks   [list]
  set anIncPaths    [list]
  set aTKDefines    [list]
  set aTKSrcFiles   [list]

  osutils:tkinfo "../../.." $theToolKit aUsedToolKits aFrameworks anIncPaths aTKDefines aTKSrcFiles

  return [osutils:cbp $theOutDir $theToolKit $aTKSrcFiles $aUsedToolKits $aFrameworks $anIncPaths $aTKDefines]
}

# Generates Code Blocks workspace.
proc OS:cworkspace { theSolName theModules theOutDir } {
  global path
  set aWsFilePath "${theOutDir}/${theSolName}.workspace"
  set aFile [open $aWsFilePath "w"]
  set isActiveSet 0
  puts $aFile "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>"
  puts $aFile "<CodeBlocks_workspace_file>"
  puts $aFile "\t<Workspace title=\"${theSolName}\">"

  # collect list of projects to be created
  foreach aModule $theModules {
    # toolkits
    foreach aToolKit [osutils:tk:sort [${aModule}:toolkits]] {
      set aDependencies [LibToLink $aToolKit]
      if { [llength $aDependencies] == 0 } {
        puts $aFile "\t\t<Project filename=\"${aToolKit}.cbp\" />"
      } else {
        puts $aFile "\t\t<Project filename=\"${aToolKit}.cbp\" >"
        foreach aDepTk $aDependencies {
          puts $aFile "\t\t\t<Depends filename=\"${aDepTk}.cbp\" />"
        }
        puts $aFile "\t\t</Project>"
      }
    }

    # executables, assume one project per cxx file...
    foreach aUnit [OS:executable ${aModule}] {
      set aUnitLoc $aUnit
      set src_files [_get_used_files $aUnit false]
      set aSrcFiles {}
      foreach s $src_files { 
        regexp {source ([^\s]+)} $s dummy name
        lappend aSrcFiles $name
      }
      foreach aSrcFile $aSrcFiles {
        set aFileExtension [file extension $aSrcFile]
        if { $aFileExtension == ".cxx" } {
          set aPrjName [file rootname $aSrcFile]
          set aDependencies [list]
          if {[file isdirectory $path/src/$aUnitLoc]} {
            set aDependencies [LibToLinkX $aUnitLoc [file rootname $aSrcFile]]
          }
          set anActiveState ""
          if { $isActiveSet == 0 } {
            set anActiveState " active=\"1\""
            set isActiveSet 1
          }
          if { [llength $aDependencies] == 0 } {
            puts $aFile "\t\t<Project filename=\"${aPrjName}.cbp\"${anActiveState}/>"
          } else {
            puts $aFile "\t\t<Project filename=\"${aPrjName}.cbp\"${anActiveState}>"
            foreach aDepTk $aDependencies {
              puts $aFile "\t\t\t<Depends filename=\"${aDepTk}.cbp\" />"
            }
            puts $aFile "\t\t</Project>"
          }
        }
      }
    }
  }

  puts $aFile "\t</Workspace>"
  puts $aFile "</CodeBlocks_workspace_file>"
  close $aFile

  return $aWsFilePath
}

# Generate Code::Blocks project file for Executable
proc osutils:cbpx { theOutDir theToolKit } {
  global path targetStation
  set aWokStation "$targetStation"
  set aWokArch    "$::env(ARCH)"

  set aCbpFiles {}
  foreach aSrcFile [osutils:tk:files $theToolKit osutils:compilable 0] {
    # collect list of referred libraries to link with
    set aUsedToolKits [list]
    set aFrameworks   [list]
    set anIncPaths    [list]
    set aTKDefines    [list]
    set aTKSrcFiles   [list]
    set aProjName [file rootname [file tail $aSrcFile]]

    set aDepToolkits [LibToLinkX $theToolKit $aProjName]
    foreach tkx $aDepToolkits {
      if {[_get_type $tkx] == "t"} {
        lappend aUsedToolKits "${tkx}"
      }
      if {[lsearch [glob -tails -directory "$path/src" -types d *] $tkx] == "-1"} {
        lappend aUsedToolKits "${tkx}"
      }
    }

    #wokparam -l CSF

    foreach tk $aDepToolkits {
      foreach element [osutils:tk:hascsf $path/src/${tk}/EXTERNLIB] {
        if {[_get_options lin csf $element] == ""} {
          continue
        }
        set isFrameworkNext 0
        foreach fl [split [_get_options lin csf $element]] {
          if {[string first "-libpath" $fl] != "-1"} {
            # this is library search path, not the library name
            continue
          } elseif {[string first "-framework" $fl] != "-1"} {
            set isFrameworkNext 1
            continue
          }

          set felem [file tail $fl]
          if {$isFrameworkNext == 1} {
            if {[lsearch $aFrameworks $felem] == "-1"} {
              lappend aFrameworks "${felem}"
            }
            set isFrameworkNext 0
          } elseif {[lsearch $aUsedToolKits $felem] == "-1"} {
            if {$felem != "\{\}" & $felem != "lib"} {
              if {[lsearch -nocase [osutils:optinal_libs] $felem] == -1} {
                lappend aUsedToolKits [string trimleft "${felem}" "-l"]
              }
            }
          }
        }
      }
    }

    set WOKSteps_exec_link [_get_options lin WOKSteps_exec_link $theToolKit]
    if { [regexp {WOKStep_DLLink} $WOKSteps_exec_link] || [regexp {WOKStep_Libink} $WOKSteps_exec_link] } {
      set isExecutable "false"
    } else {
      set isExecutable "true"
    }

    if { ![info exists written([file tail $aSrcFile])] } {
      set written([file tail $aSrcFile]) 1
      lappend aTKSrcFiles $aSrcFile
    } else {
      puts "Warning : in cbp there are more than one occurences for [file tail $aSrcFile]"
    }

    # macros for correct DLL exports
    if { "$aWokStation" == "wnt" } {
      lappend aTKDefines "__${theToolKit}_DLL"
    }

    # common include paths
    lappend anIncPaths "../../../inc"

    # macros for UNIX to use config.h file
    lappend aTKDefines "CSFDB"
    if { "$aWokStation" == "wnt" } {
      lappend aTKDefines "WNT"
      lappend aTKDefines "_CRT_SECURE_NO_DEPRECATE"
    } else {
      lappend aTKDefines "HAVE_WOK_CONFIG_H"
      lappend aTKDefines "HAVE_CONFIG_H"
      if { "$aWokStation" == "lin" } {
        lappend aTKDefines "LIN"
      }
      lappend aTKDefines "OCC_CONVERT_SIGNALS"
      #lappend aTKDefines "_GNU_SOURCE=1"
    }

    lappend aCbpFiles [osutils:cbp $theOutDir $aProjName $aTKSrcFiles $aUsedToolKits $aFrameworks $anIncPaths $aTKDefines $isExecutable]
  }

  return $aCbpFiles
}

proc osutils:optinal_libs { } {
  return [list tbb.lib tbbmalloc.lib FreeImage.lib FreeImagePlus.lib gl2ps.lib]
}

# This function intended to generate Code::Blocks project file
# @param theOutDir     - output directory to place project file
# @param theProjName   - project name
# @param theSrcFiles   - list of source files
# @param theLibsList   - dependencies (libraries  list)
# @param theFrameworks - dependencies (frameworks list, Mac OS X specific)
# @param theIncPaths   - header search paths
# @param theDefines    - compiler macro definitions
# @param theIsExe      - flag to indicate executable / library target
proc osutils:cbp { theOutDir theProjName theSrcFiles theLibsList theFrameworks theIncPaths theDefines {theIsExe "false"} } {
  global targetStation
  set aWokStation "$targetStation"
  set aWokArch    "$::env(ARCH)"

  set aCbpFilePath "${theOutDir}/${theProjName}.cbp"
  set aFile [open $aCbpFilePath "w"]
  puts $aFile "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>"
  puts $aFile "<CodeBlocks_project_file>"
  puts $aFile "\t<FileVersion major=\"1\" minor=\"6\" />"
  puts $aFile "\t<Project>"
  puts $aFile "\t\t<Option title=\"$theProjName\" />"
  puts $aFile "\t\t<Option pch_mode=\"2\" />"
  if { "$aWokStation" == "wnt" } {
    puts $aFile "\t\t<Option compiler=\"msvc8\" />"
  } else {
    puts $aFile "\t\t<Option compiler=\"gcc\" />"
  }
  puts $aFile "\t\t<Build>"

  # Release target configuration
  puts $aFile "\t\t\t<Target title=\"Release\">"
  if { "$theIsExe" == "true" } {
    puts $aFile "\t\t\t\t<Option output=\"../../../${aWokStation}/cbp/bin/${theProjName}\" prefix_auto=\"1\" extension_auto=\"1\" />"
    puts $aFile "\t\t\t\t<Option type=\"1\" />"
  } else {
    if { "$aWokStation" == "wnt" } {
      puts $aFile "\t\t\t\t<Option output=\"../../../${aWokStation}/cbp/lib/${theProjName}\" prefix_auto=\"1\" extension_auto=\"1\" />"
    } else {
      puts $aFile "\t\t\t\t<Option output=\"../../../${aWokStation}/cbp/lib/lib${theProjName}\" prefix_auto=\"1\" extension_auto=\"1\" />"
    }
    puts $aFile "\t\t\t\t<Option type=\"3\" />"
  }
  puts $aFile "\t\t\t\t<Option object_output=\"../../../${aWokStation}/cbp/obj\" />"
  if { "$aWokStation" == "wnt" } {
    puts $aFile "\t\t\t\t<Option compiler=\"msvc8\" />"
  } else {
    puts $aFile "\t\t\t\t<Option compiler=\"gcc\" />"
  }
  puts $aFile "\t\t\t\t<Option createDefFile=\"1\" />"
  puts $aFile "\t\t\t\t<Option createStaticLib=\"1\" />"

  # compiler options per TARGET (including defines)
  puts $aFile "\t\t\t\t<Compiler>"
  if { "$aWokStation" == "wnt" } {
    puts $aFile "\t\t\t\t\t<Add option=\"-MD\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-arch:SSE2\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-EHsc\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-O2\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-W4\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-MP\" />"
  } else {
    puts $aFile "\t\t\t\t\t<Add option=\"-O2\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-mmmx\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-msse\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-msse2\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-mfpmath=sse\" />"
  }
  foreach aMacro $theDefines {
    puts $aFile "\t\t\t\t\t<Add option=\"-D${aMacro}\" />"
  }
  puts $aFile "\t\t\t\t\t<Add option=\"-DNDEBUG\" />"
  puts $aFile "\t\t\t\t\t<Add option=\"-DNo_Exception\" />"

  puts $aFile "\t\t\t\t</Compiler>"

  puts $aFile "\t\t\t\t<Linker>"
  puts $aFile "\t\t\t\t\t<Add directory=\"../../../${aWokStation}/cbp/lib\" />"
  if { "$aWokStation" == "mac" && [ lsearch $theLibsList X11 ] >= 0} {
    puts $aFile "\t\t\t\t\t<Add directory=\"/usr/X11/lib\" />"
  }
  puts $aFile "\t\t\t\t\t<Add option=\"\$(CSF_OPT_LNK${aWokArch})\" />"
  puts $aFile "\t\t\t\t</Linker>"

  puts $aFile "\t\t\t</Target>"

  # Debug target configuration
  puts $aFile "\t\t\t<Target title=\"Debug\">"
  if { "$theIsExe" == "true" } {
    puts $aFile "\t\t\t\t<Option output=\"../../../${aWokStation}/cbp/bind/${theProjName}\" prefix_auto=\"1\" extension_auto=\"1\" />"
    puts $aFile "\t\t\t\t<Option type=\"1\" />"
  } else {
    if { "$aWokStation" == "wnt" } {
      puts $aFile "\t\t\t\t<Option output=\"../../../${aWokStation}/cbp/libd/${theProjName}\" prefix_auto=\"1\" extension_auto=\"1\" />"
    } else {
      puts $aFile "\t\t\t\t<Option output=\"../../../${aWokStation}/cbp/libd/lib${theProjName}\" prefix_auto=\"1\" extension_auto=\"1\" />"
    }
    puts $aFile "\t\t\t\t<Option type=\"3\" />"
  }
  puts $aFile "\t\t\t\t<Option object_output=\"../../../${aWokStation}/cbp/objd\" />"
  if { "$aWokStation" == "wnt" } {
    puts $aFile "\t\t\t\t<Option compiler=\"msvc8\" />"
  } else {
    puts $aFile "\t\t\t\t<Option compiler=\"gcc\" />"
  }
  puts $aFile "\t\t\t\t<Option createDefFile=\"1\" />"
  puts $aFile "\t\t\t\t<Option createStaticLib=\"1\" />"

  # compiler options per TARGET (including defines)
  puts $aFile "\t\t\t\t<Compiler>"
  if { "$aWokStation" == "wnt" } {
    puts $aFile "\t\t\t\t\t<Add option=\"-MDd\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-arch:SSE2\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-EHsc\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-Od\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-Zi\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-W4\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-MP\" />"
  } else {
    puts $aFile "\t\t\t\t\t<Add option=\"-O0\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-g\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-mmmx\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-msse\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-msse2\" />"
    puts $aFile "\t\t\t\t\t<Add option=\"-mfpmath=sse\" />"
  }
  foreach aMacro $theDefines {
    puts $aFile "\t\t\t\t\t<Add option=\"-D${aMacro}\" />"
  }
  puts $aFile "\t\t\t\t\t<Add option=\"-D_DEBUG\" />"
  puts $aFile "\t\t\t\t\t<Add option=\"-DDEB\" />"
  puts $aFile "\t\t\t\t</Compiler>"

  puts $aFile "\t\t\t\t<Linker>"
  puts $aFile "\t\t\t\t\t<Add directory=\"../../../${aWokStation}/cbp/libd\" />"
  if { "$aWokStation" == "mac" && [ lsearch $theLibsList X11 ] >= 0} {
    puts $aFile "\t\t\t\t\t<Add directory=\"/usr/X11/lib\" />"
  }
  puts $aFile "\t\t\t\t\t<Add option=\"\$(CSF_OPT_LNK${aWokArch}D)\" />"
  puts $aFile "\t\t\t\t</Linker>"

  puts $aFile "\t\t\t</Target>"

  puts $aFile "\t\t</Build>"

  # COMMON compiler options
  puts $aFile "\t\t<Compiler>"
  puts $aFile "\t\t\t<Add option=\"-Wall\" />"
  puts $aFile "\t\t\t<Add option=\"-fexceptions\" />"
  puts $aFile "\t\t\t<Add option=\"-fPIC\" />"
  puts $aFile "\t\t\t<Add option=\"\$(CSF_OPT_CMPL)\" />"
  foreach anIncPath $theIncPaths {
    puts $aFile "\t\t\t<Add directory=\"$anIncPath\" />"
  }
  puts $aFile "\t\t</Compiler>"

  # COMMON linker options
  puts $aFile "\t\t<Linker>"
  foreach aFrameworkName $theFrameworks {
    if { "$aFrameworkName" != "" } {
      puts $aFile "\t\t\t<Add option=\"-framework $aFrameworkName\" />"
    }
  }
  foreach aLibName $theLibsList {
    if { "$aLibName" != "" } {
      puts $aFile "\t\t\t<Add library=\"$aLibName\" />"
    }
  }
  puts $aFile "\t\t</Linker>"

  # list of sources
  foreach aSrcFile $theSrcFiles {
    if {[string equal -nocase [file extension $aSrcFile] ".mm"]} {
      puts $aFile "\t\t<Unit filename=\"$aSrcFile\">"
      puts $aFile "\t\t\t<Option compile=\"1\" />"
      puts $aFile "\t\t\t<Option link=\"1\" />"
      puts $aFile "\t\t</Unit>"
    } elseif {[string equal -nocase [file extension $aSrcFile] ".c"]} {
      puts $aFile "\t\t<Unit filename=\"$aSrcFile\">"
      puts $aFile "\t\t\t<Option compilerVar=\"CC\" />"
      puts $aFile "\t\t</Unit>"
    } else {
      puts $aFile "\t\t<Unit filename=\"$aSrcFile\" />"
    }
  }

  puts $aFile "\t</Project>"
  puts $aFile "</CodeBlocks_project_file>"
  close $aFile

  return $aCbpFilePath
}

# Auxiliary function to achieve complete information to build Toolkit
# @param theRelativePath - relative path to CASROOT
# @param theToolKit      - Toolkit name
# @param theUsedLib      - dependencies (libraries  list)
# @param theFrameworks   - dependencies (frameworks list, Mac OS X specific)
# @param theIncPaths     - header search paths
# @param theTKDefines    - compiler macro definitions
# @param theTKSrcFiles   - list of source files
proc osutils:tkinfo { theRelativePath theToolKit theUsedLib theFrameworks theIncPaths theTKDefines theTKSrcFiles } {
  global path targetStation
  set aWokStation "$targetStation"

  # collect list of referred libraries to link with
  upvar $theUsedLib    aUsedLibs
  upvar $theFrameworks aFrameworks
  upvar $theIncPaths   anIncPaths
  upvar $theTKDefines  aTKDefines
  upvar $theTKSrcFiles aTKSrcFiles

  set aDepToolkits [wokUtils:LIST:Purge [osutils:tk:close $theToolKit]]
  foreach tkx $aDepToolkits {
    lappend aUsedLibs "${tkx}"
  }

 # wokparam -l CSF

  foreach tk [lappend aDepToolkits $theToolKit] {
    foreach element [osutils:tk:hascsf $path/src/${tk}/EXTERNLIB] {
      if {[_get_options lin csf $element] == ""} {
        continue
      }
      set isFrameworkNext 0
      foreach fl [split [_get_options lin csf $element]] {
        if {[string first "-libpath" $fl] != "-1"} {
          # this is library search path, not the library name
          continue
        } elseif {[string first "-framework" $fl] != "-1"} {
          set isFrameworkNext 1
          continue
        }

        set felem [file tail $fl]
        if {$isFrameworkNext == 1} {
          if {[lsearch $aFrameworks $felem] == "-1"} {
            lappend aFrameworks "${felem}"
          }
          set isFrameworkNext 0
        } elseif {[lsearch $aUsedLibs $felem] == "-1"} {
          if {$felem != "\{\}" & $felem != "lib"} {
            if {[lsearch -nocase [osutils:optinal_libs] $felem] == -1} {
              lappend aUsedLibs [string trimleft "${felem}" "-l"]
            }
          }
        }
      }
    }
  }

  lappend anIncPaths "$theRelativePath/inc"
  set listloc [osutils:tk:units $theToolKit]

  if { [llength $listloc] == 0 } {
    set listloc $theToolKit
  }

  if { "$aWokStation" == "wnt" } {
    set resultloc [osutils:justwnt  $listloc]
  } else {
    set resultloc [osutils:justunix $listloc]
  }
  if [array exists written] { unset written }
  foreach fxlo $resultloc {
    set xlo       $fxlo
    set aSrcFiles [osutils:tk:files $xlo osutils:compilable 0]
    foreach aSrcFile [lsort $aSrcFiles] {
      if { ![info exists written([file tail $aSrcFile])] } {
        set written([file tail $aSrcFile]) 1
        lappend aTKSrcFiles "${theRelativePath}/[wokUtils:FILES:wtail $aSrcFile 3]"
      } else {
        puts "Warning : more than one occurences for [file tail $aSrcFile]"
      }
    }

    # macros for correct DLL exports
    if { "$aWokStation" == "wnt" } {
      lappend aTKDefines "__${xlo}_DLL"
    }

    # common include paths
#    lappend anIncPaths "${theRelativePath}/src/${xlo}"
  }

  # macros for UNIX to use config.h file
  lappend aTKDefines "CSFDB"
  if { "$aWokStation" == "wnt" } {
    lappend aTKDefines "WNT"
    lappend aTKDefines "_CRT_SECURE_NO_DEPRECATE"
  } else {
    lappend aTKDefines "HAVE_WOK_CONFIG_H"
    lappend aTKDefines "HAVE_CONFIG_H"
    if { "$aWokStation" == "lin" } {
      lappend aTKDefines "LIN"
    }
    lappend aTKDefines "OCC_CONVERT_SIGNALS"
    #lappend aTKDefines "_GNU_SOURCE=1"
  }
}

# Define libraries to link using only EXTERNLIB file
proc LibToLinkX {thePackage theDummyName} {
  set aToolKits [LibToLink $thePackage]
  return $aToolKits
}

# launch generation
genproj {*}$::argv
