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
# This script defines command gendoc compiling OCCT documents 
# from *.md files to HTML pages
# =======================================================================

# load auxiliary tools
source [file join [file dirname [info script]] occaux.tcl]

# ======================================
#  Common functions
# ======================================

# Prints help message
proc OCCDoc_PrintHelpMessage {} {
    puts "\nUsage: gendoc \[-h\] {-refman|-overview} \[-html|-pdf|-chm\] \[-m=<list of modules>|-ug=<list of docs>\] \[-v\] \[-s=<search_mode>\] \[-mathjax=<path>\]"
    puts ""
    puts "Options are:"
    puts ""
    puts "choice of documentation to be generated:"
    puts "  -overview          : To generate Overview and User Guides"
    puts "                       (cannot be used with -refman)"
    puts "  -refman            : To generate class Reference Manual"
    puts "                       (cannot be used with -overview)"
    puts ""
    puts "choice of output format:"
    puts "  -html              : To generate HTML files"
    puts "                       (default, cannot be used with -pdf or -chm)"
    puts "  -pdf               : To generate PDF files"
    puts "                       (cannot be used with -refman, -html, or -chm)"
    puts "  -chm               : To generate CHM files"
    puts "                       (cannot be used with -html or -pdf)"
    puts ""
    puts "additional options:"
    puts "  -m=<modules_list>  : List of OCCT modules (separated with comma),"
    puts "                       for generation of Reference Manual"
    puts "  -ug=<docs_list>    : List of MarkDown documents (separated with comma),"
    puts "                       to use for generation of Overview / User Guides"
    puts "  -mathjax=<path>    : To use local or alternative copy of MathJax"
    puts "  -s=<search_mode>   : Specifies the Search mode of HTML documents"
    puts "                       Can be: none | local | server | external"
    puts "  -h                 : Prints this help message"
    puts "  -v                 : Enables more verbose output"
}

