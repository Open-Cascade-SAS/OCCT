# -----------------------------------------------------------------------
#  Script name: CompileDocs.tcl
#  This script compiles OCCT documents from *.md files to HTML pages
#  Author: omy
# -----------------------------------------------------------------------

# Generates Doxygen configuration file for Overview documentation
proc OverviewDoc_MakeDoxyfile {casDir outDir tagFileDir {doxyFileName} {generatorMode ""} DocFilesList verboseMode} {

    if {$verboseMode == "YES"} {
        puts "INFO: Doxygen is now generating Doxyfile..."
    }

    set doxyFile [open $doxyFileName "w"]
    set casroot  $casDir
    set inputDir $casDir/dox

    # Common configs
    puts $doxyFile "DOXYFILE_ENCODING      = UTF-8"
    puts $doxyFile "PROJECT_NAME           = \"Open CASCADE Technology\""
    puts $doxyFile "PROJECT_NUMBER         = 6.7.0"
    puts $doxyFile "PROJECT_BRIEF          = "
    puts $doxyFile "PROJECT_LOGO           = $inputDir/resources/occt_logo.png"

    puts $doxyFile "OUTPUT_DIRECTORY       = $outDir"
    puts $doxyFile "CREATE_SUBDIRS         = NO"
    puts $doxyFile "OUTPUT_LANGUAGE        = English"
    puts $doxyFile "ABBREVIATE_BRIEF       = \"The \$name class\" \
                                             \"The \$name widget\" \
                                             \"The \$name file\" \
                                             is \
                                             provides \
                                             specifies \
                                             contains \
                                             represents \
                                             a \
                                             an \
                                             the"

    puts $doxyFile "FULL_PATH_NAMES        = YES"
    puts $doxyFile "INHERIT_DOCS           = YES"
    puts $doxyFile "TAB_SIZE               = 4"
    puts $doxyFile "MARKDOWN_SUPPORT       = YES"
    puts $doxyFile "EXTRACT_ALL            = YES"
    puts $doxyFile "CASE_SENSE_NAMES       = NO"
    puts $doxyFile "INLINE_INFO            = YES"
    puts $doxyFile "SORT_MEMBER_DOCS       = YES"
    puts $doxyFile "WARNINGS               = YES"
    puts $doxyFile "WARN_IF_UNDOCUMENTED   = YES"
    puts $doxyFile "WARN_IF_DOC_ERROR      = YES"
    puts $doxyFile "WARN_NO_PARAMDOC       = NO"
    puts $doxyFile "WARN_FORMAT            = \"\$file:\$line: \$text\""
    puts $doxyFile "INPUT_ENCODING         = UTF-8"
    puts $doxyFile "FILE_PATTERNS          = *.md *.dox "
    puts $doxyFile "RECURSIVE              = YES"
    puts $doxyFile "SOURCE_BROWSER         = NO"
    puts $doxyFile "INLINE_SOURCES         = YES"
    puts $doxyFile "COLS_IN_ALPHA_INDEX    = 5"
    
    # Generation options
    puts $doxyFile "GENERATE_DOCSET        = NO"
    puts $doxyFile "GENERATE_HTMLHELP      = NO"
    puts $doxyFile "GENERATE_CHI           = NO"
    puts $doxyFile "GENERATE_QHP           = NO"
    puts $doxyFile "GENERATE_ECLIPSEHELP   = NO"
    puts $doxyFile "GENERATE_RTF           = NO"
    puts $doxyFile "GENERATE_MAN           = NO"
    puts $doxyFile "GENERATE_XML           = NO"
    puts $doxyFile "GENERATE_DOCBOOK       = NO"
    puts $doxyFile "GENERATE_AUTOGEN_DEF   = NO"
    puts $doxyFile "GENERATE_PERLMOD       = NO"

    set PARAM_INPUT "INPUT                  = "
    set PARAM_IMAGEPATH "IMAGE_PATH         = $inputDir/resources/ "

    foreach docFile $DocFilesList {
        set NEW_IMG_PATH [file normalize [file dirname "$inputDir/$docFile"]]
        if { [string compare $NEW_IMG_PATH $casroot] != 0 } {
          if {[file isdirectory "$NEW_IMG_PATH/images"]} {
            append PARAM_IMAGEPATH " " "$NEW_IMG_PATH/images"
          }
        }
        append PARAM_INPUT " " $inputDir/$docFile
    }
    puts $doxyFile $PARAM_INPUT
    puts $doxyFile $PARAM_IMAGEPATH
    
    if { $generatorMode == "PDF_ONLY"} {
        set PARAM_LATEX_EF "LATEX_EXTRA_FILES      ="
        foreach docFile $DocFilesList {
            set NEW_LEF_PATH [file normalize [file dirname "$inputDir/$docFile"]]
            if { [string compare $NEW_LEF_PATH $casroot] != 0 } {
                append PARAM_LATEX_EF " " "$NEW_LEF_PATH/images"
            }
        }
        puts $doxyFile $PARAM_LATEX_EF
    }

    if { $generatorMode == "HTML_ONLY"} {
        # Set a reference to a TAGFILE
        if { $tagFileDir != "" } {
            if {[file exists $tagFileDir/OCCT.tag] == 1} {
                set tagPath [OverviewDoc_GetRelPath $tagFileDir $outDir/html]
                puts $doxyFile "TAGFILES               = $tagFileDir/OCCT.tag=$tagPath/html"
            }
        }

        # HTML Output
        puts $doxyFile "GENERATE_LATEX         = NO"
        puts $doxyFile "GENERATE_HTML          = YES"
        puts $doxyFile "HTML_COLORSTYLE_HUE    = 220"
        puts $doxyFile "HTML_COLORSTYLE_SAT    = 100"
        puts $doxyFile "HTML_COLORSTYLE_GAMMA  = 80"
        puts $doxyFile "HTML_TIMESTAMP         = YES"
        puts $doxyFile "HTML_DYNAMIC_SECTIONS  = YES"
        puts $doxyFile "HTML_INDEX_NUM_ENTRIES = 100"
        puts $doxyFile "DISABLE_INDEX          = YES"
        puts $doxyFile "GENERATE_TREEVIEW      = YES"
        puts $doxyFile "ENUM_VALUES_PER_LINE   = 8"
        puts $doxyFile "TREEVIEW_WIDTH         = 250"
        puts $doxyFile "EXTERNAL_PAGES         = NO"
        puts $doxyFile "SEARCHENGINE           = YES"
        puts $doxyFile "SERVER_BASED_SEARCH    = NO"
        puts $doxyFile "EXTERNAL_SEARCH        = NO"
        puts $doxyFile "SEARCHDATA_FILE        = searchdata.xml"
        puts $doxyFile "SKIP_FUNCTION_MACROS   = YES"
        # Formula options
        puts $doxyFile "FORMULA_FONTSIZE       = 12"
        puts $doxyFile "FORMULA_TRANSPARENT    = YES"
        puts $doxyFile "USE_MATHJAX            = YES"
        puts $doxyFile "MATHJAX_FORMAT         = HTML-CSS"
        puts $doxyFile "MATHJAX_RELPATH        = http://cdn.mathjax.org/mathjax/latest"
        
    } elseif { $generatorMode == "PDF_ONLY"} {
        
        puts $doxyFile "GENERATE_HTML          = NO"
        puts $doxyFile "DISABLE_INDEX          = YES"
        puts $doxyFile "GENERATE_TREEVIEW      = NO"
        puts $doxyFile "PREDEFINED             = PDF_ONLY"
        puts $doxyFile "GENERATE_LATEX         = YES"
        puts $doxyFile "COMPACT_LATEX          = YES"
        puts $doxyFile "PDF_HYPERLINKS         = YES"
        puts $doxyFile "USE_PDFLATEX           = YES"
        puts $doxyFile "LATEX_BATCHMODE        = YES"
        puts $doxyFile "LATEX_OUTPUT           = latex"
        puts $doxyFile "LATEX_CMD_NAME         = latex"
        puts $doxyFile "MAKEINDEX_CMD_NAME     = makeindex"
    }

    close $doxyFile 
}

