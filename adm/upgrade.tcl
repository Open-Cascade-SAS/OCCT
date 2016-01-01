# This script provides commands for upgrade of OCCT and software based on it
# to a newer version of OCCT (7.0)


# source code for upgrading
set ArgName(HelpInfo)       "h"

set ArgName(SourceCode)     "src"
set ArgName(IncPath)        "inc"

set ArgName(IncExtension)   "incext"
set ArgName(SrcExtension)   "srcext"

set ArgName(RTTI)           "rtti"

set ArgName(CStyleCastHandle) "handlecast"
set ArgName(All)            "all"

set ArgName(Handle)         "handle"
set ArgName(TCollection)    "tcollection"

set ArgName(CompatibleMode) "compat"

set ArgName(Recurse)        "recurse"
set ArgName(Rename)         "rename"

set ArgName(CheckOnly)      "check"
set ArgName(WLog)           "wlog"
set ArgName(Log)            "log"

proc HelpInformation {} {
  global ArgName
  global DataSectionName

  loginfo "Tool for upgrade of application code from older versions of OCCT."
  loginfo ""
  loginfo "Required parameter:"
  loginfo "  -$ArgName(SourceCode)=<path>  - path to sources to upgrade"
  loginfo ""
  loginfo "File search options:"
  loginfo "  -$ArgName(IncPath)=<path>  - path to header files of OCCT or other used libraries"
  loginfo "  -$ArgName(Recurse)     - process all subfolders of '-$ArgName(SourceCode)' and '-$ArgName(IncPath)'"
  loginfo "  -$ArgName(SrcExtension)=cxx,cpp       - extensions of source files"
  loginfo "  -$ArgName(IncExtension)=hxx,h,lxx,gxx - extensions of header files"
  loginfo ""
  loginfo "Upgrade options:"
  loginfo "  -$ArgName(All)         - do all upgrades (if neither of below options are given)"
  loginfo "  -$ArgName(RTTI)        - adapt code for changes in type system (RTTI) in OCCT 7.0"
  loginfo "  -$ArgName(Handle)      - adapt code for changes in OCCT Handle"
  loginfo "  -$ArgName(TCollection) - replace forward declaration of TCollection classes by #include"
  loginfo "  -$ArgName(CStyleCastHandle)  - replace c-style casts of Handle by DownCast()"
  loginfo "  -$ArgName(Rename)      - apply renaming of classes"
  loginfo ""
  loginfo "Advanced options:"
  loginfo "  -$ArgName(CompatibleMode)      - preserve old RTTI macros for compatability with OCCT 6.x"
  loginfo "  -$ArgName(CheckOnly)       - do check only, no modifications will be made"
  loginfo "  -$ArgName(WLog)        - show gui log of upgrade process"
  loginfo "  -$ArgName(Log)=<file path> - put the log into a file"

  return
}

proc ParseArgs {theArgValues theArgs {theRemoveFromArgs "false"}} {
  upvar $theArgValues anArgValues

  global ArgName
  global DataSectionName

  # help information
  set anArgValues(HelpInfo) [SeekArg $ArgName(HelpInfo) theArgs "false" $theRemoveFromArgs]

  # sources that will be upgraded
  set anArgValues(SourceCode) [SeekArg $ArgName(SourceCode) theArgs "" $theRemoveFromArgs]

  set anArgValues(IncExtension) [SeekArg $ArgName(IncExtension) theArgs "h,hpp,hxx,gxx,lxx" $theRemoveFromArgs]
  set anArgValues(SrcExtension) [SeekArg $ArgName(SrcExtension) theArgs "c,cpp,cxx"         $theRemoveFromArgs]

  # inc folder
  set anArgValues(IncPath) [SeekArg $ArgName(IncPath) theArgs "$anArgValues(SourceCode)" $theRemoveFromArgs]

  set anArgValues(RTTI)         [SeekArg $ArgName(RTTI)                 theArgs "false" $theRemoveFromArgs]
  set anArgValues(CStyleCastHandle) [SeekArg $ArgName(CStyleCastHandle) theArgs "false" $theRemoveFromArgs]

  set anArgValues(Handle)       [SeekArg $ArgName(Handle)               theArgs "false" $theRemoveFromArgs]
  set anArgValues(TCollection)  [SeekArg $ArgName(TCollection)          theArgs "false" $theRemoveFromArgs]

  set anArgValues(Rename) [SeekArg $ArgName(Rename) theArgs "false" $theRemoveFromArgs]

  set aHasAgentArgs [expr {$anArgValues(RTTI)   || $anArgValues(CStyleCastHandle) || \
                           $anArgValues(Handle) || $anArgValues(TCollection)}     || \
                           $anArgValues(Rename)]

  set anArgValues(All)       [SeekArg $ArgName(All) theArgs [expr {!$aHasAgentArgs}] $theRemoveFromArgs]

  set anArgValues(Recurse)        [SeekArg $ArgName(Recurse)        theArgs "false" $theRemoveFromArgs]
  set anArgValues(CompatibleMode) [SeekArg $ArgName(CompatibleMode) theArgs "false" $theRemoveFromArgs]
  set anArgValues(CheckOnly)      [SeekArg $ArgName(CheckOnly)      theArgs "false" $theRemoveFromArgs]
  set anArgValues(WLog)           [SeekArg $ArgName(WLog)           theArgs "false" $theRemoveFromArgs]

  set anArgValues(Log)            [SeekArg $ArgName(Log)            theArgs ""      $theRemoveFromArgs]

  return $theArgs
}

proc SeekArg {theSoughtArgName theArgs {theDefaultArgValue ""} {theRemoveFromArgs false}} {
  upvar ${theArgs} anArgs

  set aBooleanValue [string is boolean -strict $theDefaultArgValue]

  set anArgValues {}

  set anArgsIndex -1
  foreach anArg $anArgs {
    incr anArgsIndex

    if {[regexp -- "-${theSoughtArgName}\=\(.\*\)" $anArg dummy anArgValue]} {
      set anArgValue [regsub -all {\\} $anArgValue {/}]
      if {$theRemoveFromArgs} {
        set anArgs [lreplace $anArgs $anArgsIndex $anArgsIndex]
        incr anArgsIndex -1
      }
      if {"$anArgValue" != ""} {
        lappend anArgValues $anArgValue  
      } else {
        logwarn "'-${theSoughtArgName}' is skipped because it has empty value"
      }
    } elseif [string match "-${theSoughtArgName}" $anArg] {
      if {$theRemoveFromArgs} {
        set anArgs [lreplace $anArgs $anArgsIndex $anArgsIndex]
      }
      # skip non-boolean empty argument; do not break the foreach loop
      if {$aBooleanValue} {
        lappend anArgValues "true"
        break
      } else {
        logwarn "'-${theSoughtArgName}' is skipped because it has empty value"
      }
    }
  }

  # return boolean value as string
  if {$aBooleanValue} {
    if {[llength $anArgValues] > 0} {
      return [lindex $anArgValues 0]
    } else {
      return $theDefaultArgValue
    }
  }

  if {[llength $anArgValues] == 0 && "$theDefaultArgValue" != ""} {
    lappend anArgValues $theDefaultArgValue
  }

  return $anArgValues
}