# A command for User Documentation compilation
proc gendoc {args} {

  # Parameters
  set DOC_TYPE                  "REFMAN"
  set GEN_MODE                  "HTML_ONLY"
  set DOCFILES                  {}
  set MODULES                   {}
  set DOCLABEL                  ""
  set VERB_MODE                 "NO"
  set SEARCH_MODE               "none"
  set MATHJAX_LOCATION          "https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.1"
  set mathjax_js_name           "MathJax.js"
  set DOCTYPE_COMBO_FLAG        0
  set GENMODE_COMBO_FLAG        0
  set GENERATE_PRODUCTS_REFMAN "NO"

  global available_docfiles;   # The full list of md files for HTML or CHM generation
  global available_pdf;        # The full list of md files for PDF generation
  global tcl_platform
  global args_names
  global args_values
  global env

  # Load list of docfiles
  if { [OCCDoc_LoadFilesList] != 0 } {
    puts "Error: File FILES_HTML.txt or FILES_PDF.txt was not found on this computer.\nAborting..."
    return -1
  }

  # Parse CL arguments
  if {[OCCDoc_ParseArguments $args] == 1} {
    return -1
  }

  # Print help message if no arguments provided
  if {[llength $args_names] == 0} {
    OCCDoc_PrintHelpMessage
    return 0
  }

  foreach arg_n $args_names {
    if {$arg_n == "h"} {
      OCCDoc_PrintHelpMessage
      return 0
    } elseif {$arg_n == "html"} {
      if { ([ lsearch $args_names "refman" ]   == -1) &&
           ([ lsearch $args_names "overview" ] == -1) } {
        puts "Warning: Please specify -refman or -overview argument."
        return -1
      }
      if { [ lsearch $args_names "refman" ] != -1 } {
        continue
      }
      if { $GENMODE_COMBO_FLAG != 1 } {
        set GEN_MODE "HTML_ONLY"
        set GENMODE_COMBO_FLAG 1
      } else {
        puts "Error: Options -html, -pdf and -chm can not be combined."
        return -1
      }
    } elseif {$arg_n == "chm"} {
      if { ([ lsearch $args_names "refman" ]   == -1) &&
           ([ lsearch $args_names "overview" ] == -1) } {
        puts "Warning: Please specify -refman or -overview argument."
        return -1
      }
      if { [ lsearch $args_names "refman" ] != -1 } {
        continue
      }
      if { $GENMODE_COMBO_FLAG != 1 } { 
        set GEN_MODE "CHM_ONLY"
        set GENMODE_COMBO_FLAG 1
      } else {
        puts "Error: Options -html, -pdf and -chm cannot be combined."
        return -1
      }
    } elseif {$arg_n == "pdf"} {
      if { ([ lsearch $args_names "refman" ]   == -1) &&
           ([ lsearch $args_names "overview" ] == -1) } {
        puts "Warning: Please specify -refman or -overview argument."
        return -1
      }
      if { [ lsearch $args_names "refman" ] != -1 } {
        continue
      }
      if { $GENMODE_COMBO_FLAG != 1 } { 
        set GEN_MODE "PDF_ONLY"
        set GENMODE_COMBO_FLAG 1
      } else {
        puts "Error: Options -html, -pdf and -chm cannot be combined."
        return -1
      }
    } elseif {$arg_n == "overview"} {
      if { $DOCTYPE_COMBO_FLAG != 1 } {
        set DOC_TYPE "OVERVIEW"
        set DOCTYPE_COMBO_FLAG 1
      } else {
        puts "Error: Options -refman and -overview cannot be combined."
        return -1
      }

      # Print ignored options
      if { [ lsearch $args_names "m" ] != -1 } {
        puts "\nInfo: The following options will be ignored: \n"
        puts "  * -m"
      }
      puts ""
    } elseif {$arg_n == "refman"} {
      if { $DOCTYPE_COMBO_FLAG != 1 } { 
        set DOC_TYPE "REFMAN"
        set DOCTYPE_COMBO_FLAG 1
        if { [file exists [OCCDoc_GetProdRootDir]/src/VAS/Products.tcl] } {
          set GENERATE_PRODUCTS_REFMAN "YES"
        }
      } else {
        puts "Error: Options -refman and -overview cannot be combined."
        return -1
      }
      # Print ignored options
      if { ([ lsearch $args_names "pdf" ]     != -1) || 
           ([ lsearch $args_names "chm" ]     != -1) || 
           ([ lsearch $args_names "ug" ]      != -1) } {
        puts "\nInfo: The following options will be ignored: \n"
        if { [ lsearch $args_names "pdf" ] != -1 } {
          puts "  * -pdf"
        }
        if { [ lsearch $args_names "chm" ] != -1 } {
          puts "  * -chm"
        }
        if { [ lsearch $args_names "ug" ] != -1 } {
          puts "  * -ug"
        }
        puts ""
      }
      
    } elseif {$arg_n == "v"} {
      set VERB_MODE "YES"
    } elseif {$arg_n == "ug"} {
      if { ([ lsearch $args_names "refman" ]   != -1) } {
        continue
      }
      if {$args_values(ug) != "NULL"} {
        set DOCFILES $args_values(ug)
      } else {
        puts "Error in argument ug."
        return -1
      }
      # Check if all chosen docfiles are correct
      foreach docfile $DOCFILES {
        if { [ lsearch $args_names "pdf" ] == -1 } {
          # Check to generate HTMLs
          if { [lsearch $available_docfiles $docfile] == -1 } {
            puts "Error: File \"$docfile\" is not presented in the list of available docfiles."
            puts "       Please specify the correct docfile name."
            return -1
          } 
        } else {
          # Check to generate PDFs
          if { [lsearch $available_pdf $docfile] == -1 } {
            puts "Error: File \"$docfile\" is not presented in the list of generic PDFs."
            puts "       Please specify the correct pdf name."
            return -1
          }
        }
      }
    } elseif {$arg_n == "m"} {
      if { [ lsearch $args_names "overview" ] != -1 } {
        continue
      }
      if {$args_values(m) != "NULL"} {
        set MODULES $args_values(m)
      } else {
        puts "Error in argument m."
        return -1
      }
    } elseif {$arg_n == "s"} {
      if { [ lsearch $args_names "pdf" ] != -1 } {
        continue
      }
      if {$args_values(s) != "NULL"} {
        set SEARCH_MODE $args_values(s)
      } else {
        puts "Error in argument s."
        return -1
      }
    } elseif {$arg_n == "mathjax"} {
      if { [ lsearch $args_names "pdf" ] != -1 } {
        set possible_mathjax_loc $args_values(mathjax)
        if {[file exist [file join $possible_mathjax_loc $mathjax_js_name]]} {
          set MATHJAX_LOCATION $args_values(mathjax)
          puts "$MATHJAX_LOCATION"
        } else {
          puts "Warning: $mathjax_js_name is not found in $possible_mathjax_loc."
          puts "         MathJax will be used from $MATHJAX_LOCATION"
        }
      } else {
        puts "Warning: MathJax is not used with pdf and will be ignored."
      }
    } else {
      puts "\nWrong argument: $arg_n"
      OCCDoc_PrintHelpMessage
      return -1
    }
  }

  # Check the existence of the necessary tools
  set DOXYGEN_PATH  ""
  set GRAPHVIZ_PATH ""
  set INKSCAPE_PATH ""
  set PDFLATEX_PATH ""
  set HHC_PATH      ""

  OCCDoc_DetectNecessarySoftware $DOXYGEN_PATH $GRAPHVIZ_PATH $INKSCAPE_PATH $HHC_PATH $PDFLATEX_PATH

  if {$DOXYGEN_PATH == ""} {
    puts " Aborting..."
    return -1
  }

  if {"$::tcl_platform(platform)" == "windows"} {
    if { ($GEN_MODE == "CHM_ONLY") && ($HHC_PATH == "") } {
      puts " Aborting..."
      return -1
    }
  }

  if { ($PDFLATEX_PATH == "") && ($GEN_MODE == "PDF_ONLY") } {
    puts " Aborting..."
    return -1
  }

  # If we do not specify list for docfiles with -m argument,
  # we assume that we have to generate all docfiles
  if { [llength $DOCFILES] == 0 } {
    if { $GEN_MODE != "PDF_ONLY" } {
      set DOCFILES $available_docfiles
    } else {
      set DOCFILES $available_pdf
    }
  }

  puts ""

  # Start main activities
  if { $GEN_MODE != "PDF_ONLY" } {
    if { [OCCDoc_GetProdRootDir] == ""} {
      OCCDoc_Main $DOC_TYPE $DOCFILES $MODULES $GEN_MODE $VERB_MODE $SEARCH_MODE $MATHJAX_LOCATION $GENERATE_PRODUCTS_REFMAN $DOXYGEN_PATH $GRAPHVIZ_PATH $INKSCAPE_PATH $HHC_PATH
    } else {
      if { $DOC_TYPE == "REFMAN" } {
        if { $MODULES != "" } {
          foreach module $MODULES {
            OCCDoc_Main $DOC_TYPE $DOCFILES $module $GEN_MODE $VERB_MODE $SEARCH_MODE $MATHJAX_LOCATION $GENERATE_PRODUCTS_REFMAN $DOXYGEN_PATH $GRAPHVIZ_PATH $INKSCAPE_PATH $HHC_PATH
          }
        } else {
          OCCDoc_Main $DOC_TYPE $DOCFILES $MODULES $GEN_MODE $VERB_MODE $SEARCH_MODE $MATHJAX_LOCATION $GENERATE_PRODUCTS_REFMAN $DOXYGEN_PATH $GRAPHVIZ_PATH $INKSCAPE_PATH $HHC_PATH
        }
      } else {
        foreach md $DOCFILES {
          OCCDoc_Main $DOC_TYPE $md $MODULES $GEN_MODE $VERB_MODE $SEARCH_MODE $MATHJAX_LOCATION $GENERATE_PRODUCTS_REFMAN $DOXYGEN_PATH $GRAPHVIZ_PATH $INKSCAPE_PATH $HHC_PATH
        }
      }
    }
  } else {
    puts "Generating OCCT User Guides in PDF format..."
    foreach pdf $DOCFILES {

      puts "\nInfo: Processing file $pdf"

      # Some values are hardcoded because they are related only to PDF generation
      OCCDoc_Main "OVERVIEW" [list $pdf] {} "PDF_ONLY" $VERB_MODE "none" $MATHJAX_LOCATION "NO" $DOXYGEN_PATH $GRAPHVIZ_PATH $INKSCAPE_PATH $HHC_PATH
    }
    puts "\n[clock format [clock seconds] -format {%Y-%m-%d %H:%M}] Generation completed."
  }
}