# Returns the relative path between two directories
proc OverviewDoc_GetRelPath {targetFile currentpath} {
  set cc [file split [file normalize $currentpath]]
  set tt [file split [file normalize $targetFile]]
  
  if {![string equal [lindex $cc 0] [lindex $tt 0]]} {
      # not on *n*x then
      return -code error "$targetFile not on same volume as $currentpath"
  }
  while {[string equal [lindex $cc 0] [lindex $tt 0]] && [llength $cc] > 0} {
      # discard matching components from the front
      set cc [lreplace $cc 0 0]
      set tt [lreplace $tt 0 0]
  }
  set prefix ""
  if {[llength $cc] == 0} {
      # just the file name, so targetFile is lower down (or in same place)
      set prefix "."
  }
  # step up the tree
  for {set i 0} {$i < [llength $cc]} {incr i} {
      append prefix " .."
  }
  # stick it all together (the eval is to flatten the targetFile list)
  return [eval file join $prefix $tt]
}

# Prints Help message
proc OverviewDoc_PrintHelpMessage {} {
    puts "\nUsage : occdoc \[-h\] \[-html\] \[-pdf\] \[-m=<list of files>\] \[-l=<document name>\] \[-v\]"
    puts ""
    puts " Options are : "
    puts "    -html                : To generate HTML files"
    puts "                           (cannot be used with -pdf)"
    puts "    -pdf                 : To generate PDF files"
    puts "                           (cannot be used with -html)"
    puts "    -m=<modules_list>    : Specifies list of documents to generate."
    puts "                           If it is not specified, all files, "
    puts "                           mentioned in FILES.txt are processed."
    puts "    -l=<document_name>   : Specifies the document caption "
    puts "                           for a single document"
    puts "    -h                   : Prints help message"
    puts "    -v                   : Specifies the Verbose mode"
    puts "                           (info on all script actions is shown)"
}