# section names in the data file
set DataSectionName(TCollection)  "tcollection"
set DataSectionName(Rename)       "rename"

proc DataFileName {} {
  return [file join [file dirname [info script]] upgrade.dat]
}

proc IsDataFileExist {} {
  return [file exists [DataFileName]]
}

proc ReadFromDataFile {theSectionName} {
  if {![file exists [DataFileName]]} {
    return
  }

  set aFileContent [ReadFileToList [DataFileName] aFileRawContent aDataEOL]

  set aSectionValueList {}

  set anIsSection false
  set aSectionPattern {^ *\[ *([A-Za-z0-9_\.]*) *\]+}
  foreach aLine $aFileContent {
    if {[regexp -- $aSectionPattern $aLine dummy aSectionName]} {
      if {"$aSectionName" == "$theSectionName"} {
        set anIsSection true
        continue
      } elseif {$anIsSection == true} {
        set anIsSection false
        break
      }
    }

    if {$anIsSection == true} {
      set aTrimmedLine [string trimright $aLine]
      if {"$aTrimmedLine" != ""} {
        lappend aSectionValueList $aTrimmedLine  
      }
    }
  }

  return $aSectionValueList
}

proc SaveToDataFile {theSectionName theSectionContent} {
  if {![file exists [DataFileName]]} {
    return
  }

  set aFileContent [ReadFileToList [DataFileName] aFileRawContent aDataEOL]

  set aLinesBefore {}
  set aLinesAfter  {}

  set anIsSection false
  set anIsSectionBefore true
  set anIsSectionAfter  false

  set aSectionPattern {^ *\[ *([A-Za-z0-9_\.]*) *\]+}
  foreach aLine $aFileContent {
    if {$anIsSectionBefore} {
      lappend aLinesBefore $aLine  
    }

    if {[regexp -- $aSectionPattern $aLine dummy aSectionName]} {
      if {"$aSectionName" == "$theSectionName"} {
        set anIsSectionBefore false
        set anIsSection true
      } elseif {$anIsSection == true} {
        set anIsSection false
        set anIsSectionAfter true
      }
    }

    if {$anIsSection == true} {
      continue
    }

    if {$anIsSectionAfter} {
      lappend aLinesAfter $aLine  
    }
  }

  # write to file
  SaveListToFile [DataFileName] [list {*}$aLinesBefore {*}$theSectionContent {*}$aLinesAfter] $aDataEOL
}

# Main tool, accepts path to location of source tree to be upgraded.
proc upgrade {args} {

  global ArgName
  global LogFilePath
  global DataSectionName

  set theArgs $args
  set anUnparsedArgs [ParseArgs anArgValues $theArgs "true"]

  if {"$anUnparsedArgs" != ""} {
    logerr "undefined arguments: $anUnparsedArgs"
    loginfo "use -$ArgName(HelpInfo) to show all the arguments"
    return
  }

  if {$anArgValues(HelpInfo) || [llength $anArgValues(SourceCode)] == 0} {
    HelpInformation
    return
  }

  if {"$anArgValues(Log)" != ""} {
    set LogFilePath $anArgValues(Log)

    # clean file before writing
    if {[file exists "$LogFilePath"]} {
      set fd [open "$LogFilePath" r+]
      chan truncate $fd 0
      close $fd
    }
  }

  if {$anArgValues(WLog)} {
    _create_logger
  }

  # collect src directory structure (all subdirs)
  set anIncPaths {}
  foreach aSrcDir $anArgValues(SourceCode) {
    lappend anIncPaths $aSrcDir
    foreach aSubSrcDir [CollectDirStructure $aSrcDir] {
      lappend anIncPaths $aSubSrcDir
    } 
  }

  foreach anIncDir $anArgValues(IncPath) {
    lappend anIncPaths $anIncDir 
    foreach aSubIncDir [CollectDirStructure $anIncDir] {
      lappend anIncPaths $aSubIncDir 
    } 
  }

  set anIncPaths [lsort -unique -dictionary $anIncPaths]
  # end the collect

  set aRawNewNames [ReadFromDataFile $DataSectionName(Rename)]
  foreach aRawName $aRawNewNames {
    set aRawName [split $aRawName " "]
    if {[llength $aRawName] > 1} {
      # set aNewNames (old name) [new name]
      set aNewNames([lindex ${aRawName} 0]) [lindex ${aRawName} 1]
    }
  }

  set aDoRename true
  if {[llength [array names aNewNames]] == 0} {
    set aDoRename false

    logwarn "renaming skipped. there is no class names to rename"
    logwarn "see the content of [DataFileName] file, $DataSectionName(Rename) section"
  }

  set aProcNameWithArgs "[lindex [info level 0] 0]"
  foreach anArgName [array names anArgValues] {
    if [string is boolean -strict $anArgValues($anArgName)] {
      if [string is true "$anArgValues($anArgName)"] {
        set aProcNameWithArgs [format "$aProcNameWithArgs -%s" "$ArgName($anArgName)"]  
      }
    } else {
      set aProcNameWithArgs [format "$aProcNameWithArgs -%s" "$ArgName($anArgName)=$anArgValues($anArgName)"]
    }    
  }

  loginfo "$aProcNameWithArgs" 

  # merge all processed extensions
  set anExtensions "$anArgValues(SrcExtension),$anArgValues(IncExtension)"

  set aSourceCodePaths $anArgValues(SourceCode)
  while {[llength $aSourceCodePaths]} {
    set aSourceCodePaths [lassign $aSourceCodePaths aProcessedPath]

    loginfo "Processing: $aProcessedPath"

    if {$anArgValues(All) || $anArgValues(RTTI)} {
      ConvertRtti $aProcessedPath \
                  $anIncPaths \
                  $anArgValues(CheckOnly) \
                  $anArgValues(CompatibleMode) \
                  $anArgValues(IncExtension) \
                  $anArgValues(SrcExtension)
    }

    if {$anArgValues(All) || $anArgValues(Handle)} {
      ConvertHandle $aProcessedPath $anIncPaths $anArgValues(CheckOnly) $anExtensions
    }

    if {$anArgValues(All) || $anArgValues(TCollection)} {
      ConvertTColFwd $aProcessedPath $anArgValues(IncExtension)
    }

    if {$anArgValues(All) || $anArgValues(CStyleCastHandle)} {
      ConvertCStyleHandleCast $aProcessedPath $anExtensions $anArgValues(CheckOnly)
    }

    if {$anArgValues(All) || $anArgValues(Rename)} {
      if {$aDoRename} {
        Rename $aProcessedPath $anExtensions aNewNames $anArgValues(CheckOnly)
      }
    }

    # Recurse processing
    if {$anArgValues(Recurse)} {
      lappend aSourceCodePaths {*}[glob -nocomplain -directory $aProcessedPath -type d *]
    }
  }
}