# Main procedure for documents compilation
proc OCCDoc_Main {docType {docfiles {}} {modules {}} generatorMode verboseMode searchMode mathjaxLocation generateProductsRefman DOXYGEN_PATH GRAPHVIZ_PATH INKSCAPE_PATH HHC_PATH} {

  global available_docfiles
  global available_pdf

  set ROOTDIR    [OCCDoc_GetRootDir [OCCDoc_GetProdRootDir]]
  set INDIR      [OCCDoc_GetDoxDir]
  set OUTDIR     $ROOTDIR/doc
  set PDFDIR     $OUTDIR/pdf
  set UGDIR      $PDFDIR/user_guides
  set DGDIR      $PDFDIR/dev_guides
  set TAGFILEDIR $OUTDIR/refman
  set HTMLDIR    $OUTDIR/overview/html
  set LATEXDIR   $OUTDIR/overview/latex
  set DOXYFILE   $OUTDIR/OCCT.cfg

  # OUTDIR for products documentation should be separate directories for each components
  if { [OCCDoc_GetProdRootDir] != ""} {
    if { $docType == "REFMAN" } {
      if { "$modules" != "" } {
        source "[OCCDoc_GetSourceDir [OCCDoc_GetProdRootDir]]/VAS/${modules}.tcl"
        set doc_component_name [${modules}:documentation_name]
        set OUTDIR     $OUTDIR/$doc_component_name
      }
    } else {
      if {[regexp {([^/]+)/([^/]+)/([^/]+)} $docfiles dump doc_type doc_component doc_name]} {
        set PDFNAME [file rootname $doc_name]
        set OUTDIR     $OUTDIR/$doc_component
      } else {
        error "Could not parse input path to *.md file: \"${docfiles}\""
      }
    }
    set HTMLDIR    $OUTDIR/html
    set LATEXDIR   $OUTDIR/latex
    set DOXYFILE   $OUTDIR/OCCT.cfg
    set TAGFILEDIR $OUTDIR/refman
  }

  # Create or cleanup the output folders
  if { [string compare -nocase $generateProductsRefman "YES"] != 0 } {
    if { ![file exists $OUTDIR] } {
      file mkdir $OUTDIR
    } 
    if { ![file exists $HTMLDIR] } {
      file mkdir $HTMLDIR
    }
    if { [OCCDoc_GetProdRootDir] == ""} {
      if { ![file exists $PDFDIR] } {
        file mkdir $PDFDIR
      }
      if { ![file exists $UGDIR] } {
        file mkdir $UGDIR
      }
      if { ![file exists $DGDIR] } {
        file mkdir $DGDIR
      }
    }

    if { $generatorMode == "PDF_ONLY" } {
      if { [file exists $LATEXDIR] } {
        file delete -force $LATEXDIR
      }
      file mkdir $LATEXDIR
    }
  }
  if { $docType == "REFMAN" } {
    if { ![file exists $TAGFILEDIR] } {
      file mkdir $TAGFILEDIR
    }
  }

  # is MathJax HLink?
  set mathjax_relative_location $mathjaxLocation
  if { [file isdirectory "$mathjaxLocation"] } {
    if { $generatorMode == "HTML_ONLY" } {
      # related path
      set mathjax_relative_location [OCCDoc_GetRelPath $HTMLDIR $mathjaxLocation]
    } elseif { $generatorMode == "CHM_ONLY" } {
      # absolute path
      set mathjax_relative_location [file normalize $mathjaxLocation]
    }
  }

  if { $generateProductsRefman == "YES" } {
    set DOCDIR "$OUTDIR/refman"
    puts "\nGenerating OCC Products Reference Manual\n"
  } else {
    if { $docType == "REFMAN"} {
      set DOCDIR "$OUTDIR/refman"
      puts "\nGenerating Open CASCADE Reference Manual\n"
    } elseif { $docType == "OVERVIEW" } {
      if { [OCCDoc_GetProdRootDir] == ""} {
        set DOCDIR "$OUTDIR/overview"
      } else {
        set DOCDIR "$OUTDIR"
      }
      set FORMAT ""
      if { ($generatorMode == "HTML_ONLY") || ($generatorMode == "CHM_ONLY") } {
        if { $generatorMode == "HTML_ONLY" } { 
          set FORMAT " in HTML format..."
        } elseif { $generatorMode == "CHM_ONLY" } {
          set FORMAT " in CHM format..."
        }
        puts "Generating OCCT User Guides$FORMAT\n"
      }
    } else {
      puts "Error: Invalid documentation type: $docType. Can not process."
      return -1
    }
  }

  # Generate Doxyfile
  puts "[clock format [clock seconds] -format {%Y-%m-%d %H:%M}] Generating Doxyfile..."

  if { [OCCDoc_MakeDoxyfile $docType $DOCDIR $TAGFILEDIR $DOXYFILE $generatorMode $docfiles $modules $verboseMode $searchMode $HHC_PATH $mathjax_relative_location $GRAPHVIZ_PATH [OCCDoc_GetProdRootDir]] == -1 } {
    return -1
  }

  # Run doxygen tool
  set starttimestamp [clock format [clock seconds] -format {%Y-%m-%d %H:%M}]

  if { ($generatorMode == "HTML_ONLY") || ($docType == "REFMAN") } {
    set LOGPREFIX "html_"
    puts "$starttimestamp Generating HTML files..."

    # Copy index file to provide fast access to HTML documentation
    file copy -force $INDIR/resources/index.html $DOCDIR/index.html
  } elseif { $generatorMode == "CHM_ONLY" } {
    set LOGPREFIX "chm_"
    puts "$starttimestamp Generating CHM file..."
  } elseif { $generatorMode == "PDF_ONLY" } {
    set LOGPREFIX "[file rootname [file tail [lindex $docfiles 0]]]_"
    puts "$starttimestamp Generating PDF file..."
  }

  # Clean logfiles  
  set DOXYLOG $OUTDIR/${LOGPREFIX}doxygen_err.log
  set DOXYOUT $OUTDIR/${LOGPREFIX}doxygen_out.log
  file delete -force $DOXYLOG
  file delete -force $DOXYOUT

  set RESULT [catch {exec $DOXYGEN_PATH $DOXYFILE >> $DOXYOUT} DOX_ERROR] 
  if {$RESULT != 0} {
    set NbErrors [regexp -all -line {^\s*[^\s]+} $DOX_ERROR]
    if {$NbErrors > 0} {
      puts "Warning: Doxygen reported $NbErrors messages."
      puts "See log in $DOXYLOG"
      set DOX_ERROR_FILE [open $DOXYLOG "a"]
      if {$generatorMode == "PDF_ONLY"} {
        puts $DOX_ERROR_FILE "\n===================================================="
        puts $DOX_ERROR_FILE "Logfile for $docfiles"
        puts $DOX_ERROR_FILE "====================================================\n"
      }
      puts $DOX_ERROR_FILE $DOX_ERROR
      close $DOX_ERROR_FILE
    }
  }

  # Close the Doxygen application
  after 300

  # Start Post Processing
  set curtime [clock format [clock seconds] -format {%Y-%m-%d %H:%M}]
  if { $docType == "REFMAN" } {
    # Post Process generated HTML pages and draw dependency graphs
    if {[OCCDoc_PostProcessor $DOCDIR] == 0} {
      puts "$curtime Generation completed."
      puts "\nInfo: doxygen log file is located in:"
      puts "${DOXYOUT}."
      puts "\nReference Manual is generated in \n$DOCDIR"
    }
  } elseif { $docType == "OVERVIEW" } {
    # Start PDF generation routine
    if { $generatorMode == "PDF_ONLY" } {
      set OS $::tcl_platform(platform)
      if { $OS == "unix" } {
        set PREFIX ".sh"
      } elseif { $OS == "windows" } {
        set PREFIX ".bat"
      }

      # Prepare a list of TeX files, generated by Doxygen
      cd $LATEXDIR

      set TEXFILES   [glob $LATEXDIR -type f -directory $LATEXDIR -tails "*.tex" ]
      foreach path $TEXFILES {
        if { [string compare -nocase $path $LATEXDIR] == 0 } {
          set DEL_IDX [lsearch $TEXFILES $path]
          if { $DEL_IDX != -1 } {
            set TEXFILES [lreplace $TEXFILES $DEL_IDX $DEL_IDX]
          }
        }
      }
      set TEXFILES   [string map [list refman.tex ""] $TEXFILES]
      if {$verboseMode == "YES"} {
        puts "Info: Preprocessing generated TeX files..."
      }
      OCCDoc_ProcessTex $TEXFILES $LATEXDIR $verboseMode

      if {$verboseMode == "YES"} {
        puts "Info: Converting SVG images to PNG format..."
      }

      if { $INKSCAPE_PATH != "" } {
        OCCDoc_ProcessSvg $LATEXDIR $verboseMode
      } else {
        puts "Warning: SVG images will be lost in PDF documents."
      }

      if {$verboseMode == "YES"} {
        puts "Info: Generating PDF file from TeX files..."
      }
      foreach TEX $TEXFILES {
        # Rewrite existing REFMAN.tex file...
        set TEX [lindex [split $TEX "."] 0]

        if {$verboseMode == "YES"} {
          puts "Info: Generating PDF file from $TEX..."
        }

        OCCDoc_MakeRefmanTex $TEX $LATEXDIR $verboseMode $available_pdf

        if {"$::tcl_platform(platform)" == "windows"} {
          set is_win "yes"
        } else {
          set is_win "no"
        }
        if {$verboseMode == "YES"} {
          # ...and use it to generate PDF from TeX...
          if {$is_win == "yes"} {
            puts "Info: Executing $LATEXDIR/make.bat..."
          } else {
            puts "Info: Executing $LATEXDIR/Makefile..."
          }
        }

        set PDFLOG $OUTDIR/${LOGPREFIX}pdflatex_err.log
        set PDFOUT $OUTDIR/${LOGPREFIX}pdflatex_out.log
        file delete -force $PDFLOG
        file delete -force $PDFOUT

        if {"$is_win" == "yes"} {
          set RESULT [catch {eval exec [auto_execok $LATEXDIR/make.bat] >> "$PDFOUT"} LaTeX_ERROR]
        } else {
          set RESULT [catch {eval exec "make -f $LATEXDIR/Makefile" >> "$PDFOUT"} LaTeX_ERROR]

          # Small workaround for *nix stations
          set prev_loc [pwd]
          cd $LATEXDIR
          set RESULT [catch {eval exec "pdflatex refman.tex" >> "$PDFOUT"} LaTeX_ERROR]
          cd $prev_loc
        }

        if {$RESULT != 0} {
          set NbErrors [regexp -all -line {^\s*[^\s]+} $LaTeX_ERROR]
          if {$NbErrors > 0} {
            puts "Warning: PDFLaTeX reported $NbErrors messages.\nSee log in $PDFLOG"
            set LaTeX_ERROR_FILE [open $PDFLOG "a+"]
            puts $LaTeX_ERROR_FILE "\n===================================================="
            puts $LaTeX_ERROR_FILE "Logfile of file $TEX:"
            puts $LaTeX_ERROR_FILE "====================================================\n"
            puts $LaTeX_ERROR_FILE $LaTeX_ERROR
            close $LaTeX_ERROR_FILE
          }
        }

        # ...and place it to the specific folder
        if {![file exists "$LATEXDIR/refman.pdf"]} {
          puts "Fatal: PDFLaTeX failed to create output file, stopping!"
          return -1
        }

        set destFolder $PDFDIR
        set parsed_string [split $TEX "_"]
        if { [OCCDoc_GetProdRootDir] == ""} {
          if { [lsearch $parsed_string "tutorial"] != -1 } {
            set TEX [string map [list occt__ occt_] $TEX]
            set destFolder $PDFDIR
          } elseif { [lsearch $parsed_string "user"] != -1 } {
            set TEX [string map [list user_guides__ ""] $TEX]
            set destFolder $UGDIR
          } elseif { [lsearch $parsed_string "dev"]  != -1 } {
            set TEX [string map [list dev_guides__ ""] $TEX]
            set destFolder $DGDIR
          }
        } else {
          set destFolder $OUTDIR
          set TEX "$PDFNAME"
        }
        file rename -force $LATEXDIR/refman.pdf "$destFolder/$TEX.pdf"
        puts "Generated $destFolder/$TEX.pdf"
      }
    } elseif { $generatorMode == "CHM_ONLY" } {
      if { [OCCDoc_GetProdRootDir] == ""} {
        file rename $OUTDIR/overview.chm $OUTDIR/occt_overview.chm
      } else {
        file rename -force $ROOTDIR/doc/overview.chm $OUTDIR/occt_overview.chm
      }
    }
    cd $INDIR

    if { $generatorMode == "HTML_ONLY" } {
      puts "HTML documentation is generated in \n$DOCDIR"
    } elseif { $generatorMode == "CHM_ONLY" } {
      puts "Generated CHM documentation is in \n$OUTDIR/overview.chm"
    }
  }

  # Remove temporary Doxygen files
  set deleteList [glob -nocomplain -type f "*.tmp"]
  foreach file $deleteList {
    file delete $file
  }

  return 0
}

