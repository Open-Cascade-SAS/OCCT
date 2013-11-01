 Developer Guides {#dev_guides}
================

## Source Repository

This directory contains sources of Open CASCADE Technology (OCCT), a collection
of C++ libraries providing services for 3D surface and solid modeling, CAD data
exchange, and visualization. OCCT can be best applied in development of
software dealing with 3D modeling (CAD), manufacturing / measuring (CAM) or
numerical simulation (CAE).

The OCCT code is subject to the Open CASCADE Technology Public License Version
6.6 (the "License"). You may not use the content of the relevant files except in
compliance with the License. Please see the LICENSE file or obtain a copy of the
License at http://www.opencascade.org and read it completely before using this
software.

## Automatic tests

OCCT automatic testing system is integrated with @ref draw "DRAW Test Harness",
a console application based on Tcl (a scripting language).
All tests are run from DRAW command prompt (run **draw.bat** or 
**draw.sh** to start it).

Standard OCCT tests are located in subdirectory **tests** of the OCCT root 
folder. This location is set as default at DRAW start (see environment variable 
_CSF_TestScriptsPath_ defined in **src/DrawResources/DrawDefaults**).

The tests are organized in three levels:
- Group: a group of related test grids, usually testing a particular subset of OCCT functionality (e.g. *blend*). 
- Grid: a set of test cases within a group, usually aimed at testing a particular aspect or mode of execution of the relevant functionality (e.g. *buildevol*).
- Test case: a script implementing an individual test (e.g. *K4*).

To run all tests, type command *testgrid*:

    Draw[]\> testgrid

For running only a group or a grid of tests, give additional arguments indicating the group and (if needed) the grid name:

    Draw[]\> testgrid blend simple

As the tests progress, the result of each test case is reported. 
At the end of the log a summary of test cases is output, including the list of 
detected regressions and improvements, if any.
The tests are considered as non-regressive if only OK, BAD (i.e. known problem), 
and SKIPPED (i.e. not executed, typically because of lack of a data file) 
statuses are reported. 

To run a single test, type command 'test' followed by the names of 
group, grid, and test case. 

    Draw[1]\> test blend simple A1
    CASE blend simple A1: OK

To see intermediate commands and their output during the test execution, 
add one more argument '-echo' at the end of the command line, or type 'dlog get'
after test completion. 

For more information consult \subpage dev_guides__tests

## docs
**short description**

\subpage dev_guides__documentation

## wok
**short description**

\subpage dev_guides__wok

## building
**short description**

\subpage dev_guides__building

## cdl
**short description**

\subpage dev_guides__cdl