# search and rename the indeces (old names) of @theNewNames with their values (new ones)
#  processes files that have @theExtensions only in @thePath folder
proc Rename {thePath theExtensions theNewNames theCheckMode} {
  upvar $theNewNames aNewNames

  set aNames [array names aNewNames]

  foreach aFile [glob -nocomplain -type f -directory $thePath *.{$theExtensions}] {
#    loginfo "$aFile processing"
    set aFileContent [ReadFileToRawText $aFile]

    set aHasChanges false
    foreach aName $aNames {
      set anIndexInRow 0
      set aClassNameTmpl "\\m$aName\\M"
      while { [regexp -start $anIndexInRow -indices -lineanchor $aClassNameTmpl $aFileContent aFoundClassNameLoc] } {
        set anIndexInRow [lindex $aFoundClassNameLoc 1]

        if {$theCheckMode} {
          logwarn "Warning: $aFile contains $aName"
          break
        } else {
          set aHasChanges true
          ReplaceSubString aFileContent $aFoundClassNameLoc "$aNewNames($aName)" anIndexInRow
          incr anIndexInRow -1
        }
      }
    }

    if {$aHasChanges} {
      SaveTextToFile $aFile $aFileContent
    }
  }
}

# @thePackagePath eather file or folder. If it is a folder, 
# all files with @theHeaderExtensions are processed.
# "fwd.tcollection" section from upgrade.ini file is used to find out what 
# classes have been converted and, thus, what forward declarations can be replaced
proc ConvertTColFwd {thePackagePath theHeaderExtensions} {
  global DataSectionName

  # Note: the content of theHeaderExtensions should have
  #       further form (values separated by comma): "ext1,ext2,ext3"
  # this form will be used below in reg expression to collect all header files

  if {! [file exists $thePackagePath]} {
    logerr "Error: $thePackagePath does not exist"
    return 
  }

  # read the list of already converted TCollection classes
  if [IsDataFileExist] {
    set aConvertedTColClasses [ReadFromDataFile $DataSectionName(TCollection)]
  } else {
    logerr "[DataFileName] file  of upgrade process does not exist"
    return
  }

  # pattern that will be used
  set aForwardDeclPattern {^ *class *([A-Za-z0-9_/\.]+) *;}

  set aTargetPaths ${thePackagePath}
  while {[llength $aTargetPaths]} {
    set aTargetPaths [lassign $aTargetPaths aProcessedPath]

    # if aProcessedPath is a folder, collect all files with $theHeaderExtensions from it
    set aProcessedHeaders ${aProcessedPath}
    if {[file isdirectory $aProcessedPath]} {
      # get all header files
      set aProcessedHeaders [glob -nocomplain -type f -directory $aProcessedPath *.{$theHeaderExtensions}]
    }

    foreach aHeader $aProcessedHeaders {
      set aHeaderLineIndex -1
      set aHeaderContentUpdated false

      # read the content of the header file
      set aHeaderContent [ReadFileToList $aHeader aHeaderRawContent aHeaderEOL]
      
      # remove _isMulti variable that used in _check_line
      set _isMulti false

      foreach aHeaderContentLine $aHeaderContent {
        incr aHeaderLineIndex

        # remove _cmnt variable that used in _check_line
        set _cmnt ""

        set aHeaderContentLine [_check_line $aHeaderContentLine]
        if {[regexp {^ *class *([A-Za-z0-9_/\.]+) *;} $aHeaderContentLine dummy aForwardDeclClass]} {
          if {[lsearch $aConvertedTColClasses $aForwardDeclClass] != -1} {
            set aHeaderContentUpdated true
            set aHeaderContentRow "\#include <$aForwardDeclClass.hxx>"
            set aHeaderContent [lreplace $aHeaderContent $aHeaderLineIndex $aHeaderLineIndex $aHeaderContentRow]
          }
        }
      }

      if {$aHeaderContentUpdated} {
        loginfo "$aHeader updated"
        SaveListToFile $aHeader $aHeaderContent $aHeaderEOL
      }
    }
  }
}