# Generates Doxygen configuration file for Overview documentation
proc OCCDoc_MakeDoxyfile {docType outDir tagFileDir {doxyFileName} {generatorMode ""} {DocFilesList {}} {ModulesList {}} verboseMode searchMode hhcPath mathjaxLocation graphvizPath productsPath} {
  set inputDir      [OCCDoc_GetDoxDir [OCCDoc_GetProdRootDir]]

  set TEMPLATES_DIR [OCCDoc_GetDoxDir]/resources
  set occt_version  [OCCDoc_DetectCasVersion]

  # Delete existent doxyfile
  file delete $doxyFileName

  # Copy specific template to the target folder
  if { $docType == "REFMAN" } {
    file copy "$TEMPLATES_DIR/occt_rm.doxyfile" $doxyFileName
  } elseif { $docType == "OVERVIEW" } {
    if { $generatorMode == "HTML_ONLY" || $generatorMode == "CHM_ONLY" } {
      file copy "$TEMPLATES_DIR/occt_ug_html.doxyfile" $doxyFileName
    } elseif { $generatorMode == "PDF_ONLY"} {
      file copy "$TEMPLATES_DIR/occt_ug_pdf.doxyfile" $doxyFileName
    } else {
      puts "Error: Unknown generation mode"
      return -1
    }
  } else {
    puts "Error: Cannot generate unknown document type"
    return -1
  }

  set doxyFile [open $doxyFileName "a"]
  # Write specific options
  if { $docType == "REFMAN" } {

    # Load lists of modules scripts
    if { $productsPath == "" } {
      set modules_scripts [glob -nocomplain -type f -directory "[OCCDoc_GetSourceDir $productsPath]/OS/" *.tcl]
    } else {
      set modules_scripts [glob -nocomplain -type f -directory "[OCCDoc_GetSourceDir $productsPath]/VAS/" *.tcl]
    }
    if { [llength $modules_scripts] != 0} {
      foreach module_file $modules_scripts {
        source $module_file
      }
    }

    set ALL_MODULES [OCCDoc_GetModulesList $productsPath]
    if { [llength $ModulesList] == 0 } {
      # by default take all modules
      set modules $ALL_MODULES
    } else {
      set modules $ModulesList
    }

    # Detect invalid names of modules
    foreach module $modules {
      if { $module == "" } {
        continue
      }
      if {[lsearch $ALL_MODULES $module] == -1 } {
        puts "Error: No module $module is known. Aborting..."
        return -1
      }
    }

    # Set context
    set one_module [expr [llength $modules] == 1]
    if { $one_module } {
      set title "OCCT [$modules:name]"
      set name $modules
    } else {
      set title "Open CASCADE Technology"
      set name OCCT
    }

    # Get list of header files in the specified modules
    set filelist {}
    foreach module $modules {
      if { $module == "" } {
        continue
      }
      foreach tk [$module:toolkits] {
        foreach pk [split [OCCDoc_GetPackagesList [OCCDoc_Locate $tk $productsPath]]] {
          if { [llength $pk] != "{}" } {
            lappend filelist [OCCDoc_GetHeadersList "p" "$pk" "$productsPath"]
          }
        }
      }
    }

    # Filter out files Handle_*.hxx and *.lxx
    set hdrlist {}
    foreach fileset $filelist {
      set hdrset {}
      foreach hdr $fileset {
        if { ! [regexp {Handle_.*[.]hxx} $hdr] && ! [regexp {.*[.]lxx} $hdr] } {
          lappend hdrset $hdr
        }
      }
      lappend hdrlist $hdrset
    }
    set filelist $hdrlist

    set doxyFile [open $doxyFileName "a"]
  
    puts $doxyFile "PROJECT_NAME           = \"$title\""
    puts $doxyFile "PROJECT_NUMBER         = $occt_version"
    puts $doxyFile "OUTPUT_DIRECTORY       = $outDir/."
    puts $doxyFile "GENERATE_TAGFILE       = $outDir/${name}.tag"

    if { [string tolower $searchMode] == "none" } {
      puts $doxyFile "SEARCHENGINE           = NO"
      puts $doxyFile "SERVER_BASED_SEARCH    = NO"
      puts $doxyFile "EXTERNAL_SEARCH        = NO"
    } else {
      puts $doxyFile "SEARCHENGINE           = YES"
      if { [string tolower $searchMode] == "local" } {
        puts $doxyFile "SERVER_BASED_SEARCH    = NO"
        puts $doxyFile "EXTERNAL_SEARCH        = NO"
      } elseif { [string tolower $searchMode] == "server" } {
        puts $doxyFile "SERVER_BASED_SEARCH    = YES"
        puts $doxyFile "EXTERNAL_SEARCH        = NO"
      } elseif { [string tolower $searchMode] == "external" } {
        puts $doxyFile "SERVER_BASED_SEARCH    = YES"
        puts $doxyFile "EXTERNAL_SEARCH        = YES"
      } else {
        puts "Error: Wrong search engine type: $searchMode."
        close $doxyFile 
        return -1
      }
    }

    puts $doxyFile "DOTFILE_DIRS             = $outDir/html"
    puts $doxyFile "DOT_PATH                 = $graphvizPath"
    puts $doxyFile "INCLUDE_PATH             = [OCCDoc_GetSourceDir $productsPath]"
    
    # list of files to generate
    set mainpage [OCCDoc_MakeMainPage $outDir $outDir/$name.dox $modules $productsPath]
    puts $doxyFile ""
    puts $doxyFile "INPUT    = $mainpage \\"
    foreach header $filelist {
      puts $doxyFile "               $header \\"
    }

    puts $doxyFile "MATHJAX_FORMAT         = HTML-CSS"
    puts $doxyFile "MATHJAX_RELPATH        = ${mathjaxLocation}"

    puts $doxyFile ""

  } elseif { $docType == "OVERVIEW" } {

    # Add common options for generation of Overview and User Guides
    puts $doxyFile "PROJECT_NUMBER         = $occt_version"
    puts $doxyFile "OUTPUT_DIRECTORY       = $outDir/."
    puts $doxyFile "PROJECT_LOGO           = [OCCDoc_GetDoxDir]/resources/occ_logo.png"

    set PARAM_INPUT "INPUT                 ="
    set PARAM_IMAGEPATH "IMAGE_PATH        = [OCCDoc_GetDoxDir]/resources/ "
    foreach docFile $DocFilesList {
      set NEW_IMG_PATH "$inputDir/$docFile"
      if { [string compare $NEW_IMG_PATH [OCCDoc_GetRootDir $productsPath]] != 0 } {
        set img_string [file dirname $NEW_IMG_PATH]/images
        if { [file exists $img_string] } {
          append PARAM_IMAGEPATH " $img_string"
        }
      }
      append PARAM_INPUT " " $inputDir/$docFile
    }
    puts $doxyFile $PARAM_INPUT
    puts $doxyFile $PARAM_IMAGEPATH

    # Add document type-specific options
    if { $generatorMode == "HTML_ONLY"} {
      # generate tree view
      puts $doxyFile "GENERATE_TREEVIEW      = YES"

      # Set a reference to a TAGFILE
      if { $tagFileDir != "" } {
        if {[file exists $tagFileDir/OCCT.tag] == 1} {
          #set tagPath [OCCDoc_GetRelPath $tagFileDir $outDir/html]
          set tagPath $tagFileDir
          puts $doxyFile "TAGFILES               = $tagFileDir/OCCT.tag=../../refman/html"
        }
      }
      # HTML Search engine options
      if { [string tolower $searchMode] == "none" } {
        puts $doxyFile "SEARCHENGINE           = NO"
        puts $doxyFile "SERVER_BASED_SEARCH    = NO"
        puts $doxyFile "EXTERNAL_SEARCH        = NO"
      } else {
        puts $doxyFile "SEARCHENGINE           = YES"
        if { [string tolower $searchMode] == "local" } {
          puts $doxyFile "SERVER_BASED_SEARCH    = NO"
          puts $doxyFile "EXTERNAL_SEARCH        = NO"
        } elseif { [string tolower $searchMode] == "server" } {
          puts $doxyFile "SERVER_BASED_SEARCH    = YES"
          puts $doxyFile "EXTERNAL_SEARCH        = NO"
        } elseif { [string tolower $searchMode] == "external" } {
          puts $doxyFile "SERVER_BASED_SEARCH    = YES"
          puts $doxyFile "EXTERNAL_SEARCH        = YES"
        } else {
          puts "Error: Wrong search engine type: $searchMode."
          close $doxyFile 
          return -1
        }
      }
    } elseif { $generatorMode == "CHM_ONLY"} {
      # specific options for CHM file generation
      puts $doxyFile "GENERATE_TREEVIEW      = NO"
      puts $doxyFile "SEARCHENGINE           = NO"
      puts $doxyFile "GENERATE_HTMLHELP      = YES"
      puts $doxyFile "CHM_FILE               = ../../overview.chm"
      puts $doxyFile "HHC_LOCATION           = \"$hhcPath\""
      puts $doxyFile "DISABLE_INDEX          = YES"
    }

    # Formula options
    puts $doxyFile "MATHJAX_RELPATH        = ${mathjaxLocation}"
  }

  close $doxyFile
  return 0
}
