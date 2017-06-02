# =======================================================================
# Created on: 2014-03-21
# Created by: OMY
# Copyright (c) 1996-1999 Matra Datavision
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
 
# =======================================================================
# This script contains auxilary functions which can be used 
# in documentation generation process
# =======================================================================

# ==============================================
# Commonly used functions
# ==============================================

# Parses arguments line like "-arg1=val1 -arg2=val2 ..." to array args_names and map args_values
proc OCCDoc_ParseArguments {arguments} {
  global args_names
  global args_values
  set args_names {}
  array set args_values {}

  foreach arg $arguments {
    if {[regexp {^(-)[a-z_]+$} $arg] == 1} {
      set name [string range $arg 1 [string length $arg]-1]
      lappend args_names $name
      set args_values($name) "NULL"
      continue
    } elseif {[regexp {^(-)[a-z_]+=.+$} $arg] == 1} {
      set equal_symbol_position [string first "=" $arg]
      set name [string range $arg 1 $equal_symbol_position-1]
      lappend args_names $name
      set value [string range $arg $equal_symbol_position+1 [string length $arguments]-1]
      
      # To parse a list of values for -m parameter
      if { [string first "," $value] != -1 } {
        set value [split $value ","];
      }

      set args_values($name) $value

    } else {
      puts "Error in argument $arg."
      return 1
    }
  }
  return 0
}

# Returns script parent folder
proc OCCDoc_GetDoxDir { {theProductsPath ""} } {
  if { $theProductsPath == "" } {
    return [file normalize [file dirname [info script]]/../dox]
  } else {
    return [file normalize $theProductsPath]/dox
  }
}

# Returns products root folder
proc OCCDoc_GetProdRootDir {} {
  if {[info exists ::env(PRODROOT)]} {
    return [file normalize $::env(PRODROOT)]
  }
}

# Returns OCCT root dir
proc OCCDoc_GetOCCTRootDir {} {
  set path [OCCDoc_GetDoxDir]
  return [file normalize $path/..]
}

# Returns root dir
proc OCCDoc_GetRootDir { {theProductsPath ""} } {
  if { $theProductsPath == "" } {
    return [OCCDoc_GetOCCTRootDir]
  } else {
    return [file normalize $theProductsPath]
  }
}

# Returns OCCT include dir
proc OCCDoc_GetIncDir { {theProductsPath ""} } {
  set path [OCCDoc_GetRootDir $theProductsPath]
  return "$path/inc"
}

# Returns OCCT source dir
proc OCCDoc_GetSourceDir { {theProductsPath ""} } {
  set path [OCCDoc_GetRootDir $theProductsPath]
  return "$path/src"
}

# Returns name of the package from the current toolkit
proc OCCDoc_GetNameFromPath { thePath } {

  set splitted_path [split $thePath "/" ]
  set package_name  [lindex $splitted_path end]

  return $package_name
}