# try to find source file corresponding to the specified header and either
# inject macro IMPLEMENT_STANDARD_RTTIEXT in it, or check it already present,
# and depending on this, return suffix to be used for corresponding macro
# DEFINE_STANDARD_RTTI... (either inline or out-of-line variant)
proc DefineExplicitRtti {hxxfile class base theSourceExtensions} {
  # if current file is not a header (by extension), exit with "inline" variant
  # (there is no need to bother with out-of-line instantiations for local class)
  set ext [string range [file extension $hxxfile] 1 end]
  if { [lsearch -exact [split $theSourceExtensions ,] $ext] >=0 } {
    return "_INLINE"
  }

  # try to find source file with the same name but source-type extension 
  # in the same folder
  set filename [file rootname $hxxfile]
  foreach ext [split $theSourceExtensions ,] {
#    puts "Checking ${filename}.$ext"
    if { ! [file readable ${filename}.$ext] } { continue }

    # check the file content
    set aFileContent [ReadFileToList ${filename}.$ext aFileRawContent aEOL]

    # try to find existing macro IMPLEMENT_STANDARD_RTTIEXT and check that 
    # it is consistent
    foreach line $aFileContent {
      if { [regexp "^\\s*IMPLEMENT_STANDARD_RTTIEXT\\s*\\(\\s*$class\\s*,\\s*(\[A-Za-z0-9_\]+)\\s*\\)" $line res impl_base] } {
        # implementation is in place, just report warning if second argument
        # is different
        if { $base != $impl_base } {
          logwarn "Warning in ${filename}.$ext: second argument of macro"
          logwarn "        IMPLEMENT_STANDARD_RTTIEXT($class,$impl_base)"
          logwarn "        is not the same as detected base class, $base"
        }
        return "EXT"
      }
    }

    # inject a new macro before the first non-empty, non-comment, and 
    # non-preprocessor line
    set aNewFileContent {}
    set injected 0
    set inc_found 0
    foreach line $aFileContent {
      if { ! $injected } {
        # add macro before first non-empty line after #includes
        if { [regexp {^\s*$} $line] } {
        } elseif { [regexp {^\s*\#\s*include} $line] } {
          set inc_found 1
        } elseif { $inc_found } {
          set injected 1
          lappend aNewFileContent "IMPLEMENT_STANDARD_RTTIEXT($class,$base)"
          if { ! [regexp "^IMPLEMENT_" $line] } {
            lappend aNewFileContent ""
          }
        }
      }
      lappend aNewFileContent $line
    }
    if { ! $injected } {
      lappend aNewFileContent "IMPLEMENT_STANDARD_RTTIEXT($class,$base)"
    }
    SaveListToFile ${filename}.$ext $aNewFileContent $aEOL

    return "EXT"
  }

  logwarn "Warning in ${hxxfile}: cannot find corresponding source file,"
  logwarn "           will use inline version of DEFINE_STANDARD_RTTI"
  return "_INLINE"
}

# Parse source files and:
#
# - add second argument to macro DEFINE_STANDARD_RTTI specifying first base 
#   class found in the class declaration;
# - replace includes of Standard_DefineHandle.hxx by Standard_Type.hxx;
# - add #includes for all classes used as argument to macro
#   STANDARD_TYPE(), except of already included ones
#
# If theCompatibleMode is false, in addition:
# - removes macros IMPLEMENT_DOWNCAST() and IMPLEMENT_STANDARD_*();
proc ConvertRtti {theProcessedPath theIncPaths theCheckMode theCompatibleMode \
                  theHeaderExtensions theSourceExtensions} {

  # iterate by header and source files
  foreach aProcessedFile [glob -nocomplain -type f -directory $theProcessedPath *.{$theHeaderExtensions,$theSourceExtensions}] {
    set aProcessedFileName [file tail $aProcessedFile]

    set aProcessedFileContent [ReadFileToRawText $aProcessedFile]

    # find all declarations of classes with public base in this header file;
    # the result is stored in array inherits(class)
    set index 0
    array unset inherits
    set pattern_class {^\s*class\s+([A-Za-z_0-9:]+)\s*:\s*public\s+([A-Za-z_0-9:]+)\s*([,]?)}
    while {[regexp -start $index -indices -lineanchor $pattern_class $aProcessedFileContent location class base comma]} {
      set index [lindex $location 1]

      set class [eval string range \$aProcessedFileContent $class]
      set base  [eval string range \$aProcessedFileContent $base]

      if { [info exists inherits($class)] } {
        set inherits($class,multiple) "found multiple declarations of class $class"
      } else {
        if { [lindex $comma 0] <= [lindex $comma 1] } {
          set inherits($class,multiple) "class $class uses multiple inheritance"
        }
        set inherits($class) $base
      }
    }

    set change_flag 0

    # find all instances of DEFINE_STANDARD_RTTI with single or two arguments
    set index 0
    set pattern_rtti {^(\s*DEFINE_STANDARD_RTTI)([_A-Z]+)?\s*\(\s*([A-Za-z_0-9,\s]+)\s*\)}
    while { [regexp -start $index -indices -lineanchor $pattern_rtti \
                    $aProcessedFileContent location start suffix clist] } {
      set index [lindex $location 1]

      set start  [eval string range \$aProcessedFileContent $start]
      set suffix [eval string range \$aProcessedFileContent $suffix]
      set clist  [split [eval string range \$aProcessedFileContent $clist] ,]

      if { [llength $clist] == 1 } {
        set class [string trim [lindex $clist 0]]
        if { [info exists inherits($class)] } {
          if { ! $theCheckMode } {
            if { [info exists inherits($class,multiple)] } {
              logwarn "Warning in $aProcessedFileName: $inherits($class,multiple);"
              logwarn "macro DEFINE_STANDARD_RTTI is changed assuming it inherits $inherits($class), please check!"
            }
            set change_flag 1
            ReplaceSubString aProcessedFileContent $location \
                             "${start}EXT($class,$inherits($class))" index
          }
        } else {
          logwarn "Error in $aProcessedFile: Macro DEFINE_STANDARD_RTTI used for class $class whose declaration is not found in this file, cannot fix"
        }
      } elseif { [llength $clist] == 2 } {
        set class [string trim [lindex $clist 0]]
        set base  [string trim [lindex $clist 1]]
        if { ! [info exists inherits($class)] } {
          logwarn "Warning in $aProcessedFile: Macro DEFINE_STANDARD_RTTI used for class $class whose declaration is not found in this file"
        } elseif { $base != $inherits($class) && ! [info exists inherits($class,multiple)] } {
          logwarn "Warning in $aProcessedFile: Second argument in macro DEFINE_STANDARD_RTTI for class $class is $base while $class seems to inherit from $inherits($class)"
        }
        # convert intermediate version of macro DEFINE_STANDARD_RTTI
        # with two arguments to either _INLINE or EXT variant
        if { ! $theCheckMode && "$suffix" == "" } {
          set change_flag 1
          # try to inject macro IMPLEMENT_STANDARD_RTTIEXT in the 
          # corresponding source file (or check it already present),
          # and depending on this, use either inline or out-of-line variant
          set rtti_suffix [DefineExplicitRtti $aProcessedFile $class $base $theSourceExtensions]
          ReplaceSubString aProcessedFileContent $location \
                           "${start}${rtti_suffix}($class,$base)" index
        }
      }
    }

    # replace includes of Standard_DefineHandle.hxx by Standard_Type.hxx
#    set index 0
#    set pattern_definehandle {\#\s*include\s*<\s*Standard_DefineHandle.hxx\s*>}
#    while { [regexp -start $index -indices -lineanchor $pattern_definehandle $aProcessedFileContent location] } {
#      set index [lindex $location 1]
#      if { ! $theCheckMode } {
#        set change_flag 1
#        ReplaceSubString aProcessedFileContent $location "\#include <Standard_Type.hxx>" index
#        incr index -1
#      } else {
#        logwarn "Warning: $aProcessedFile contains obsolete forward declarations of Handle classes"
#        break
#      }
#    }

    # remove macros IMPLEMENT_DOWNCAST() and IMPLEMENT_STANDARD_*();
    if { ! $theCompatibleMode } {
      set index 0
      set first_newline \n\n
      set pattern_implement {\\?\n\s*IMPLEMENT_(DOWNCAST|STANDARD_[A-Z_]+|HARRAY1|HARRAY2|HUBTREE|HEBTREE|HSEQUENCE)\s*\([A-Za-z0-9_ ,]*\)\s*;?}
      while { [regexp -start $index -indices -lineanchor $pattern_implement $aProcessedFileContent location macro] } {
        set index [lindex $location 1]
        # macro IMPLEMENT_STANDARD_RTTIEXT is retained
        if { [eval string range \$aProcessedFileContent $macro] == "STANDARD_RTTIEXT" } {
          continue
        }
        if { ! $theCheckMode } {
          set change_flag 1
          ReplaceSubString aProcessedFileContent $location $first_newline index
#          set first_newline ""
          incr index -1
        } else {
          logwarn "Warning: $aProcessedFile contains deprecated macros IMPLEMENT_*"
          break
        }
      }
    }

    # find all uses of macro STANDARD_TYPE and method DownCast and ensure that
    # argument class is explicitly included
    set pattern_incbeg {\s*#\s*include\s*[\"<]\s*([A-Za-z0-9_/]*/)?}
    set pattern_incend {[.][a-zA-Z]+\s*[\">]}
    set index 0
    set addtype {}
    set pattern_type1 {STANDARD_TYPE\s*\(\s*([A-Za-z0-9_]+)\s*\)}
    while { [regexp -start $index -indices $pattern_type1 $aProcessedFileContent location name] } {
      set index [lindex $location 1]
      set name [eval string range \$aProcessedFileContent $name]
      if { ! [regexp -lineanchor "^${pattern_incbeg}${name}${pattern_incend}" $aProcessedFileContent] &&
           [lsearch -exact $addtype $name] < 0 &&
           [SearchForFile $theIncPaths $name.hxx]} {
        lappend addtype $name
      }
    }
    set pattern_type2 {Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)\s*::\s*DownCast}
    while { [regexp -start $index -indices $pattern_type2 $aProcessedFileContent location name] } {
      set index [lindex $location 1]
      set name [eval string range \$aProcessedFileContent $name]
      if { ! [regexp -lineanchor "^${pattern_incbeg}${name}${pattern_incend}" $aProcessedFileContent] &&
           [lsearch -exact $addtype $name] < 0 &&
           [SearchForFile $theIncPaths $name.hxx]} {
        lappend addtype $name
      }
    }
    if { [llength $addtype] > 0 } {
      if { ! $theCheckMode } {
        set addinc ""
        foreach type $addtype {
          if { "$aProcessedFileName" != "$type.hxx" } {
            append addinc "\n#include <$type.hxx>"
          }
        }
        if { [regexp -indices ".*\n${pattern_incbeg}\[A-Za-z0-9_/\]+${pattern_incend}" $aProcessedFileContent location] } {
          set change_flag 1
          ReplaceSubString aProcessedFileContent $location "[eval string range \$aProcessedFileContent $location]$addinc" index
        } else {
          logerr "Error: $aProcessedFile: Cannot find #include statement to add more includes..."
        }
      } else {
        logwarn "Warning: $aProcessedFile: The following class names are used as arguments of STANDARD_TYPE"
        logwarn "         macro, but not included directly: $addtype"
        break
      }
    }

    # apply changes to the header file
    if { $change_flag } {
      SaveTextToFile $aProcessedFile $aProcessedFileContent
    }
  }
}

# replace all forward declarations of "class Handle(...)" with fwd of "class ..."
proc ConvertHandle {theTargetPath theIncPaths theCheckMode theExtensions} {

  # iterate by header files
  foreach aHeader [glob -nocomplain -type f -directory $theTargetPath *.{$theExtensions}] {
    set aCurrentHeaderName [file tail $aHeader]

    # skip gxx files, as names Handle_xxx used there are in most cases 
    # placeholders of the argument types substituted by #define
    if {[file extension $aHeader] == ".gxx"} {
      continue
    }

    # read the content of the header
    if { [catch {set fd [open $aHeader rb]}] } {
      logerr "Error: cannot open $aHeader"
      continue
    }
    close $fd

    set aHeaderContent [ReadFileToList $aHeader aHeaderRawContent aHeaderEOL]

    set anUpdateHeader false

    # if file contains "slots:" or "signals:", assume it defines some QObject
    # class(es). 
    # In this case, type names "Handle_T" will not be replaced by Handle(T) to
    # prevent failure of compilation of MOC code if such types are used in 
    # slots or signals (MOC does not expand macros).
    # Forward declaration of a Handle will be then replaced by #include of 
    # corresponding class header (if such header is found), assuming that name
    # typedefed Handle_T is defined in corresponding header (as typedef).
    set isQObject [expr [regexp "Q_OBJECT" $aHeaderContent] && [regexp "(slots|signals)\s*:" $aHeaderContent]]

    # replace all IDs with prefix Handle_ by use of Handle() macro
    if { ! $isQObject } {
      set anUpdatedHeaderContent {}    
      set pattern_handle {\mHandle_([A-Za-z0-9_]+)}
      foreach line $aHeaderContent {
        # do not touch typedefs, #include, and #if... statements
        if { [regexp {^\s*typedef} $line] || 
             [regexp {^\s*\#\s*include} $line] || [regexp {^\s*\#\s*if} $line] } {
          lappend anUpdatedHeaderContent $line
          continue
        }

        # in other preprocessor statements, skip first expression to avoid
        # replacements in #define Handle_... and similar cases 
        set index 0
        if { [regexp -indices {\s*#\s*[A-Za-z]+\s+[^\s]+} $line location] } {
          set index [expr 1 + [lindex $location 1]]
        }

        # replace Handle_T by Handle(T)
        while { [regexp -start $index -indices $pattern_handle $line location class] } {
          set index [lindex $location 1]

          set class [eval string range \$line $class]
#          puts "Found: [eval string range \$line $location]"

          if { ! $theCheckMode } {
            set anUpdateHeader true
            ReplaceSubString line $location "Handle($class)" index
          } else {
            logwarn "Warning: $aHeader refers to IDs starting with \"Handle_\" which are likely"
            logwarn "  instances of OCCT Handle classes (e.g. \"$class\"); these are to be "
            logwarn "  replaced by template opencascade::handle<> or legacy macro Handle()"
            set index -1 ;# to break outer cycle
            break
          }
        }
        lappend anUpdatedHeaderContent $line

        if { $index < 0 } { 
          set anUpdatedHeaderContent $aHeaderContent
          break
        }
      }
      set aHeaderContent $anUpdatedHeaderContent
    }

    # replace NS::Handle(A) by Handle(NS::A)
    set anUpdatedHeaderContent {}    
    set pattern_nshandle {([A-Za-z0-9_]+)\s*::\s*Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)}
    foreach line $aHeaderContent {
      set index 0

      while { [regexp -start $index -indices -lineanchor $pattern_nshandle $line location scope class]} {
        set index [lindex $location 1]

        set scope [eval string range \$line $scope]
        set class [eval string range \$line $class]

        if { ! $theCheckMode } {
           set anUpdateHeader true
          ReplaceSubString line $location "Handle(${scope}::${class})" index
        } else {
          logwarn "Warning in $aHeader: usage of Handle macro inside scope is incorrect: [eval string range \$line $location]"
          set index -1 ;# to break outer cycle
          break
        }
      }
      lappend anUpdatedHeaderContent $line

      if { $index < 0 } { 
        set anUpdatedHeaderContent $aHeaderContent
        break
      }
    }
    set aHeaderContent $anUpdatedHeaderContent

    # remove all forward declarations of Handle classes
    set anUpdatedHeaderContent {}    
    set aFwdHandlePattern     {^\s*class\s+Handle[_\(]([A-Za-z0-9_]+)[\)]?\s*\;\s*$}
    foreach aHeaderContentLine $aHeaderContent {
      if {[regexp $aFwdHandlePattern $aHeaderContentLine dummy aForwardDeclHandledClass]} {
        if {$theCheckMode} {
          loginfo "Info: $aHeader contains statement involving forward decl of Handle_$aForwardDeclHandledClass"
        } else {
          # replace by forward declaration of a class or its include unless 
          # it is already declared or included
          if { ! [regexp "\#\\s*include\\s*\[\<\"\]\\s*(\[A-Za-z0-9_/\]*/)?$aForwardDeclHandledClass\[.\]hxx\\s*\[\>\"\]" $aHeaderContent] } {
            if { $isQObject && "$aCurrentHeaderName" != "${aForwardDeclHandledClass}.hxx" } {
              lappend anUpdatedHeaderContent "#include <${aForwardDeclHandledClass}.hxx>"
              if { ! [SearchForFile $theIncPaths ${aForwardDeclHandledClass}.hxx] } {
                loginfo "Warning: include ${aForwardDeclHandledClass}.hxx added in $aHeader, assuming it exists and defines Handle_$aForwardDeclHandledClass"
              }
            } elseif { ! [regexp "^\s*class\s+$aForwardDeclHandledClass\s*;" $aHeaderContent] } {
              lappend anUpdatedHeaderContent "class $aForwardDeclHandledClass;"
            }
          }
          set anUpdateHeader true
          continue
        }
      }
      lappend anUpdatedHeaderContent $aHeaderContentLine
    }
    set aHeaderContent $anUpdatedHeaderContent
    
    # remove all typedefs using Handle() macro to generate typedefed name
    set anUpdatedHeaderContent {}    
    set aTypedefHandlePattern {^\s*typedef\s+[_A-Za-z\<\>, \s]+\s+Handle\([A-Za-z0-9_]+\)\s*\;\s*$}
    foreach aHeaderContentLine $aHeaderContent {
      if {[regexp $aTypedefHandlePattern $aHeaderContentLine aFoundPattern]} {
        if {$theCheckMode} {
          loginfo "Info: $aHeader contains typedef using Handle macro to generate name: $aFoundPattern"
        } else {
          set anUpdateHeader true
          continue
        }
      }
      lappend anUpdatedHeaderContent $aHeaderContentLine
    }
    set aHeaderContent $anUpdatedHeaderContent
    
    # remove all #include statements for files starting with "Handle_"
    set anUpdatedHeaderContent {}    
    set anIncHandlePattern    {^\s*\#\s*include\s+[\<\"]\s*(Handle[\(_][A-Za-z0-9_.]+[\)]?)\s*[\>\"]\s*$}
    foreach aHeaderContentLine $aHeaderContent {
      if {[regexp $anIncHandlePattern $aHeaderContentLine aFoundPattern anHxxName] &&
                ! [SearchForFile $theIncPaths $anHxxName]} {
        if {$theCheckMode} {
          loginfo "Info: $aHeader includes missing header: $anHxxName"
        } else {
          set anUpdateHeader true
          continue
        }
      }
      lappend anUpdatedHeaderContent $aHeaderContentLine
    }

    # save result    
    if {$anUpdateHeader} {
      SaveListToFile $aHeader $anUpdatedHeaderContent $aHeaderEOL
    }
  }
}

# Replaces C-style casts of Handle object to Handle to derived type 
# by call to DownCast() method
proc ConvertCStyleHandleCast {pkpath theExtensions theCheckMode} {

  # iterate by header files
  foreach afile [glob -nocomplain -type f -directory $pkpath *.\{$theExtensions\}] {
    set hxx [ReadFileToRawText $afile]

    set change_flag 0

    # replace ((Handle(A)&)b) by Handle(A)::DownCast(b)
    set index 0
    set pattern_refcast1 {\(\(\s*Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)\s*[&]\s*\)\s*([A-Za-z0-9_]+)\)}
    while { [regexp -start $index -indices -lineanchor $pattern_refcast1 $hxx location class var]} {
      set index [lindex $location 1]

      set class [eval string range \$hxx $class]
      set var   [eval string range \$hxx $var]

      if { ! $theCheckMode } {
         set change_flag 1
        ReplaceSubString hxx $location "Handle($class)::DownCast ($var)" index
      } else {
        logwarn "Warning in $afile: C-style cast: [eval string range \$hxx $location]"
      }
    }

    # replace  (Handle(A)&)b, by Handle(A)::DownCast(b),
    # replace  (Handle(A)&)b; by Handle(A)::DownCast(b);
    # replace  (Handle(A)&)b) by Handle(A)::DownCast(b))
    set index 0
    set pattern_refcast2 {\(\s*Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)\s*[&]\s*\)\s*([A-Za-z0-9_]+)(\s*[,;\)])}
    while { [regexp -start $index -indices -lineanchor $pattern_refcast2 $hxx location class var end]} {
      set index [lindex $location 1]

      set class [eval string range \$hxx $class]
      set var   [eval string range \$hxx $var]
      set end   [eval string range \$hxx $end]

      if { ! $theCheckMode } {
        set change_flag 1
        ReplaceSubString hxx $location "Handle($class)::DownCast ($var)$end" index
      } else {
        logwarn "Warning in $afile: C-style cast: [eval string range \$hxx $location]"
      }
    }

    # replace (*((Handle(A)*)&b)) by Handle(A)::DownCast(b)
    set index 0
    set pattern_ptrcast1 {([^A-Za-z0-9_]\s*)\(\s*[*]\s*\(\(Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)\s*[*]\s*\)\s*[&]\s*([A-Za-z0-9_]+)\s*\)\s*\)}
    while { [regexp -start $index -indices -lineanchor $pattern_ptrcast1 $hxx location start class var] } {
      set index [lindex $location 1]

      set start [eval string range \$hxx $start]
      set class [eval string range \$hxx $class]
      set var   [eval string range \$hxx $var]

      if { ! $theCheckMode } {
        set change_flag 1
        ReplaceSubString hxx $location "${start}Handle($class)::DownCast ($var)" index
      } else {
        logwarn "Warning in $afile: C-style cast: [eval string range \$hxx $location]"
      }
    }

    # replace  *((Handle(A)*)&b)  by Handle(A)::DownCast(b)
    set index 0
    set pattern_ptrcast2 {[*]\s*\(\(Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)\s*[*]\s*\)\s*[&]\s*([A-Za-z0-9_]+)\s*\)}
    while { [regexp -start $index -indices -lineanchor $pattern_ptrcast2 $hxx location class var] } {
      set index [lindex $location 1]

      set class [eval string range \$hxx $class]
      set var   [eval string range \$hxx $var]

      if { ! $theCheckMode } {
        set change_flag 1
        ReplaceSubString hxx $location "Handle($class)::DownCast ($var)" index
      } else {
        logwarn "Warning in $afile: C-style cast: [eval string range \$hxx $location]"
      }
    }

    # replace (*(Handle(A)*)&b) by Handle(A)::DownCast(b)
    set index 0
    set pattern_ptrcast3 {([^A-Za-z0-9_]\s*)\(\s*[*]\s*\(Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)\s*[*]\s*\)\s*[&]\s*([A-Za-z0-9_]+)\s*\)}
    while { [regexp -start $index -indices -lineanchor $pattern_ptrcast3 $hxx location start class var] } {
      set index [lindex $location 1]

      set start [eval string range \$hxx $start]
      set class [eval string range \$hxx $class]
      set var   [eval string range \$hxx $var]

      if { ! $theCheckMode } {
        set change_flag 1
        ReplaceSubString hxx $location "${start}Handle($class)::DownCast ($var)" index
      } else {
        logwarn "Warning in $afile: C-style cast: [eval string range \$hxx $location]"
      }
    }

    # replace  *(Handle(A)*)&b,  by Handle(A)::DownCast(b),
    # replace  *(Handle(A)*)&b;  by Handle(A)::DownCast(b);
    # replace  *(Handle(A)*)&b)  by Handle(A)::DownCast(b))
    set index 0
    set pattern_ptrcast4 {[*]\s*\(Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)\s*[*]\s*\)\s*[&]\s*([A-Za-z0-9_]+)(\s*[,;\)])}
    while { [regexp -start $index -indices -lineanchor $pattern_ptrcast4 $hxx location class var end] } {
      set index [lindex $location 1]

      set class [eval string range \$hxx $class]
      set var   [eval string range \$hxx $var]
      set end   [eval string range \$hxx $end]

      if { ! $theCheckMode } {
        set change_flag 1
        ReplaceSubString hxx $location "Handle($class)::DownCast ($var)$end" index
      } else {
        logwarn "Warning in $afile: C-style cast: [eval string range \$hxx $location]"
      }
    }

    # just warn if some casts to & are still there
    set index 0
    set pattern_refcast0 {\(\s*Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)\s*[&]\s*\)\s*([A-Za-z0-9_]+)}
    while { [regexp -start $index -indices -lineanchor $pattern_refcast0 $hxx location class var] } {
      set index [lindex $location 1]

      set var   [eval string range \$hxx $var]
      if { "$var" != "const" && "$var" != "Standard_OVERRIDE" } {
        logwarn "Warning in $afile: C-style cast: [eval string range \$hxx $location]"
      }
    }

    # replace const Handle(A)& a = Handle(B)::DownCast (b); by 
    #               Handle(A)  a ( Handle(B)::DownCast (b) );
    set index 0
    set pattern_refvar {\mconst\s+Handle\s*\(\s*([A-Za-z0-9_]+)\s*\)\s*[&]\s*([A-Za-z0-9_]+)\s*=\s*(Handle\s*\(\s*[A-Za-z0-9_]+\s*\)\s*::\s*DownCast\s*\([^;]+);}
    while { [regexp -start $index -indices -lineanchor $pattern_refvar $hxx location class var hexpr] } {
      set index [lindex $location 1]

      set class [eval string range \$hxx $class]
      set var   [eval string range \$hxx $var]
      set hexpr [eval string range \$hxx $hexpr]

      if { ! $theCheckMode } {
        set change_flag 1
        ReplaceSubString hxx $location "Handle($class) $var ($hexpr);" index
      } else {
        logwarn "Warning in $afile: C-style cast: [eval string range \$hxx $location]"
      }
    }

    # apply changes to the header file
    if { $change_flag } {
      SaveTextToFile $afile $hxx
    }
  }
}

