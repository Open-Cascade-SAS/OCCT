# Copyright (c) 2013-2014 OPEN CASCADE SAS
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

############################################################################
# This file defines scripts for execution of OCCT tests.
# It should be loaded automatically when DRAW is started, and provides
# top-level commands starting with 'test'. Type 'help test' to get their
# synopsys.
# See OCCT Tests User Guide for description of the test system.
#
# Note: procedures with names starting with underscore are for internal use 
# inside the test system.
############################################################################

# Default verbose level for command _run_test
set _tests_verbose 0

# regexp for parsing test case results in summary log
set _test_case_regexp {^CASE\s+([\w.-]+)\s+([\w.-]+)\s+([\w.-]+)\s*:\s*([\w]+)(.*)}

# Basic command to run indicated test case in DRAW
help test {
  Run specified test case
  Use: test group grid casename [options...]
  Allowed options are:
  -echo: all commands and results are echoed immediately,
         but log is not saved and summary is not produced
         It is also possible to use "1" instead of "-echo"
         If echo is OFF, log is stored in memory and only summary
         is output (the log can be obtained with command "dlog get")
  -outfile filename: set log file (should be non-existing),
         it is possible to save log file in text file or
         in html file(with snapshot), for that "filename"
         should have ".html" extension
  -overwrite: force writing log in existing file
  -beep: play sound signal at the end of the test
  -errors: show all lines from the log report that are recognized as errors
         This key will be ignored if the "-echo" key is already set.
}
proc test {group grid casename {args {}}} {
    # set default values of arguments
    set echo 0
    set errors 0
    set logfile ""
    set overwrite 0
    set signal 0

    # get test case paths (will raise error if input is invalid)
    _get_test $group $grid $casename dir gridname casefile

    # check arguments
    for {set narg 0} {$narg < [llength $args]} {incr narg} {
        set arg [lindex $args $narg]
        # if echo specified as "-echo", convert it to bool
        if { $arg == "-echo" || $arg == "1" } {
            set echo t
            continue
        }

        # output log file
        if { $arg == "-outfile" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } {
                set logfile [lindex $args $narg]
            } else {
                error "Option -outfile requires argument"
            }
            continue
        }

        # allow overwrite existing log
        if { $arg == "-overwrite" } {
            set overwrite 1
            continue
        }

        # sound signal at the end of the test
        if { $arg == "-beep" } {
            set signal t
            continue
        }

        # if errors specified as "-errors", convert it to bool
        if { $arg == "-errors" } {
            set errors t
            continue
        }

        # unsupported option
        error "Error: unsupported option \"$arg\""
    }
    # run test
    uplevel _run_test $dir $group $gridname $casefile $echo 

    # check log
    if { !$echo } {
        _check_log $dir $group $gridname $casename $errors [dlog get] summary html_log

        # create log file
        if { ! $overwrite && [file isfile $logfile] } {
            error "Error: Specified log file \"$logfile\" exists; please remove it before running test or use -overwrite option"
        }
        if {$logfile != ""} {
            if {[file extension $logfile] == ".html"} {
                if {[regexp {vdump ([^\s\n]+)} $html_log dump snapshot]} {
                    catch {file copy -force $snapshot [file rootname $logfile][file extension $snapshot]}
                }
                _log_html $logfile $html_log "Test $group $grid $casename"
            } else {
                _log_save $logfile "[dlog get]\n$summary" "Test $group $grid $casename"
            }
        }
    }

    # play sound signal at the end of test
    if {$signal} {
        puts "\7\7\7\7"
    }
    return
}

# Basic command to run indicated test case in DRAW
help testgrid {
  Run all tests, or specified group, or one grid
  Use: testgrid [groupmask [gridmask [casemask]]] [options...]
  Allowed options are:
  -parallel N: run N parallel processes (default is number of CPUs, 0 to disable)
  -refresh N: save summary logs every N seconds (default 600, minimal 1, 0 to disable)
  -outdir dirname: set log directory (should be empty or non-existing)
  -overwrite: force writing logs in existing non-empty directory
  -xml filename: write XML report for Jenkins (in JUnit-like format)
  -beep: play sound signal at the end of the tests
  -regress dirname: re-run only a set of tests that have been detected as regressions on some previous run.
                    Here "dirname" is path to directory containing results of previous run.
  Groups, grids, and test cases to be executed can be specified by list of file 
  masks, separated by spaces or comma; default is all (*).
}
proc testgrid {args} {
    global env tcl_platform _tests_verbose

    ######################################################
    # check arguments
    ######################################################

    # check that environment variable defining paths to test scripts is defined
    if { ! [info exists env(CSF_TestScriptsPath)] || 
        [llength $env(CSF_TestScriptsPath)] <= 0 } {
        error "Error: Environment variable CSF_TestScriptsPath is not defined"
    }

    # treat options
    set parallel [_get_nb_cpus]
    set refresh 60
    set logdir ""
    set overwrite 0
    set xmlfile ""
    set signal 0
    set regress 0
    set prev_logdir ""
    for {set narg 0} {$narg < [llength $args]} {incr narg} {
        set arg [lindex $args $narg]

        # parallel execution
        if { $arg == "-parallel" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set parallel [expr [lindex $args $narg]]
            } else {
                error "Option -parallel requires argument"
            }
            continue
        }

        # refresh logs time
        if { $arg == "-refresh" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set refresh [expr [lindex $args $narg]]
            } else {
                error "Option -refresh requires argument"
            }
            continue
        }

        # output directory
        if { $arg == "-outdir" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set logdir [lindex $args $narg]
            } else {
                error "Option -outdir requires argument"
            }
            continue
        }

        # allow overwrite logs 
        if { $arg == "-overwrite" } {
            set overwrite 1
            continue
        }

        # refresh logs time
        if { $arg == "-xml" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set xmlfile [lindex $args $narg]
            }
            if { $xmlfile == "" } {
                set xmlfile TESTS-summary.xml
            }
            continue
        }

        # sound signal at the end of the test
        if { $arg == "-beep" } {
            set signal t
            continue
        }

        # re-run only a set of tests that have been detected as regressions on some previous run
        if { $arg == "-regress" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } {
                set prev_logdir [lindex $args $narg]
                set regress 1
            } else {
                error "Option -regress requires argument"
            }
            continue
        }

        # unsupported option
        if { [regexp {^-} $arg] } {
            error "Error: unsupported option \"$arg\""
        }

        # treat arguments not recognized as options as group and grid names
        if { ! [info exists groupmask] } {
            set groupmask [split $arg ,]
        } elseif { ! [info exists gridmask] } {
            set gridmask [split $arg ,]
        } elseif { ! [info exists casemask] } {
            set casemask [split $arg ,]
        } else {
            error "Error: cannot interpret argument $narg ($arg)"
        }
    }

    # check that target log directory is empty or does not exist
    set logdir [file normalize [string trim $logdir]]
    set prev_logdir [file normalize [string trim $prev_logdir]]
    if { $logdir == "" } {
        # if specified logdir is empty string, generate unique name like 
        # results/<branch>_<timestamp>
        set prefix ""
        if { ! [catch {exec git branch} gitout] &&
             [regexp {[*] ([\w-]+)} $gitout res branch] } {
            set prefix "${branch}_"
        }
        set logdir "results/${prefix}[clock format [clock seconds] -format {%Y-%m-%dT%H%M}]"

        set logdir [file normalize $logdir]
    }
    if { [file isdirectory $logdir] && ! $overwrite && ! [catch {glob -directory $logdir *}] } {
        error "Error: Specified log directory \"$logdir\" is not empty; please clean it before running tests"
    } 
    if { [catch {file mkdir $logdir}] || ! [file writable $logdir] } {
        error "Error: Cannot create directory \"$logdir\", or it is not writable"
    }

    # masks for search of test groups, grids, and cases
    if { ! [info exists groupmask] } { set groupmask * }
    if { ! [info exists gridmask ] } { set gridmask  * }
    if { ! [info exists casemask ] } { set casemask  * }

    # Find test cases with FAILED and IMPROVEMENT statuses in previous run
    # if option "regress" is given
    set rerun_group_grid_case {}

    if { ${regress} > 0 } {
        if { "${groupmask}" != "*"} {
            lappend rerun_group_grid_case [list $groupmask $gridmask $casemask]
        }
    } else {
        lappend rerun_group_grid_case [list $groupmask $gridmask $casemask]
    }

    if { ${regress} > 0 } {
        if { [file exists ${prev_logdir}/tests.log] } {
            set fd [open ${prev_logdir}/tests.log]
            while { [gets $fd line] >= 0 } {
                if {[regexp {CASE ([^\s]+) ([^\s]+) ([^\s]+): FAILED} $line dump group grid casename] ||
                    [regexp {CASE ([^\s]+) ([^\s]+) ([^\s]+): IMPROVEMENT} $line dump group grid casename]} {
                    lappend rerun_group_grid_case [list $group $grid $casename]
                }
            }
            close $fd
        } else {
            error "Error: file ${prev_logdir}/tests.log is not found, check your input arguments!"
        }
    }

    ######################################################
    # prepare list of tests to be performed
    ######################################################

    # list of tests, each defined by a list of:
    # test scripts directory
    # group (subfolder) name
    # grid (subfolder) name
    # test case name
    # path to test case file
    set tests_list {}

    foreach group_grid_case ${rerun_group_grid_case} {
        set groupmask [lindex $group_grid_case 0]
        set gridmask  [lindex $group_grid_case 1]
        set casemask  [lindex $group_grid_case 2]

        # iterate by all script paths
        foreach dir [lsort -unique [_split_path $env(CSF_TestScriptsPath)]] {
            # protection against empty paths
            set dir [string trim $dir]
            if { $dir == "" } { continue }

            if { $_tests_verbose > 0 } { _log_and_puts log "Examining tests directory $dir" }

            # check that directory exists
            if { ! [file isdirectory $dir] } {
                _log_and_puts log "Warning: directory $dir listed in CSF_TestScriptsPath does not exist, skipped"
                continue
            }

            # search all directories in the current dir with specified mask
            if [catch {glob -directory $dir -tail -types d {*}$groupmask} groups] { continue }

            # iterate by groups
            if { $_tests_verbose > 0 } { _log_and_puts log "Groups to be executed: $groups" }
            foreach group [lsort -dictionary $groups] {
                if { $_tests_verbose > 0 } { _log_and_puts log "Examining group directory $group" }

                # file grids.list must exist: it defines sequence of grids in the group
                if { ! [file exists $dir/$group/grids.list] } {
                    _log_and_puts log "Warning: directory $dir/$group does not contain file grids.list, skipped"
                    continue
                }

                # read grids.list file and make a list of grids to be executed
                set gridlist {}
                set fd [open $dir/$group/grids.list]
                set nline 0
                while { [gets $fd line] >= 0 } {
                    incr nline

                    # skip comments and empty lines
                    if { [regexp "\[ \t\]*\#.*" $line] } { continue }
                    if { [string trim $line] == "" } { continue }

                    # get grid id and name
                    if { ! [regexp "^\(\[0-9\]+\)\[ \t\]*\(\[A-Za-z0-9_.-\]+\)\$" $line res gridid grid] } {
                        _log_and_puts log "Warning: cannot recognize line $nline in file $dir/$group/grids.list as \"gridid gridname\"; ignored"
                        continue
                    }

                    # check that grid fits into the specified mask
                    foreach mask $gridmask {
                        if { $mask == $gridid || [string match $mask $grid] } {
                            lappend gridlist $grid
                        }
                    }
                }
                close $fd

                # iterate by all grids
                foreach grid $gridlist {

                    # check if this grid is aliased to another one
                    set griddir $dir/$group/$grid
                    if { [file exists $griddir/cases.list] } {
                        set fd [open $griddir/cases.list]
                        if { [gets $fd line] >= 0 } {
                            set griddir [file normalize $dir/$group/$grid/[string trim $line]]
                        }
                        close $fd
                    }

                    # check if grid directory actually exists
                    if { ! [file isdirectory $griddir] } {
                        _log_and_puts log "Error: tests directory for grid $grid ($griddir) is missing; skipped"
                        continue
                    }

                    # create directory for logging test results
                    if { $logdir != "" } { file mkdir $logdir/$group/$grid }

                    # iterate by all tests in the grid directory
                    if { [catch {glob -directory $griddir -type f {*}$casemask} testfiles] } { continue }
                    foreach casefile [lsort -dictionary $testfiles] {
                        # filter out files with reserved names
                        set casename [file tail $casefile]
                        if { $casename == "begin" || $casename == "end" ||
                             $casename == "parse.rules" } {
                            continue
                        }

                        lappend tests_list [list $dir $group $grid $casename $casefile]
                    }
                }
            }
        }
    }
    if { [llength $tests_list] < 1 } {
        error "Error: no tests are found, check your input arguments and variable CSF_TestScriptsPath!"
    } else {
        puts "Running tests (total [llength $tests_list] test cases)..."
    }

    ######################################################
    # run tests
    ######################################################
    
    # log command arguments and environment
    lappend log "Command: testgrid $args"
    lappend log "Host: [info hostname]"
    lappend log "Started on: [clock format [clock seconds] -format {%Y-%m-%d %H:%M:%S}]"
    catch {lappend log "DRAW build:\n[dversion]" }
    lappend log "Environment:"
    foreach envar [lsort [array names env]] {
        lappend log "$envar=\"$env($envar)\""
    }
    lappend log ""

    set refresh_timer [clock seconds]
    uplevel dchrono _timer reset
    uplevel dchrono _timer start

    # if parallel execution is requested, allocate thread pool
    if { $parallel > 0 } {
        if { ! [info exists tcl_platform(threaded)] || [catch {package require Thread}] } {
            _log_and_puts log "Warning: Tcl package Thread is not available, running in sequential mode"
            set parallel 0
        } else {
            set worker [tpool::create -minworkers $parallel -maxworkers $parallel]
            # suspend the pool until all jobs are posted, to prevent blocking of the process
            # of starting / processing jobs by running threads
            catch {tpool::suspend $worker}
            if { $_tests_verbose > 0 } { _log_and_puts log "Executing tests in (up to) $parallel threads" }
            # limit number of jobs in the queue by reasonable value
            # to prevent slowdown due to unnecessary queue processing
            set nbpooled 0
            set nbpooled_max [expr 10 * $parallel]
            set nbpooled_ok  [expr  5 * $parallel]
        }
    }

    # start test cases
    set userbreak 0
    foreach test_def $tests_list {
        # check for user break
        if { $userbreak || "[info commands dbreak]" == "dbreak" && [catch dbreak] } {
            set userbreak 1
            break
        }

        set dir       [lindex $test_def 0]
        set group     [lindex $test_def 1]
        set grid      [lindex $test_def 2]
        set casename  [lindex $test_def 3]
        set casefile  [lindex $test_def 4]

        # command to set tests for generation of image in results directory
        set imgdir_cmd ""
        if { $logdir != "" } { set imgdir_cmd "set imagedir $logdir/$group/$grid" }

        # prepare command file for running test case in separate instance of DRAW
        set fd_cmd [open $logdir/$group/$grid/${casename}.tcl w]
        puts $fd_cmd "$imgdir_cmd"
        puts $fd_cmd "set test_image $casename"
        puts $fd_cmd "_run_test $dir $group $grid $casefile t"

        # use dlog command to obtain complete output of the test when it is absent (i.e. since OCCT 6.6.0)
        # note: this is not needed if echo is set to 1 in call to _run_test above
        if { ! [catch {dlog get}] } {
            puts $fd_cmd "puts \[dlog get\]"
        } else {
            # else try to use old-style QA_ variables to get more output...
            set env(QA_DUMP) 1
            set env(QA_DUP) 1
            set env(QA_print_command) 1
        }

        # final 'exit' is needed when running on Linux under VirtualGl
        puts $fd_cmd "exit"
        close $fd_cmd

        # commant to run DRAW with a command file;
        # note that empty string is passed as standard input to avoid possible 
        # hang-ups due to waiting for stdin of the launching process
        set command "exec <<{} DRAWEXE -f $logdir/$group/$grid/${casename}.tcl"

        # alternative method to run without temporary file; disabled as it needs too many backslashes
        # else {
        # set command "exec <<\"\" DRAWEXE -c $imgdir_cmd\\\; set test_image $casename\\\; \
        # _run_test $dir $group $grid $casefile\\\; \
        # puts \\\[dlog get\\\]\\\; exit"
        # }

        # run test case, either in parallel or sequentially
        if { $parallel > 0 } {
            # parallel execution
            set job [tpool::post -nowait $worker "catch \"$command\" output; return \$output"]
            set job_def($job) [list $logdir $dir $group $grid $casename]
            incr nbpooled
            if { $nbpooled > $nbpooled_max } {
                _testgrid_process_jobs $worker $nbpooled_ok
            }
        } else {
            # sequential execution
            catch {eval $command} output
            _log_test_case $output $logdir $dir $group $grid $casename log

            # update summary log with requested period
            if { $logdir != "" && $refresh > 0 && [expr [clock seconds] - $refresh_timer > $refresh] } {
                # update and dump summary
                _log_summarize $logdir $log
                set refresh_timer [clock seconds]
            }
        }
    }

    # get results of started threads
    if { $parallel > 0 } {
        _testgrid_process_jobs $worker
        # release thread pool
        if { $nbpooled > 0 } {
            tpool::cancel $worker [array names job_def]
        }
        catch {tpool::resume $worker}
        tpool::release $worker
    }

    uplevel dchrono _timer stop
    set time [lindex [split [uplevel dchrono _timer show] "\n"] 0]

    if { $userbreak } {
        _log_and_puts log "*********** Stopped by user break ***********"
        set time "${time} \nNote: the process is not finished, stopped by user break!"
    }

    ######################################################
    # output summary logs and exit
    ######################################################

    _log_summarize $logdir $log $time
    if { $logdir != "" } {
        puts "Detailed logs are saved in $logdir"
    }
    if { $logdir != "" && $xmlfile != "" } {
        # XML output file is assumed relative to log dir unless it is absolute
        if { [ file pathtype $xmlfile] == "relative" } {
            set xmlfile [file normalize $logdir/$xmlfile]
        }
        _log_xml_summary $logdir $xmlfile $log 0
        puts "XML summary is saved to $xmlfile"
    }
    # play sound signal at the end of test
    if {$signal} {
        puts "\7\7\7\7"
    }
    return
}