# Parses command line arguments
proc OverviewDoc_ParseArguments {arguments} {
    global args_names
    global args_values
    set args_names {}
    array set args_values {}

    foreach arg $arguments {
        if {[regexp {^(-)[a-z]+$} $arg] == 1} {
            set name [string range $arg 1 [string length $arg]-1]
            lappend args_names $name
            set args_values($name) "NULL"
            continue
        } elseif {[regexp {^(-)[a-z]+=.+$} $arg] == 1} {
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
            puts "Error in argument $arg"
            return 1
        }
    }
    return 0
}

# Loads a list of docfiles from file FILES.txt
proc OverviewDoc_LoadFilesList {} {

    set INPUTDIR [file normalize [file dirname [info script]]]
    
    global available_docfiles
    set available_docfiles {}

    # Read data from file
    if { [file exists "$INPUTDIR/FILES.txt"] == 1 } {
        set FILE [open "$INPUTDIR/FILES.txt" r]
        while {1} {
            set line [gets $FILE]
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
    return 0
}

# Writes new tex file for conversion from tex to pdf for a specific doc
proc OverviewDoc_MakeRefmanTex {fileName latexDir docLabel verboseMode} {

    if {$verboseMode == "YES"} {
        puts "INFO: Making refman.tex file for $fileName"
    }
    set DOCNAME "$latexDir/refman.tex"
    if {[file exists $DOCNAME] == 1} {
        file delete -force $DOCNAME
    }
    set texfile [open $DOCNAME w]

    puts $texfile "\\batchmode"
    puts $texfile "\\nonstopmode"
    puts $texfile "\\documentclass\[oneside\]{article}"
    puts $texfile "\n% Packages required by doxygen"
    puts $texfile "\\usepackage{calc}"
    puts $texfile "\\usepackage{doxygen}"
    puts $texfile "\\usepackage{graphicx}"
    puts $texfile "\\usepackage\[utf8\]{inputenc}"
    puts $texfile "\\usepackage{makeidx}"
    puts $texfile "\\usepackage{multicol}"
    puts $texfile "\\usepackage{multirow}"
    puts $texfile "\\usepackage{textcomp}"
    puts $texfile "\\usepackage{amsmath}"
    puts $texfile "\\usepackage\[table\]{xcolor}"
    puts $texfile "\\usepackage{indentfirst}"
    puts $texfile ""
    puts $texfile "% Font selection"
    puts $texfile "\\usepackage\[T1\]{fontenc}"
    puts $texfile "\\usepackage{mathptmx}"
    puts $texfile "\\usepackage\[scaled=.90\]{helvet}"
    puts $texfile "\\usepackage{courier}"
    puts $texfile "\\usepackage{amssymb}"
    puts $texfile "\\usepackage{sectsty}"
    puts $texfile "\\renewcommand{\\familydefault}{\\sfdefault}"
    puts $texfile "\\allsectionsfont{%"
    puts $texfile "  \\fontseries{bc}\\selectfont%"
    puts $texfile "  \\color{darkgray}%"
    puts $texfile "}"
    puts $texfile "\\renewcommand{\\DoxyLabelFont}{%"
    puts $texfile "  \\fontseries{bc}\\selectfont%"
    puts $texfile "  \\color{darkgray}%"
    puts $texfile "}"
    puts $texfile ""
    puts $texfile "% Page & text layout"
    puts $texfile "\\usepackage{geometry}"
    puts $texfile "\\geometry{%"
    puts $texfile "  a4paper,%"
    puts $texfile "  top=2.5cm,%"
    puts $texfile "  bottom=2.5cm,%"
    puts $texfile "  left=2.5cm,%"
    puts $texfile "  right=2.5cm%"
    puts $texfile "}"
    puts $texfile "\\tolerance=750"
    puts $texfile "\\hfuzz=15pt"
    puts $texfile "\\hbadness=750"
    puts $texfile "\\setlength{\\emergencystretch}{15pt}"
    puts $texfile "\\setlength{\\parindent}{0.75cm}"
    puts $texfile "\\setlength{\\parskip}{0.2cm}"
    puts $texfile "\\makeatletter"
    puts $texfile "\\renewcommand{\\paragraph}{%"
    puts $texfile "  \@startsection{paragraph}{4}{0ex}{-1.0ex}{1.0ex}{%"
    puts $texfile "\\normalfont\\normalsize\\bfseries\\SS@parafont%"
    puts $texfile "  }%"
    puts $texfile "}"
    puts $texfile "\\renewcommand{\\subparagraph}{%"
    puts $texfile "  \\@startsection{subparagraph}{5}{0ex}{-1.0ex}{1.0ex}{%"
    puts $texfile "\\normalfont\\normalsize\\bfseries\\SS@subparafont%"
    puts $texfile "  }%"
    puts $texfile "}"
    puts $texfile "\\makeatother"
    puts $texfile ""
    puts $texfile "% Headers & footers"
    puts $texfile "\\usepackage{fancyhdr}"
    puts $texfile "\\pagestyle{fancyplain}"
    puts $texfile "\\fancyhead\[LE\]{\\fancyplain{}{\\bfseries\\thepage}}"
    puts $texfile "\\fancyhead\[CE\]{\\fancyplain{}{}}"
    puts $texfile "\\fancyhead\[RE\]{\\fancyplain{}{\\bfseries\\leftmark}}"
    puts $texfile "\\fancyhead\[LO\]{\\fancyplain{}{\\bfseries\\rightmark}}"
    puts $texfile "\\fancyhead\[CO\]{\\fancyplain{}{}}"
    puts $texfile "\\fancyhead\[RO\]{\\fancyplain{}{\\bfseries\\thepage}}"
    puts $texfile "\\fancyfoot\[LE\]{\\fancyplain{}{}}"
    puts $texfile "\\fancyfoot\[CE\]{\\fancyplain{}{}}"
    puts $texfile "\\fancyfoot\[RE\]{\\fancyplain{}{\\bfseries\\scriptsize (c) Open CASCADE Technology 2001\-2013}}"
    puts $texfile "\\fancyfoot\[LO\]{\\fancyplain{}{\\bfseries\\scriptsize (c) Open CASCADE Technology 2001\-2013}}"
    puts $texfile "\\fancyfoot\[CO\]{\\fancyplain{}{}}"
    puts $texfile "\\fancyfoot\[RO\]{\\fancyplain{}{}}"
    puts $texfile "\\renewcommand{\\footrulewidth}{0.4pt}"
    puts $texfile "\\renewcommand{\\sectionmark}\[1\]{%"
    puts $texfile "  \\markright{\\thesection\\ #1}%"
    puts $texfile "}"
    puts $texfile ""
    puts $texfile "% Indices & bibliography"
    puts $texfile "\\usepackage{natbib}"
    puts $texfile "\\usepackage\[titles\]{tocloft}"
    puts $texfile "\\renewcommand{\\cftsecleader}{\\cftdotfill{\\cftdotsep}}"
    puts $texfile "\\setcounter{tocdepth}{3}"
    puts $texfile "\\setcounter{secnumdepth}{5}"
    puts $texfile "\\makeindex"
    puts $texfile ""
    puts $texfile "% Hyperlinks (required, but should be loaded last)"
    puts $texfile "\\usepackage{ifpdf}"
    puts $texfile "\\ifpdf"
    puts $texfile "  \\usepackage\[pdftex,pagebackref=true\]{hyperref}"
    puts $texfile "\\else"
    puts $texfile "  \\usepackage\[ps2pdf,pagebackref=true\]{hyperref}"
    puts $texfile "\\fi"
    puts $texfile "\\hypersetup{%"
    puts $texfile "  colorlinks=true,%"
    puts $texfile "  linkcolor=black,%"
    puts $texfile "  citecolor=black,%"
    puts $texfile "  urlcolor=blue,%"
    puts $texfile "  unicode%"
    puts $texfile "}"
    puts $texfile ""
    puts $texfile "% Custom commands"
    puts $texfile "\\newcommand{\\clearemptydoublepage}{%"
    puts $texfile "  \\newpage{\\pagestyle{empty}\\cleardoublepage}%"
    puts $texfile "}"
    puts $texfile "\n"
    puts $texfile "%===== C O N T E N T S =====\n"
#    puts $texfile "\\DeclareUnicodeCharacter{00A0}{ }"
    puts $texfile "\\begin{document}"
    puts $texfile ""
    puts $texfile "% Titlepage & ToC"
    puts $texfile "\\hypersetup{pageanchor=false}"
    puts $texfile "\\pagenumbering{roman}"
    puts $texfile "\\begin{titlepage}"
    puts $texfile "\\vspace*{7cm}"
    puts $texfile "\\begin{center}%"
    puts $texfile "\\includegraphics\[width=0.75\\textwidth, height=0.2\\textheight\]{overview_occttransparent.png}\\\\\\\\"
    puts $texfile "{\\Large Open C\\-A\\-S\\-C\\-A\\-D\\-E Technology \\\\\[1ex\]\\Large 6.\\-6.\\-0 }\\\\"
    puts $texfile "\\vspace*{1cm}"
    puts $texfile "{\\Large $docLabel}\\\\"
    puts $texfile "\\vspace*{1cm}"
    puts $texfile "{\\large Generated by Doxygen 1.8.4}\\\\"
    puts $texfile "\\vspace*{0.5cm}"
    puts $texfile "{\\small \\today}\\"
    puts $texfile "\\end{center}"
    puts $texfile "\\end{titlepage}"
    puts $texfile "\\clearpage"
    puts $texfile "\\pagenumbering{roman}"
    puts $texfile "\\tableofcontents"
    puts $texfile "\\newpage"
    puts $texfile "\\pagenumbering{arabic}"
    puts $texfile "\\hypersetup{pageanchor=true}"
    puts $texfile ""
    puts $texfile "\\hypertarget{$fileName}{}"
    puts $texfile "\\input{$fileName}"
    puts $texfile ""
    puts $texfile "% Index"
    puts $texfile "\\newpage"
    puts $texfile "\\phantomsection"
    puts $texfile "\\addcontentsline{toc}{part}{Index}"
    puts $texfile "\\printindex\n"
    puts $texfile "\\end{document}"

    close $texfile
}

# Postprocesses generated TeX files
proc OverviewDoc_ProcessTex {{texFiles {}} {latexDir} verboseMode} {

    foreach TEX $texFiles {
        if {$verboseMode == "YES"} {
            puts "INFO: Preprocessing file $TEX"
        }

         if {![file exists $TEX]} {
          puts "file $TEX doesn't exist"
          return
        }

        set IN_F  [open "$TEX" r]
        set TMPFILENAME "$latexDir/temp.tex"

        set OUT_F [open $TMPFILENAME w]
        
        while {1} {
            set line [gets $IN_F]
            if { [string first "\\includegraphics" $line] != -1 } {
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

# Main procedure for documents compilation
proc OverviewDoc_Main { {docfiles {}} generatorMode docLabel verboseMode} {

    set INDIR      [file normalize [file dirname [info script]]]
    set CASROOT    [file normalize [file dirname "$INDIR/../../"]]
    set OUTDIR     $CASROOT/doc
    set PDFDIR     $OUTDIR/overview/pdf
    set HTMLDIR    $OUTDIR/overview/html
    set LATEXDIR   $OUTDIR/overview/latex
    set TAGFILEDIR $OUTDIR/refman
    set DOXYFILE   $OUTDIR/OCCT.cfg

    # Create or clean the output folders
    if {[file exists $OUTDIR] == 0} {
        file mkdir $OUTDIR
    } 
    if {[file exists $HTMLDIR] == 0} {
        file mkdir $HTMLDIR
    }
    if {[file exists $LATEXDIR] == 0} {
        file mkdir $LATEXDIR
    }
    if {[file exists $PDFDIR] == 0} {
        file mkdir $PDFDIR
    }

    # Run tools to compile documents
    puts ""
    puts " [clock format [clock seconds] -format {%Y.%m.%d %H:%M}] Generation process started..."
    puts ""
    puts " Please, wait while Doxygen finishes it\'s work"
    OverviewDoc_MakeDoxyfile $CASROOT "$OUTDIR/overview" $TAGFILEDIR $DOXYFILE $generatorMode $docfiles $verboseMode

    # Run doxygen tool
    if { $generatorMode == "HTML_ONLY"} {
        puts " [clock format [clock seconds] -format {%Y.%m.%d %H:%M}] Doxygen is now generating HTML files...\n"
    }
    set RESULT [catch {exec doxygen $DOXYFILE > $OUTDIR/doxygen_out.log} DOX_ERROR] 
    if {$RESULT != 0} {
        if {[llength [split $DOX_ERROR "\n"]] > 1} {
            if {$verboseMode == "YES"} {
                puts "INFO: See Doxygen messages in $OUTDIR/doxygen_warnings_and_errors.log"
            }
            set DOX_ERROR_FILE [open "$OUTDIR/doxygen_warnings_and_errors.log" "w"]
            puts $DOX_ERROR_FILE $DOX_ERROR
            close $DOX_ERROR_FILE
        } else {
            puts $DOX_ERROR
        }
    }
    # Close the Doxygen application
    after 300

    # Start PDF generation routine
    if { $generatorMode == "PDF_ONLY" } {
        puts ""
        puts " [clock format [clock seconds] -format {%Y.%m.%d %H:%M}] Doxygen is now generating PDF files...\n"

        set OS $::tcl_platform(platform)
        if { $OS == "unix" } {
            set PREFIX ".sh"
        } elseif { $OS == "windows" } {
            set PREFIX ".bat"
        }

        # Prepare a list of TeX files, generated by Doxygen
        puts "go to $LATEXDIR"
        cd $LATEXDIR
        
        set TEXFILES [glob $LATEXDIR -type f -directory $LATEXDIR -tails "*.tex" ]
        set REFMAN_IDX [lsearch $TEXFILES "refman.tex"]
        set TEXFILES [lreplace $TEXFILES $REFMAN_IDX $REFMAN_IDX]


        set IDX [lsearch $TEXFILES "$LATEXDIR"]
        while { $IDX != -1} {
            set TEXFILES [lreplace $TEXFILES $IDX $IDX]
            set IDX [lsearch $TEXFILES "$LATEXDIR"]
        }

        puts "Preprocess generated TeX files"
        OverviewDoc_ProcessTex $TEXFILES $LATEXDIR $verboseMode

        puts "Generate PDF files from"
        foreach TEX $TEXFILES {
            # Rewrite existing REFMAN.tex file...
            set TEX [string range $TEX 0 [ expr "[string length $TEX] - 5"]]
            OverviewDoc_MakeRefmanTex $TEX $LATEXDIR $docLabel $verboseMode
            
            if {$verboseMode == "YES"} {
                puts "INFO: Generating PDF file from $TEX"
            }
            # ...and use it to generate PDF from TeX...
            puts "execute $LATEXDIR/make$PREFIX"
            set RESULT [catch {eval exec [auto_execok $LATEXDIR/make$PREFIX] > "$OUTDIR/pdflatex_out.log"} LaTeX_ERROR]
            if {$RESULT != 0} {
                if {[llength [split $LaTeX_ERROR "\n"]] > 1} {
                    set LaTeX_ERROR_FILE [open "$OUTDIR/pdflatex_warnings_and_errors.log" "w"]
                    puts $LaTeX_ERROR_FILE $LaTeX_ERROR
                    close $LaTeX_ERROR_FILE
                } else {
                    puts $DOX_ERROR
                }
            }

            # ...and place it to the specific folder
            if { [file exists $PDFDIR/$TEX.pdf] == 1 } {
                file delete -force $PDFDIR/$TEX.pdf
            }
            
            if {![file exists "$LATEXDIR/refman.pdf"]} {
              puts "file $LATEXDIR/refman.pdf doesn't exist"
              return
            }
            
            file rename $LATEXDIR/refman.pdf "$PDFDIR/$TEX.pdf"
        }
        if {$verboseMode == "YES"} {
            puts "INFO: See LaTeX messages in $OUTDIR/pdflatex_warnings_and_errors.log"
        }
    }

    cd $INDIR
    puts " [clock format [clock seconds] -format {%Y.%m.%d %H:%M}] Generation process finished..."
    puts ""
    puts "--------------------------------------------------------------------"
    if { $generatorMode == "HTML_ONLY" } {
        puts " You can look at generated HTML pages by opening: "
        set RESFILE $OUTDIR/overview/html/index.html
        puts " $RESFILE"
    }
    if { $generatorMode == "PDF_ONLY" } {
        puts " You can look at generated PDF files in: "
        puts " $OUTDIR/overview/pdf folder"
    }
    puts ""
    puts " Copyright \u00a9 Open CASCADE Technology 2001-2013"
    puts "--------------------------------------------------------------------\n"
}

# A command for User Documentation compilation
proc occdoc {args} {
    # Programm options
    set GEN_HTML  "NO"
    set GEN_PDF   "NO"
    set DOCFILES  {}
    set DOCLABEL  "Default OCCT Document"
    set VERB_MODE "NO"
    set GEN_MODE  "DEFAULT"
    global available_docfiles
    global args_names
    global args_values

    # Load list of docfiles
    if { [OverviewDoc_LoadFilesList] != 0 } {
        puts "ERROR: File FILES.txt was not found"
        return
    }

    # Parse CL arguments
    if {[OverviewDoc_ParseArguments $args] == 1} {
        return
    }
    if {$args_names == {}} {
        set GEN_HTML "YES"
        set VERB_MODE "YES"
    } else {
        foreach arg_n $args_names {
            if {$arg_n == "h"} {
              OverviewDoc_PrintHelpMessage
              return
            } elseif {$arg_n == "html"} {
                set GEN_HTML "YES"
            } elseif {$arg_n == "pdf"} {
                set GEN_PDF "YES"
            } elseif {$arg_n == "v"} {
                set VERB_MODE "YES"
            } elseif {$arg_n == "m"} {
                if {$args_values(m) != "NULL"} {
                    set DOCFILES $args_values(m)
                } else {
                    puts "Error in argument m"
                    return
                }
                # Check if all chosen docfiles are correct
                foreach docfile $DOCFILES {
                    if { [lsearch $available_docfiles $docfile] == -1 } {
                        puts "File \"$docfile\" is not presented in the list of available docfiles"
                        puts "Please, specify the correct docfile name"
                        return
                    } else {
                        puts "File $docfile is presented in FILES.TXT"
                    }
                }
            } elseif {$arg_n == "l"} {
                if { [llength $DOCFILES] <= 1 } {
                    if {$args_values(l) != "NULL"} {
                        set DOCLABEL $args_values(l)
                    } else {
                        puts "Error in argument l"
                        return
                    }
                }
            } else {
                puts "\nWrong argument: $arg_n"
                OverviewDoc_PrintHelpMessage
                return
            }
        }
    }

    # Specify generation mode
    if {$GEN_HTML == "YES" && $GEN_PDF == "NO"} {
        set GEN_MODE "HTML_ONLY"
    } elseif {$GEN_PDF == "YES"} {
        set GEN_MODE "PDF_ONLY"
    }
    # Check if -v is the only option
    if {$GEN_MODE == "DEFAULT"} {
        if { $VERB_MODE == "YES" } {
            puts "\nArgument -v can't be used without -pdf or -html argument"
            OverviewDoc_PrintHelpMessage
        }
        return
    }
    
    # Specify verbose mode
    if { $GEN_PDF != "YES" && [llength $DOCFILES] > 1 } {
        set DOCLABEL ""
    }
    
    # If we don't specify list for docfiles with -m argument,
    # we assume that we have to generate all docfiles
    if { [llength $DOCFILES] == 0 } {
        set DOCFILES $available_docfiles
    }

    # Start main activities
    OverviewDoc_Main $DOCFILES $GEN_MODE $DOCLABEL $VERB_MODE
}