# Remove unnecessary forward declaration of a class if found immediately before 
# its use in DEFINE_STANDARD_HANDLE
proc RemoveFwdClassForDefineStandardHandle {pkpath theCheckMode} {

  # iterate by header files
  foreach afile [glob -nocomplain -type f -directory $pkpath *.?xx] {

    # load a file
    if { [catch {set fd [open $afile rb]}] } {
      logerr "Error: cannot open $afile"
      continue
    }
    set hxx [read $fd]
    close $fd  

    set change_flag 0

    # replace
    set index 0
    set pattern_fwddef {class\s+([A-Za-z0-9_]+)\s*;\s*DEFINE_STANDARD_HANDLE\s*\(\s*([A-Za-z0-9_]+)\s*,\s*([A-Za-z0-9_]+)\s*\)[ \t]*}
    while { [regexp -start $index -indices -lineanchor $pattern_fwddef $aProcessedFileContent location fwdclass class1 class2] } {
      set index [lindex $location 1]

      set fwdclass [eval string range \$aProcessedFileContent $fwdclass]
      set class1   [eval string range \$aProcessedFileContent $class1]
      set class2   [eval string range \$aProcessedFileContent $class2]

      if { $fwdclass != $class1 } {
        continue
      }

      if { ! $theCheckMode } {
        set change_flag 1
        ReplaceSubString aProcessedFileContent $location "DEFINE_STANDARD_HANDLE($class1, $class2)" index
        incr index -1
      } else {
        logwarn "Warning: $aProcessedFile contains unnecessary forward declarations of class $fwdclass"
        break
      }
    }

    # apply changes to the header file
    if { $change_flag } {
      SaveTextToFile $afile $hxx
    }
  }
}

