# =======================================================================
# Created on: 2014-07-24
# Created by: SKI
# Copyright (c) 2014 OPEN CASCADE SAS
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

# =======================================================================
# This script defines Tcl command genproj generating project files for 
# different IDEs and platforms. Run it with -help to get synopsis.
# =======================================================================

source [file join [file dirname [info script]] genconfdeps.tcl]

# the script is assumed to be run from CASROOT (or dependent Products root)
set path [file normalize .]
set THE_CASROOT ""
set fBranch ""
if { [info exists ::env(CASROOT)] } {
  set THE_CASROOT [file normalize "$::env(CASROOT)"]
}

proc _get_options { platform type branch } {
  set res ""
  if {[file exists "$::THE_CASROOT/adm/CMPLRS"]} {
    set fd [open "$::THE_CASROOT/adm/CMPLRS" rb]
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
  set UDLIST {}
  if {[file exists "$::path/adm/UDLIST"]} {
    set fd [open "$::path/adm/UDLIST" rb]
    set UDLIST [concat $UDLIST [split [read $fd] "\n"]]
    close $fd
  }
  if { "$::path/adm/UDLIST" != "$::THE_CASROOT/adm/UDLIST" && [file exists "$::THE_CASROOT/adm/UDLIST"] } {
    set fd [open "$::THE_CASROOT/adm/UDLIST" rb]
    set UDLIST [concat $UDLIST [split [read $fd] "\n"]]
    close $fd
  }

  foreach uitem $UDLIST {
    set line [split $uitem]
    if {[lindex $line 1] == "$name"} {
      return [lindex $line 0]
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

# return location of the path within src directory
proc osutils:findSrcSubPath {theSubPath} {
  if {[file exists "$::path/src/$theSubPath"]} {
    return "$::path/src/$theSubPath"
  }
  return "$::THE_CASROOT/src/$theSubPath"
}

# Auxiliary tool comparing content of two files line-by-line.
proc osutils:isEqualContent { theContent1 theContent2 } {
  set aLen1 [llength $theContent1]
  set aLen2 [llength $theContent2]
  if { $aLen1 != $aLen2 } {
    return false
  }

  for {set aLineIter 0} {$aLineIter < $aLen1} {incr aLineIter} {
    set aLine1 [lindex $theContent1 $aLineIter]
    set aLine2 [lindex $theContent2 $aLineIter]
    if { $aLine1 != $aLine2 } {
      return false
    }
  }
  return true
}

# Auxiliary function for writing new file content only if it has been actually changed
# (e.g. to preserve file timestamp on no change).
# Useful for automatically (re)generated files.
proc osutils:writeTextFile { theFile theContent {theEol lf} } {
  if {[file exists "${theFile}"]} {
    set aFileOld [open "${theFile}" rb]
    fconfigure $aFileOld -translation crlf
    set aLineListOld [split [read $aFileOld] "\n"]
    close $aFileOld

    # append empty line for proper comparison (which will be implicitly added by last puts below)
    set aContent $theContent
    lappend aContent ""
    if { [osutils:isEqualContent $aLineListOld $aContent] == true } {
      return false
    }

    file delete -force "${theFile}"
  }

  set anOutFile [open "$theFile" "w"]
  fconfigure $anOutFile -translation $theEol
  foreach aLine ${theContent} {
    puts $anOutFile "${aLine}"
  }
  close $anOutFile
  return true
}

# Function re-generating header files for specified text resource
proc genResources { theResource } {
  global path

  set aResFileList {}
  set aResourceAbsPath [file normalize "${path}/src/${theResource}"]
  set aResourceDirectory ""
  set isResDirectory false

  if {[file isdirectory "${aResourceAbsPath}"]} {
    if {[file exists "${aResourceAbsPath}/FILES"]} {
      set aFilesFile [open "${aResourceAbsPath}/FILES" rb]
      set aResFileList [split [read $aFilesFile] "\n"]
      close $aFilesFile
    }
    set aResFileList [lsearch -inline -all -not -exact $aResFileList ""]
    set aResourceDirectory "${theResource}"
    set isResDirectory true
  } else {
    set aResourceName [file tail "${theResource}"]
    lappend aResFileList "res:::${aResourceName}"
    set aResourceDirectory [file dirname "${theResource}"]
  }

  foreach aResFileIter ${aResFileList} {
    if {![regexp {^[^:]+:::(.+)} "${aResFileIter}" dump aResFileIter]} {
	  continue
	}

    set aResFileName [file tail "${aResFileIter}"]
    regsub -all {\.} "${aResFileName}" {_} aResFileName
    set aHeaderFileName "${aResourceDirectory}_${aResFileName}.pxx"
    if { $isResDirectory == true && [lsearch $aResFileList $aHeaderFileName] == -1 } {
      continue
    }

    # generate
    set aContent {}
    lappend aContent "// This file has been automatically generated from resource file src/${aResourceDirectory}/${aResFileIter}"
	lappend aContent ""

    # generate necessary structures
    set aLineList {}
    if {[file exists "${path}/src/${aResourceDirectory}/${aResFileIter}"]} {
      set anInputFile [open "${path}/src/${aResourceDirectory}/${aResFileIter}" rb]
      fconfigure $anInputFile -translation crlf
      set aLineList [split [read $anInputFile] "\n"]
      close $anInputFile
    }

    # drop empty trailing line
    set anEndOfFile ""
    if { [lindex $aLineList end] == "" } {
      set aLineList [lreplace $aLineList end end]
      set anEndOfFile "\\n"
    }

    lappend aContent "static const char ${aResourceDirectory}_${aResFileName}\[\] ="
    set aNbLines  [llength $aLineList]
    set aLastLine [expr $aNbLines - 1]
    for {set aLineIter 0} {$aLineIter < $aNbLines} {incr aLineIter} {
      set aLine [lindex $aLineList $aLineIter]
      regsub -all {\"} "${aLine}" {\\"} aLine
      if { $aLineIter == $aLastLine } {
        lappend aContent "  \"${aLine}${anEndOfFile}\";"
      } else {
        lappend aContent "  \"${aLine}\\n\""
      }
    }

    # Save generated content to header file
    set aHeaderFilePath "${path}/src/${aResourceDirectory}/${aHeaderFileName}"
    if { [osutils:writeTextFile $aHeaderFilePath $aContent] == true } {
      puts "Generating header file from resource file: ${path}/src/${aResourceDirectory}/${aResFileIter}"
    } else {
	  #puts "Header file from resource ${path}/src/${aResourceDirectory}/${aResFileIter} is up-to-date"
    }
  }
}

# Function re-generating header files for all text resources
proc genAllResources {} {
  global path
  set aCasRoot [file normalize $path]
  if {![file exists "$aCasRoot/adm/RESOURCES"]} {
    puts "OCCT directory is not defined correctly: $aCasRoot"
    return
  }

  set aFileResources [open "$aCasRoot/adm/RESOURCES" rb]
  set anAdmResources [split [read $aFileResources] "\r\n"]
  close $aFileResources
  set anAdmResources [lsearch -inline -all -not -exact $anAdmResources ""]

  foreach line $anAdmResources {
    genResources "${line}"
  }
}

# Wrapper-function to generate VS project files
proc genproj {theFormat args} {
  set aSupportedFormats { "vc7" "vc8" "vc9" "vc10" "vc11" "vc12" "vc14" "vc141" "cbp" "xcd"}
  set aSupportedPlatforms { "wnt" "uwp" "lin" "mac" "ios" "qnx" }
  set isHelpRequire false

  # check format argument
  if { $theFormat == "-h" || $theFormat == "-help" || $theFormat == "--help" } {
    set isHelpRequire true
  } elseif { [lsearch -exact $aSupportedFormats $theFormat] < 0 } {
    puts "Error: genproj: unrecognized project format \"$theFormat\""
    set isHelpRequire true
  }

  # choice of compiler for Code::Blocks, currently hard-coded
  set aCmpl "gcc"

  # Determine default platform: wnt for vc*, mac for xcd, current for cbp
  if { [regexp "^vc" $theFormat] } {
    set aPlatform "wnt"
  } elseif { $theFormat == "xcd" || $::tcl_platform(os) == "Darwin" } {
    set aPlatform "mac"
  } elseif { $::tcl_platform(platform) == "windows" } {
    set aPlatform "wnt"
  } elseif { $::tcl_platform(platform) == "unix" } {
    set aPlatform "lin"
  }

  # Check optional arguments
  set aLibType "dynamic"
  foreach arg $args {
    if { $arg == "-h" || $arg == "-help" || $arg == "--help" } {
      set isHelpRequire true
    } elseif { [lsearch -exact $aSupportedPlatforms $arg] >= 0 } {
      set aPlatform $arg
    } elseif { $arg == "-static" } {
      set aLibType "static"
      puts "Static build has been selected"
    } elseif { $arg == "-dynamic" } {
      set aLibType "dynamic"
      puts "Dynamic build has been selected"
    } else {
      puts "Error: genproj: unrecognized option \"$arg\""
      set isHelpRequire true
    }
  }

  if {  $isHelpRequire == true } {
    puts "usage: genproj Format \[Platform\] \[-static\] \[-h|-help|--help\]

    Format must be one of:
      vc8      -  Visual Studio 2005
      vc9      -  Visual Studio 2008
      vc10     -  Visual Studio 2010
      vc11     -  Visual Studio 2012
      vc12     -  Visual Studio 2013
      vc14     -  Visual Studio 2015
      vc141    -  Visual Studio 2017
      cbp      -  CodeBlocks
      xcd      -  XCode

    Platform (optional):
      wnt   -  Windows Desktop
      uwp   -  Universal Windows Platform
      lin   -  Linux
      mac   -  OS X
      ios   -  iOS
      qnx   -  QNX

    Option -static can be used with XCode to build static libraries
    "
    return
  }

  if { ! [info exists aPlatform] } {
    puts "Error: genproj: Cannon identify default platform, please specify!"
    return
  }

  puts "Preparing to generate $theFormat projects for $aPlatform platform..."

  # base path to where to generate projects, hardcoded from current dir
  set anAdmPath [file normalize "${::path}/adm"]

  OS:MKPRC "$anAdmPath" "$theFormat" "$aLibType" "$aPlatform" "$aCmpl"

  genprojbat "$theFormat" "$aPlatform"
  genAllResources
}

# copy file providing warning if the target file exists and has 
# different date or size; if it is newer than source, save it as .bak
proc copy_with_warning {from to} {
  if { [file exists "$to"] &&
      ([file size   "$to"] != [file size  "$from"] ||
       [file mtime  "$to"] != [file mtime "$from"]) } {
    puts "Warning: file $to is updated (copied from $from)!"
    if { [file mtime $to] > [file mtime $from] } {
      puts "Info: old content of file $to is saved in ${to}.bak"
      file copy -force -- "$to" "${to}.bak"
    }
  }

  file copy -force -- "$from" "$to"
}

proc genprojbat {theFormat thePlatform} {
  set aTargetPlatformExt sh
  if { $thePlatform == "wnt" || $thePlatform == "uwp" } {
    set aTargetPlatformExt bat
  }

  if {"$theFormat" != "cmake"} {
    # copy env.bat/sh only if not yet present
    if { ! [file exists "$::path/env.${aTargetPlatformExt}"] } {
      set anEnvTmplFile [open "$::THE_CASROOT/adm/templates/env.${aTargetPlatformExt}" "r"]
      set anEnvTmpl [read $anEnvTmplFile]
      close $anEnvTmplFile

      set aCasRoot ""
      if { [file normalize "$::path"] != [file normalize "$::THE_CASROOT"] } {
        set aCasRoot [relativePath "$::path" "$::THE_CASROOT"]
      }

      regsub -all -- {__CASROOT__}   $anEnvTmpl "$aCasRoot" anEnvTmpl

      set anEnvFile [open "$::path/env.${aTargetPlatformExt}" "w"]
      puts $anEnvFile $anEnvTmpl
      close $anEnvFile
    }

    copy_with_warning "$::THE_CASROOT/adm/templates/draw.${aTargetPlatformExt}" "$::path/draw.${aTargetPlatformExt}"
  }

  if { [regexp {^vc} $theFormat] } {
    copy_with_warning "$::THE_CASROOT/adm/templates/msvc.bat" "$::path/msvc.bat"
  } else {
    switch -exact -- "$theFormat" {
      "cbp"   {
        file copy -force -- "$::THE_CASROOT/adm/templates/codeblocks.sh"  "$::path/codeblocks.sh"
        file copy -force -- "$::THE_CASROOT/adm/templates/codeblocks.bat" "$::path/codeblocks.bat"
        # Code::Blocks 16.01 does not create directory for import libs, help him
        file mkdir "$::path/$thePlatform/cbp/lib"
        file mkdir "$::path/$thePlatform/cbp/libd"
      }
      "xcd"   { file copy -force -- "$::THE_CASROOT/adm/templates/xcode.sh"      "$::path/xcode.sh" }
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

set aTKNullKey "TKNull"
set THE_GUIDS_LIST($aTKNullKey) "{00000000-0000-0000-0000-000000000000}"

# Entry function to generate project files
# @param theOutDir   Root directory for project files
# @param theFormat   Project format name (vc.. for Visual Studio projects, cbp for Code::Blocks, xcd for XCode)
# @param theLibType  Library type - dynamic or static
# @param thePlatform Optional target platform for cross-compiling, e.g. ios for iOS
# @param theCmpl     Compiler option (msvc or gcc)
proc OS:MKPRC { theOutDir theFormat theLibType thePlatform theCmpl } {
  global path
  set anOutRoot $theOutDir
  if { $anOutRoot == "" } {
    error "Error : \"theOutDir\" is not initialized"
  }

  # Create output directory
  set aWokStation "$thePlatform"
  if { [regexp {^vc} $theFormat] } {
    set aWokStation "msvc"
  }
  set aSuffix ""
  set isUWP 0
  if { $thePlatform == "uwp" } {
    set aSuffix "-uwp"
    set isUWP 1
  }
  set anOutDir "${anOutRoot}/${aWokStation}/${theFormat}${aSuffix}"

  # read map of already generated GUIDs
  set aGuidsFilePath [file join $anOutDir "wok_${theFormat}_guids.txt"]
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
  if { "$thePlatform" == "ios" } {
    set goaway [list Draw]
    set aModules [osutils:juststation $goaway $aModules]
  }

  # Draw module is turned off due to it is not supported on UWP
  if { $isUWP } {
    set aDrawIndex [lsearch -exact ${aModules} "Draw"]
    if { ${aDrawIndex} != -1 } {
      set aModules [lreplace ${aModules} ${aDrawIndex} ${aDrawIndex}]
    }
  }

  # generate one solution for all projects if complete OS or VAS is processed
  set anAllSolution "OCCT"

  wokUtils:FILES:mkdir $anOutDir
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
  osutils:collectinc $aModules $path/inc

  # Generating project files for the selected format
  switch -exact -- "$theFormat" {
    "vc7"   -
    "vc8"   -
    "vc9"   -
    "vc10"  -
    "vc11"  -
    "vc12"  -
    "vc14"  -
    "vc141"    { OS:MKVC  $anOutDir $aModules $anAllSolution $theFormat $isUWP}
    "cbp"      { OS:MKCBP $anOutDir $aModules $anAllSolution $thePlatform $theCmpl }
    "xcd"      {
      set ::THE_GUIDS_LIST($::aTKNullKey) "000000000000000000000000"
      OS:MKXCD $anOutDir $aModules $anAllSolution $theLibType $thePlatform
    }
  }

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
proc OS:MKVC { theOutDir theModules theAllSolution theVcVer isUWP } {

  puts stderr "Generating VS project files for $theVcVer"

  # generate projects for toolkits and separate solution for each module
  foreach aModule $theModules {
    OS:vcsolution $theVcVer $aModule $aModule $theOutDir ::THE_GUIDS_LIST
    OS:vcproj     $theVcVer $isUWP   $aModule $theOutDir ::THE_GUIDS_LIST
  }

  # generate single solution "OCCT" containing projects from all modules
  if { "$theAllSolution" != "" } {
    OS:vcsolution $theVcVer $theAllSolution $theModules $theOutDir ::THE_GUIDS_LIST
  }

  puts "The Visual Studio solution and project files are stored in the $theOutDir directory"
}

proc OS:init {{os {}}} {
  set askplat $os
  set aModules {}
  if { "$os" == "" } {
    set os $::tcl_platform(os)
  }

  if [file exists "$::path/src/VAS/Products.tcl"] {
    source "$::path/src/VAS/Products.tcl"
    foreach aModuleIter [VAS:Products] {
      set aFileTcl "$::path/src/VAS/${aModuleIter}.tcl"
      if [file exists $aFileTcl] {
        source $aFileTcl
        lappend aModules $aModuleIter
      } else {
        puts stderr "Definition file for module $aModuleIter is not found in unit VAS"
      }
    }
    return $aModules
  }

  # Load list of OCCT modules and their definitions
  source "$::path/src/OS/Modules.tcl"
  foreach aModuleIter [OS:Modules] {
    set aFileTcl "$::path/src/OS/${aModuleIter}.tcl"
    if [file exists $aFileTcl] {
      source $aFileTcl
      lappend aModules $aModuleIter
    } else {
      puts stderr "Definition file for module $aModuleIter is not found in unit OS"
    }
  }

  return $aModules
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
#    puts "osutils:tk:close($ltk) ids='$ids'"
    set eated [osutils:tk:eatpk $ids]
    set result [concat $result $eated]
    set ids [LibToLink $dir]
    set result [concat $result $ids]

    foreach file $eated {
      set kds [osutils:findSrcSubPath "$file/EXTERNLIB"]
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
  regexp {^.*:([^:]+)$} $theTKit dummy theTKit
  set type [_get_type $theTKit]
  if {$type != "t" && $type != "x"} {
    return
  }
  set aToolkits {}
  set anExtLibList [osutils:tk:eatpk [osutils:findSrcSubPath "$theTKit/EXTERNLIB"]]
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
  set theNamePath [osutils:findSrcSubPath "$theName"]
  if {[file isdirectory $theNamePath]} {
    return $theNamePath
  }
  return ""
}

proc OS:genGUID { {theFormat "vc"} } {
  if { "$theFormat" == "vc" } {
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
proc osutils:collectinc {theModules theIncPath} {
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
  set anUsedToolKits [lsort -unique $anUsedToolKits]

  set anUnits {}
  foreach anUsedToolKit $anUsedToolKits {
    set anUnits [concat $anUnits [osutils:tk:units $anUsedToolKit]]
  }
  set anUnits [lsort -unique $anUnits]

  # define copying style
  set aCopyType "copy"
  if { [info exists ::env(SHORTCUT_HEADERS)] } {
    if { [string equal -nocase $::env(SHORTCUT_HEADERS) "hard"]
      || [string equal -nocase $::env(SHORTCUT_HEADERS) "hardlink"] } {
      set aCopyType "hardlink"
    } elseif { [string equal -nocase $::env(SHORTCUT_HEADERS) "true"]
            || [string equal -nocase $::env(SHORTCUT_HEADERS) "shortcut"] } {
      set aCopyType "shortcut"
    }
  }

  set allHeaderFiles {}
  if { $aCopyType == "shortcut" } {
    # template preparation
    if { ![file exists $::THE_CASROOT/adm/templates/header.in] } {
      puts "template file does not exist: $::THE_CASROOT/adm/templates/header.in"
      return
    }
    set aHeaderTmpl [wokUtils:FILES:FileToString $::THE_CASROOT/adm/templates/header.in]

    # relative anIncPath in connection with aCasRoot/src
    set aFromBuildIncToSrcPath [relativePath "$anIncPath" "$aCasRoot/src"]

    # create and copy short-cut header files
    foreach anUnit $anUnits {
      osutils:checksrcfiles ${anUnit}

      set aHFiles [_get_used_files ${anUnit} true false]
      foreach aHeaderFile ${aHFiles} {
        set aHeaderFileName [lindex ${aHeaderFile} 1]
        lappend allHeaderFiles "${aHeaderFileName}"

        regsub -all -- {@OCCT_HEADER_FILE_CONTENT@} $aHeaderTmpl "#include \"$aFromBuildIncToSrcPath/$anUnit/$aHeaderFileName\"" aShortCutHeaderFileContent

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
          file delete -force "$theIncPath/$aHeaderFileName"
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
      osutils:checksrcfiles ${anUnit}

      set aHFiles [_get_used_files ${anUnit} true false]
      foreach aHeaderFile ${aHFiles} {
        set aHeaderFileName [lindex ${aHeaderFile} 1]
        lappend allHeaderFiles "${aHeaderFileName}"

        # copy file only if target does not exist or is older than original
        set torig [file mtime $aCasRoot/src/$anUnit/$aHeaderFileName]
        set tcopy 0
        if { [file isfile $anIncPath/$aHeaderFileName] } {
          set tcopy [file mtime $anIncPath/$aHeaderFileName]
        }
        if { $tcopy < $torig } {
          incr nbcopied
          if { $aCopyType == "hardlink" } {
            if { $tcopy != 0 } {
              file delete -force "$theIncPath/$aHeaderFileName"
            }
            file link -hard  $anIncPath/$aHeaderFileName $aCasRoot/src/$anUnit/$aHeaderFileName
          } else {
            file copy -force $aCasRoot/src/$anUnit/$aHeaderFileName $anIncPath/$aHeaderFileName
          }
        } elseif { $tcopy != $torig } {
          puts "Warning: file $anIncPath/$aHeaderFileName is newer than $aCasRoot/src/$anUnit/$aHeaderFileName, not changed!"
        }
      }
    }
    puts "Info: $nbcopied files updated"
  }

  # remove header files not listed in FILES
  set anIncFiles [glob -tails -nocomplain -dir ${anIncPath} "*"]
  foreach anIncFile ${anIncFiles} {
    if { [lsearch -exact ${allHeaderFiles} ${anIncFile}] == -1 } {
      puts "Warning: file ${anIncPath}/${anIncFile} is not presented in the sources and will be removed from ${theIncPath}!"
      file delete -force "${theIncPath}/${anIncFile}"
    }
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
      "Microsoft Visual Studio Solution File, Format Version 12.00\n" \
      "# Visual Studio 2013\n"
  } elseif { "$vcversion" == "vc14"  || "$vcversion" == "vc141"} {
    append var \
      "Microsoft Visual Studio Solution File, Format Version 12.00\n" \
      "# Visual Studio 14\n"
  } else {
    puts stderr "Error: Visual Studio version $vcversion is not supported by this function!"
  }
  return $var
}
# Returns extension (without dot) for project files of given version of VC

proc osutils:vcproj:ext { vcversion } {
  if { "$vcversion" == "vc7" || "$vcversion" == "vc8" || "$vcversion" == "vc9" } {
    return "vcproj"
  } else {
    return "vcxproj"
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
  } else {
    append var \
      "Global\n" \
      "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n" \
      "\t\tDebug|Win32 = Debug|Win32\n" \
      "\t\tRelease|Win32 = Release|Win32\n" \
      "\t\tDebug|x64 = Debug|x64\n" \
      "\t\tRelease|x64 = Release|x64\n" \
      "\tEndGlobalSection\n" \
      "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n"
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
  } else {
    append var \
      "\t\t$guid.Debug|Win32.ActiveCfg = Debug|Win32\n" \
      "\t\t$guid.Debug|Win32.Build.0 = Debug|Win32\n" \
      "\t\t$guid.Release|Win32.ActiveCfg = Release|Win32\n" \
      "\t\t$guid.Release|Win32.Build.0 = Release|Win32\n" \
      "\t\t$guid.Debug|x64.ActiveCfg = Debug|x64\n" \
      "\t\t$guid.Debug|x64.Build.0 = Debug|x64\n" \
      "\t\t$guid.Release|x64.ActiveCfg = Release|x64\n" \
      "\t\t$guid.Release|x64.Build.0 = Release|x64\n"
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
  } else {
    append var \
      "\tEndGlobalSection\n" \
      "\tGlobalSection(SolutionProperties) = preSolution\n" \
      "\t\tHideSolutionNode = FALSE\n" \
      "\tEndGlobalSection\n"
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

proc OS:vcproj { theVcVer isUWP theModules theOutDir theGuidsMap } {
  upvar $theGuidsMap aGuidsMap

  set aProjectFiles {}

  foreach aModule $theModules {
    foreach aToolKit [${aModule}:toolkits] {
      lappend aProjectFiles [osutils:vcproj  $theVcVer $isUWP $theOutDir $aToolKit     aGuidsMap]
    }
    foreach anExecutable [OS:executable ${aModule}] {
      lappend aProjectFiles [osutils:vcprojx $theVcVer $isUWP $theOutDir $anExecutable aGuidsMap]
    }
  }
  return $aProjectFiles
}
# generate template name and load it for given version of Visual Studio and platform

proc osutils:vcproj:readtemplate {theVcVer isUWP isExec} {
  set anExt $theVcVer
  if { "$theVcVer" != "vc7" && "$theVcVer" != "vc8" && "$theVcVer" != "vc9" } {
    set anExt vc10
  }

  # determine versions of runtime and toolset
  set aVCRTVer $theVcVer 
  set aToolset "v[string range $theVcVer 2 3]0"
  if { $theVcVer == "vc141" } {
    set aVCRTVer "vc14"
    set aToolset "v141"
  }

  set what "$theVcVer"
  set aCmpl32 ""
  set aCmpl64 ""
  set aCharSet "Unicode"
  if { $isExec } {
    set anExt "${anExt}x"
    set what "$what executable"
  }
  if { "$theVcVer" == "vc10" } {
    # SSE2 is enabled by default in vc11+, but not in vc10 for 32-bit target
    set aCmpl32 "<EnableEnhancedInstructionSet>StreamingSIMDExtensions2</EnableEnhancedInstructionSet>"
  }
  set aTmpl [osutils:readtemplate $anExt "MS VC++ project ($what)"]

  if { $isUWP } {
    set UwpWinRt "<CompileAsWinRT>false</CompileAsWinRT>"
    foreach bitness {32 64} {
      set indent ""
      if {"[set aCmpl${bitness}]" != ""} {
        set indent "\n      "
      }
      set aCmpl${bitness} "[set aCmpl${bitness}]${indent}${UwpWinRt}"
    }
  }

  set format_template "\[\\r\\n\\s\]*"
  foreach bitness {32 64} {
    set format_templateloc ""
    if {"[set aCmpl${bitness}]" == ""} {
      set format_templateloc "$format_template"
    }
    regsub -all -- "${format_templateloc}__VCMPL${bitness}__" $aTmpl "[set aCmpl${bitness}]" aTmpl
  }

  set aDebugInfo "no"
  set aReleaseLnk ""
  if { "$::HAVE_RelWithDebInfo" == "true" } {
    set aDebugInfo "true"
    set aReleaseLnk "\n      <OptimizeReferences>true</OptimizeReferences>\n      <EnableCOMDATFolding>true</EnableCOMDATFolding>"
  }

  regsub -all -- {__VCVER__}     $aTmpl $aVCRTVer aTmpl
  regsub -all -- {__VCVEREXT__}  $aTmpl $aToolset aTmpl
  regsub -all -- {__VCCHARSET__} $aTmpl $aCharSet aTmpl
  regsub -all -- {__VCReleasePDB__} $aTmpl $aDebugInfo aTmpl
  regsub -all -- "${format_template}__VCLNKREL__" $aTmpl "${aReleaseLnk}" aTmpl

  return $aTmpl
}

proc osutils:readtemplate {ext what} {
  set loc "$::THE_CASROOT/adm/templates/template.$ext"
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
proc osutils:compilable {thePlatform} {
  if { "$thePlatform" == "mac" || "$thePlatform" == "ios" } {
    return [list .c .cxx .cpp .mm]
  }
  return [list .c .cxx .cpp]
}

proc osutils:commonUsedTK { theToolKit } {
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
proc osutils:tk:csfInExternlib { EXTERNLIB } {
  set l [wokUtils:FILES:FileToList $EXTERNLIB]
  set lret  {STLPort}
  foreach str $l {
    if [regexp -- {(CSF_[^ ]*)} $str csf] {
      lappend lret $csf
    }
  }
  return $lret
}

# Collect dependencies map depending on target OS (libraries for CSF_ codenames used in EXTERNLIB) .
# @param theOS         - target OS
# @param theCsfLibsMap - libraries  map
# @param theCsfFrmsMap - frameworks map, OS X specific
proc osutils:csfList { theOS theCsfLibsMap theCsfFrmsMap } {
  upvar $theCsfLibsMap aLibsMap
  upvar $theCsfFrmsMap aFrmsMap

  unset theCsfLibsMap
  unset theCsfFrmsMap

  set aLibsMap(CSF_FREETYPE)  "freetype"
  set aLibsMap(CSF_TclLibs)   "tcl8.6"
  set aLibsMap(CSF_TclTkLibs) "tk8.6"
  if { "$::HAVE_FREEIMAGE" == "true" } {
    if { "$theOS" == "wnt" } {
      set aLibsMap(CSF_FreeImagePlus) "FreeImage"
    } else {
      set aLibsMap(CSF_FreeImagePlus) "freeimage"
    }
  } elseif { "$theOS" == "wnt" } {
    set aLibsMap(CSF_FreeImagePlus) "windowscodecs"
  }
  if { "$::HAVE_FFMPEG" == "true" } {
    set aLibsMap(CSF_FFmpeg) "avcodec avformat swscale avutil"
  }
  if { "$::HAVE_GL2PS" == "true" } {
    set aLibsMap(CSF_GL2PS) "gl2ps"
  }
  if { "$::HAVE_TBB" == "true" } {
    set aLibsMap(CSF_TBB) "tbb tbbmalloc"
  }
  if { "$::HAVE_VTK" == "true" } {
    if { "$theOS" == "wnt" } {
      set aLibsMap(CSF_VTK) [osutils:vtkCsf "wnt"]
    } else {
      set aLibsMap(CSF_VTK) [osutils:vtkCsf "unix"]
    }
  }
  if { "$::HAVE_ZLIB" == "true" } {
    set aLibsMap(CSF_ZLIB) "zlib"
  }
  if { "$::HAVE_LIBLZMA" == "true" } {
    set aLibsMap(CSF_LIBLZMA) "liblzma"
  }

  if { "$theOS" == "wnt" } {
    #  WinAPI libraries
    set aLibsMap(CSF_kernel32)     "kernel32"
    set aLibsMap(CSF_advapi32)     "advapi32"
    set aLibsMap(CSF_gdi32)        "gdi32"
    set aLibsMap(CSF_user32)       "user32 comdlg32"
    set aLibsMap(CSF_opengl32)     "opengl32"
    set aLibsMap(CSF_wsock32)      "wsock32"
    set aLibsMap(CSF_netapi32)     "netapi32"
    set aLibsMap(CSF_OpenGlLibs)   "opengl32"
    if { "$::HAVE_GLES2" == "true" } {
      set aLibsMap(CSF_OpenGlLibs) "libEGL libGLESv2"
    }
    set aLibsMap(CSF_psapi)        "Psapi"
    set aLibsMap(CSF_d3d9)         "d3d9"

    # the naming is different on Windows
    set aLibsMap(CSF_TclLibs)      "tcl86"
    set aLibsMap(CSF_TclTkLibs)    "tk86"

    set aLibsMap(CSF_QT)           "QtCore4 QtGui4"

    # tbb headers define different pragma lib depending on debug/release
    set aLibsMap(CSF_TBB) ""
  } else {
    set aLibsMap(CSF_dl)           "dl"
    if { "$theOS" == "mac" } {
      set aLibsMap(CSF_objc)       "objc"
      set aFrmsMap(CSF_Appkit)     "Appkit"
      set aFrmsMap(CSF_IOKit)      "IOKit"
      set aFrmsMap(CSF_OpenGlLibs) "OpenGL"
      set aFrmsMap(CSF_TclLibs)    "Tcl"
      set aLibsMap(CSF_TclLibs)    ""
      set aFrmsMap(CSF_TclTkLibs)  "Tk"
      set aLibsMap(CSF_TclTkLibs)  ""
      set aLibsMap(CSF_QT)         "QtCore QtGui"
    } else {
      if { "$theOS" == "qnx" } {
        # CSF_ThreadLibs - pthread API is part of libc on QNX
        set aLibsMap(CSF_OpenGlLibs) "EGL GLESv2"
      } else {
        set aLibsMap(CSF_ThreadLibs) "pthread rt"
        set aLibsMap(CSF_OpenGlLibs) "GL"
        set aLibsMap(CSF_TclTkLibs)  "X11 tk8.6"
        set aLibsMap(CSF_XwLibs)     "X11 Xext Xmu Xi"
        set aLibsMap(CSF_MotifLibs)  "X11"
      }

      if { "$::HAVE_GLES2" == "true" } {
        set aLibsMap(CSF_OpenGlLibs) "EGL GLESv2"
      }
    }
  }
}

# Returns string of library dependencies for generation of Visual Studio project or make lists.
proc osutils:vtkCsf {{theOS ""}} {
  set aVtkVer "6.1"

  set aPathSplitter ":"
  if {"$theOS" == "wnt"} {
    set aPathSplitter ";"
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
    lset aLibArray $anIdx $anItem-$aVtkVer
    incr anIdx
  }

  return [join $aLibArray " "]
}

# @param theLibsList   - dependencies (libraries  list)
# @param theFrameworks - dependencies (frameworks list, OS X specific)
proc osutils:usedOsLibs { theToolKit theOS theLibsList theFrameworks } {
  global path
  upvar $theLibsList   aLibsList
  upvar $theFrameworks aFrameworks
  set aLibsList   [list]
  set aFrameworks [list]

  osutils:csfList $theOS aLibsMap aFrmsMap

  foreach aCsfElem [osutils:tk:csfInExternlib "$path/src/${theToolKit}/EXTERNLIB"] {
    if [info exists aLibsMap($aCsfElem)] {
      foreach aLib [split "$aLibsMap($aCsfElem)"] {
        if { [lsearch $aLibsList $aLib] == "-1" } {
          lappend aLibsList $aLib
        }
      }
    }
    if [info exists aFrmsMap($aCsfElem)] {
      foreach aFrm [split "$aFrmsMap($aCsfElem)"] {
        if { [lsearch $aFrameworks $aFrm] == "-1" } {
          lappend aFrameworks $aFrm
        }
      }
    }
  }
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
proc osutils:vcxproj:file { file params } {
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
  set aLoc "$::THE_CASROOT/adm/templates/template_dll.rc"
  set aBody [wokUtils:FILES:FileToString $aLoc]
  regsub -all -- {__TKNAM__} $aBody $theToolKit aBody

  set aFile [open "${theOutDir}/${theToolKit}.rc" "w"]
  fconfigure $aFile -translation lf
  puts $aFile $aBody
  close $aFile
  return "${theOutDir}/${theToolKit}.rc"
}

# Generate Visual Studio project file for ToolKit
proc osutils:vcproj { theVcVer isUWP theOutDir theToolKit theGuidsMap } {
  set theProjTmpl [osutils:vcproj:readtemplate $theVcVer $isUWP 0]

  set l_compilable [osutils:compilable wnt]
  regsub -all -- {__TKNAM__} $theProjTmpl $theToolKit theProjTmpl

  upvar $theGuidsMap aGuidsMap
  if { ! [info exists aGuidsMap($theToolKit)] } {
    set aGuidsMap($theToolKit) [OS:genGUID]
  }
  regsub -all -- {__PROJECT_GUID__} $theProjTmpl $aGuidsMap($theToolKit) theProjTmpl

  set theProjTmpl [osutils:uwp:proj $isUWP ${theProjTmpl}]

  set aUsedLibs [list]

  if { $isUWP } {
    lappend aUsedLibs "WindowsApp.lib"
  }

  foreach tkx [osutils:commonUsedTK  $theToolKit] {
    lappend aUsedLibs "${tkx}.lib"
  }

  osutils:usedOsLibs $theToolKit "wnt" aLibs aFrameworks
  foreach aLibIter $aLibs {
    lappend aUsedLibs "${aLibIter}.lib"
  }

  # correct names of referred third-party libraries that are named with suffix
  # depending on VC version
  set aVCRTVer [string range $theVcVer 0 3]
  regsub -all -- {vc[0-9]+} $aUsedLibs $aVCRTVer aUsedLibs

  # and put this list to project file
  #puts "$theToolKit requires  $aUsedLibs"
  if { "$theVcVer" != "vc7" && "$theVcVer" != "vc8" && "$theVcVer" != "vc9" } {
    set aUsedLibs [join $aUsedLibs {;}]
  }
  regsub -all -- {__TKDEP__} $theProjTmpl $aUsedLibs theProjTmpl

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
    set aSrcFiles [osutils:tk:files $xlo wnt]
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

    # Format of projects in vc10+ is different from vc7-9
    if { "$theVcVer" != "vc7" && "$theVcVer" != "vc8" && "$theVcVer" != "vc9" } {
      foreach aSrcFile [lsort $aSrcFiles] {
        if { ![info exists written([file tail $aSrcFile])] } {
          set written([file tail $aSrcFile]) 1
          append aFilesSection [osutils:vcxproj:file $aSrcFile $needparam]
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

  # write filters file for vc10+
  if { "$theVcVer" == "vc7" || "$theVcVer" == "vc8" || "$theVcVer" == "vc9" } {
    # nothing
  } elseif { "$theVcVer" == "vc10" } {
    lappend aVcFiles [osutils:vcxproj:filters $theOutDir $theToolKit aVcFilesX]
  } else {
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
# Tfiles lists for each unit the type of file that can be compiled.
proc osutils:tk:files { tkloc thePlatform } {
  set Tfiles(source,nocdlpack)     {source pubinclude}
  set Tfiles(source,toolkit)       {}
  set Tfiles(source,executable)    {source pubinclude}
  set listloc [concat [osutils:tk:units $tkloc] $tkloc]
  #puts " listloc = $listloc"

  set l_comp [osutils:compilable $thePlatform]
  set resultloc $listloc
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
          lappend lret $f
        }
      }
    }
  }
  return $lret
}

# Generate Visual Studio project file for executable
proc osutils:vcprojx { theVcVer isUWP theOutDir theToolKit theGuidsMap } {
  set aVcFiles {}
  foreach f [osutils:tk:files $theToolKit wnt] {
    set aProjTmpl [osutils:vcproj:readtemplate $theVcVer $isUWP 1]

    set aProjName [file rootname [file tail $f]]
    set l_compilable [osutils:compilable wnt]
    regsub -all -- {__XQTNAM__} $aProjTmpl $aProjName aProjTmpl

    upvar $theGuidsMap aGuidsMap
    if { ! [info exists aGuidsMap($aProjName)] } {
      set aGuidsMap($aProjName) [OS:genGUID]
    }
    regsub -all -- {__PROJECT_GUID__} $aProjTmpl $aGuidsMap($aProjName) aProjTmpl

    set aUsedLibs [list]
    foreach tkx [osutils:commonUsedTK  $theToolKit] {
      lappend aUsedLibs "${tkx}.lib"
    }

    osutils:usedOsLibs $theToolKit "wnt" aLibs aFrameworks
    foreach aLibIter $aLibs {
      lappend aUsedLibs "${aLibIter}.lib"
    }

    # correct names of referred third-party libraries that are named with suffix
    # depending on VC version
    set aVCRTVer [string range $theVcVer 0 3]
    regsub -all -- {vc[0-9]+} $aUsedLibs $aVCRTVer aUsedLibs

#    puts "$aProjName requires  $aUsedLibs"
    if { "$theVcVer" != "vc7" && "$theVcVer" != "vc8" && "$theVcVer" != "vc9" } {
      set aUsedLibs [join $aUsedLibs {;}]
    }
    regsub -all -- {__TKDEP__} $aProjTmpl $aUsedLibs aProjTmpl

    set aFilesSection ""
    set aVcFilesX(units) ""

    if { ![info exists written([file tail $f])] } {
      set written([file tail $f]) 1

      if { "$theVcVer" != "vc7" && "$theVcVer" != "vc8" && "$theVcVer" != "vc9" } {
        append aFilesSection [osutils:vcxproj:file $f ""]
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
    if { "$theVcVer" != "vc7" && "$theVcVer" != "vc8" && "$theVcVer" != "vc9" } {
      lappend aVcFiles [osutils:vcxproj:filters $theOutDir $aProjName aVcFilesX]
    }

    set aCommonSettingsFileTmpl ""
    if { "$theVcVer" == "vc7" || "$theVcVer" == "vc8" } {
      # nothing
    } elseif { "$theVcVer" == "vc9" } {
      set aCommonSettingsFileTmpl [wokUtils:FILES:FileToString "$::THE_CASROOT/adm/templates/vcproj.user.vc9x"]
    } else {
      set aCommonSettingsFileTmpl [wokUtils:FILES:FileToString "$::THE_CASROOT/adm/templates/vcxproj.user.vc10x"]
    }
    if { "$aCommonSettingsFileTmpl" != "" } {
      regsub -all -- {__VCVER__} $aCommonSettingsFileTmpl $aVCRTVer aCommonSettingsFileTmpl

      set aFile [open [set aVcFilePath "$aCommonSettingsFile"] w]
      fconfigure $aFile -translation crlf
      puts $aFile $aCommonSettingsFileTmpl
      close $aFile

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

# remove from listloc OpenCascade units indesirables on Unix
proc osutils:justunix { listloc } {
  if { "$::tcl_platform(os)" == "Darwin" } {
    set goaway [list Xw WNT]
  } else {
    set goaway [list WNT]
  }
  return [osutils:juststation $goaway $listloc]
}


####### CODEBLOCK ###################################################################
# Function to generate Code Blocks workspace and project files
proc OS:MKCBP { theOutDir theModules theAllSolution thePlatform theCmpl } {
  puts stderr "Generating project files for Code Blocks"

  # Generate projects for toolkits and separate workspace for each module
  foreach aModule $theModules {
    OS:cworkspace          $aModule $aModule $theOutDir
    OS:cbp        $theCmpl $aModule          $theOutDir $thePlatform
  }

  # Generate single workspace "OCCT" containing projects from all modules
  if { "$theAllSolution" != "" } {
    OS:cworkspace $theAllSolution $theModules $theOutDir
  }

  puts "The Code Blocks workspace and project files are stored in the $theOutDir directory"
}

# Generate Code Blocks projects
proc OS:cbp { theCmpl theModules theOutDir thePlatform } {
  set aProjectFiles {}
  foreach aModule $theModules {
    foreach aToolKit [${aModule}:toolkits] {
      lappend aProjectFiles [osutils:cbptk $theCmpl $theOutDir $aToolKit $thePlatform]
    }
    foreach anExecutable [OS:executable ${aModule}] {
      lappend aProjectFiles [osutils:cbpx  $theCmpl $theOutDir $anExecutable $thePlatform]
    }
  }
  return $aProjectFiles
}

# Generate Code::Blocks project file for ToolKit
proc osutils:cbptk { theCmpl theOutDir theToolKit thePlatform} {
  set aUsedLibs     [list]
  set aFrameworks   [list]
  set anIncPaths    [list]
  set aTKDefines    [list]
  set aTKSrcFiles   [list]

  # collect list of referred libraries to link with
  osutils:usedOsLibs $theToolKit $thePlatform aUsedLibs aFrameworks
  set aDepToolkits [wokUtils:LIST:Purge [osutils:tk:close $theToolKit]]
  foreach tkx $aDepToolkits {
    lappend aUsedLibs "${tkx}"
  }

  lappend anIncPaths "../../../inc"
  set listloc [osutils:tk:units $theToolKit]

  if { [llength $listloc] == 0 } {
    set listloc $theToolKit
  }

  if { $thePlatform == "wnt" || $thePlatform == "uwp" } {
    set resultloc [osutils:justwnt  $listloc]
  } else {
    set resultloc [osutils:justunix $listloc]
  }
  if [array exists written] { unset written }
  foreach fxlo $resultloc {
    set xlo       $fxlo
    set aSrcFiles [osutils:tk:files $xlo $thePlatform]
    foreach aSrcFile [lsort $aSrcFiles] {
      if { ![info exists written([file tail $aSrcFile])] } {
        set written([file tail $aSrcFile]) 1
        lappend aTKSrcFiles "../../../[wokUtils:FILES:wtail $aSrcFile 3]"
      } else {
        puts "Warning : more than one occurences for [file tail $aSrcFile]"
      }
    }

    # macros for correct DLL exports
    if { $thePlatform == "wnt" || $thePlatform == "uwp" } {
      lappend aTKDefines "__${xlo}_DLL"
    }
  }

  return [osutils:cbp $theCmpl $theOutDir $theToolKit $thePlatform $aTKSrcFiles $aUsedLibs $aFrameworks $anIncPaths $aTKDefines]
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
proc osutils:cbpx { theCmpl theOutDir theToolKit thePlatform } {
  global path
  set aWokArch    "$::env(ARCH)"

  set aCbpFiles {}
  foreach aSrcFile [osutils:tk:files $theToolKit $thePlatform] {
    # collect list of referred libraries to link with
    set aUsedLibs     [list]
    set aFrameworks   [list]
    set anIncPaths    [list]
    set aTKDefines    [list]
    set aTKSrcFiles   [list]
    set aProjName [file rootname [file tail $aSrcFile]]

    osutils:usedOsLibs $theToolKit $thePlatform aUsedLibs aFrameworks

    set aDepToolkits [LibToLinkX $theToolKit $aProjName]
    foreach tkx $aDepToolkits {
      if {[_get_type $tkx] == "t"} {
        lappend aUsedLibs "${tkx}"
      }
      if {[lsearch [glob -tails -directory "$path/src" -types d *] $tkx] == "-1"} {
        lappend aUsedLibs "${tkx}"
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
      lappend aTKSrcFiles "../../../[wokUtils:FILES:wtail $aSrcFile 3]"
    } else {
      puts "Warning : in cbp there are more than one occurences for [file tail $aSrcFile]"
    }

    # macros for correct DLL exports
    if { $thePlatform == "wnt" || $thePlatform == "uwp" } {
      lappend aTKDefines "__${theToolKit}_DLL"
    }

    # common include paths
    lappend anIncPaths "../../../inc"

    lappend aCbpFiles [osutils:cbp $theCmpl $theOutDir $aProjName $thePlatform $aTKSrcFiles $aUsedLibs $aFrameworks $anIncPaths $aTKDefines $isExecutable]
  }

  return $aCbpFiles
}

# This function intended to generate Code::Blocks project file
# @param theCmpl       - the compiler (gcc or msvc)
# @param theOutDir     - output directory to place project file
# @param theProjName   - project name
# @param theSrcFiles   - list of source files
# @param theLibsList   - dependencies (libraries  list)
# @param theFrameworks - dependencies (frameworks list, Mac OS X specific)
# @param theIncPaths   - header search paths
# @param theDefines    - compiler macro definitions
# @param theIsExe      - flag to indicate executable / library target
proc osutils:cbp { theCmpl theOutDir theProjName thePlatform theSrcFiles theLibsList theFrameworks theIncPaths theDefines {theIsExe "false"} } {
  set aWokArch    "$::env(ARCH)"

  set aCmplCbp "gcc"
  set aCmplFlags        [list]
  set aCmplFlagsRelease [list]
  set aCmplFlagsDebug   [list]
  set toPassArgsByFile 0
  set aLibPrefix "lib"
  if { "$thePlatform" == "wnt" || "$thePlatform" == "uwp" || "$thePlatform" == "qnx" } {
    set toPassArgsByFile 1
  }
  if { "$theCmpl" == "msvc" } {
    set aCmplCbp "msvc8"
    set aLibPrefix ""
  }

  if { "$theCmpl" == "msvc" } {
    set aCmplFlags        "-arch:SSE2 -EHsc -W4 -MP"
    set aCmplFlagsRelease "-MD  -O2"
    set aCmplFlagsDebug   "-MDd -Od -Zi"
    lappend aCmplFlags    "-D_CRT_SECURE_NO_WARNINGS"
    lappend aCmplFlags    "-D_CRT_NONSTDC_NO_DEPRECATE"
  } elseif { "$theCmpl" == "gcc" } {
    if { "$thePlatform" != "qnx" } {
      set aCmplFlags      "-mmmx -msse -msse2 -mfpmath=sse"
    }
    set aCmplFlagsRelease "-O2"
    set aCmplFlagsDebug   "-O0 -g"
    if { "$thePlatform" == "wnt" || "$thePlatform" == "uwp" } {
      lappend aCmplFlags "-std=gnu++0x"
      lappend aCmplFlags "-D_WIN32_WINNT=0x0501"
    } else {
      lappend aCmplFlags "-std=c++0x"
      lappend aCmplFlags "-fPIC"
      lappend aCmplFlags "-DOCC_CONVERT_SIGNALS"
    }
    lappend aCmplFlags   "-Wall"
    lappend aCmplFlags   "-fexceptions"
  }
  lappend aCmplFlagsRelease "-DNDEBUG"
  lappend aCmplFlagsRelease "-DNo_Exception"
  lappend aCmplFlagsDebug   "-D_DEBUG"
  if { "$thePlatform" == "qnx" } {
    lappend aCmplFlags "-D_QNX_SOURCE"
  }

  set aCbpFilePath    "${theOutDir}/${theProjName}.cbp"
  set aLnkFileName    "${theProjName}_obj.link"
  set aLnkDebFileName "${theProjName}_objd.link"
  set aLnkFilePath    "${theOutDir}/${aLnkFileName}"
  set aLnkDebFilePath "${theOutDir}/${aLnkDebFileName}"
  set aFile [open $aCbpFilePath "w"]
  puts $aFile "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>"
  puts $aFile "<CodeBlocks_project_file>"
  puts $aFile "\t<FileVersion major=\"1\" minor=\"6\" />"
  puts $aFile "\t<Project>"
  puts $aFile "\t\t<Option title=\"$theProjName\" />"
  puts $aFile "\t\t<Option pch_mode=\"2\" />"
  puts $aFile "\t\t<Option compiler=\"$aCmplCbp\" />"
  puts $aFile "\t\t<Build>"

  # Release target configuration
  puts $aFile "\t\t\t<Target title=\"Release\">"
  if { "$theIsExe" == "true" } {
    puts $aFile "\t\t\t\t<Option output=\"../../../${thePlatform}/cbp/bin/${theProjName}\" prefix_auto=\"0\" extension_auto=\"0\" />"
    puts $aFile "\t\t\t\t<Option type=\"1\" />"
  } else {
    if { "$thePlatform" == "wnt" || "$thePlatform" == "uwp" } {
      puts $aFile "\t\t\t\t<Option output=\"../../../${thePlatform}/cbp/bin/${aLibPrefix}${theProjName}\" imp_lib=\"../../../${thePlatform}/cbp/lib/\$(TARGET_OUTPUT_BASENAME)\" prefix_auto=\"1\" extension_auto=\"1\" />"
    } else {
      puts $aFile "\t\t\t\t<Option output=\"../../../${thePlatform}/cbp/lib/lib${theProjName}.so\" prefix_auto=\"0\" extension_auto=\"0\" />"
    }
    puts $aFile "\t\t\t\t<Option type=\"3\" />"
  }
  puts $aFile "\t\t\t\t<Option object_output=\"../../../${thePlatform}/cbp/obj\" />"
  puts $aFile "\t\t\t\t<Option compiler=\"$aCmplCbp\" />"
  puts $aFile "\t\t\t\t<Option createDefFile=\"0\" />"
  if { "$thePlatform" == "wnt" || "$thePlatform" == "uwp" } {
    puts $aFile "\t\t\t\t<Option createStaticLib=\"1\" />"
  } else {
    puts $aFile "\t\t\t\t<Option createStaticLib=\"0\" />"
  }

  # compiler options per TARGET (including defines)
  puts $aFile "\t\t\t\t<Compiler>"
  foreach aFlagIter $aCmplFlagsRelease {
    puts $aFile "\t\t\t\t\t<Add option=\"$aFlagIter\" />"
  }
  foreach aMacro $theDefines {
    puts $aFile "\t\t\t\t\t<Add option=\"-D${aMacro}\" />"
  }
  puts $aFile "\t\t\t\t</Compiler>"

  puts $aFile "\t\t\t\t<Linker>"
  if { $toPassArgsByFile == 1 } {
    puts $aFile "\t\t\t\t\t<Add option=\"\@$aLnkFileName\" />"
  }
  puts $aFile "\t\t\t\t\t<Add directory=\"../../../${thePlatform}/cbp/lib\" />"
  if { "$thePlatform" == "mac" } {
    if { [ lsearch $theLibsList X11 ] >= 0} {
      puts $aFile "\t\t\t\t\t<Add directory=\"/usr/X11/lib\" />"
    }
  }
  puts $aFile "\t\t\t\t\t<Add option=\"\$(CSF_OPT_LNK${aWokArch})\" />"
  if { "$thePlatform" == "lin" } {
    puts $aFile "\t\t\t\t\t<Add option=\"-Wl,-rpath-link=../../../${thePlatform}/cbp/lib\" />"
  }
  puts $aFile "\t\t\t\t</Linker>"

  puts $aFile "\t\t\t</Target>"

  # Debug target configuration
  puts $aFile "\t\t\t<Target title=\"Debug\">"
  if { "$theIsExe" == "true" } {
    puts $aFile "\t\t\t\t<Option output=\"../../../${thePlatform}/cbp/bind/${theProjName}\" prefix_auto=\"0\" extension_auto=\"0\" />"
    puts $aFile "\t\t\t\t<Option type=\"1\" />"
  } else {
    if { "$thePlatform" == "wnt" || "$thePlatform" == "uwp" } {
      puts $aFile "\t\t\t\t<Option output=\"../../../${thePlatform}/cbp/bind/${aLibPrefix}${theProjName}\" imp_lib=\"../../../${thePlatform}/cbp/libd/\$(TARGET_OUTPUT_BASENAME)\" prefix_auto=\"1\" extension_auto=\"1\" />"
    } else {
      puts $aFile "\t\t\t\t<Option output=\"../../../${thePlatform}/cbp/libd/lib${theProjName}.so\" prefix_auto=\"0\" extension_auto=\"0\" />"
    }
    puts $aFile "\t\t\t\t<Option type=\"3\" />"
  }
  puts $aFile "\t\t\t\t<Option object_output=\"../../../${thePlatform}/cbp/objd\" />"
  puts $aFile "\t\t\t\t<Option compiler=\"$aCmplCbp\" />"
  puts $aFile "\t\t\t\t<Option createDefFile=\"0\" />"
  if { "$thePlatform" == "wnt" || "$thePlatform" == "uwp" } {
    puts $aFile "\t\t\t\t<Option createStaticLib=\"1\" />"
  } else {
    puts $aFile "\t\t\t\t<Option createStaticLib=\"0\" />"
  }

  # compiler options per TARGET (including defines)
  puts $aFile "\t\t\t\t<Compiler>"
  foreach aFlagIter $aCmplFlagsDebug {
    puts $aFile "\t\t\t\t\t<Add option=\"$aFlagIter\" />"
  }
  foreach aMacro $theDefines {
    puts $aFile "\t\t\t\t\t<Add option=\"-D${aMacro}\" />"
  }
  puts $aFile "\t\t\t\t</Compiler>"

  puts $aFile "\t\t\t\t<Linker>"
  if { $toPassArgsByFile == 1 } {
    puts $aFile "\t\t\t\t\t<Add option=\"\@$aLnkDebFileName\" />"
  }
  puts $aFile "\t\t\t\t\t<Add directory=\"../../../${thePlatform}/cbp/libd\" />"
  if { "$thePlatform" == "mac" } {
    if { [ lsearch $theLibsList X11 ] >= 0} {
      puts $aFile "\t\t\t\t\t<Add directory=\"/usr/X11/lib\" />"
    }
  }
  puts $aFile "\t\t\t\t\t<Add option=\"\$(CSF_OPT_LNK${aWokArch}D)\" />"
  if { "$thePlatform" == "lin" } {
    puts $aFile "\t\t\t\t\t<Add option=\"-Wl,-rpath-link=../../../${thePlatform}/cbp/libd\" />"
  }
  puts $aFile "\t\t\t\t</Linker>"

  puts $aFile "\t\t\t</Target>"

  puts $aFile "\t\t</Build>"

  # COMMON compiler options
  puts $aFile "\t\t<Compiler>"
  foreach aFlagIter $aCmplFlags {
    puts $aFile "\t\t\t<Add option=\"$aFlagIter\" />"
  }
  puts $aFile "\t\t\t<Add option=\"\$(CSF_OPT_CMPL)\" />"
  foreach anIncPath $theIncPaths {
    puts $aFile "\t\t\t<Add directory=\"$anIncPath\" />"
  }
  puts $aFile "\t\t</Compiler>"

  # COMMON linker options
  puts $aFile "\t\t<Linker>"
  if { "$thePlatform" == "wnt" && "$theCmpl" == "gcc" } {
    puts $aFile "\t\t\t<Add option=\"-Wl,--export-all-symbols\" />"
  }
  foreach aFrameworkName $theFrameworks {
    if { "$aFrameworkName" != "" } {
      puts $aFile "\t\t\t<Add option=\"-framework $aFrameworkName\" />"
    }
  }
  foreach aLibName $theLibsList {
    if { "$aLibName" != "" } {
      if { "$theCmpl" == "msvc" } {
        puts $aFile "\t\t\t<Add library=\"${aLibName}.lib\" />"
      } else {
        puts $aFile "\t\t\t<Add library=\"${aLibName}\" />"
      }
    }
  }
  puts $aFile "\t\t</Linker>"

  # list of sources

  set aFileLnkObj ""
  set aFileLnkObjd ""
  set isFirstSrcFile 1
  if { $toPassArgsByFile == 1 } {
    set aFileLnkObj  [open $aLnkFilePath    "w"]
    set aFileLnkObjd [open $aLnkDebFilePath "w"]
  }

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
    } elseif { $toPassArgsByFile == 1 && $isFirstSrcFile == 0 && [string equal -nocase [file extension $aSrcFile] ".cxx" ] } {
      # pass at list single source file to Code::Blocks as is
      # and pack the list of other files into the dedicated file to workaround process arguments limits on systems like Windows
      puts $aFile "\t\t<Unit filename=\"$aSrcFile\">"
      puts $aFile "\t\t\t<Option link=\"0\" />"
      puts $aFile "\t\t</Unit>"

      set aFileObj  [string map {.cxx .o} [string map [list "/src/" "/$thePlatform/cbp/obj/src/"]  $aSrcFile]]
      set aFileObjd [string map {.cxx .o} [string map [list "/src/" "/$thePlatform/cbp/objd/src/"] $aSrcFile]]
      puts -nonewline $aFileLnkObj  "$aFileObj "
      puts -nonewline $aFileLnkObjd "$aFileObjd "
    } else {
      puts $aFile "\t\t<Unit filename=\"$aSrcFile\" />"
      set isFirstSrcFile 0
    }
  }

  if { "$thePlatform" == "wnt" || "$thePlatform" == "uwp" } {
    close $aFileLnkObj
    close $aFileLnkObjd
  }

  puts $aFile "\t</Project>"
  puts $aFile "</CodeBlocks_project_file>"
  close $aFile

  return $aCbpFilePath
}

# Define libraries to link using only EXTERNLIB file
proc LibToLinkX {thePackage theDummyName} {
  set aToolKits [LibToLink $thePackage]
  return $aToolKits
}

# Function to generate Xcode workspace and project files
proc OS:MKXCD { theOutDir {theModules {}} {theAllSolution ""} {theLibType "dynamic"} {thePlatform ""} } {

  puts stderr "Generating project files for Xcode"

  # Generate projects for toolkits and separate workspace for each module
  foreach aModule $theModules {
    OS:xcworkspace $aModule $aModule $theOutDir
    OS:xcodeproj   $aModule          $theOutDir ::THE_GUIDS_LIST $theLibType $thePlatform
  }

  # Generate single workspace "OCCT" containing projects from all modules
  if { "$theAllSolution" != "" } {
    OS:xcworkspace $theAllSolution $theModules $theOutDir
  }
}

# Generates toolkits sections for Xcode workspace file.
proc OS:xcworkspace:toolkits { theModule } {
  set aBuff ""

  # Adding toolkits for module in workspace.
  foreach aToolKit [osutils:tk:sort [${theModule}:toolkits]] {
    append aBuff "         <FileRef\n"
    append aBuff "            location = \"group:${aToolKit}.xcodeproj\">\n"
    append aBuff "         </FileRef>\n"
  }

  # Adding executables for module, assume one project per cxx file...
  foreach aUnit [OS:executable ${theModule}] {
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
        append aBuff "         <FileRef\n"
        append aBuff "            location = \"group:${aPrjName}.xcodeproj\">\n"
        append aBuff "         </FileRef>\n"
      }
    }
  }

  # Removing unnecessary newline character from the end.
  set aBuff [string replace $aBuff end end]
  return $aBuff
}

# Generates workspace files for Xcode.
proc OS:xcworkspace { theWorkspaceName theModules theOutDir } {
  # Creating workspace directory for Xcode.
  set aWorkspaceDir "${theOutDir}/${theWorkspaceName}.xcworkspace"
  wokUtils:FILES:mkdir $aWorkspaceDir
  if { ! [file exists $aWorkspaceDir] } {
    puts stderr "Error: Could not create workspace directory \"$aWorkspaceDir\""
    return
  }

  # Creating workspace file.
  set aWsFilePath "${aWorkspaceDir}/contents.xcworkspacedata"
  set aFile [open $aWsFilePath "w"]

  # Adding header and section for main Group.
  puts $aFile "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  puts $aFile "<Workspace"
  puts $aFile "   version = \"1.0\">"
  puts $aFile "   <Group"
  puts $aFile "      location = \"container:\""
  puts $aFile "      name = \"${theWorkspaceName}\">"

  # Adding modules.
  if { [llength "$theModules"] > 1 } {
    foreach aModule $theModules {
      puts $aFile "      <Group"
      puts $aFile "         location = \"container:\""
      puts $aFile "         name = \"${aModule}\">"
      puts $aFile [OS:xcworkspace:toolkits $aModule]
      puts $aFile "      </Group>"
    }
  } else {
    puts $aFile [OS:xcworkspace:toolkits $theModules]
  }

  # Adding footer.
  puts $aFile "   </Group>"
  puts $aFile "</Workspace>"
  close $aFile
}

# Generates Xcode project files.
proc OS:xcodeproj { theModules theOutDir theGuidsMap theLibType thePlatform} {
  upvar $theGuidsMap aGuidsMap

  set isStatic 0
  if { "$theLibType" == "static" } {
    set isStatic 1
  } elseif { "$thePlatform" == "ios" } {
    set isStatic 1
  }

  set aProjectFiles {}
  foreach aModule $theModules {
    foreach aToolKit [${aModule}:toolkits] {
      lappend aProjectFiles [osutils:xcdtk $theOutDir $aToolKit     aGuidsMap $isStatic $thePlatform "dylib"]
    }
    foreach anExecutable [OS:executable ${aModule}] {
      lappend aProjectFiles [osutils:xcdtk $theOutDir $anExecutable aGuidsMap $isStatic $thePlatform "executable"]
    }
  }
  return $aProjectFiles
}

# Generates dependencies section for Xcode project files.
proc osutils:xcdtk:deps {theToolKit theTargetType theGuidsMap theFileRefSection theDepsGuids theDepsRefGuids theIsStatic} {
  upvar $theGuidsMap         aGuidsMap
  upvar $theFileRefSection   aFileRefSection
  upvar $theDepsGuids        aDepsGuids
  upvar $theDepsRefGuids     aDepsRefGuids

  set aBuildFileSection ""
  set aUsedLibs         [wokUtils:LIST:Purge [osutils:tk:close $theToolKit]]
  set aDepToolkits      [lappend [wokUtils:LIST:Purge [osutils:tk:close $theToolKit]] $theToolKit]

  if { "$theTargetType" == "executable" } {
    set aFile [osutils:tk:files $theToolKit mac]
    set aProjName [file rootname [file tail $aFile]]
    set aDepToolkits [LibToLinkX $theToolKit $aProjName]
  }

  set aLibExt "dylib"
  if { $theIsStatic == 1 } {
    set aLibExt "a"
    if { "$theTargetType" != "executable" } {
      return $aBuildFileSection
    }
  }

  osutils:usedOsLibs $theToolKit "mac" aLibs aFrameworks
  set aUsedLibs [concat $aUsedLibs $aLibs]
  set aUsedLibs [concat $aUsedLibs $aFrameworks]
  foreach tkx $aUsedLibs {
    set aDepLib    "${tkx}_Dep"
    set aDepLibRef "${tkx}_DepRef"

    if { ! [info exists aGuidsMap($aDepLib)] } {
      set aGuidsMap($aDepLib) [OS:genGUID "xcd"]
    }
    if { ! [info exists aGuidsMap($aDepLibRef)] } {
      set aGuidsMap($aDepLibRef) [OS:genGUID "xcd"]
    }

    append aBuildFileSection "\t\t$aGuidsMap($aDepLib) = \{isa = PBXBuildFile; fileRef = $aGuidsMap($aDepLibRef) ; \};\n"
    if {[lsearch -nocase $aFrameworks $tkx] == -1} {
      append aFileRefSection   "\t\t$aGuidsMap($aDepLibRef) = \{isa = PBXFileReference; lastKnownFileType = file; name = lib${tkx}.${aLibExt}; path = lib${tkx}.${aLibExt}; sourceTree = \"<group>\"; \};\n"
    } else {
      append aFileRefSection   "\t\t$aGuidsMap($aDepLibRef) = \{isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = ${tkx}.framework; path = /System/Library/Frameworks/${tkx}.framework; sourceTree = \"<absolute>\"; \};\n"
    }
    append aDepsGuids        "\t\t\t\t$aGuidsMap($aDepLib) ,\n"
    append aDepsRefGuids     "\t\t\t\t$aGuidsMap($aDepLibRef) ,\n"
  }

  return $aBuildFileSection
}

# Generates PBXBuildFile and PBXGroup sections for project file.
proc osutils:xcdtk:sources {theToolKit theTargetType theSrcFileRefSection theGroupSection thePackageGuids theSrcFileGuids theGuidsMap theIncPaths} {
  upvar $theSrcFileRefSection aSrcFileRefSection
  upvar $theGroupSection      aGroupSection
  upvar $thePackageGuids      aPackagesGuids
  upvar $theSrcFileGuids      aSrcFileGuids
  upvar $theGuidsMap          aGuidsMap
  upvar $theIncPaths          anIncPaths

  set listloc [osutils:tk:units $theToolKit]
  set resultloc [osutils:justunix $listloc]
  set aBuildFileSection ""
  set aPackages [lsort -nocase $resultloc]
  if { "$theTargetType" == "executable" } {
    set aPackages [list "$theToolKit"]
  }

  # Generating PBXBuildFile, PBXGroup sections and groups for each package.
  foreach fxlo $aPackages {
    set xlo       $fxlo
    set aPackage "${xlo}_Package"
    set aSrcFileRefGuids ""
    if { ! [info exists aGuidsMap($aPackage)] } {
      set aGuidsMap($aPackage) [OS:genGUID "xcd"]
    }

    set aSrcFiles [osutils:tk:files $xlo mac]
    foreach aSrcFile [lsort $aSrcFiles] {
      set aFileExt "sourcecode.cpp.cpp"

      if { [file extension $aSrcFile] == ".c" } {
        set aFileExt "sourcecode.c.c"
      } elseif { [file extension $aSrcFile] == ".mm" } {
        set aFileExt "sourcecode.cpp.objcpp"
      }

      if { ! [info exists aGuidsMap($aSrcFile)] } {
        set aGuidsMap($aSrcFile) [OS:genGUID "xcd"]
      }
      set aSrcFileRef "${aSrcFile}_Ref"
      if { ! [info exists aGuidsMap($aSrcFileRef)] } {
        set aGuidsMap($aSrcFileRef) [OS:genGUID "xcd"]
      }
      if { ! [info exists written([file tail $aSrcFile])] } {
        set written([file tail $aSrcFile]) 1
        append aBuildFileSection  "\t\t$aGuidsMap($aSrcFile) = \{isa = PBXBuildFile; fileRef = $aGuidsMap($aSrcFileRef) ;\};\n"
        append aSrcFileRefSection "\t\t$aGuidsMap($aSrcFileRef) = \{isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = ${aFileExt}; name = [wokUtils:FILES:wtail $aSrcFile 1]; path = ../../../[wokUtils:FILES:wtail $aSrcFile 3]; sourceTree = \"<group>\"; \};\n"
        append aSrcFileGuids      "\t\t\t\t$aGuidsMap($aSrcFile) ,\n"
        append aSrcFileRefGuids   "\t\t\t\t$aGuidsMap($aSrcFileRef) ,\n"
      } else {
        puts "Warning : more than one occurences for [file tail $aSrcFile]"
      }
    }

    append aGroupSection "\t\t$aGuidsMap($aPackage) = \{\n"
    append aGroupSection "\t\t\tisa = PBXGroup;\n"
    append aGroupSection "\t\t\tchildren = (\n"
    append aGroupSection $aSrcFileRefGuids
    append aGroupSection "\t\t\t);\n"
    append aGroupSection "\t\t\tname = $xlo;\n"
    append aGroupSection "\t\t\tsourceTree = \"<group>\";\n"
    append aGroupSection "\t\t\};\n"

    # Storing packages IDs for adding them later as a child of toolkit
    append aPackagesGuids "\t\t\t\t$aGuidsMap($aPackage) ,\n"
  }

  # Removing unnecessary newline character from the end.
  set aPackagesGuids [string replace $aPackagesGuids end end]

  return $aBuildFileSection
}

# Creates folders structure and all necessary files for Xcode project.
proc osutils:xcdtk { theOutDir theToolKit theGuidsMap theIsStatic thePlatform {theTargetType "dylib"} } {
  set aPBXBuildPhase "Headers"
  set aRunOnlyForDeployment "0"
  set aProductType "library.dynamic"
  set anExecExtension "\t\t\t\tEXECUTABLE_EXTENSION = dylib;"
  set anExecPrefix "\t\t\t\tEXECUTABLE_PREFIX = lib;"
  set aWrapperExtension "\t\t\t\tWRAPPER_EXTENSION = dylib;"
  set aTKDefines [list "OCC_CONVERT_SIGNALS"]
  if { $theIsStatic == 1 } {
    lappend aTKDefines "OCCT_NO_PLUGINS"
  }

  if { "$theTargetType" == "executable" } {
    set aPBXBuildPhase "CopyFiles"
    set aRunOnlyForDeployment "1"
    set aProductType "tool"
    set anExecExtension ""
    set anExecPrefix ""
    set aWrapperExtension ""
  } elseif { $theIsStatic == 1 } {
    set aProductType "library.static"
    set anExecExtension "\t\t\t\tEXECUTABLE_EXTENSION = a;"
    set aWrapperExtension "\t\t\t\tWRAPPER_EXTENSION = a;"
  }

  set aUsername [exec whoami]

  # Creation of folders for Xcode projectP.
  set aToolkitDir "${theOutDir}/${theToolKit}.xcodeproj"
  wokUtils:FILES:mkdir $aToolkitDir
  if { ! [file exists $aToolkitDir] } {
    puts stderr "Error: Could not create project directory \"$aToolkitDir\""
    return
  }

  set aUserDataDir "${aToolkitDir}/xcuserdata"
  wokUtils:FILES:mkdir $aUserDataDir
  if { ! [file exists $aUserDataDir] } {
    puts stderr "Error: Could not create xcuserdata directorty in \"$aToolkitDir\""
    return
  }

  set aUserDataDir "${aUserDataDir}/${aUsername}.xcuserdatad"
  wokUtils:FILES:mkdir $aUserDataDir
  if { ! [file exists $aUserDataDir] } {
    puts stderr "Error: Could not create ${aUsername}.xcuserdatad directorty in \"$aToolkitDir\"/xcuserdata"
    return
  }

  set aSchemesDir "${aUserDataDir}/xcschemes"
  wokUtils:FILES:mkdir $aSchemesDir
  if { ! [file exists $aSchemesDir] } {
    puts stderr "Error: Could not create xcschemes directorty in \"$aUserDataDir\""
    return
  }
  # End of folders creation.

  # Generating GUID for tookit.
  upvar $theGuidsMap aGuidsMap
  if { ! [info exists aGuidsMap($theToolKit)] } {
    set aGuidsMap($theToolKit) [OS:genGUID "xcd"]
  }

  # Creating xcscheme file for toolkit from template.
  set aXcschemeTmpl [osutils:readtemplate "xcscheme" "xcd"]
  regsub -all -- {__TOOLKIT_NAME__} $aXcschemeTmpl $theToolKit aXcschemeTmpl
  regsub -all -- {__TOOLKIT_GUID__} $aXcschemeTmpl $aGuidsMap($theToolKit) aXcschemeTmpl
  set aXcschemeFile [open "$aSchemesDir/${theToolKit}.xcscheme"  "w"]
  puts $aXcschemeFile $aXcschemeTmpl
  close $aXcschemeFile

  # Creating xcschememanagement.plist file for toolkit from template.
  set aPlistTmpl [osutils:readtemplate "plist" "xcd"]
  regsub -all -- {__TOOLKIT_NAME__} $aPlistTmpl $theToolKit aPlistTmpl
  regsub -all -- {__TOOLKIT_GUID__} $aPlistTmpl $aGuidsMap($theToolKit) aPlistTmpl
  set aPlistFile [open "$aSchemesDir/xcschememanagement.plist"  "w"]
  puts $aPlistFile $aPlistTmpl
  close $aPlistFile

  # Creating project.pbxproj file for toolkit.
  set aPbxprojFile [open "$aToolkitDir/project.pbxproj" "w"]
  puts $aPbxprojFile "// !\$*UTF8*\$!"
  puts $aPbxprojFile "\{"
  puts $aPbxprojFile "\tarchiveVersion = 1;"
  puts $aPbxprojFile "\tclasses = \{"
  puts $aPbxprojFile "\t\};"
  puts $aPbxprojFile "\tobjectVersion = 46;"
  puts $aPbxprojFile "\tobjects = \{\n"

  # Begin PBXBuildFile section
  set aPackagesGuids ""
  set aGroupSection ""
  set aSrcFileRefSection ""
  set aSrcFileGuids ""
  set aDepsFileRefSection ""
  set aDepsGuids ""
  set aDepsRefGuids ""
  set anIncPaths [list "../../../inc"]
  set anLibPaths ""

  if { [info exists ::env(CSF_OPT_INC)] } {
    set anIncCfg [split "$::env(CSF_OPT_INC)" ":"]
    foreach anIncCfgPath $anIncCfg {
      lappend anIncPaths $anIncCfgPath
    }
  }
  if { [info exists ::env(CSF_OPT_LIB64)] } {
    set anLibCfg [split "$::env(CSF_OPT_LIB64)" ":"]
    foreach anLibCfgPath $anLibCfg {
      lappend anLibPaths $anLibCfgPath
    }
  }

  puts $aPbxprojFile [osutils:xcdtk:sources $theToolKit $theTargetType aSrcFileRefSection aGroupSection aPackagesGuids aSrcFileGuids aGuidsMap anIncPaths]
  puts $aPbxprojFile [osutils:xcdtk:deps    $theToolKit $theTargetType aGuidsMap aDepsFileRefSection aDepsGuids aDepsRefGuids $theIsStatic]
  # End PBXBuildFile section

  # Begin PBXFileReference section
  set aToolkitLib "lib${theToolKit}.dylib"
  set aPath "$aToolkitLib"
  if { "$theTargetType" == "executable" } {
    set aPath "$theToolKit"
  } elseif { $theIsStatic == 1 } {
    set aToolkitLib "lib${theToolKit}.a"
  }

  if { ! [info exists aGuidsMap($aToolkitLib)] } {
    set aGuidsMap($aToolkitLib) [OS:genGUID "xcd"]
  }

  puts $aPbxprojFile "\t\t$aGuidsMap($aToolkitLib) = {isa = PBXFileReference; explicitFileType = \"compiled.mach-o.${theTargetType}\"; includeInIndex = 0; path = $aPath; sourceTree = BUILT_PRODUCTS_DIR; };\n"
  puts $aPbxprojFile $aSrcFileRefSection
  puts $aPbxprojFile $aDepsFileRefSection
  # End PBXFileReference section


  # Begin PBXFrameworksBuildPhase section
  set aTkFrameworks "${theToolKit}_Frameworks"
  if { ! [info exists aGuidsMap($aTkFrameworks)] } {
    set aGuidsMap($aTkFrameworks) [OS:genGUID "xcd"]
  }

  puts $aPbxprojFile "\t\t$aGuidsMap($aTkFrameworks) = \{"
  puts $aPbxprojFile "\t\t\tisa = PBXFrameworksBuildPhase;"
  puts $aPbxprojFile "\t\t\tbuildActionMask = 2147483647;"
  puts $aPbxprojFile "\t\t\tfiles = ("
  puts $aPbxprojFile $aDepsGuids
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\trunOnlyForDeploymentPostprocessing = 0;"
  puts $aPbxprojFile "\t\t\};\n"
  # End PBXFrameworksBuildPhase section

  # Begin PBXGroup section
  set aTkPBXGroup "${theToolKit}_PBXGroup"
  if { ! [info exists aGuidsMap($aTkPBXGroup)] } {
    set aGuidsMap($aTkPBXGroup) [OS:genGUID "xcd"]
  }

  set aTkSrcGroup "${theToolKit}_SrcGroup"
  if { ! [info exists aGuidsMap($aTkSrcGroup)] } {
    set aGuidsMap($aTkSrcGroup) [OS:genGUID "xcd"]
  }

  puts $aPbxprojFile $aGroupSection
  puts $aPbxprojFile "\t\t$aGuidsMap($aTkPBXGroup) = \{"
  puts $aPbxprojFile "\t\t\tisa = PBXGroup;"
  puts $aPbxprojFile "\t\t\tchildren = ("
  puts $aPbxprojFile $aDepsRefGuids
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($aTkSrcGroup) ,"
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($aToolkitLib) ,"
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\tsourceTree = \"<group>\";"
  puts $aPbxprojFile "\t\t\};"
  puts $aPbxprojFile "\t\t$aGuidsMap($aTkSrcGroup) = \{"
  puts $aPbxprojFile "\t\t\tisa = PBXGroup;"
  puts $aPbxprojFile "\t\t\tchildren = ("
  puts $aPbxprojFile $aPackagesGuids
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\tname = \"Source files\";"
  puts $aPbxprojFile "\t\t\tsourceTree = \"<group>\";"
  puts $aPbxprojFile "\t\t\};\n"
  # End PBXGroup section

  # Begin PBXHeadersBuildPhase section
  set aTkHeaders "${theToolKit}_Headers"
  if { ! [info exists aGuidsMap($aTkHeaders)] } {
    set aGuidsMap($aTkHeaders) [OS:genGUID "xcd"]
  }

  puts $aPbxprojFile "\t\t$aGuidsMap($aTkHeaders) = \{"
  puts $aPbxprojFile "\t\t\tisa = PBX${aPBXBuildPhase}BuildPhase;"
  puts $aPbxprojFile "\t\t\tbuildActionMask = 2147483647;"
  puts $aPbxprojFile "\t\t\tfiles = ("
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\trunOnlyForDeploymentPostprocessing = ${aRunOnlyForDeployment};"
  puts $aPbxprojFile "\t\t\};\n"
  # End PBXHeadersBuildPhase section

  # Begin PBXNativeTarget section
  set aTkBuildCfgListNativeTarget "${theToolKit}_BuildCfgListNativeTarget"
  if { ! [info exists aGuidsMap($aTkBuildCfgListNativeTarget)] } {
    set aGuidsMap($aTkBuildCfgListNativeTarget) [OS:genGUID "xcd"]
  }

  set aTkSources "${theToolKit}_Sources"
  if { ! [info exists aGuidsMap($aTkSources)] } {
    set aGuidsMap($aTkSources) [OS:genGUID "xcd"]
  }

  puts $aPbxprojFile "\t\t$aGuidsMap($theToolKit) = \{"
  puts $aPbxprojFile "\t\t\tisa = PBXNativeTarget;"
  puts $aPbxprojFile "\t\t\tbuildConfigurationList = $aGuidsMap($aTkBuildCfgListNativeTarget) ;"
  puts $aPbxprojFile "\t\t\tbuildPhases = ("
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($aTkSources) ,"
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($aTkFrameworks) ,"
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($aTkHeaders) ,"
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\tbuildRules = ("
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\tdependencies = ("
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\tname = $theToolKit;"
  puts $aPbxprojFile "\t\t\tproductName = $theToolKit;"
  puts $aPbxprojFile "\t\t\tproductReference = $aGuidsMap($aToolkitLib) ;"
  puts $aPbxprojFile "\t\t\tproductType = \"com.apple.product-type.${aProductType}\";"
  puts $aPbxprojFile "\t\t\};\n"
  # End PBXNativeTarget section

  # Begin PBXProject section
  set aTkProjectObj "${theToolKit}_ProjectObj"
  if { ! [info exists aGuidsMap($aTkProjectObj)] } {
    set aGuidsMap($aTkProjectObj) [OS:genGUID "xcd"]
  }

  set aTkBuildCfgListProj "${theToolKit}_BuildCfgListProj"
  if { ! [info exists aGuidsMap($aTkBuildCfgListProj)] } {
    set aGuidsMap($aTkBuildCfgListProj) [OS:genGUID "xcd"]
  }

  puts $aPbxprojFile "\t\t$aGuidsMap($aTkProjectObj) = \{"
  puts $aPbxprojFile "\t\t\tisa = PBXProject;"
  puts $aPbxprojFile "\t\t\tattributes = \{"
  puts $aPbxprojFile "\t\t\t\tLastUpgradeCheck = 0430;"
  puts $aPbxprojFile "\t\t\t\};"
  puts $aPbxprojFile "\t\t\tbuildConfigurationList = $aGuidsMap($aTkBuildCfgListProj) ;"
  puts $aPbxprojFile "\t\t\tcompatibilityVersion = \"Xcode 3.2\";"
  puts $aPbxprojFile "\t\t\tdevelopmentRegion = English;"
  puts $aPbxprojFile "\t\t\thasScannedForEncodings = 0;"
  puts $aPbxprojFile "\t\t\tknownRegions = ("
  puts $aPbxprojFile "\t\t\t\ten,"
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\tmainGroup = $aGuidsMap($aTkPBXGroup);"
  puts $aPbxprojFile "\t\t\tproductRefGroup = $aGuidsMap($aTkPBXGroup);"
  puts $aPbxprojFile "\t\t\tprojectDirPath = \"\";"
  puts $aPbxprojFile "\t\t\tprojectRoot = \"\";"
  puts $aPbxprojFile "\t\t\ttargets = ("
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($theToolKit) ,"
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\};\n"
  # End PBXProject section

  # Begin PBXSourcesBuildPhase section
  puts $aPbxprojFile "\t\t$aGuidsMap($aTkSources) = \{"
  puts $aPbxprojFile "\t\t\tisa = PBXSourcesBuildPhase;"
  puts $aPbxprojFile "\t\t\tbuildActionMask = 2147483647;"
  puts $aPbxprojFile "\t\t\tfiles = ("
  puts $aPbxprojFile $aSrcFileGuids
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\trunOnlyForDeploymentPostprocessing = 0;"
  puts $aPbxprojFile "\t\t\};\n"
  # End PBXSourcesBuildPhase section

  # Begin XCBuildConfiguration section
  set aTkDebugProject "${theToolKit}_DebugProject"
  if { ! [info exists aGuidsMap($aTkDebugProject)] } {
    set aGuidsMap($aTkDebugProject) [OS:genGUID "xcd"]
  }

  set aTkReleaseProject "${theToolKit}_ReleaseProject"
  if { ! [info exists aGuidsMap($aTkReleaseProject)] } {
    set aGuidsMap($aTkReleaseProject) [OS:genGUID "xcd"]
  }

  set aTkDebugNativeTarget "${theToolKit}_DebugNativeTarget"
  if { ! [info exists aGuidsMap($aTkDebugNativeTarget)] } {
    set aGuidsMap($aTkDebugNativeTarget) [OS:genGUID "xcd"]
  }

  set aTkReleaseNativeTarget "${theToolKit}_ReleaseNativeTarget"
  if { ! [info exists aGuidsMap($aTkReleaseNativeTarget)] } {
    set aGuidsMap($aTkReleaseNativeTarget) [OS:genGUID "xcd"]
  }

  # Debug target
  puts $aPbxprojFile "\t\t$aGuidsMap($aTkDebugProject) = \{"
  puts $aPbxprojFile "\t\t\tisa = XCBuildConfiguration;"
  puts $aPbxprojFile "\t\t\tbuildSettings = \{"

  puts $aPbxprojFile "\t\t\t\tDEBUG_INFORMATION_FORMAT = dwarf;"
  puts $aPbxprojFile "\t\t\t\tALWAYS_SEARCH_USER_PATHS = NO;"
  if { "$thePlatform" == "ios" } {
    puts $aPbxprojFile "\t\t\t\t\"ARCHS\[sdk=iphoneos\*\]\" = \"\$(ARCHS_STANDARD)\";";
    puts $aPbxprojFile "\t\t\t\t\"ARCHS\[sdk=iphonesimulator\*\]\" = \"x86_64\";";
    puts $aPbxprojFile "\t\t\t\tCLANG_ENABLE_MODULES = YES;"
    puts $aPbxprojFile "\t\t\t\tCLANG_ENABLE_OBJC_ARC = YES;"
  }
  puts $aPbxprojFile "\t\t\t\tARCHS = \"\$(ARCHS_STANDARD_64_BIT)\";"
  puts $aPbxprojFile "\t\t\t\tCLANG_CXX_LIBRARY = \"libc++\";"
  puts $aPbxprojFile "\t\t\t\tCLANG_CXX_LANGUAGE_STANDARD = \"c++0x\";"
  puts $aPbxprojFile "\t\t\t\tCOPY_PHASE_STRIP = NO;"
  puts $aPbxprojFile "\t\t\t\tGCC_C_LANGUAGE_STANDARD = gnu99;"
  puts $aPbxprojFile "\t\t\t\tGCC_DYNAMIC_NO_PIC = NO;"
  puts $aPbxprojFile "\t\t\t\tGCC_ENABLE_OBJC_EXCEPTIONS = YES;"
  puts $aPbxprojFile "\t\t\t\tGCC_OPTIMIZATION_LEVEL = 0;"
  puts $aPbxprojFile "\t\t\t\tGCC_PREPROCESSOR_DEFINITIONS = ("
  puts $aPbxprojFile "\t\t\t\t\t\"DEBUG=1\","
  puts $aPbxprojFile "\t\t\t\t\t\"\$\(inherited\)\","
  puts $aPbxprojFile "\t\t\t\t);"
  puts $aPbxprojFile "\t\t\t\tGCC_SYMBOLS_PRIVATE_EXTERN = NO;"
  puts $aPbxprojFile "\t\t\t\tGCC_VERSION = com.apple.compilers.llvm.clang.1_0;"
  puts $aPbxprojFile "\t\t\t\tGCC_WARN_64_TO_32_BIT_CONVERSION = YES;"
  puts $aPbxprojFile "\t\t\t\tGCC_WARN_ABOUT_RETURN_TYPE = YES;"
  puts $aPbxprojFile "\t\t\t\tGCC_WARN_UNINITIALIZED_AUTOS = YES;"
  puts $aPbxprojFile "\t\t\t\tGCC_WARN_UNUSED_VARIABLE = YES;"
  puts $aPbxprojFile "\t\t\t\tOTHER_LDFLAGS = \"\$(CSF_OPT_LNK64D)\"; "
  if { "$thePlatform" == "ios" } {
    puts $aPbxprojFile "\t\t\t\tONLY_ACTIVE_ARCH = NO;"
    puts $aPbxprojFile "\t\t\t\tSDKROOT = iphoneos;"
  } else {
    puts $aPbxprojFile "\t\t\t\tONLY_ACTIVE_ARCH = YES;"
  }
  puts $aPbxprojFile "\t\t\t\};"

  puts $aPbxprojFile "\t\t\tname = Debug;"
  puts $aPbxprojFile "\t\t\};"

  # Release target
  puts $aPbxprojFile "\t\t$aGuidsMap($aTkReleaseProject) = \{"
  puts $aPbxprojFile "\t\t\tisa = XCBuildConfiguration;"
  puts $aPbxprojFile "\t\t\tbuildSettings = \{"

  puts $aPbxprojFile "\t\t\t\tDEBUG_INFORMATION_FORMAT = \"dwarf-with-dsym\";"
  puts $aPbxprojFile "\t\t\t\tALWAYS_SEARCH_USER_PATHS = NO;"
  if { "$thePlatform" == "ios" } {
    puts $aPbxprojFile "\t\t\t\t\"ARCHS\[sdk=iphoneos\*\]\" = \"\$(ARCHS_STANDARD)\";";
    puts $aPbxprojFile "\t\t\t\t\"ARCHS\[sdk=iphonesimulator\*\]\" = \"x86_64\";";
    puts $aPbxprojFile "\t\t\t\tCLANG_ENABLE_MODULES = YES;"
    puts $aPbxprojFile "\t\t\t\tCLANG_ENABLE_OBJC_ARC = YES;"
  }
  puts $aPbxprojFile "\t\t\t\tARCHS = \"\$(ARCHS_STANDARD_64_BIT)\";"
  puts $aPbxprojFile "\t\t\t\tCLANG_CXX_LIBRARY = \"libc++\";"
  puts $aPbxprojFile "\t\t\t\tCLANG_CXX_LANGUAGE_STANDARD = \"c++0x\";"
  puts $aPbxprojFile "\t\t\t\tCOPY_PHASE_STRIP = YES;"
  puts $aPbxprojFile "\t\t\t\tGCC_C_LANGUAGE_STANDARD = gnu99;"
  puts $aPbxprojFile "\t\t\t\tGCC_ENABLE_OBJC_EXCEPTIONS = YES;"
  puts $aPbxprojFile "\t\t\t\tDEAD_CODE_STRIPPING = NO;"
  puts $aPbxprojFile "\t\t\t\tGCC_OPTIMIZATION_LEVEL = 2;"
  puts $aPbxprojFile "\t\t\t\tGCC_VERSION = com.apple.compilers.llvm.clang.1_0;"
  puts $aPbxprojFile "\t\t\t\tGCC_WARN_64_TO_32_BIT_CONVERSION = YES;"
  puts $aPbxprojFile "\t\t\t\tGCC_WARN_ABOUT_RETURN_TYPE = YES;"
  puts $aPbxprojFile "\t\t\t\tGCC_WARN_UNINITIALIZED_AUTOS = YES;"
  puts $aPbxprojFile "\t\t\t\tGCC_WARN_UNUSED_VARIABLE = YES;"
  puts $aPbxprojFile "\t\t\t\tOTHER_LDFLAGS = \"\$(CSF_OPT_LNK64)\";"
  if { "$thePlatform" == "ios" } {
    puts $aPbxprojFile "\t\t\t\tIPHONEOS_DEPLOYMENT_TARGET = 7.0;"
    puts $aPbxprojFile "\t\t\t\tSDKROOT = iphoneos;"
  }
  puts $aPbxprojFile "\t\t\t\};"
  puts $aPbxprojFile "\t\t\tname = Release;"
  puts $aPbxprojFile "\t\t\};"
  puts $aPbxprojFile "\t\t$aGuidsMap($aTkDebugNativeTarget) = \{"
  puts $aPbxprojFile "\t\t\tisa = XCBuildConfiguration;"
  puts $aPbxprojFile "\t\t\tbuildSettings = \{"
  puts $aPbxprojFile "${anExecExtension}"
  puts $aPbxprojFile "${anExecPrefix}"
  puts $aPbxprojFile "\t\t\t\tGCC_PREPROCESSOR_DEFINITIONS = ("
  foreach aMacro $aTKDefines {
    puts $aPbxprojFile "\t\t\t\t\t${aMacro} ,"
  }
  puts $aPbxprojFile "\t\t\t\t);"

  puts $aPbxprojFile "\t\t\t\tHEADER_SEARCH_PATHS = ("
  foreach anIncPath $anIncPaths {
    puts $aPbxprojFile "\t\t\t\t\t${anIncPath},"
  }
  puts $aPbxprojFile "\t\t\t\t\t\"\$(CSF_OPT_INC)\","
  puts $aPbxprojFile "\t\t\t\t);"

  puts $aPbxprojFile "\t\t\t\tLIBRARY_SEARCH_PATHS = ("
  foreach anLibPath $anLibPaths {
    puts $aPbxprojFile "\t\t\t\t\t${anLibPath},"
  }
  puts $aPbxprojFile "\t\t\t\t);"

  puts $aPbxprojFile "\t\t\t\tOTHER_CFLAGS = ("
  puts $aPbxprojFile "\t\t\t\t\t\"\$(CSF_OPT_CMPL)\","
  puts $aPbxprojFile "\t\t\t\t);"
  puts $aPbxprojFile "\t\t\t\tOTHER_CPLUSPLUSFLAGS = ("
  puts $aPbxprojFile "\t\t\t\t\t\"\$(OTHER_CFLAGS)\","
  puts $aPbxprojFile "\t\t\t\t);"
  puts $aPbxprojFile "\t\t\t\tPRODUCT_NAME = \"\$(TARGET_NAME)\";"
  set anUserHeaderSearchPath "\t\t\t\tUSER_HEADER_SEARCH_PATHS = \""
  foreach anIncPath $anIncPaths {
    append anUserHeaderSearchPath " ${anIncPath}"
  }
  append anUserHeaderSearchPath "\";"
  puts $aPbxprojFile $anUserHeaderSearchPath
  puts $aPbxprojFile "${aWrapperExtension}"
  puts $aPbxprojFile "\t\t\t\};"
  puts $aPbxprojFile "\t\t\tname = Debug;"
  puts $aPbxprojFile "\t\t\};"
  puts $aPbxprojFile "\t\t$aGuidsMap($aTkReleaseNativeTarget) = \{"
  puts $aPbxprojFile "\t\t\tisa = XCBuildConfiguration;"
  puts $aPbxprojFile "\t\t\tbuildSettings = \{"
  puts $aPbxprojFile "${anExecExtension}"
  puts $aPbxprojFile "${anExecPrefix}"
  puts $aPbxprojFile "\t\t\t\tGCC_PREPROCESSOR_DEFINITIONS = ("
  foreach aMacro $aTKDefines {
    puts $aPbxprojFile "\t\t\t\t\t${aMacro} ,"
  }
  puts $aPbxprojFile "\t\t\t\t);"
  puts $aPbxprojFile "\t\t\t\tHEADER_SEARCH_PATHS = ("
  foreach anIncPath $anIncPaths {
    puts $aPbxprojFile "\t\t\t\t\t${anIncPath},"
  }
  puts $aPbxprojFile "\t\t\t\t\t\"\$(CSF_OPT_INC)\","
  puts $aPbxprojFile "\t\t\t\t);"

  puts $aPbxprojFile "\t\t\t\tLIBRARY_SEARCH_PATHS = ("
  foreach anLibPath $anLibPaths {
    puts $aPbxprojFile "\t\t\t\t\t${anLibPath},"
  }
  puts $aPbxprojFile "\t\t\t\t);"

  puts $aPbxprojFile "\t\t\t\tOTHER_CFLAGS = ("
  puts $aPbxprojFile "\t\t\t\t\t\"\$(CSF_OPT_CMPL)\","
  puts $aPbxprojFile "\t\t\t\t);"
  puts $aPbxprojFile "\t\t\t\tOTHER_CPLUSPLUSFLAGS = ("
  puts $aPbxprojFile "\t\t\t\t\t\"\$(OTHER_CFLAGS)\","
  puts $aPbxprojFile "\t\t\t\t);"
  puts $aPbxprojFile "\t\t\t\tPRODUCT_NAME = \"\$(TARGET_NAME)\";"
  puts $aPbxprojFile $anUserHeaderSearchPath
  puts $aPbxprojFile "${aWrapperExtension}"
  puts $aPbxprojFile "\t\t\t\};"
  puts $aPbxprojFile "\t\t\tname = Release;"
  puts $aPbxprojFile "\t\t\};\n"
  # End XCBuildConfiguration section

  # Begin XCConfigurationList section
  puts $aPbxprojFile "\t\t$aGuidsMap($aTkBuildCfgListProj) = \{"
  puts $aPbxprojFile "\t\t\tisa = XCConfigurationList;"
  puts $aPbxprojFile "\t\tbuildConfigurations = ("
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($aTkDebugProject) ,"
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($aTkReleaseProject) ,"
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\tdefaultConfigurationIsVisible = 0;"
  puts $aPbxprojFile "\t\t\tdefaultConfigurationName = Release;"
  puts $aPbxprojFile "\t\t\};"
  puts $aPbxprojFile "\t\t$aGuidsMap($aTkBuildCfgListNativeTarget) = \{"
  puts $aPbxprojFile "\t\t\tisa = XCConfigurationList;"
  puts $aPbxprojFile "\t\t\tbuildConfigurations = ("
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($aTkDebugNativeTarget) ,"
  puts $aPbxprojFile "\t\t\t\t$aGuidsMap($aTkReleaseNativeTarget) ,"
  puts $aPbxprojFile "\t\t\t);"
  puts $aPbxprojFile "\t\t\tdefaultConfigurationIsVisible = 0;"
  puts $aPbxprojFile "\t\t\tdefaultConfigurationName = Release;"
  puts $aPbxprojFile "\t\t\};\n"
  # End XCConfigurationList section

  puts $aPbxprojFile "\t\};"
  puts $aPbxprojFile "\trootObject = $aGuidsMap($aTkProjectObj) ;"
  puts $aPbxprojFile "\}"

  close $aPbxprojFile
}

proc osutils:xcdx { theOutDir theExecutable theGuidsMap } {
  set aUsername [exec whoami]

  # Creating folders for Xcode project file.
  set anExecutableDir "${theOutDir}/${theExecutable}.xcodeproj"
  wokUtils:FILES:mkdir $anExecutableDir
  if { ! [file exists $anExecutableDir] } {
    puts stderr "Error: Could not create project directory \"$anExecutableDir\""
    return
  }

  set aUserDataDir "${anExecutableDir}/xcuserdata"
  wokUtils:FILES:mkdir $aUserDataDir
  if { ! [file exists $aUserDataDir] } {
    puts stderr "Error: Could not create xcuserdata directorty in \"$anExecutableDir\""
    return
  }

  set aUserDataDir "${aUserDataDir}/${aUsername}.xcuserdatad"
  wokUtils:FILES:mkdir $aUserDataDir
  if { ! [file exists $aUserDataDir] } {
    puts stderr "Error: Could not create ${aUsername}.xcuserdatad directorty in \"$anExecutableDir\"/xcuserdata"
    return
  }

  set aSchemesDir "${aUserDataDir}/xcschemes"
  wokUtils:FILES:mkdir $aSchemesDir
  if { ! [file exists $aSchemesDir] } {
    puts stderr "Error: Could not create xcschemes directorty in \"$aUserDataDir\""
    return
  }
  # End folders creation.

  # Generating GUID for tookit.
  upvar $theGuidsMap aGuidsMap
  if { ! [info exists aGuidsMap($theExecutable)] } {
    set aGuidsMap($theExecutable) [OS:genGUID "xcd"]
  }

  # Creating xcscheme file for toolkit from template.
  set aXcschemeTmpl [osutils:readtemplate "xcscheme" "xcode"]
  regsub -all -- {__TOOLKIT_NAME__} $aXcschemeTmpl $theExecutable aXcschemeTmpl
  regsub -all -- {__TOOLKIT_GUID__} $aXcschemeTmpl $aGuidsMap($theExecutable) aXcschemeTmpl
  set aXcschemeFile [open "$aSchemesDir/${theExecutable}.xcscheme"  "w"]
  puts $aXcschemeFile $aXcschemeTmpl
  close $aXcschemeFile

  # Creating xcschememanagement.plist file for toolkit from template.
  set aPlistTmpl [osutils:readtemplate "plist" "xcode"]
  regsub -all -- {__TOOLKIT_NAME__} $aPlistTmpl $theExecutable aPlistTmpl
  regsub -all -- {__TOOLKIT_GUID__} $aPlistTmpl $aGuidsMap($theExecutable) aPlistTmpl
  set aPlistFile [open "$aSchemesDir/xcschememanagement.plist"  "w"]
  puts $aPlistFile $aPlistTmpl
  close $aPlistFile
}

# Returns available Windows SDKs versions
proc osutils:sdk { theSdkMajorVer {isQuietMode false} {theSdkDirectories {}} } {
  if { ![llength ${theSdkDirectories}] } {
    foreach anEnvVar { "ProgramFiles" "ProgramFiles\(x86\)" "ProgramW6432" } {
      if {[ info exists ::env(${anEnvVar}) ]} {
        lappend theSdkDirectories "$::env(${anEnvVar})/Windows Kits/${theSdkMajorVer}/Include"
      }
    }
  }

  set sdk_versions {}
  foreach sdk_dir ${theSdkDirectories} {
    if { [file isdirectory ${sdk_dir}] } {
      lappend sdk_versions [glob -tails -directory "${sdk_dir}" -type d *]
    }
  }

  if {![llength ${sdk_versions}] && !${isQuietMode}} {
    error "Error : Could not find Windows SDK ${theSdkMajorVer}"
  }

  return [join [lsort -unique ${sdk_versions}] " "]
}

# Generate global properties to Visual Studio project file for UWP solution
proc osutils:uwp:proj { isUWP theProjTmpl } {

  set uwp_properties ""
  set uwp_generate_metadata ""
  set uwp_app_container ""

  set format_template ""

  if { $isUWP } {
    set sdk_versions [osutils:sdk 10]
    set sdk_max_ver [lindex ${sdk_versions} end]

    set uwp_properties "<DefaultLanguage>en-US</DefaultLanguage>\n   \
<ApplicationType>Windows Store</ApplicationType>\n   \
<ApplicationTypeRevision>10.0</ApplicationTypeRevision>\n   \
<MinimumVisualStudioVersion>14.0</MinimumVisualStudioVersion>\n   \
<AppContainerApplication>true</AppContainerApplication>\n   \
<WindowsTargetPlatformVersion>${sdk_max_ver}</WindowsTargetPlatformVersion>\n   \
<WindowsTargetPlatformMinVersion>${sdk_max_ver}</WindowsTargetPlatformMinVersion>"

    set uwp_generate_metadata        "<GenerateWindowsMetadata>false</GenerateWindowsMetadata>"

    regsub -all -- {[\r\n\s]*<BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>} ${theProjTmpl} "" theProjTmpl
  } else {
    set format_template "\[\\r\\n\\s\]*"
  }

  regsub -all -- "${format_template}__UWP_PROPERTIES__"        ${theProjTmpl} "${uwp_properties}" theProjTmpl
  regsub -all -- "${format_template}__UWP_GENERATE_METADATA__" ${theProjTmpl} "${uwp_generate_metadata}" theProjTmpl

  return ${theProjTmpl}
}

# Report all files found in package directory but not listed in FILES
proc osutils:checksrcfiles { theUnit } {
  global path
  set aCasRoot [file normalize ${path}]

  if {![file isdirectory ${aCasRoot}]} {
    puts "OCCT directory is not defined correctly: ${aCasRoot}"
    return
  }

  set anUnitAbsPath [file normalize "${aCasRoot}/src/${theUnit}"]

  if {[file exists "${anUnitAbsPath}/FILES"]} {
    set aFilesFile [open "${anUnitAbsPath}/FILES" rb]
    set aFilesFileList [split [read ${aFilesFile}] "\n"]
    close ${aFilesFile}

    set aFilesFileList [lsearch -inline -all -not -exact ${aFilesFileList} ""]

    # report all files not listed in FILES
    set anAllFiles [glob -tails -nocomplain -dir ${anUnitAbsPath} "*"]
    foreach aFile ${anAllFiles} {
      if { "${aFile}" == "FILES" } {
        continue
      }
      if { [lsearch -exact ${aFilesFileList} ${aFile}] == -1 } {
        puts "Warning: file ${anUnitAbsPath}/${aFile} is not listed in ${anUnitAbsPath}/FILES!"
      }
    }
  }
}