# Procedure to regenerate summary log from logs of test cases
help testsummarize {
  Regenerate summary log in the test directory from logs of test cases.
  This can be necessary if test grids are executed separately (e.g. on
  different stations) or some grids have been re-executed.
  Use: testsummarize dir
}
proc testsummarize {dir} {
    global _test_case_regexp

    if { ! [file isdirectory $dir] } {
        error "Error: \"$dir\" is not a directory"
    }

    # get summary statements from all test cases in one log
    set log {}

    # to avoid huge listing of logs, first find all subdirectories and iterate
    # by them, parsing log files in each subdirectory independently 
    foreach grid [glob -directory $dir -types d -tails */*] {
        foreach caselog [glob -nocomplain -directory [file join $dir $grid] -types f -tails *.log] {
            set file [file join $dir $grid $caselog]
            set nbfound 0
            set fd [open $file r]
            while { [gets $fd line] >= 0 } {
                if { [regexp $_test_case_regexp $line res grp grd cas status message] } {
                    if { "[file join $grid $caselog]" != "[file join $grp $grd ${cas}.log]" } { 
                        puts "Error: $file contains status line for another test case ($line)"
                    }
                    lappend log $line
                    incr nbfound
                }
            }
            close $fd

            if { $nbfound != 1 } { 
                puts "Error: $file contains $nbfound status lines, expected 1"
            }
        }
    }

    _log_summarize $dir $log "Summary regenerated from logs at [clock format [clock seconds]]"
    return
}

# Procedure to compare results of two runs of test cases
help testdiff {
  Compare results of two executions of tests (CPU times, ...)
  Use: testdiff dir1 dir2 [groupname [gridname]] [options...]
  Where dir1 and dir2 are directories containing logs of two test runs.
  Allowed options are:
  -image [filename]: compare only images and save its in specified file (default 
                   name is <dir1>/diffimage-<dir2>.log)
  -cpu [filename]: compare only CPU and save it in specified file (default 
                   name is <dir1>/diffcpu-<dir2>.log)
  -memory [filename]: compare only memory and save it in specified file (default 
                   name is <dir1>/diffmemory-<dir2>.log)
  -save filename: save resulting log in specified file (default name is
                  <dir1>/diff-<dir2>.log); HTML log is saved with same name
                  and extension .html
  -status {same|ok|all}: filter cases for comparing by their status:
          same - only cases with same status are compared (default)
          ok   - only cases with OK status in both logs are compared
          all  - results are compared regardless of status
  -verbose level: 
          1 - output only differences 
          2 - output also list of logs and directories present in one of dirs only
          3 - (default) output also progress messages 
  -highlight_percent value: highlight considerable (>value in %) deviations
                            of CPU and memory (default value is 5%)
}
proc testdiff {dir1 dir2 args} {
    if { "$dir1" == "$dir2" } {
        error "Input directories are the same"
    }

    ######################################################
    # check arguments
    ######################################################

    # treat options
    set logfile [file join $dir1 "diff-[file tail $dir2].log"]
    set logfile_image ""
    set logfile_cpu ""
    set logfile_memory ""
    set image false
    set cpu false
    set memory false
    set basename ""
    set save false
    set status "same"
    set verbose 3
    set highlight_percent 5
    for {set narg 0} {$narg < [llength $args]} {incr narg} {
        set arg [lindex $args $narg]
        # log file name
        if { $arg == "-save" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set logfile [lindex $args $narg]
            } else {
                error "Error: Option -save must be followed by log file name"
            } 
            set save true
            continue
        }
        
        # image compared log
        if { $arg == "-image" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set logfile_image [lindex $args $narg]
            } else {
                set logfile_image [file join $dir1 "diffimage-[file tail $dir2].log"]
                incr narg -1
            }
            set image true
            continue
        }
        
        # CPU compared log
        if { $arg == "-cpu" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set logfile_cpu [lindex $args $narg]
            } else {
                set logfile_cpu [file join $dir1 "diffcpu-[file tail $dir2].log"]
                incr narg -1
            }
            set cpu true
            continue
        }
        
        # memory compared log
        if { $arg == "-memory" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set logfile_memory [lindex $args $narg]
            } else {
                set logfile_memory [file join $dir1 "diffmemory-[file tail $dir2].log"]
                incr narg -1
            }
            set memory true
            continue
        }
        
        # status filter
        if { $arg == "-status" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set status [lindex $args $narg]
            } else {
                set status ""
            }
            if { "$status" != "same" && "$status" != "all" && "$status" != "ok" } {
                error "Error: Option -status must be followed by one of \"same\", \"all\", or \"ok\""
            }
            continue
        }

        # verbose level
        if { $arg == "-verbose" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set verbose [expr [lindex $args $narg]]
            } else {
                error "Error: Option -verbose must be followed by integer verbose level"
            }
            continue
        }

        # highlight_percent
        if { $arg == "-highlight_percent" } {
            incr narg
            if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } { 
                set highlight_percent [expr [lindex $args $narg]]
            } else {
                error "Error: Option -highlight_percent must be followed by integer value"
            }
            continue
        }

        if { [regexp {^-} $arg] } {
            error "Error: unsupported option \"$arg\""
        }

        # non-option arguments form a subdirectory path
        set basename [file join $basename $arg]
    }
    
    if {$image != false || $cpu != false || $memory != false} {
        if {$save != false} {
            error "Error: Option -save can not be used with image/cpu/memory options"
        }
    }

    # run diff procedure (recursive)
    _test_diff $dir1 $dir2 $basename $image $cpu $memory $status $verbose log log_image log_cpu log_memory
    
    # save result to log file
    if {$image == false && $cpu == false && $memory == false} {
        if { "$logfile" != "" } {
            _log_save $logfile [join $log "\n"]
            _log_html_diff "[file rootname $logfile].html" $log $dir1 $dir2 ${highlight_percent}
            puts "Log is saved to $logfile (and .html)"
        }
    } else {
        foreach mode {image cpu memory} {
            if {"[set logfile_${mode}]" != ""} {
                _log_save "[set logfile_${mode}]" [join "[set log_${mode}]" "\n"]
                _log_html_diff "[file rootname [set logfile_${mode}]].html" "[set log_${mode}]" $dir1 $dir2 ${highlight_percent}
                puts "Log (${mode}) is saved to [set logfile_${mode}] (and .html)"
            }
        }
    }
    return
}

# Procedure to check data file before adding it to repository
help testfile {
  Check data file and prepare it for putting to test data files repository.
  Use: testfile [filelist]

  Will report if:
  - data file (non-binary) is in DOS encoding (CR/LF)
  - same data file (with same or another name) already exists in the repository
  - another file with the same name already exists 
  Note that names are assumed to be case-insensitive (for Windows).

  Unless the file is already in the repository, tries to load it, reports
  the recognized file format, file size, number of faces and edges in the 
  loaded shape (if any), and makes snapshot (in the temporary directory).
  Finally it advises whether the file should be put to public section of the 
  repository.
}
proc testfile {filelist} {
    global env

    # check that CSF_TestDataPath is defined
    if { ! [info exists env(CSF_TestDataPath)] } {
        error "Environment variable CSF_TestDataPath must be defined!"
    }

    # build registry of existing data files (name -> path) and (size -> path)
    puts "Checking available test data files..."
    foreach dir [_split_path $env(CSF_TestDataPath)] {
        while {[llength $dir] != 0} {
            set curr [lindex $dir 0]
            set dir [lrange $dir 1 end]
            eval lappend dir [glob -nocomplain -directory $curr -type d *]
            foreach file [glob -nocomplain -directory $curr -type f *] {
                set name [file tail $file]
                set name_lower [string tolower $name]

                # check that the file is not in DOS encoding
                if { [_check_dos_encoding $file] } {
                    puts "Warning: file $file is in DOS encoding; was this intended?"
                }
                _check_file_format $file

                # check if file with the same name is present twice or more
                if { [info exists names($name_lower)] } {
                    puts "Error: more than one file with name $name is present in the repository:"
                    if { [_diff_files $file $names($name_lower)] } {
                        puts "(files are different by content)"
                    } else {
                        puts "(files are same by content)"
                    }
                    puts "--> $file"
                    puts "--> $names($name_lower)"
                    continue
                } 
                
                # check if file with the same content exists
                set size [file size $file]
                if { [info exists sizes($size)] } {
                    foreach other $sizes($size) {
                        if { ! [_diff_files $file $other] } {
                            puts "Warning: two files with the same content found:"
                            puts "--> $file"
                            puts "--> $other"
                        }
                    }
                }

                # add the file to the registry
                set names($name_lower) $file
                lappend sizes($size) $file
            }
        }
    }
    if { [llength $filelist] <= 0 } { return }

    # check the new files
    set has_images f
    puts "Checking new file(s)..."
    foreach file $filelist {
        # check for DOS encoding
        if { [_check_dos_encoding $file] } {
            puts "$file: Warning: DOS encoding detected"
        }

        set name [file tail $file]
        set name_lower [string tolower $name]

        # check for presence of the file with same name
        if { [info exists names($name_lower)] } {
            if { [_diff_files $file $names($name_lower)] } {
                puts "$file: Error: name is already used by existing file\n--> $names($name_lower)"
            } else {
                puts "$file: OK: already in the repository \n--> $names($name_lower)"
                continue
            }
        }
                
        # check if file with the same content exists
        set size [file size $file]
        if { [info exists sizes($size)] } {
            set found f
            foreach other $sizes($size) {
                if { ! [_diff_files $file $other] } {
                    puts "$file: OK: the same file is already present under name [file tail $other]\n--> $other"
                    set found t
                    break
                }
            }
            if { $found } { continue }
        }

        # try to read the file
        set format [_check_file_format $file]
        if { [catch {uplevel load_data_file $file $format a}] } {
            puts "$file: Error: Cannot read as $format file"
            continue
        }

        # get number of faces and edges
        set edges 0
        set faces 0
        set nbs [uplevel nbshapes a]
        regexp {EDGE[ \t:]*([0-9]+)} $nbs res edges
        regexp {FACE[ \t:]*([0-9]+)} $nbs res faces

        # classify; first check file size and number of faces and edges
        if { $size < 95000 && $faces < 20 && $edges < 100 } {
            set dir public
        } else {
            set dir private
            # check if one of names of that file corresponds to typical name for 
            # MDTV bugs or has extension .rle, this should be old model
            if { [regexp -nocase {.*(cts|ats|pro|buc|ger|fra|usa|uki)[0-9]+.*} $name] ||
                 [regexp -nocase {[.]rle\y} $name] } {
                set dir old
            }
        }

        # add stats
        puts "$file: $format size=[expr $size / 1024] KiB, nbfaces=$faces, nbedges=$edges -> $dir"

        set tmpdir [_get_temp_dir]
        file mkdir $tmpdir/$dir

        # make snapshot
        pload AISV
        uplevel vdisplay a
        uplevel vfit
        uplevel vzfit
        uplevel vdump $tmpdir/$dir/[file rootname [file tail $file]].png
        set has_images t
    }
    if { $has_images } {
        puts "Snapshots are saved in subdirectory [_get_temp_dir]"
    }
}

# Procedure to locate data file for test given its name.
# The search is performed assuming that the function is called
# from the test case script; the search order is:
# - subdirectory "data" of the test script (grid) folder
# - subdirectories in environment variable CSF_TestDataPath
# - subdirectory set by datadir command
# If file is not found, raises Tcl error.
proc locate_data_file {filename} {
    global env groupname gridname casename

    # check if the file is located in the subdirectory data of the script dir
    set scriptfile [info script]
    if { $scriptfile != "" } {
        set path [file join [file dirname $scriptfile] data $filename]
        if { [file exists $path] } {
            return [file normalize $path]
        }
    }

    # check sub-directories in paths indicated by CSF_TestDataPath
    if { [info exists env(CSF_TestDataPath)] } {
        foreach dir [_split_path $env(CSF_TestDataPath)] {
            while {[llength $dir] != 0} { 
                set name [lindex $dir 0]
                set dir [lrange $dir 1 end]
                # skip directories starting with dot
                if { [regexp {^[.]} $name] } { continue }
                if { [file exists $name/$filename] } {
                    return [file normalize $name/$filename]
                }
                eval lappend dir [glob -nocomplain -directory $name -type d *]
            }
        }
    }

    # check current datadir
    if { [file exists [uplevel datadir]/$filename] } {
        return [file normalize [uplevel datadir]/$filename]
    }

    # raise error
    error [join [list "File $filename could not be found" \
                      "(should be in paths indicated by CSF_TestDataPath environment variable, " \
                      "or in subfolder data in the script directory)"] "\n"]
}

# Internal procedure to find test case indicated by group, grid, and test case names;
# returns:
# - dir: path to the base directory of the tests group
# - gridname: actual name of the grid
# - casefile: path to the test case script 
# if no such test is found, raises error with appropriate message
proc _get_test {group grid casename _dir _gridname _casefile} {
    upvar $_dir dir
    upvar $_gridname gridname
    upvar $_casefile casefile

    global env
 
    # check that environment variable defining paths to test scripts is defined
    if { ! [info exists env(CSF_TestScriptsPath)] || 
         [llength $env(CSF_TestScriptsPath)] <= 0 } {
        error "Error: Environment variable CSF_TestScriptsPath is not defined"
    }

    # iterate by all script paths
    foreach dir [_split_path $env(CSF_TestScriptsPath)] {
        # protection against empty paths
        set dir [string trim $dir]
        if { $dir == "" } { continue }

        # check that directory exists
        if { ! [file isdirectory $dir] } {
            puts "Warning: directory $dir listed in CSF_TestScriptsPath does not exist, skipped"
            continue
        }

        # check if test group with given name exists in this dir
        # if not, continue to the next test dir
        if { ! [file isdirectory $dir/$group] } { continue }

        # check that grid with given name (possibly alias) exists; stop otherwise
        set gridname $grid
        if { ! [file isdirectory $dir/$group/$gridname] } {
            # check if grid is named by alias rather than by actual name
            if { [file exists $dir/$group/grids.list] } {
                set fd [open $dir/$group/grids.list]
                while { [gets $fd line] >= 0 } {
                    if { [regexp "\[ \t\]*\#.*" $line] } { continue }
                    if { [regexp "^$grid\[ \t\]*\(\[A-Za-z0-9_.-\]+\)\$" $line res gridname] } {
                        break
                    }
                }
                close $fd
            }
        }
        if { ! [file isdirectory $dir/$group/$gridname] } { continue }

        # get actual file name of the script; stop if it cannot be found
        set casefile $dir/$group/$gridname/$casename
        if { ! [file exists $casefile] } {
            # check if this grid is aliased to another one
            if { [file exists $dir/$group/$gridname/cases.list] } {
                set fd [open $dir/$group/$gridname/cases.list]
                if { [gets $fd line] >= 0 } {
                    set casefile [file normalize $dir/$group/$gridname/[string trim $line]/$casename]
                }
                close $fd
            }
        }
        if { [file exists $casefile] } { 
            # normal return
            return 
        }
    }

    # coming here means specified test is not found; report error
    error [join [list "Error: test case $group / $grid / $casename is not found in paths listed in variable" \
                      "CSF_TestScriptsPath (current value is \"$env(CSF_TestScriptsPath)\")"] "\n"]
}

# Internal procedure to run test case indicated by base directory, 
# grid and grid names, and test case file path.
# The log can be obtained by command "dlog get".
proc _run_test {scriptsdir group gridname casefile echo} {
    global env

    # start timer
    uplevel dchrono _timer reset
    uplevel dchrono _timer start
    catch {uplevel meminfo h} membase

    # enable commands logging; switch to old-style mode if dlog command is not present
    set dlog_exists 1
    if { [catch {dlog reset}] } {
        set dlog_exists 0
    } elseif { $echo } {
        decho on
    } else {
        dlog reset
        dlog on
        rename puts puts-saved
        proc puts args { 
            global _tests_verbose

            # log only output to stdout and stderr, not to file!
            if {[llength $args] > 1} {
                set optarg [lindex $args end-1]
                if { $optarg == "stdout" || $optarg == "stderr" || $optarg == "-newline" } {
                    dlog add [lindex $args end]
                } else {
                    eval puts-saved $args
                }
            } else {
                dlog add [lindex $args end]
            }
        }
    }

    # evaluate test case 
    set tmp_imagedir 0
    if [catch {
        # set variables identifying test case
        uplevel set casename [file tail $casefile]
        uplevel set groupname $group
        uplevel set gridname $gridname
        uplevel set dirname  $scriptsdir

        # set path for saving of log and images (if not yet set) to temp dir
        if { ! [uplevel info exists imagedir] } {
            uplevel set test_image \$casename

            # create subdirectory in temp named after group and grid with timestamp
            set rootlogdir [_get_temp_dir]
        
            set imagedir "${group}-${gridname}-${::casename}-[clock format [clock seconds] -format {%Y-%m-%dT%Hh%Mm%Ss}]"
            set imagedir [file normalize ${rootlogdir}/$imagedir]

            if { [catch {file mkdir $imagedir}] || ! [file writable $imagedir] ||
                 ! [catch {glob -directory $imagedir *}] } {
                 # puts "Warning: Cannot create directory \"$imagedir\", or it is not empty; \"${rootlogdir}\" is used"
                set imagedir $rootlogdir
            }

            uplevel set imagedir \"$imagedir\"
            set tmp_imagedir 1
        }

        # execute test scripts 
        if { [file exists $scriptsdir/$group/begin] } {
            puts "Executing $scriptsdir/$group/begin..."; flush stdout
            uplevel source $scriptsdir/$group/begin
        }
        if { [file exists $scriptsdir/$group/$gridname/begin] } {
            puts "Executing $scriptsdir/$group/$gridname/begin..."; flush stdout
            uplevel source $scriptsdir/$group/$gridname/begin
        }

        puts "Executing $casefile..."; flush stdout
        uplevel source $casefile

        if { [file exists $scriptsdir/$group/$gridname/end] } {
            puts "Executing $scriptsdir/$group/$gridname/end..."; flush stdout
            uplevel source $scriptsdir/$group/$gridname/end
        }
        if { [file exists $scriptsdir/$group/end] } {
            puts "Executing $scriptsdir/$group/end..."; flush stdout
            uplevel source $scriptsdir/$group/end
        }
    } res] {
        puts "Tcl Exception: $res"
    }

    # stop logging
    if { $dlog_exists } {
        if { $echo } {
            decho off
        } else {
            rename puts {}
            rename puts-saved puts
            dlog off
        }
    }

    # stop cpulimit killer if armed by the test
    cpulimit

    # add memory and timing info
    set stats ""
    if { ! [catch {uplevel meminfo h} memuse] } {
        append stats "MEMORY DELTA: [expr ($memuse - $membase) / 1024] KiB\n"
    }
    uplevel dchrono _timer stop
    set time [uplevel dchrono _timer show]
    if { [regexp -nocase {CPU user time:[ \t]*([0-9.e-]+)} $time res cpu_usr] } {
        append stats "TOTAL CPU TIME: $cpu_usr sec\n"
    }
    if { $dlog_exists && ! $echo } {
        dlog add $stats
    } else {
        puts $stats
    }

    # unset global vars
    uplevel unset casename groupname gridname dirname
    if { $tmp_imagedir } { uplevel unset imagedir test_image }
}

# Internal procedure to check log of test execution and decide if it passed or failed
proc _check_log {dir group gridname casename errors log {_summary {}} {_html_log {}}} {
    global env
    if { $_summary != "" } { upvar $_summary summary }
    if { $_html_log != "" } { upvar $_html_log html_log }
    set summary {}
    set html_log {}
    set errors_log {}

    if [catch {

        # load definition of 'bad words' indicating test failure
        # note that rules are loaded in the order of decreasing priority (grid - group - common),
        # thus grid rules will override group ones
        set badwords {}
        foreach rulesfile [list $dir/$group/$gridname/parse.rules $dir/$group/parse.rules $dir/parse.rules] {
            if [catch {set fd [open $rulesfile r]}] { continue }
            while { [gets $fd line] >= 0 } {
                # skip comments and empty lines
                if { [regexp "\[ \t\]*\#.*" $line] } { continue }
                if { [string trim $line] == "" } { continue }
                # extract regexp
                if { ! [regexp {^([^/]*)/([^/]*)/(.*)$} $line res status rexp comment] } { 
                    puts "Warning: cannot recognize parsing rule \"$line\" in file $rulesfile"
                    continue 
                }
                set status [string trim $status]
                if { $comment != "" } { append status " ([string trim $comment])" }
                set rexp [regsub -all {\\b} $rexp {\\y}] ;# convert regexp from Perl to Tcl style
                lappend badwords [list $status $rexp]
            }
            close $fd
        }
        if { [llength $badwords] <= 0 } { 
            puts "Warning: no definition of error indicators found (check files parse.rules)" 
        }

        # analyse log line-by-line
        set todos {} ;# TODO statements
        set requs {} ;# REQUIRED statements
        set todo_incomplete -1
        set status ""
        foreach line [split $log "\n"] {
            # check if line defines specific treatment of some messages
            if [regexp -nocase {^[ \s]*TODO ([^:]*):(.*)$} $line res platforms pattern] {
                if { ! [regexp -nocase {\mAll\M} $platforms] && 
                     ! [regexp -nocase "\\m[checkplatform]\\M" $platforms] } {
                    lappend html_log [_html_highlight IGNORE $line]
                    continue ;# TODO statement is for another platform
                }

                # record TODOs that mark unstable cases
                if { [regexp {[\?]} $platforms] } {
                    set todos_unstable([llength $todos]) 1
                }

                # convert legacy regexps from Perl to Tcl style
                set pattern [regsub -all {\\b} [string trim $pattern] {\\y}]

                # special case: TODO TEST INCOMPLETE
                if { [string trim $pattern] == "TEST INCOMPLETE" } {
                    set todo_incomplete [llength $todos]
                }

                lappend todos [list $pattern [llength $html_log] $line]
                lappend html_log [_html_highlight BAD $line]
                continue
            }
            if [regexp -nocase {^[ \s]*REQUIRED ([^:]*):[ \s]*(.*)$} $line res platforms pattern] {
                if { ! [regexp -nocase {\mAll\M} $platforms] && 
                     ! [regexp -nocase "\\m[checkplatform]\\M" $platforms] } {
                    lappend html_log [_html_highlight IGNORE $line]
                    continue ;# REQUIRED statement is for another platform
                }
                lappend requs [list $pattern [llength $html_log] $line]
                lappend html_log [_html_highlight OK $line]
                continue
            }

            # check for presence of required messages 
            set ismarked 0
            for {set i 0} {$i < [llength $requs]} {incr i} {
                set pattern [lindex $requs $i 0]
                if { [regexp $pattern $line] } {
                    incr required_count($i)
                    lappend html_log [_html_highlight OK $line]
                    set ismarked 1
                    continue
                }
            }
            if { $ismarked } {
                continue
            }

            # check for presence of messages indicating test result
            foreach bw $badwords {
                if { [regexp [lindex $bw 1] $line] } { 
                    # check if this is known bad case
                    set is_known 0
                    for {set i 0} {$i < [llength $todos]} {incr i} {
                        set pattern [lindex $todos $i 0]
                        if { [regexp $pattern $line] } {
                            set is_known 1
                            incr todo_count($i)
                            lappend html_log [_html_highlight BAD $line]
                            break
                        }
                    }

                    # if it is not in todo, define status
                    if { ! $is_known } {
                        set stat [lindex $bw 0 0]
                        if {$errors} {
                            lappend errors_log $line
                        }
                        lappend html_log [_html_highlight $stat $line]
                        if { $status == "" && $stat != "OK" && ! [regexp -nocase {^IGNOR} $stat] } {
                            set status [lindex $bw 0]
                        }
                    }
                    set ismarked 1
                    break
                }
            }
            if { ! $ismarked } { 
               lappend html_log $line
            }
        }

        # check for presence of TEST COMPLETED statement
        if { $status == "" && ! [regexp {TEST COMPLETED} $log] } {
            # check whether absence of TEST COMPLETED is known problem
            if { $todo_incomplete >= 0 } {
                incr todo_count($todo_incomplete)
            } else {
                set status "FAILED (no final message is found)"
            }
        }

        # report test as failed if it doesn't contain required pattern
        if { $status == "" } {
            for {set i 0} {$i < [llength $requs]} {incr i} {
                if { ! [info exists required_count($i)] } {
                    set linenum [lindex $requs $i 1]
                    set html_log [lreplace $html_log $linenum $linenum [_html_highlight FAILED [lindex $requs $i 2]]]
                    set status "FAILED (REQUIRED statement no. [expr $i + 1] is not found)"
                }
            }
        }

        # check declared bad cases and diagnose possible improvement 
        # (bad case declared but not detected).
        # Note that absence of the problem marked by TODO with question mark
        # (unstable) is not reported as improvement.
        if { $status == "" } {
            for {set i 0} {$i < [llength $todos]} {incr i} {
                if { ! [info exists todos_unstable($i)] &&
                     (! [info exists todo_count($i)] || $todo_count($i) <= 0) } {
                    set linenum [lindex $todos $i 1]
                    set html_log [lreplace $html_log $linenum $linenum [_html_highlight IMPROVEMENT [lindex $todos $i 2]]]
                    set status "IMPROVEMENT (expected problem TODO no. [expr $i + 1] is not detected)"
                    break;
                }
            }
        }

        # report test as known bad if at least one of expected problems is found
        if { $status == "" && [llength [array names todo_count]] > 0 } {
            set status "BAD (known problem)"
        }

        # report normal OK
        if { $status == "" } {set status "OK" }

    } res] {
        set status "FAILED ($res)"
    }

    # put final message
    _log_and_puts summary "CASE $group $gridname $casename: $status"
    set summary [join $summary "\n"]
    if {$errors} {
        foreach error $errors_log {
            _log_and_puts summary "  $error"
        }
    }
    set html_log "[_html_highlight [lindex $status 0] $summary]\n[join $html_log \n]"
}

# Auxiliary procedure putting message to both cout and log variable (list)
proc _log_and_puts {logvar message} {
    if { $logvar != "" } { 
        upvar $logvar log
        lappend log $message
    }
    puts $message
}

# Auxiliary procedure to log result on single test case
proc _log_test_case {output logdir dir group grid casename logvar} {
    upvar $logvar log
    set show_errors 0
    # check result and make HTML log
    _check_log $dir $group $grid $casename $show_errors $output summary html_log
    lappend log $summary

    # save log to file
    if { $logdir != "" } {
        _log_html $logdir/$group/$grid/$casename.html $html_log "Test $group $grid $casename"
        _log_save $logdir/$group/$grid/$casename.log "$output\n$summary" "Test $group $grid $casename"
    }
}

# Auxiliary procedure to save log to file
proc _log_save {file log {title {}}} {
    # create missing directories as needed
    catch {file mkdir [file dirname $file]}

    # try to open a file
    if [catch {set fd [open $file w]} res] {
        error "Error saving log file $file: $res"
    }
    
    # dump log and close
    puts $fd "$title\n"
    puts $fd $log
    close $fd
    return
}

# Auxiliary procedure to make a (relative if possible) URL to a file for 
# inclusion a reference in HTML log
proc _make_url {htmldir file} {
    set htmlpath [file split [file normalize $htmldir]]
    set filepath [file split [file normalize $file]]
    for {set i 0} {$i < [llength $htmlpath]} {incr i} {
        if { "[lindex $htmlpath $i]" != "[lindex $filepath $i]" } {
            if { $i == 0 } { break }
            return "[string repeat "../" [expr [llength $htmlpath] - $i - 1]][eval file join [lrange $filepath $i end]]"
        }
    }

    # if relative path could not be made, return full file URL
    return "file://[file normalize $file]"
}

# Auxiliary procedure to save log to file
proc _log_html {file log {title {}}} {
    # create missing directories as needed
    catch {file mkdir [file dirname $file]}

    # try to open a file
    if [catch {set fd [open $file w]} res] {
        error "Error saving log file $file: $res"
    }
    
    # print header
    puts $fd "<html><head><title>$title</title></head><body><h1>$title</h1>"

    # add images if present; these should have either PNG, GIF, or JPG extension,
    # and start with name of the test script, with optional suffix separated
    # by underscore or dash
    set imgbasename [file rootname [file tail $file]]
    foreach img [lsort [glob -nocomplain -directory [file dirname $file] -tails \
                             ${imgbasename}.gif   ${imgbasename}.png   ${imgbasename}.jpg \
                             ${imgbasename}_*.gif ${imgbasename}_*.png ${imgbasename}_*.jpg \
                             ${imgbasename}-*.gif ${imgbasename}-*.png ${imgbasename}-*.jpg]] {
        puts $fd "<p>[file tail $img]<br><img src=\"$img\"/><p>"
    }

    # print log body, trying to add HTML links to script files on lines like
    # "Executing <filename>..."
    puts $fd "<pre>"
    foreach line [split $log "\n"] {
        if { [regexp {Executing[ \t]+([a-zA-Z0-9._/:-]+[^.])} $line res script] &&
             [file exists $script] } {
            set line [regsub $script $line "<a href=\"[_make_url $file $script]\">$script</a>"]
        }
        puts $fd $line
    }
    puts $fd "</pre></body></html>"

    close $fd
    return
}

# Auxiliary method to make text with HTML highlighting according to status
proc _html_color {status} {
    # choose a color for the cell according to result
    if { $status == "OK" } { 
        return lightgreen
    } elseif { [regexp -nocase {^FAIL} $status] } { 
        return ff8080
    } elseif { [regexp -nocase {^BAD} $status] } { 
        return yellow
    } elseif { [regexp -nocase {^IMP} $status] } { 
        return orange
    } elseif { [regexp -nocase {^SKIP} $status] } { 
        return gray
    } elseif { [regexp -nocase {^IGNOR} $status] } { 
        return gray
    } else {
        puts "Warning: no color defined for status $status, using red as if FAILED"
        return red
    }
}

# Format text line in HTML to be colored according to the status
proc _html_highlight {status line} {
    return "<table><tr><td bgcolor=\"[_html_color $status]\">$line</td></tr></table>"
}

# Internal procedure to generate HTML page presenting log of the tests
# execution in tabular form, with links to reports on individual cases
proc _log_html_summary {logdir log totals regressions improvements total_time} {
    global _test_case_regexp

    # create missing directories as needed
    file mkdir $logdir

    # try to open a file and start HTML
    if [catch {set fd [open $logdir/summary.html w]} res] {
        error "Error creating log file: $res"
    }

    # write HRML header, including command to refresh log if still in progress
    puts $fd "<html><head>"
    puts $fd "<title>Tests summary</title>"
    if { $total_time == "" } {
        puts $fd "<meta http-equiv=\"refresh\" content=\"10\">"
    }
    puts $fd "<meta http-equiv=\"pragma\" content=\"NO-CACHE\">"
    puts $fd "</head><body>"

    # put summary
    set legend(OK)          "Test passed OK"
    set legend(FAILED)      "Test failed (regression)"
    set legend(BAD)         "Known problem"
    set legend(IMPROVEMENT) "Possible improvement (expected problem not detected)"
    set legend(SKIPPED)     "Test skipped due to lack of data file"
    puts $fd "<h1>Summary</h1><table>"
    foreach nbstat $totals {
        set status [lindex $nbstat 1]
        if { [info exists legend($status)] } { 
            set comment $legend($status) 
        } else {
            set comment "User-defined status"
        }
        puts $fd "<tr><td align=\"right\">[lindex $nbstat 0]</td><td bgcolor=\"[_html_color $status]\">$status</td><td>$comment</td></tr>"
    }
    puts $fd "</table>"

    # time stamp and elapsed time info
    if { $total_time != "" } { 
        puts $fd "<p>Generated on [clock format [clock seconds] -format {%Y-%m-%d %H:%M:%S}] on [info hostname]\n<p>"
        puts $fd [join [split $total_time "\n"] "<p>"]
    } else {
        puts $fd "<p>NOTE: This is intermediate summary; the tests are still running! This page will refresh automatically until tests are finished."
    }
   
    # print regressions and improvements
    foreach featured [list $regressions $improvements] {
        if { [llength $featured] <= 1 } { continue }
        set status [string trim [lindex $featured 0] { :}]
        puts $fd "<h2>$status</h2>"
        puts $fd "<table>"
        set groupgrid ""
        foreach test [lrange $featured 1 end] {
            if { ! [regexp {^(.*)\s+([\w.]+)$} $test res gg name] } {
                set gg UNKNOWN
                set name "Error building short list; check details"
            }
            if { $gg != $groupgrid } {
                if { $groupgrid != "" } { puts $fd "</tr>" }
                set groupgrid $gg
                puts $fd "<tr><td>$gg</td>"
            }
            puts $fd "<td bgcolor=\"[_html_color $status]\"><a href=\"[regsub -all { } $gg /]/${name}.html\">$name</a></td>"
        }
        if { $groupgrid != "" } { puts $fd "</tr>" }
        puts $fd "</table>"
    }

    # put detailed log with TOC
    puts $fd "<hr><h1>Details</h1>"
    puts $fd "<div style=\"float:right; padding: 10px; border-style: solid; border-color: blue; border-width: 2px;\">"

    # process log line-by-line
    set group {}
    set letter {}
    set body {}
    foreach line [lsort -dictionary $log] {
        # check that the line is case report in the form "CASE group grid name: result (explanation)"
        if { ! [regexp $_test_case_regexp $line res grp grd casename result message] } {
            continue
        }

        # start new group
        if { $grp != $group } {
            if { $letter != "" } { lappend body "</tr></table>" }
            set letter {}
            set group $grp
            set grid {}
            puts $fd "<a href=\"#$group\">$group</a><br>"
            lappend body "<h2><a name=\"$group\">Group $group</a></h2>"
        }

        # start new grid
        if { $grd != $grid } {
            if { $letter != "" } { lappend body "</tr></table>" }
            set letter {}
            set grid $grd
            puts $fd "&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"#$group-$grid\">$grid</a><br>"
            lappend body "<h2><a name=\"$group-$grid\">Grid $group $grid</a></h2>"
        }

        # check if test case name is <letter><digit>; 
        # if not, set alnum to period "." to recognize non-standard test name
        if { ! [regexp {\A([A-Za-z]{1,2})([0-9]{1,2})\Z} $casename res alnum number] &&
             ! [regexp {\A([A-Za-z0-9]+)_([0-9]+)\Z} $casename res alnum number] } {
            set alnum $casename
        }

        # start new row when letter changes or for non-standard names
        if { $alnum != $letter || $alnum == "." } {
            if { $letter != "" } { 
                lappend body "</tr><tr>" 
            } else {
                lappend body "<table><tr>"
            }
            set letter $alnum
        }    

        lappend body "<td bgcolor=\"[_html_color $result]\"><a href=\"$group/$grid/${casename}.html\">$casename</a></td>"
    }
    puts $fd "</div>\n[join $body "\n"]</tr></table>"

    # add remaining lines of log as plain text
    puts $fd "<h2>Plain text messages</h2>\n<pre>"
    foreach line $log {
        if { ! [regexp $_test_case_regexp $line] } {
            puts $fd "$line"
        }
    }
    puts $fd "</pre>"

    # close file and exit
    puts $fd "</body>"
    close $fd
    return
}

# Procedure to dump summary logs of tests
proc _log_summarize {logdir log {total_time {}}} {

    # sort log records alphabetically to have the same behavior on Linux and Windows 
    # (also needed if tests are run in parallel)
    set loglist [lsort -dictionary $log]

    # classify test cases by status
    foreach line $loglist {
        if { [regexp {^CASE ([^:]*): ([[:alnum:]]+).*$} $line res caseid status] } {
            lappend stat($status) $caseid
        }
    }
    set totals {}
    set improvements {Improvements:}
    set regressions {Failed:}
    if { [info exists stat] } {
        foreach status [lsort [array names stat]] {
            lappend totals [list [llength $stat($status)] $status]

            # separately count improvements (status starting with IMP) and regressions (all except IMP, OK, BAD, and SKIP)
            if { [regexp -nocase {^IMP} $status] } {
                eval lappend improvements $stat($status)
            } elseif { $status != "OK" && ! [regexp -nocase {^BAD} $status] && ! [regexp -nocase {^SKIP} $status] } {
                eval lappend regressions $stat($status)
            }
        }
    }

    # if time is specified, add totals
    if { $total_time != "" } {
        if { [llength $improvements] > 1 } {
            _log_and_puts log [join $improvements "\n  "]
        }
        if { [llength $regressions] > 1 } {
            _log_and_puts log [join $regressions "\n  "]
        }
        if { [llength $improvements] == 1 && [llength $regressions] == 1 } {
            _log_and_puts log "No regressions"
        }
        _log_and_puts log "Total cases: [join $totals {, }]"
        _log_and_puts log $total_time
    }

    # save log to files
    if { $logdir != "" } {
        _log_html_summary $logdir $log $totals $regressions $improvements $total_time
        _log_save $logdir/tests.log [join $log "\n"] "Tests summary"
    }

    return
}

# Internal procedure to generate XML log in JUnit style, for further
# consumption by Jenkins or similar systems.
#
# The output is intended to conform to XML schema supported by Jenkins found at
# https://svn.jenkins-ci.org/trunk/hudson/dtkit/dtkit-format/dtkit-junit-model/src/main/resources/com/thalesgroup/dtkit/junit/model/xsd/junit-4.xsd
#
# The mapping of the fields is inspired by annotated schema of Apache Ant JUnit XML format found at
# http://windyroad.org/dl/Open%20Source/JUnit.xsd
proc _log_xml_summary {logdir filename log include_cout} {
    global _test_case_regexp

    catch {file mkdir [file dirname $filename]}

    # try to open a file and start XML
    if [catch {set fd [open $filename w]} res] {
        error "Error creating XML summary file $filename: $res"
    }
    puts $fd "<?xml version='1.0' encoding='utf-8'?>"
    puts $fd "<testsuites>"

    # prototype for command to generate test suite tag
    set time_and_host "timestamp=\"[clock format [clock seconds] -format {%Y-%m-%dT%H:%M:%S}]\" hostname=\"[info hostname]\""
    set cmd_testsuite {puts $fd "<testsuite name=\"$group $grid\" tests=\"$nbtests\" failures=\"$nbfail\" errors=\"$nberr\" time=\"$time\" skipped=\"$nbskip\" $time_and_host>\n$testcases\n</testsuite>\n"}

    # sort log and process it line-by-line
    set group {}
    foreach line [lsort -dictionary $log] {
        # check that the line is case report in the form "CASE group grid name: result (explanation)"
        if { ! [regexp $_test_case_regexp $line res grp grd casename result message] } {
            continue
        }
        set message [string trim $message " \t\r\n()"]

        # start new testsuite for each grid
        if { $grp != $group || $grd != $grid } {

            # write previous test suite
            if [info exists testcases] { eval $cmd_testsuite }

            set testcases {}
            set nbtests 0
            set nberr 0
            set nbfail 0
            set nbskip 0
            set time 0.

            set group $grp
            set grid $grd
        }

        incr nbtests
 
        # parse test log and get its CPU time
        set testout {}
        set add_cpu {}
        if { [catch {set fdlog [open $logdir/$group/$grid/${casename}.log r]} ret] } { 
            puts "Error: cannot open $logdir/$group/$grid/${casename}.log: $ret"
        } else {
            while { [gets $fdlog logline] >= 0 } {
                if { $include_cout } {
                    append testout "$logline\n"
                }
                if [regexp -nocase {TOTAL CPU TIME:\s*([\d.]+)\s*sec} $logline res cpu] {
                    set add_cpu " time=\"$cpu\""
                    set time [expr $time + $cpu]
                }
            }
            close $fdlog
        }
        if { ! $include_cout } {
            set testout "$line\n"
        }

        # record test case with its output and status
        # Mapping is: SKIPPED, BAD, and OK to OK, all other to failure
        append testcases "\n  <testcase name=\"$casename\"$add_cpu status=\"$result\">\n"
        append testcases "\n    <system-out>\n$testout    </system-out>"
        if { $result != "OK" } {
            if { [regexp -nocase {^SKIP} $result] } {
                incr nberr
                append testcases "\n    <error name=\"$result\" message=\"$message\"/>"
            } elseif { [regexp -nocase {^BAD} $result] } {
                incr nbskip
                append testcases "\n    <skipped>$message</skipped>"
            } else {
                incr nbfail
                append testcases "\n    <failure name=\"$result\" message=\"$message\"/>"
            }
        }
        append testcases "\n  </testcase>"
    }

    # write last test suite
    if [info exists testcases] { eval $cmd_testsuite }

    # the end
    puts $fd "</testsuites>"
    close $fd
    return
}

# Auxiliary procedure to split path specification (usually defined by
# environment variable) into list of directories or files
proc _split_path {pathspec} {
    global tcl_platform

    # first replace all \ (which might occur on Windows) by /  
    regsub -all "\\\\" $pathspec "/" pathspec

    # split path by platform-specific separator
    return [split $pathspec [_path_separator]]
}

# Auxiliary procedure to define platform-specific separator for directories in
# path specification
proc _path_separator {} {
    global tcl_platform

    # split path by platform-specific separator
    if { $tcl_platform(platform) == "windows" } {
        return ";"
    } else {
        return ":"
    }
}

# Procedure to make a diff and common of two lists
proc _list_diff {list1 list2 _in1 _in2 _common} {
    upvar $_in1 in1
    upvar $_in2 in2
    upvar $_common common

    set in1 {}
    set in2 {}
    set common {}
    foreach item $list1 {
        if { [lsearch -exact $list2 $item] >= 0 } {
            lappend common $item
        } else {
            lappend in1 $item
        }
    }
    foreach item $list2 {
        if { [lsearch -exact $common $item] < 0 } {
            lappend in2 $item
        }
    }
    return
}

# procedure to load a file to Tcl string
proc _read_file {filename} {
    set fd [open $filename r]
    set result [read -nonewline $fd]
    close $fd
    return $result
}

# procedure to construct name for the mage diff file
proc _diff_img_name {dir1 dir2 casepath imgfile} {
    return [file join $dir1 $casepath "diff-[file tail $dir2]-$imgfile"]
}

# auxiliary procedure to produce string comparing two values
proc _diff_show_ratio {value1 value2} {
    if {[expr double ($value2)] == 0.} {
        return "$value1 / $value2"
    } else {
        return "$value1 / $value2 \[[format "%+5.2f%%" [expr 100 * ($value1 - $value2) / double($value2)]]\]"
    }
}

# procedure to check cpu user time
proc _check_time {regexp_msg} {
    upvar log log
    upvar log1 log1
    upvar log2 log2
    upvar log_cpu log_cpu
    upvar cpu cpu
    upvar basename basename
    upvar casename casename
    set time1_list [dict create]
    set time2_list [dict create]
    set cpu_find UNDEFINED

    foreach line1 [split $log1 "\n"] {
        if { [regexp "${regexp_msg}" $line1 dump chronometer_name cpu_find] } {
            dict set time1_list "${chronometer_name}" "${cpu_find}"
        }
    }

    foreach line2 [split $log2 "\n"] {
        if { [regexp "${regexp_msg}" $line2 dump chronometer_name cpu_find] } {
            dict set time2_list "${chronometer_name}" "${cpu_find}"
        }
    }

    if { [llength [dict keys $time1_list]] != [llength [dict keys $time2_list]] } {
        puts "Error: number of dchrono/chrono COUNTER are different in the same test cases"
    } else {
        foreach key [dict keys $time1_list] {
            set time1 [dict get $time1_list $key]
            set time2 [dict get $time2_list $key]

            # compare CPU user time with 10% precision (but not less 0.5 sec)
            if { [expr abs ($time1 - $time2) > 0.5 + 0.05 * abs ($time1 + $time2)] } {
                if {$cpu != false} {
                    _log_and_puts log_cpu "COUNTER $key: [split $basename /] $casename: [_diff_show_ratio $time1 $time2]"
                } else {
                    _log_and_puts log "COUNTER $key: [split $basename /] $casename: [_diff_show_ratio $time1 $time2]"
                }
            }
        }
    }
}

# Procedure to compare results of two runs of test cases
proc _test_diff {dir1 dir2 basename image cpu memory status verbose _logvar _logimage _logcpu _logmemory {_statvar ""}} {
    upvar $_logvar log
    upvar $_logimage log_image
    upvar $_logcpu log_cpu
    upvar $_logmemory log_memory

    # make sure to load diffimage command
    uplevel pload VISUALIZATION

    # prepare variable (array) for collecting statistics
    if { "$_statvar" != "" } {
        upvar $_statvar stat
    } else {
        set stat(cpu1) 0
        set stat(cpu2) 0
        set stat(mem1) 0
        set stat(mem2) 0
        set log {}
        set log_image {}
        set log_cpu {}
        set log_memory {}
    }

    # first check subdirectories
    set path1 [file join $dir1 $basename]
    set path2 [file join $dir2 $basename]
    set list1 [glob -directory $path1 -types d -tails -nocomplain *]
    set list2 [glob -directory $path2 -types d -tails -nocomplain *]
    if { [llength $list1] >0 || [llength $list2] > 0 } {
        _list_diff $list1 $list2 in1 in2 common
        if { "$verbose" > 1 } {
            if { [llength $in1] > 0 } { _log_and_puts log "Only in $path1: $in1" }
            if { [llength $in2] > 0 } { _log_and_puts log "Only in $path2: $in2" }
        }
        foreach subdir $common {
            if { "$verbose" > 2 } {
                _log_and_puts log "Checking [file join $basename $subdir]"
            }
            _test_diff $dir1 $dir2 [file join $basename $subdir] $image $cpu $memory $status $verbose log log_image log_cpu log_memory stat
        }
    } else {
        # check log files (only if directory has no subdirs)
        set list1 [glob -directory $path1 -types f -tails -nocomplain *.log]
        set list2 [glob -directory $path2 -types f -tails -nocomplain *.log]
        _list_diff $list1 $list2 in1 in2 common
        if { "$verbose" > 1 } {
            if { [llength $in1] > 0 } { _log_and_puts log "Only in $path1: $in1" }
            if { [llength $in2] > 0 } { _log_and_puts log "Only in $path2: $in2" }
        }
        set gcpu1 0
        set gcpu2 0
        set gmem1 0
        set gmem2 0
        foreach logfile $common {
            # load two logs
            set log1 [_read_file [file join $dir1 $basename $logfile]]
            set log2 [_read_file [file join $dir2 $basename $logfile]]
            set casename [file rootname $logfile]
            
            # check execution statuses
            if {$image == false && $cpu == false && $memory == false} {
                set status1 UNDEFINED
                set status2 UNDEFINED
                if { ! [regexp {CASE [^:]*:\s*([\w]+)} $log1 res1 status1] ||
                    ! [regexp {CASE [^:]*:\s*([\w]+)} $log2 res2 status2] ||
                    "$status1" != "$status2" } {
                    _log_and_puts log "STATUS [split $basename /] $casename: $status1 / $status2"
                    # if test statuses are different, further comparison makes 
                    # no sense unless explicitly requested
                    if { "$status" != "all" } {
                        continue
                    }
                }
                if { "$status" == "ok" && "$status1" != "OK" } { 
                    continue
                }
            }

            if { ! $image } {
                # check CPU user time in test cases
                set checkCPURegexp "COUNTER (.+): (\[-0-9.+eE\]+)"
                if { [regexp "${checkCPURegexp}" $log1] &&
                     [regexp "${checkCPURegexp}" $log2] } {
                  _check_time "${checkCPURegexp}"
                }
            }
            
            # check CPU times
            if {$cpu != false || ($image == false && $cpu == false && $memory == false)} {
                set cpu1 UNDEFINED
                set cpu2 UNDEFINED
                if { [regexp {TOTAL CPU TIME:\s*([\d.]+)} $log1 res1 cpu1] &&
                     [regexp {TOTAL CPU TIME:\s*([\d.]+)} $log2 res1 cpu2] } {
                    set stat(cpu1) [expr $stat(cpu1) + $cpu1]
                    set stat(cpu2) [expr $stat(cpu2) + $cpu2]
                    set gcpu1 [expr $gcpu1 + $cpu1]
                    set gcpu2 [expr $gcpu2 + $cpu2]

                    # compare CPU times with 10% precision (but not less 0.5 sec)
                    if { [expr abs ($cpu1 - $cpu2) > 0.5 + 0.05 * abs ($cpu1 + $cpu2)] } {
                        if {$cpu != false} {
                            _log_and_puts log_cpu "CPU [split $basename /] $casename: [_diff_show_ratio $cpu1 $cpu2]"
                        } else {
                            _log_and_puts log "CPU [split $basename /] $casename: [_diff_show_ratio $cpu1 $cpu2]"
                        }
                    }
                }
            }

            # check memory delta
            if {$memory != false || ($image == false && $cpu == false && $memory == false)} {
                set mem1 UNDEFINED
                set mem2 UNDEFINED
                if { [regexp {MEMORY DELTA:\s*([\d.]+)} $log1 res1 mem1] &&
                     [regexp {MEMORY DELTA:\s*([\d.]+)} $log2 res1 mem2] } {
                    set stat(mem1) [expr $stat(mem1) + $mem1]
                    set stat(mem2) [expr $stat(mem2) + $mem2]
                    set gmem1 [expr $gmem1 + $mem1]
                    set gmem2 [expr $gmem2 + $mem2]

                    # compare memory usage with 10% precision (but not less 16 KiB)
                    if { [expr abs ($mem1 - $mem2) > 16 + 0.05 * abs ($mem1 + $mem2)] } {
                        if {$memory != false} {
                            _log_and_puts log_memory "MEMORY [split $basename /] $casename: [_diff_show_ratio $mem1 $mem2]"
                        } else {
                            _log_and_puts log "MEMORY [split $basename /] $casename: [_diff_show_ratio $mem1 $mem2]"
                        }
                    }
                }
            }

            # check images
            if {$image != false || ($image == false && $cpu == false && $memory == false)} {
                set imglist1 [glob -directory $path1 -types f -tails -nocomplain ${casename}.{png,gif} ${casename}-*.{png,gif} ${casename}_*.{png,gif}]
                set imglist2 [glob -directory $path2 -types f -tails -nocomplain ${casename}.{png,gif} ${casename}-*.{png,gif} ${casename}_*.{png,gif}]
                _list_diff $imglist1 $imglist2 imgin1 imgin2 imgcommon
                if { "$verbose" > 1 } {
                    if { [llength $imgin1] > 0 } {
                        if {$image != false} {
                            _log_and_puts log_image "Only in $path1: $imgin1"
                        } else {
                            _log_and_puts log "Only in $path1: $imgin1"
                        }
                    }
                    if { [llength $imgin2] > 0 } {
                        if {$image != false} {
                            _log_and_puts log_image "Only in $path2: $imgin2"
                        } else {
                            _log_and_puts log "Only in $path2: $imgin2"
                        }
                    }
                }
                foreach imgfile $imgcommon {
                    # if { $verbose > 1 } { _log_and_puts log "Checking [split basename /] $casename: $imgfile" }
                    set diffile [_diff_img_name $dir1 $dir2 $basename $imgfile]
                    if { [catch {diffimage [file join $dir1 $basename $imgfile] \
                                           [file join $dir2 $basename $imgfile] \
                                           0 0 0 $diffile} diff] } {
                        if {$image != false} {
                            _log_and_puts log_image "IMAGE [split $basename /] $casename: $imgfile cannot be compared"
                        } else {
                            _log_and_puts log "IMAGE [split $basename /] $casename: $imgfile cannot be compared"
                        }
                        file delete -force $diffile ;# clean possible previous result of diffimage
                    } elseif { $diff != 0 } {
                        if {$image != false} {
                            _log_and_puts log_image "IMAGE [split $basename /] $casename: $imgfile differs"
                        } else {
                            _log_and_puts log "IMAGE [split $basename /] $casename: $imgfile differs"
                        }
                    } else {
                        file delete -force $diffile ;# clean useless artifact of diffimage
                    }
                }
            }
        }
        
        # report CPU and memory difference in group if it is greater than 10%
        if {$cpu != false || ($image == false && $cpu == false && $memory == false)} {
            if { [expr abs ($gcpu1 - $gcpu2) > 0.5 + 0.005 * abs ($gcpu1 + $gcpu2)] } {
                if {$cpu != false} {
                    _log_and_puts log_cpu "CPU [split $basename /]: [_diff_show_ratio $gcpu1 $gcpu2]"
                } else {
                    _log_and_puts log "CPU [split $basename /]: [_diff_show_ratio $gcpu1 $gcpu2]"
                }
            }
        }
        if {$memory != false || ($image == false && $cpu == false && $memory == false)} {
            if { [expr abs ($gmem1 - $gmem2) > 16 + 0.005 * abs ($gmem1 + $gmem2)] } {
                if {$memory != false} {
                    _log_and_puts log_memory "MEMORY [split $basename /]: [_diff_show_ratio $gmem1 $gmem2]"
                } else {
                    _log_and_puts log "MEMORY [split $basename /]: [_diff_show_ratio $gmem1 $gmem2]"
                }
            }
        }
    }

    if { "$_statvar" == "" } {
        if {$memory != false || ($image == false && $cpu == false && $memory == false)} {
            if {$memory != false} {
                _log_and_puts log_memory "Total MEMORY difference: [_diff_show_ratio $stat(mem1) $stat(mem2)]"
            } else {
                _log_and_puts log "Total MEMORY difference: [_diff_show_ratio $stat(mem1) $stat(mem2)]"
            }
        }
        if {$cpu != false || ($image == false && $cpu == false && $memory == false)} {
            if {$cpu != false} {
                _log_and_puts log_cpu "Total CPU difference: [_diff_show_ratio $stat(cpu1) $stat(cpu2)]"
            } else {
                _log_and_puts log "Total CPU difference: [_diff_show_ratio $stat(cpu1) $stat(cpu2)]"
            }
        }
    }
}

# Auxiliary procedure to save log of results comparison to file
proc _log_html_diff {file log dir1 dir2 highlight_percent} {
    # create missing directories as needed
    catch {file mkdir [file dirname $file]}

    # try to open a file
    if [catch {set fd [open $file w]} res] {
        error "Error saving log file $file: $res"
    }
    
    # print header
    puts $fd "<html><head><title>Diff $dir1 vs. $dir2</title></head><body>"
    puts $fd "<h1>Comparison of test results:</h1>"
    puts $fd "<h2>Version A - $dir1</h2>"
    puts $fd "<h2>Version B - $dir2</h2>"

    # add script for switching between images on click
    puts $fd ""
    puts $fd "<script type=\"text/javascript\">"
    puts $fd "  function diffimage_toggle(img,url1,url2)"
    puts $fd "  {"
    puts $fd "    if (img.show2nd) { img.src = url1; img.show2nd = false; }"
    puts $fd "    else { img.src = url2; img.show2nd = true; }"
    puts $fd "  }"
    puts $fd "  function diffimage_reset(img,url) { img.src = url; img.show2nd = true; }"
    puts $fd "</script>"
    puts $fd ""

    # print log body
    puts $fd "<pre>"
    set logpath [file split [file normalize $file]]
    foreach line $log {
        # put a line; highlight considerable (> ${highlight_percent}%) deviations of CPU and memory
        if { [regexp "\[\\\[](\[0-9.e+-]+)%\[\]]" $line res value] && 
             [expr abs($value)] > ${highlight_percent} } {
            puts $fd "<table><tr><td bgcolor=\"[expr $value > 0 ? \"ff8080\" : \"lightgreen\"]\">$line</td></tr></table>"
        } else {
            puts $fd $line
        }

        # add images
        if { [regexp {IMAGE[ \t]+([^:]+):[ \t]+([A-Za-z0-9_.-]+)} $line res case img] } {
            if { [catch {eval file join "" [lrange $case 0 end-1]} gridpath] } {
                # note: special handler for the case if test grid directoried are compared directly
                set gridpath ""
            }
            set aCaseName [lindex $case end]
            set img1url [_make_url $file [file join $dir1 $gridpath $img]]
            set img2url [_make_url $file [file join $dir2 $gridpath $img]]
            set img1 "<a href=\"[_make_url $file [file join $dir1 $gridpath $aCaseName.html]]\"><img src=\"$img1url\"></a>"
            set img2 "<a href=\"[_make_url $file [file join $dir2 $gridpath $aCaseName.html]]\"><img src=\"$img2url\"></a>"

            set difffile [_diff_img_name $dir1 $dir2 $gridpath $img]
            set imgdurl [_make_url $file $difffile]
            if { [file exists $difffile] } {
                set imgd "<img src=\"$imgdurl\" onmouseout=diffimage_reset(this,\"$imgdurl\") onclick=diffimage_toggle(this,\"$img1url\",\"$img2url\")>"
            } else {
                set imgd "N/A"
            }

            puts $fd "<table><tr><th><abbr title=\"$dir1\">Version A</abbr></th><th><abbr title=\"$dir2\">Version B</abbr></th><th>Diff (click to toggle)</th></tr>"
            puts $fd "<tr><td>$img1</td><td>$img2</td><td>$imgd</td></tr></table>"
        }
    }
    puts $fd "</pre></body></html>"

    close $fd
    return
}

# get number of CPUs on the system
proc _get_nb_cpus {} {
    global tcl_platform env

    if { "$tcl_platform(platform)" == "windows" } {
        # on Windows, take the value of the environment variable 
        if { [info exists env(NUMBER_OF_PROCESSORS)] &&
             ! [catch {expr $env(NUMBER_OF_PROCESSORS) > 0} res] && $res >= 0 } {
            return $env(NUMBER_OF_PROCESSORS)
        }
    } elseif { "$tcl_platform(os)" == "Linux" } {
        # on Linux, take number of logical processors listed in /proc/cpuinfo
        if { [catch {open "/proc/cpuinfo" r} fd] } { 
            return 0 ;# should never happen, but...
        }
        set nb 0
        while { [gets $fd line] >= 0 } {
            if { [regexp {^processor[ \t]*:} $line] } {
                incr nb
            }
        }
        close $fd
        return $nb
    } elseif { "$tcl_platform(os)" == "Darwin" } {
        # on MacOS X, call sysctl command
        if { ! [catch {exec sysctl hw.ncpu} ret] && 
             [regexp {^hw[.]ncpu[ \t]*:[ \t]*([0-9]+)} $ret res nb] } {
            return $nb
        }
    }

    # if cannot get good value, return 0 as default
    return 0
}

# check two files for difference
proc _diff_files {file1 file2} {
    set fd1 [open $file1 "r"]
    set fd2 [open $file2 "r"]

    set differ f
    while {! $differ} {
        set nb1 [gets $fd1 line1]
        set nb2 [gets $fd2 line2]
        if { $nb1 != $nb2 } { set differ t; break }
        if { $nb1 < 0 } { break }
        if { [string compare $line1 $line2] } {
            set differ t
        }
    }

    close $fd1
    close $fd2

    return $differ
}

# Check if file is in DOS encoding.
# This check is done by presence of \r\n combination at the end of the first 
# line (i.e. prior to any other \n symbol).
# Note that presence of non-ascii symbols typically used for recognition
# of binary files is not suitable since some IGES and STEP files contain
# non-ascii symbols.
# Special check is added for PNG files which contain \r\n in the beginning.
proc _check_dos_encoding {file} {
    set fd [open $file rb]
    set isdos f
    if { [gets $fd line] && [regexp {.*\r$} $line] && 
         ! [regexp {^.PNG} $line] } {
        set isdos t
    }
    close $fd
    return $isdos
}

# procedure to recognize format of a data file by its first symbols (for OCCT 
# BREP and geometry DRAW formats, IGES, and STEP) and extension (all others)
proc _check_file_format {file} {
    set fd [open $file rb]
    set line [read $fd 1024]
    close $fd

    set warn f
    set ext [file extension $file]
    set format unknown
    if { [regexp {^DBRep_DrawableShape} $line] } {
        set format BREP
        if { "$ext" != ".brep" && "$ext" != ".rle" && 
             "$ext" != ".draw" && "$ext" != "" } {
            set warn t
        }
    } elseif { [regexp {^DrawTrSurf_} $line] } {
        set format DRAW
        if { "$ext" != ".rle" && 
             "$ext" != ".draw" && "$ext" != "" } {
            set warn t
        }
    } elseif { [regexp {^[ \t]*ISO-10303-21} $line] } {
        set format STEP
        if { "$ext" != ".step" && "$ext" != ".stp" } {
            set warn t
        }
    } elseif { [regexp {^.\{72\}S[0 ]\{6\}1} $line] } {
        set format IGES
        if { "$ext" != ".iges" && "$ext" != ".igs" } {
            set warn t
        }
    } elseif { "$ext" == ".igs" } {
        set format IGES
    } elseif { "$ext" == ".stp" } {
        set format STEP
    } else {
        set format [string toupper [string range $ext 1 end]]
    }
    
    if { $warn } {
        puts "$file: Warning: extension ($ext) does not match format ($format)"
    }

    return $format
}

# procedure to load file knowing its format
proc load_data_file {file format shape} {
    switch $format {
        BREP { uplevel restore $file $shape }
        DRAW { uplevel restore $file $shape }
        IGES { pload XSDRAW; uplevel igesbrep $file $shape * }
        STEP { pload XSDRAW; uplevel stepread $file __a *; uplevel renamevar __a_1 $shape }
        STL  { pload XSDRAW; uplevel readstl $shape $file }
        default { error "Cannot read $format file $file" }
    }
}

# procedure to get name of temporary directory,
# ensuring it is existing and writeable 
proc _get_temp_dir {} {
    global env tcl_platform

    # check typical environment variables 
    foreach var {TempDir Temp Tmp} {
        # check different case
        foreach name [list [string toupper $var] $var [string tolower $var]] {
            if { [info exists env($name)] && [file isdirectory $env($name)] &&
                 [file writable $env($name)] } {
                return [regsub -all {\\} $env($name) /]
            }
        }
    }

    # check platform-specific locations
    set fallback tmp
    if { "$tcl_platform(platform)" == "windows" } {
        set paths "c:/TEMP c:/TMP /TEMP /TMP"
        if { [info exists env(HOMEDRIVE)] && [info exists env(HOMEPATH)] } {
            set fallback [regsub -all {\\} "$env(HOMEDRIVE)$env(HOMEPATH)/tmp" /]
        }
    } else {
        set paths "/tmp /var/tmp /usr/tmp"
        if { [info exists env(HOME)] } {
            set fallback "$env(HOME)/tmp"
        }
    }
    foreach dir $paths {
        if { [file isdirectory $dir] && [file writable $dir] } {
            return $dir
        }
    }

    # fallback case: use subdir /tmp of home or current dir
    file mkdir $fallback
    return $fallback
}

# extract of code from testgrid command used to process jobs running in 
# parallel until number of jobs in the queue becomes equal or less than 
# specified value
proc _testgrid_process_jobs {worker {nb_ok 0}} {
    # bind local vars to variables of the caller procedure
    upvar log log
    upvar logdir logdir
    upvar job_def job_def
    upvar nbpooled nbpooled
    upvar userbreak userbreak
    upvar refresh refresh
    upvar refresh_timer refresh_timer

    catch {tpool::resume $worker}
    while { ! $userbreak && $nbpooled > $nb_ok } {
        foreach job [tpool::wait $worker [array names job_def]] {
            eval _log_test_case \[tpool::get $worker $job\] $job_def($job) log
            unset job_def($job)
            incr nbpooled -1
        }

        # check for user break
        if { "[info commands dbreak]" == "dbreak" && [catch dbreak] } {
            set userbreak 1
        }

        # update summary log with requested period
        if { $logdir != "" && $refresh > 0 && [clock seconds] > $refresh_timer + $refresh } {
            _log_summarize $logdir $log
            set refresh_timer [clock seconds]
        }
    }
    catch {tpool::suspend $worker}
}

help checkcolor {
  Check pixel color.
  Use: checkcolor x y red green blue
  x y - pixel coordinates
  red green blue - expected pixel color (values from 0 to 1)
  Function check color with tolerance (5x5 area)
}
# Procedure to check color using command vreadpixel with tolerance
proc checkcolor { coord_x coord_y rd_get gr_get bl_get } {
    puts "Coordinate x = $coord_x"
    puts "Coordinate y = $coord_y"
    puts "RED color of RGB is $rd_get"
    puts "GREEN color of RGB is $gr_get"
    puts "BLUE color of RGB is $bl_get"

    if { $coord_x <= 1 || $coord_y <= 1 } {
        puts "Error : minimal coordinate is x = 2, y = 2. But we have x = $coord_x y = $coord_y"
        return -1
    }

    set color ""
    catch { [set color "[vreadpixel ${coord_x} ${coord_y} rgb]"] }
    if {"$color" == ""} {
        puts "Error : Pixel coordinates (${position_x}; ${position_y}) are out of view"
    }
    set rd [lindex $color 0]
    set gr [lindex $color 1]
    set bl [lindex $color 2]
    set rd_int [expr int($rd * 1.e+05)]
    set gr_int [expr int($gr * 1.e+05)]
    set bl_int [expr int($bl * 1.e+05)]
    set rd_ch [expr int($rd_get * 1.e+05)]
    set gr_ch [expr int($gr_get * 1.e+05)]
    set bl_ch [expr int($bl_get * 1.e+05)]

    if { $rd_ch != 0 } {
        set tol_rd [expr abs($rd_ch - $rd_int)/$rd_ch]
    } else {
        set tol_rd $rd_int
    }
    if { $gr_ch != 0 } {
        set tol_gr [expr abs($gr_ch - $gr_int)/$gr_ch]
    } else {
        set tol_gr $gr_int
    }
    if { $bl_ch != 0 } {
        set tol_bl [expr abs($bl_ch - $bl_int)/$bl_ch]
    } else {
        set tol_bl $bl_int
    }

    set status 0
    if { $tol_rd > 0.2 } {
        puts "Warning : RED light of additive color model RGB is invalid"
        set status 1
    }
    if { $tol_gr > 0.2 } {
        puts "Warning : GREEN light of additive color model RGB is invalid"
        set status 1
    }
    if { $tol_bl > 0.2 } {
        puts "Warning : BLUE light of additive color model RGB is invalid"
        set status 1
    }

    if { $status != 0 } {
        puts "Warning : Colors of default coordinate are not equal"
    }

    global stat
    if { $tol_rd > 0.2 || $tol_gr > 0.2 || $tol_bl > 0.2 } {
        set info [_checkpoint $coord_x $coord_y $rd_ch $gr_ch $bl_ch]
        set stat [lindex $info end]
        if { ${stat} != 1 } {
            puts "Error : Colors are not equal in default coordinate and in the near coordinates too"
            return $stat
        } else {
            puts "Point with valid color was found"
            return $stat
        }
    } else {
        set stat 1
    }
}

# Procedure to check color in the point near default coordinate
proc _checkpoint {coord_x coord_y rd_ch gr_ch bl_ch} {
    set x_start [expr ${coord_x} - 2]
    set y_start [expr ${coord_y} - 2]
    set mistake 0
    set i 0
    while { $mistake != 1 && $i <= 5 } {
        set j 0
        while { $mistake != 1 && $j <= 5 } {
            set position_x [expr ${x_start} + $j]
            set position_y [expr ${y_start} + $i]
            puts $position_x
            puts $position_y

            set color ""
            catch { [set color "[vreadpixel ${position_x} ${position_y} rgb]"] }
            if {"$color" == ""} {
                puts "Warning : Pixel coordinates (${position_x}; ${position_y}) are out of view"
                incr j
                continue
            }
            set rd [lindex $color 0]
            set gr [lindex $color 1]
            set bl [lindex $color 2]
            set rd_int [expr int($rd * 1.e+05)]
            set gr_int [expr int($gr * 1.e+05)]
            set bl_int [expr int($bl * 1.e+05)]

            if { $rd_ch != 0 } {
                set tol_rd [expr abs($rd_ch - $rd_int)/$rd_ch]
            } else {
                set tol_rd $rd_int
            }
            if { $gr_ch != 0 } {
                set tol_gr [expr abs($gr_ch - $gr_int)/$gr_ch]
            } else {
                set tol_gr $gr_int
            }
            if { $bl_ch != 0 } {
                set tol_bl [expr abs($bl_ch - $bl_int)/$bl_ch]
            } else {
                set tol_bl $bl_int
            }

            if { $tol_rd > 0.2 || $tol_gr > 0.2 || $tol_bl > 0.2 } {
                puts "Warning : Point with true color was not found near default coordinates"
                set mistake 0
            } else {
                set mistake 1
            }
            incr j
        }
        incr i
    }
    return $mistake
}

# Procedure to check if sequence of values in listval follows linear trend
# adding the same delta on each step.
#
# The function does statistical estimation of the mean variation of the
# values of the sequence, and dispersion, and returns true only if both 
# dispersion and deviation of the mean from expected delta are within 
# specified tolerance.
#
# If mean variation differs from expected delta on more than two dispersions,
# the check fails and procedure raises error with specified message.
#
# Otherwise the procedure returns false meaning that more iterations are needed.
# Note that false is returned in any case if length of listval is less than 3.
#
# See example of use to check memory leaks in bugs/caf/bug23489
#
proc checktrend {listval delta tolerance message} {
    set nbval [llength $listval]
    if { $nbval < 3} {
        return 0
    }

    # calculate mean value
    set mean 0.
    set prev [lindex $listval 0]
    foreach val [lrange $listval 1 end] {
        set mean [expr $mean + ($val - $prev)]
        set prev $val
    }
    set mean [expr $mean / ($nbval - 1)]

    # calculate dispersion
    set sigma 0.
    set prev [lindex $listval 0]
    foreach val [lrange $listval 1 end] {
        set d [expr ($val - $prev) - $mean]
        set sigma [expr $sigma + $d * $d]
        set prev $val
    }
    set sigma [expr sqrt ($sigma / ($nbval - 2))]

    puts "Checking trend: nb = $nbval, mean delta = $mean, sigma = $sigma"

    # check if deviation is definitely too big
    if { abs ($mean - $delta) > $tolerance + 2. * $sigma } {
        puts "Checking trend failed: mean delta per step = $mean, sigma = $sigma, expected delta = $delta"
        error "$message"
    }

    # check if deviation is clearly within a range
    return [expr abs ($mean - $delta) <= $sigma && $sigma <= $tolerance]
}