# auxiliary: modifies variable text_var replacing part defined by two indices
# given in location by string str, and updates index_var variable to point to
# the end of the replaced string. Variable flag_var is set to 1.
proc ReplaceSubString {theSource theLocation theSubstitute theEndIndex} {

  upvar $theSource aSource
  upvar $theEndIndex anEndIndex

  set aStartIndex [lindex $theLocation 0]
  set anEndIndex  [lindex $theLocation 1]
  set aSource  [string replace "$aSource" $aStartIndex $anEndIndex "$theSubstitute"]
  set anEndIndex [expr $aStartIndex + [string length $theSubstitute]]
}

# Save theFileContent some text to theFilePath file 
proc SaveTextToFile {theFilePath theFileContent} {
  if { [catch {set aFile [open ${theFilePath} w];} aReason] } {
    logerr "Error: cannot open file \"${theFilePath}\" for writing: $aReason"
    return
  }

  fconfigure $aFile -translation binary
  puts -nonewline $aFile "$theFileContent"
  close $aFile

  loginfo "File $theFilePath modified"
}

# read content from theFilePath to list, theFileContent is a raw content of the file
proc ReadFileToList {theFilePath theFileContent theFileEOL} {
  upvar $theFileContent aFileContent
  upvar $theFileEOL  aFileEOL

  if {"$theFilePath" == "" || ![file exists $theFilePath]} {
    return
  }

  if { [catch {set aFile [open ${theFilePath} r]} aReason] } {
    logerr "Error: cannot open file \"${theFilePath}\" for reading: $aReason"
    return
  }

  fconfigure $aFile -translation binary
  set aFileContent [read $aFile]
  close $aFile

  # detect DOS end-of-lines
  if { [regexp "\r\n" $aFileContent] } {
    set aFileEOL "\r\n"
    set aList [split [regsub -all "\r\n" $aFileContent "\n"] "\r\n"]
  } else {
    # standard UNIX end-of-lines
    set aFileEOL "\n"
    set aList [split $aFileContent "\n"]
  }

  return $aList
}