# Returns the relative path between two folders
proc OCCDoc_GetRelPath {thePathFrom thePathTo} {
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

# Returns OCCT version string from file Standard_Version.hxx (if available)
proc OCCDoc_DetectCasVersion {} {
  set occt_ver 6.7.0
  set occt_ver_add ""
  set filename "[OCCDoc_GetSourceDir]/Standard/Standard_Version.hxx"
  if { [file exists $filename] } {
    set fh [open $filename "r"]
    set fh_loaded [read $fh]
    close $fh
    regexp {[^/]\s*#\s*define\s+OCC_VERSION_COMPLETE\s+\"([^\s]*)\"} $fh_loaded dummy occt_ver
    regexp {[^/]\s*#\s*define\s+OCC_VERSION_DEVELOPMENT\s+\"([^\s]*)\"} $fh_loaded dummy occt_ver_add
    if { "$occt_ver_add" != "" } { set occt_ver ${occt_ver}.$occt_ver_add }
  }
  return $occt_ver
}

# Checks if the necessary tools exist
proc OCCDoc_DetectNecessarySoftware { DOXYGEN_PATH GRAPHVIZ_PATH INKSCAPE_PATH HHC_PATH PDFLATEX_PATH } {

  upvar 1 DOXYGEN_PATH  doxy_path
  upvar 1 GRAPHVIZ_PATH graph_path
  upvar 1 INKSCAPE_PATH inkscape_path
  upvar 1 HHC_PATH      hhc_path
  upvar 1 PDFLATEX_PATH latex_path

  set doxy_path     ""
  set graph_path    ""
  set inkscape_path ""
  set latex_path    ""
  set hhc_path      ""

  set is_win "no"
  if { "$::tcl_platform(platform)" == "windows" } {
    set is_win "yes"
  }
  if {"$is_win" == "yes"} {
    set exe ".exe"
  } else {
    set exe ""
  }

  set g_flag "no"
  set d_flag "no"
  set i_flag "no"
  set h_flag "no"
  set l_flag "no"

  puts ""
  set envPath $::env(PATH)
  if { $is_win == "yes" } {
    set searchPathsList [split $envPath ";"]
  } else {
    set searchPathsList [split $envPath ":"]
  }

  foreach path $searchPathsList {
    if { ($is_win == "no") && 
         (($path == "/usr/bin") || ($path == "/usr/local/bin")) } {
        # Avoid searching in default bin location
        continue
    }
    if {$d_flag == "no"} {
      if { [file exists $path/doxygen$exe] } {
        catch { exec $path/doxygen -V } version_string 
        set version [lindex [split $version_string "\n"] 0]
        puts "Info: $version "
        puts "      found in $path."
        set doxy_path "$path/doxygen$exe"
        set d_flag "yes"
      }
    }
    if {$g_flag == "no"} {
      if { [file exists $path/dot$exe] } {
        catch { exec $path/dot -V } version

        puts "Info: $version "
        puts "      found in $path."
        set graph_path "$path/dot$exe"
        set g_flag "yes"
      }
    }
    if {$i_flag == "no"} {
      if { [file exists $path/inkscape$exe] } {
        catch { exec $path/inkscape -V } version
        puts "Info: $version " 
        puts "      found in $path."
        set inkscape_path "$path/inkscape$exe"
        set i_flag "yes"
      }
    }
    if {$l_flag == "no"} {
      if { [file exists $path/pdflatex$exe] } {
        catch { exec $path/pdflatex -version } version
        set version [lindex [split $version "\n"] 0]
        puts "Info: $version " 
        puts "      found in $path."
        set latex_path "$path/pdflatex$exe"
        set l_flag "yes"
      }
    }
    if { ("$is_win" == "yes") && ($h_flag == "no") } {
      if { [file exists $path/hhc.exe] } {
        puts "Info: hhc " 
        puts "      found in $path."
        set hhc_path "hhc$exe"
        set h_flag "yes"
      }
    }
    if { ($d_flag == "yes") &&
         ($i_flag == "yes") &&
         ($g_flag == "yes") &&
         ($l_flag == "yes") &&
         (($is_win == "yes") && 
          ($h_flag == "yes")) } {
      break
    }
  }

  # On Windows search for hhc.exe in the default location 
  # if it has not been found yet
  if { ("$is_win" == "yes") && ($h_flag == "no") } {
    if { [info exists ::env(ProgramFiles\(x86\))] } {
      set h_flag "yes"
      set path "$::env(ProgramFiles\(x86\))\\HTML Help Workshop"
      set hhc_path "$path\\hhc.exe"
      puts "Info: hhc " 
      puts "      found in $path."
    } else {
      if { [info exists ::env(ProgramFiles)] } {
        set h_flag   "yes"
        set path     "$::env(ProgramFiles)\\HTML Help Workshop"
        set hhc_path "$path\\hhc.exe"
        puts "Info: hhc" 
        puts "      found in $path."
      }
    }
  }

  # On *nix-like platforms, 
  # check the default binary locations if the tools had not been found yet
  if {  $is_win == "no"  &&
      (($d_flag == "no") ||
       ($i_flag == "no") ||
       ($g_flag == "no") ||
       ($l_flag == "no")) } {

    set default_path { "/usr/bin" "/usr/local/bin" }
    foreach path $default_path {
      if {$d_flag == "no"} {
        if { [file exists $path/doxygen$exe] } {
          catch { exec $path/doxygen -V } version_string 
          set version [lindex [split $version_string "\n"] 0]
          puts "Info: $version "
          puts "      found in $path."
          set doxy_path "$path/doxygen$exe"
          set d_flag "yes"
        }
      }
      if {$g_flag == "no"} {
        if { [file exists $path/dot$exe] } {
          catch { exec $path/dot -V } version

          puts "Info: $version "
          puts "      found in $path."
          set graph_path "$path/dot$exe"
          set g_flag "yes"
        }
      }
      if {$i_flag == "no"} {
        if { [file exists $path/inkscape$exe] } {
          catch { exec $path/inkscape -V } version
          puts "Info: $version " 
          puts "      found in $path."
          set inkscape_path "$path/inkscape$exe"
          set i_flag "yes"
        }
      }
      if {$l_flag == "no"} {
        if { [file exists $path/pdflatex$exe] } {
          catch { exec $path/pdflatex -version } version
          set version [lindex [split $version "\n"] 0]
          puts "Info: $version " 
          puts "      found in $path."
          set latex_path "$path/pdflatex$exe"
          set l_flag "yes"
        }
      }
    }
  }

  # Check if tools have been found
  if { $d_flag == "no" } {
    puts "Warning: Could not find doxygen installed."
    return -1
  }
  if { $g_flag == "no" } {
    puts "Warning: Could not find graphviz installed."
  }
  if { $i_flag == "no" } {
    puts "Warning: Could not find inkscape installed."
  }  
  if { $l_flag == "no" } {
    puts "Warning: Could not find pdflatex installed."
  }
  if { ("$::tcl_platform(platform)" == "windows") && ($h_flag == "no") } {
    puts "Warning: Could not find hhc installed."
  }

  puts ""
}

# Convert SVG files to PDF format to allow including them to PDF
# (requires InkScape to be in PATH)
proc OCCDoc_ProcessSvg {latexDir verboseMode} {

  foreach file [glob -nocomplain $latexDir/*.svg] {
    if {$verboseMode == "YES"} {
      puts "Info: Converting file $file..."
    }
    set pdffile "[file rootname $file].pdf"
    if { [catch {exec inkscape -z -D --file=$file --export-pdf=$pdffile} res] } {
      #puts "Error: $res."
      return
    }
  }
}

# ==============================================
# Reference Manual-specific functions
# ==============================================

# Finds dependencies between all modules  
proc OCCDoc_CreateModulesDependencyGraph {dir filename modules mpageprefix} {
  global module_dependency

  if {![catch {open $dir/$filename.dot "w"} file]} {
    puts $file "digraph $filename"
    puts $file "\{"

    foreach mod $modules {
      if { $mod == "" } {
        continue
      }
      puts $file "\t$mod \[ URL = \"[string tolower $mpageprefix$mod.html]\" \]"
      foreach mod_depend $module_dependency($mod) {
        puts $file "\t$mod_depend -> $mod \[ dir = \"back\", color = \"midnightblue\", style = \"solid\" \]"
      }
    }
    
    puts $file "\}"
    close $file

    return $filename
  }
}

# Finds dependencies between all toolkits in module
proc OCCDoc_CreateModuleToolkitsDependencyGraph {dir filename modulename tpageprefix} {
  global toolkits_in_module
  global toolkit_dependency
  global toolkit_parent_module

  if {![catch {open $dir/$filename.dot "w"} file]} {
    puts $file "digraph $filename"
    puts $file "\{"

    foreach tk $toolkits_in_module($modulename) {
      puts $file "\t$tk \[ URL = \"[string tolower $tpageprefix$tk.html]\"\ ]"
      foreach tkd $toolkit_dependency($tk) {
        if { [info exists toolkit_parent_module($tkd)] } {
          if {$toolkit_parent_module($tkd) == $modulename} {
            puts $file "\t$tkd -> $tk \[ dir = \"back\", color = \"midnightblue\", style = \"solid\" \]"    
          }
        }
      }
    }
    
    puts $file "\}"
    close $file
    
    return $filename
  }
}

# Finds dependencies between the current toolkit and other toolkits
proc OCCDoc_CreateToolkitDependencyGraph {dir filename toolkitname tpageprefix} {
  global toolkit_dependency
  
  if {![catch {open $dir/$filename.dot "w"} file]} {
    puts $file "digraph $filename"
    puts $file "\{"
    
    puts $file "\t$toolkitname \[ URL = \"[string tolower $tpageprefix$toolkitname.html]\"\, shape = box ]"
    foreach tkd $toolkit_dependency($toolkitname) {
      puts $file "\t$tkd \[ URL = \"[string tolower $tpageprefix$tkd.html]\"\ , shape = box ]"
      puts $file "\t$toolkitname -> $tkd \[ color = \"midnightblue\", style = \"solid\" \]"    
    }
    
    if {[llength $toolkit_dependency($toolkitname)] > 1} {
    puts $file "\taspect = 1"
    }
    
    puts $file "\}"
    close $file
    
    return $filename
  }
}

# Fills arrays of modules, toolkits, dependency of modules/toolkits etc 
proc OCCDoc_LoadData { {theProductsDir ""} } {
  global toolkits_in_module
  global toolkit_dependency
  global toolkit_parent_module
  global module_dependency

  if { $theProductsDir == ""} {
    set modules_files [glob -nocomplain -type f -directory "[OCCDoc_GetSourceDir $theProductsDir]/OS/" *.tcl]
  } else {
    set modules_files [glob -nocomplain -type f -directory "[OCCDoc_GetSourceDir $theProductsDir]/VAS/" *.tcl]
  }

  foreach module_file $modules_files {
    source $module_file
  }

  set modules [OCCDoc_GetModulesList $theProductsDir]
  foreach mod $modules {

    if { $mod == "" } {
      continue
    }
    # Get toolkits of current module
    set toolkits_in_module($mod) [$mod:toolkits]
    # Get all dependence of current toolkit 
    foreach tk $toolkits_in_module($mod) {
      # set parent module of current toolkit
      set toolkit_parent_module($tk) $mod
      set exlibfile      [open "[OCCDoc_GetSourceDir $theProductsDir]/$tk/EXTERNLIB" r]
      set exlibfile_data [read $exlibfile]
      set exlibfile_data [split $exlibfile_data "\n"]
        
      set toolkit_dependency($tk) {}
      foreach dtk $exlibfile_data {
        if { ([string first "TK" $dtk 0] == 0) || 
             ([string first "P"  $dtk 0] == 0) } {
          lappend toolkit_dependency($tk) $dtk
        }
      }
      close $exlibfile
    }
  }

  # Get modules dependency
  foreach mod $modules {
    set module_dependency($mod) {}
    foreach tk $toolkits_in_module($mod) {
      foreach tkd $toolkit_dependency($tk) {
        if { [info exists toolkit_parent_module($tkd)] } {
          if { $toolkit_parent_module($tkd) != $mod &&
               [lsearch $module_dependency($mod) $toolkit_parent_module($tkd)] == -1} {
            lappend module_dependency($mod) $toolkit_parent_module($tkd)
          }
        }
      }
    }
  }
}

# Returns list of packages of the given toolkit
proc OCCDoc_GetPackagesList { theToolKitPath } {

  set packages_list {}
  
  # Open file with list of packages of the given toolkit
  set fileid [open "$theToolKitPath/PACKAGES" "r"]
  
  while { [eof $fileid] == 0 } {
    set str [gets $fileid]
    if { $str != "" } {
      lappend packages_list $str
    }
  }

  close $fileid
  
  return $packages_list
}

# Returns list of modules from UDLIST
proc OCCDoc_GetModulesList { {theProductsDir ""} } {

  if { $theProductsDir == "" } {
    source "[OCCDoc_GetSourceDir $theProductsDir]/OS/Modules.tcl"
    # load a command from this file
    set modules [OS:Modules]
  } else {
    source "[OCCDoc_GetSourceDir $theProductsDir]/VAS/Products.tcl"
    # load a command from this file
    set modules [VAS:Products]
	set modules [lsearch -not -all -inline $modules "VAS"]
  }

  return $modules
}

# Returns list of desired files in the specified location
proc OCCDoc_GetHeadersList { theDesiredContent thePackageName {theProductsDir ""} } {

  # Get list of header files with path
  set files_list [split [glob -nocomplain -type f -directory "[OCCDoc_GetSourceDir $theProductsDir]/$thePackageName" "${thePackageName}.hxx" "${thePackageName}_*.hxx"]]

  # Get content according to desired type ('p' for path and 'f' for filenames only)
  if { $theDesiredContent == "p" } {
    return $files_list
  } elseif { $theDesiredContent == "f" } {

    # Cut paths from filenames
    foreach file $files_list {
      set elem_index [lsearch $files_list $file]
      lset files_list $elem_index [OCCDoc_GetNameFromPath [lindex $files_list $elem_index]]
    }
    return $files_list
  }
}

# Returns location of the toolkit
proc OCCDoc_Locate { theToolKitName {theProductsDir ""} } {
  set tk_dir "[OCCDoc_GetSourceDir $theProductsDir]/[OCCDoc_GetNameFromPath $theToolKitName]"
  return $tk_dir
}

# Gets contents of the given html node (for Post-processing)
proc OCCDoc_GetNodeContents {node props html} {
  set openTag "<$node$props>"
  set closingTag "</$node>"
  set start [string first $openTag $html]
  if {$start == -1} {
    return ""
  }
  set start [expr $start + [string length $openTag]]
  set end   [string length $html]
  set html  [string range $html $start $end]
  set start [string first $closingTag $html]
  set end   [string length $html]
  if {$start == -1} {
    return ""
  }
  set start [expr $start - 1]
  return [string range $html 0 $start]
}

# Generates main page file describing module structure
proc OCCDoc_MakeMainPage {outDir outFile modules {theProductsDir ""} } {
  global env

  set one_module [expr [llength $modules] == 1]
  set fd [open $outFile "w"]

  set module_prefix "module_"
  set toolkit_prefix "toolkit_"
  set package_prefix "package_"

  if { ! [file exists "$outDir/html"] } {
    file mkdir "$outDir/html"
  }

  OCCDoc_LoadData $theProductsDir

  # Main page: list of modules
  if { ! $one_module } {
    puts $fd "/**"
    puts $fd "\\mainpage Open CASCADE Technology"

    foreach mod $modules {
        puts $fd "\\li \\subpage [string tolower $module_prefix$mod]"
    }
    # insert modules relationship diagramm
    puts $fd "\\dotfile [OCCDoc_CreateModulesDependencyGraph $outDir/html schema_all_modules $modules $module_prefix]"
    puts $fd "**/\n"
  }

  # One page per module: list of toolkits
  set toolkits {}
  foreach mod $modules {
    if { $mod == "" } {
        continue
    }
    puts $fd "/**"
    if { $one_module } {
        puts $fd "\\mainpage OCCT Module [$mod:name]"
    } else {
        puts $fd "\\page [string tolower module_$mod] Module [$mod:name]"
    }
    foreach tk [lsort [$mod:toolkits]] {
        lappend toolkits $tk
        puts $fd "\\li \\subpage [string tolower $toolkit_prefix$tk]"
    }
    puts $fd "\\dotfile [OCCDoc_CreateModuleToolkitsDependencyGraph $outDir/html schema_$mod $mod $toolkit_prefix]"
    puts $fd "**/\n"
  }

  # One page per toolkit: list of packages
  set packages {}
  foreach tk $toolkits {
    puts $fd "/**"
    puts $fd "\\page [string tolower toolkit_$tk] Toolkit $tk"
    foreach pk [lsort [OCCDoc_GetPackagesList [OCCDoc_Locate $tk $theProductsDir]]] {
        lappend packages $pk
        set u [OCCDoc_GetNameFromPath $pk]
        puts $fd "\\li \\subpage [string tolower $package_prefix$u]"
    }
    puts $fd "\\dotfile [OCCDoc_CreateToolkitDependencyGraph $outDir/html schema_$tk $tk $toolkit_prefix]"
    puts $fd "**/\n"
  }

  # One page per package: list of classes
  foreach pk $packages {
    set u [OCCDoc_GetNameFromPath $pk]
    puts $fd "/**"
    puts $fd "\\page [string tolower $package_prefix$u] Package $u"
    foreach hdr [lsort [OCCDoc_GetHeadersList "f" "$pk" "$theProductsDir"]] {
      if { ! [regexp {^Handle_} $hdr] && [regexp {(.*)[.]hxx} $hdr str obj] } {
        puts $fd "\\li \\subpage $obj"
      }
    }
    puts $fd "**/\n"
  }

  close $fd

  return $outFile
}

# Parses generated files to add a navigation path 
proc OCCDoc_PostProcessor {outDir} {
  puts "[clock format [clock seconds] -format {%Y.%m.%d %H:%M}] Post-process is started ..."
  append outDir "/html"
  set files [glob -nocomplain -type f $outDir/package_*]
  if { $files != {} } {
    foreach f [lsort $files] {
      set packageFilePnt  [open $f r]
      set packageFile     [read $packageFilePnt]
      set navPath         [OCCDoc_GetNodeContents "div" " id=\"nav-path\" class=\"navpath\"" $packageFile]
      set packageName     [OCCDoc_GetNodeContents "div" " class=\"title\"" $packageFile]
      regsub -all {<[^<>]*>} $packageName "" packageName 

      # add package link to nav path
      set first           [expr 1 + [string last "/" $f]]
      set last            [expr [string length $f] - 1]
      set packageFileName [string range $f $first $last]
      set end             [string first "</ul>" $navPath]
      set end             [expr $end - 1]
      set navPath         [string range $navPath 0 $end]
      append navPath "  <li class=\"navelem\"><a class=\"el\" href=\"$packageFileName\">$packageName</a>      </li>\n    </ul>"

      # get list of files to update
      set listContents [OCCDoc_GetNodeContents "div" " class=\"textblock\"" $packageFile]
      set listContents [OCCDoc_GetNodeContents "ul" "" $listContents]
      set lines [split $listContents "\n"]
      foreach line $lines {
        if {[regexp {href=\"([^\"]*)\"} $line tmpLine classFileName]} {
          # check if anchor is there
          set anchorPos [string first "#" $classFileName]
          if {$anchorPos != -1} {
            set classFileName [string range $classFileName 0 [expr $anchorPos - 1]]
          }

          # read class file
          set classFilePnt [open $outDir/$classFileName r+]
          set classFile    [read $classFilePnt]

          # find position of content block 
          set contentPos   [string first "<div class=\"header\">" $classFile]
          set navPart      [string range $classFile 0 [expr $contentPos - 1]]

          # position where to insert nav path
          set posToInsert  [string last "</div>" $navPart]
          set prePart      [string range $classFile 0 [expr $posToInsert - 1]]
          set postPart     [string range $classFile $posToInsert [string length $classFile]]
          set newClassFile ""
          append newClassFile $prePart "  <div id=\"nav-path\" class=\"navpath\">" $navPath \n "  </div>" \n $postPart

          # write updated content
          seek $classFilePnt 0
          puts $classFilePnt $newClassFile
          close $classFilePnt
        } 
      }
      close $packageFilePnt
    }
    return 0
  } else {
    puts "no files found"
    return 1
  }
}

# ======================================
#  User Guides-specific functions
# ======================================

# Loads a list of docfiles from file FILES.txt
proc OCCDoc_LoadFilesList {} {
  set INPUTDIR [OCCDoc_GetDoxDir [OCCDoc_GetProdRootDir]]

  global available_docfiles
  set available_docfiles {}

  # Read data from file
  if { [file exists "$INPUTDIR/FILES_HTML.txt"] == 1 } {
    set FILE [open "$INPUTDIR/FILES_HTML.txt" r]
    while {1} {
      set line [string trim [gets $FILE]]

      # trim possible comments starting with '#'
      set line [regsub {\#.*} $line {}]
      if {$line != ""} {
        lappend available_docfiles $line
      }
      if {[eof $FILE]} {
        close $FILE
        break
      }
    }
  } else {
    return -1
  }

  global available_pdf
  set    available_pdf {}

  # Read data from file
  if { [file exists "$INPUTDIR/FILES_PDF.txt"] } {
    set FILE [open "$INPUTDIR/FILES_PDF.txt" r]
    while {1} {
      set line [string trim [gets $FILE]]

      # Trim possible comments starting with '#'
      set line [regsub {\#.*} $line {}]
      if {$line != ""} {
        lappend available_pdf $line
      }
      if {[eof $FILE]} {
        close $FILE
        break
      }
    }
  } else {
    return -1
  }
  return 0
}

# Writes new TeX file for conversion from tex to pdf for a specific doc
proc OCCDoc_MakeRefmanTex {fileName latexDir verboseMode latexFilesList} {

  if { $verboseMode == "YES" } {
    puts "Info: Making refman.tex file for $fileName..."
  }
  set DOCNAME "$latexDir/refman.tex"
  if {[file exists $DOCNAME] == 1} {
    file delete -force $DOCNAME
  }

  # Copy template file to latex folder
  if { "[OCCDoc_GetProdRootDir]" != "" } {
    file copy "[OCCDoc_GetDoxDir [OCCDoc_GetProdRootDir]]/resources/prod_pdf_template.tex" $DOCNAME
  } else {
    file copy "[OCCDoc_GetDoxDir]/resources/occt_pdf_template.tex" $DOCNAME
  }

  # Get templatized data
  set texfile [open $DOCNAME "r"]
  set texfile_loaded [read $texfile]
  close $texfile

  # Replace dummy values 
  set year       [clock format [clock seconds] -format {%Y}]
  set month      [clock format [clock seconds] -format {%B}]
  set texfile    [open $DOCNAME "w"]
  set casVersion [OCCDoc_DetectCasVersion]

  # Get name of the document
  set docLabel   ""
  foreach aFileName $latexFilesList {
    # Find the file in FILES_PDF.txt
    set parsedFileName [file rootname [lindex [split $aFileName "/" ] end]]
    if { [regexp "${parsedFileName}$" $fileName] } {
      set filepath "[OCCDoc_GetDoxDir [OCCDoc_GetProdRootDir]]/$aFileName"
      if { [file exists $filepath] } {
        set MDFile   [open $filepath "r"]
        set label    [split [gets $MDFile] "\{"]
        set docLabel [lindex $label 0]
        close $MDFile
        break
      }
    }
  }

  set occtlogo_path "[OCCDoc_GetDoxDir]/resources/occt_logo.png"
  set occlogo_path  "[OCCDoc_GetDoxDir]/resources/occ_logo.png"
  set copyright_path  "[OCCDoc_GetDoxDir [OCCDoc_GetProdRootDir]]/resources/prod_pdf_template.tex"
  set texfile_loaded [string map [list DEFDOCLABEL "$docLabel" DEFCASVERSION "$casVersion" DEFFILENAME "$fileName" DEFYEAR "$year" DEFMONTH "$month" DEFCOPYRIGHT "$copyright_path" DEFLOGO "$occtlogo_path" DEFOCCLOGO "$occlogo_path" DEFTITLE ""] $texfile_loaded]

  # Get data
  puts $texfile $texfile_loaded

  close $texfile
}

# Postprocesses generated TeX files
proc OCCDoc_ProcessTex {{texFiles {}} {latexDir} verboseMode} {

  foreach TEX $texFiles {
    if {$verboseMode == "YES"} {
      puts "Info: Preprocessing file $TEX..."
    }

    if {![file exists $TEX]} {
      puts "Error: file $TEX does not exist."
      return -1
    }

    set IN_F        [open "$TEX" "r"]
    set TMPFILENAME "$latexDir/temp.tex"
    set OUT_F       [open $TMPFILENAME w]

        while {1} {
            set line [gets $IN_F]
            if { [string first "\\includegraphics" $line] != -1 } {
              # replace svg extension by pdf
              set line [regsub {[.]svg} $line ".pdf"]
              # Center images in TeX files
              set line "\\begin{center}\n $line\n\\end{center}"
            } elseif { [string first "\\subsection" $line] != -1 } {
                # Replace \subsection with \section tag
                regsub -all "\\\\subsection" $line "\\\\section" line
            } elseif { [string first "\\subsubsection" $line] != -1 } {
                # Replace \subsubsection with \subsection tag
                regsub -all "\\\\subsubsection" $line "\\\\subsection" line
            } elseif { [string first "\\paragraph" $line] != -1 } {
                # Replace \paragraph with \subsubsection tag
                regsub -all "\\\\paragraph" $line "\\\\subsubsection" line
            }
            puts $OUT_F $line

      if {[eof $IN_F]} {
        close $IN_F
        close $OUT_F
        break
      }
    }

    file delete -force $TEX
    file rename $TMPFILENAME $TEX
  }
}