# read content from theFilePath to raw text (with unix eol)
proc ReadFileToRawText {theFilePath} {
  if {"$theFilePath" == "" || ![file exists $theFilePath]} {
    return
  }

  if { [catch {set aFile [open ${theFilePath} r]} aReason] } {
    logerr "Error: cannot open file \"${theFilePath}\" for reading: $aReason"
    return
  }

  fconfigure $aFile -translation binary
  set aFileContent [read $aFile]
  close $aFile

  set aFileEOL "\r"
  if [regexp "\r\n" $aFileContent] {
    set aFileEOL "\r\n"
  } elseif [regexp "\n" $aFileContent] {
    set aFileEOL "\n"
  }

  # convert to unix eol
  if {"$aFileEOL" != "\n"} {
    regsub -all {$aFileEOL} $aFileContent "\n" aFileContent
  }

  return $aFileContent
}

# auxiliary: saves content of "theData" list to "theFilePath"
proc SaveListToFile {theFilePath theData {theEOL "auto"}} {
  set anUsedEol $theEOL

  if {"$anUsedEol" == ""} {
    set anUsedEol "auto"
  }

  # if the file exists and "eol choice" is "auto", detect the file eol
  if {$anUsedEol == "auto" && [file exists $theFilePath]} {
    if { [catch {set aFile [open ${theFilePath} r]} aReason] } {
      logerr "Error: cannot open file \"${theFilePath}\" for reading: $aReason"
    } else {
      fconfigure $aFile -translation binary
      set aFileContent [read $aFile]
      close $aFile

      set anUsedEol "\r"
      if [regexp "\r\n" $aFileContent] {
        set anUsedEol "\r\n"
      } elseif [regexp "\n" $aFileContent] {
        set anUsedEol "\n"
      }
    }
  }

  # write
  if { [catch {set aFile [open ${theFilePath} w];} aReason] } {
    logerr "Error: cannot open file \"${theFilePath}\" for writing: $aReason"
    return
  }

  fconfigure $aFile -translation binary
  puts -nonewline $aFile [join $theData $anUsedEol]
  close $aFile

  loginfo "File $theFilePath modified"
}

# collect all subdirs of theBaseDir
proc CollectDirStructure {theBaseDir} {
  set aDirs [glob -nocomplain -directory $theBaseDir -type d *]

  set aSubDirs {}
  foreach aDir $aDirs {
    foreach aSubDir [CollectDirStructure $aDir] {
      lappend aSubDirs $aSubDir
    }
  }

  foreach aSubDir $aSubDirs {
    lappend aDirs $aSubDir
  }

  return $aDirs
}

# check existence of theFileName file in several folders (theIncPaths)
proc SearchForFile {theIncPaths theFileName} {
  foreach aPath $theIncPaths {
    if {[file exists "${aPath}/${theFileName}"]} {
      return true
    }
  }

  return false
}

# auxiliary: parse the string to comment and not comment parts
# variable "_cmnt" should be created before using the operation, it will save comment part of line
# variable "_isMulti" should be created before the loop, equal to "false" if first line in the loop is not multi-comment line
proc _check_line { line } {
  upvar _isMulti _isMulti
  upvar _cmnt _cmnt

  set string_length [string length $line]
  set c_b $string_length
  set mc_b $string_length
  set mc_e $string_length
  regexp -indices {//} $line c_b
  regexp -indices {/\*} $line mc_b
  regexp -indices {\*/} $line mc_e
  if {!${_isMulti}} {
    if {[lindex $c_b 0] < [lindex $mc_b 0] && [lindex $c_b 0] < [lindex $mc_e 0]} {
      set notComment_c [string range $line 0 [expr [lindex $c_b 0]-1]]
      set Comment_c [string range $line [lindex $c_b 0] end]
      set _cmnt $_cmnt$Comment_c
      return $notComment_c
    } elseif {[lindex $mc_b 0] < [lindex $c_b 0] && [lindex $mc_b 0] < [lindex $mc_e 0]} {
      set _isMulti true
      set _cmnt "${_cmnt}/*"
      set notComment_mc [string range $line 0 [expr [lindex $mc_b 0]-1]]
      set Comment_mc [string range $line [expr [lindex $mc_b 1]+1] end]
      return [_check_line "${notComment_mc}[_check_line ${Comment_mc}]"]
    } elseif {[lindex $mc_e 0] < [lindex $c_b 0] && [lindex $mc_e 0] < [lindex $mc_b 0]} {
      set notComment_mc [string range $line [expr [lindex $mc_e 1]+1] end]
      set Comment_mc [string range $line 0 [expr [lindex $mc_e 0]-1]]
      set _cmnt "${_cmnt}${Comment_mc}*/"
      set chk [_check_line ${notComment_mc}]
      set _isMulti true
      return $chk
    }
  } else {
    if {[lindex $mc_e 0] < [lindex $mc_b 0]} {
      set _isMulti false
      set Comment_mc [string range $line 0 [lindex $mc_e 1]]
      set notComment_mc [string range $line [expr [lindex $mc_e 1]+1] end]
      set _cmnt $_cmnt$Comment_mc
      return [_check_line $notComment_mc]
    } elseif {[lindex $mc_b 0] < [lindex $mc_e 0] } {
      set notComment_mc [string range $line 0 [expr [lindex $mc_b 0]-1]]
      set Comment_mc [string range $line [expr [lindex $mc_b 1]+1] end]
      set _cmnt "${_cmnt}/*"
      set chk [_check_line "${notComment_mc}[_check_line ${Comment_mc}]"]
      set _isMulti false
      return $chk
    } else {
      set _cmnt $_cmnt$line
      return ""
    }
  }
  return $line
}

# Create Tk-based logger which allows convenient consulting the upgrade process.
proc _create_logger {} {
    if { [catch {winfo exists .h}] } {
        logerr "Error: Tk commands are not available, cannot create UI!"
        return
    }

    if { ![winfo exists .h ] } {
        toplevel .h
        wm title .h "Conversion log"
        wm geometry .h +320+200
        wm resizable .h 0 0

        text .h.t -yscrollcommand {.h.sbar set}
        scrollbar .h.sbar -orient vertical -command {.h.t yview}

        pack .h.sbar -side right -fill y
        pack .h.t
    }
}

set LogFilePath ""

# Puts the passed string into Tk-based logger highlighting it with the
# given color for better view. If no logger exists (-wlog option was not
# activated), the standard output is used.
proc _logcommon {theLogMessage {theMessageColor ""}} {
  global LogFilePath

  if {"$LogFilePath" != ""} {
    if { ! [catch {set aLogFile [open ${LogFilePath} a];} aReason] } {
      set t [clock milliseconds]
      set aTimeAndMessage [format "\[%s\] %s" \
                            [clock format [expr {$t / 1000}] -format %T] \
                            $theLogMessage \
                          ]


      puts $aLogFile $aTimeAndMessage
      close $aLogFile
    } else {
      logerr "Error: cannot open $LogFilePath log file due to $aReason"
    }
  }

  if { ! [catch {winfo exists .h} res] && $res } {
      .h.t insert end "$theLogMessage\n"

    if {$theLogMessage != ""} {
      # We use the current number of lines to generate unique tag in the text
      set aLineNb [lindex [split [.h.t index "end - 1 line"] "."] 0]

      .h.t tag add my_tag_$aLineNb end-2l end-1l
      .h.t tag configure my_tag_$aLineNb -background $theMessageColor
    }

    update
  } else {
    puts $theLogMessage
  }
}

# Puts information message to logger.
proc loginfo {a} {
    _logcommon $a
}

# Puts warning message to logger.
proc logwarn {a} {
    _logcommon $a "pink"
}

# Puts error message to logger.
proc logerr {a} {
    _logcommon $a "red"
}
