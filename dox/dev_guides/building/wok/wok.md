Using WOK {#dev_guides__building__wok}
=========

@tableofcontents

\ref dev_guides__wok "WOK" is a legacy build environment for Open CASCADE Technology. 
It is required for generation of header files for classes defined with 
@ref dev_guides__cdl "CDL" ("Cascade Definition Language"). 
Also tools for generation of project files for other build systems, and OCCT 
documentation, are integrated to WOK.

WOK thus is needed in the following situations:
- Building from OCCT sources from Git repository (do not contain generated files)
- Building after some changes made in CDL files

Before installing and using WOK, make sure that you have installed a compiler 
(it is assumed that it is Visual Studio on Windows or gcc on Linux and MacOS) 
and third-party components required for building OCCT.

@section wok1 Installing WOK

  Download the latest version of binary distribution WOK from http://dev.opencascade.org/index.php?q=home/resources

@subsection wok11 Windows

  Run the installer. You will be prompted to read and accept the OCCT Public License to proceed:
  
  @image html /dev_guides/building/wok/images/wok_image001.png
  @image latex /dev_guides/building/wok/images/wok_image001.png
 
  Click Next and proceed with the installation.
  At the end of the installation you will be prompted to specify the version and the location of Visual Studio to be used, and the location of third-party libraries:
  
  @image html /dev_guides/building/wok/images/wok_image002.png
  @image latex /dev_guides/building/wok/images/wok_image002.png
 
  You can change these settings at any time later. For this click on the item "Customize environment (GUI tool)" in the WOK group in the Windows Start menu.
  
  The shortcuts from this group provide two ways to run WOK: 
  * In command prompt window ("WOK TCL shell"). 
  * In Emacs editor ("WOK Emacs"). Using Emacs is convenient if you need to work within WOK environment. 

  By default WOK installer creates a WOK factory with name "LOC" within workshop "dev" (WOK path :LOC:dev). 

@subsection wok12 Linux

  * Unpack the .tgz archive containing WOK distributive into an installation directory \<WOK_INSTALL_DIR\>.

  * Perform the following commands assuming that you have unpacked WOK distributive archive into \<WOK_INSTALL_DIR\>:
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.tcl}
  cd \<WOK_INSTALL_DIR\>/site
  wok_confgui.sh
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  Define all necessary paths to third-party products in the dialog window:
  
  @image html /dev_guides/building/wok/images/wok_image003.png
  @image latex /dev_guides/building/wok/images/wok_image003.png
 
  * Run the following commands to create WOK LOC factory:
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.tcl}
  cd \<WOK_INSTALL_DIR\>/site
  wok_init.sh
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  * Your installation procedure is over. To run WOK use one the following commands:
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.tcl}
  cd \<WOK_INSTALL_DIR\>/site
  wok_emacs.sh
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  or
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.tcl}
  cd \<WOK_INSTALL_DIR\>/site
  wok_tclsh.sh
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection wok13 Mac OS X

  * In the Finder double click on wokSetup.dmg file. This will open a new window. Drag and drop "wokSetup" folder from this window at the location in the Finder where you want to install WOK, i.e. \<WOK_INSTALL_DIR\>.
  
  * Browse in the Finder to the folder \<WOK_INSTALL_DIR\>/site and double click on WokConfig. This will open a window with additional search path settings. Define all necessary paths to third-party products in the dialog window:
  
  @image html /dev_guides/building/wok/images/wok_image004.png
  @image latex /dev_guides/building/wok/images/wok_image004.png
 
  * Run the following commands to create WOK LOC factory:
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.tcl}
  cd \<WOK_INSTALL_DIR\>/site
  wok_init.sh
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  * Your installation procedure is over. To run WOK in Emacs navigate in the Finder to the folder \<WOK_INSTALL_DIR\>/site and double click on WokEmacs.


@section wok2 Initialization of Workbench

  To start working with OCCT, clone the OCCT Git repository from the server (see http://dev.opencascade.org/index.php?q=home/resources for details) or unpack the source archive. 
  
  Then create a WOK workbench (command wcreate) setting its Home to the directory, where the repository is created ($CASROOT variable). The workbench should have the same name as that directory. 
  
  For example, assuming that OCCT repository has been cloned into D:/occt folder: 
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.tcl}
  LOC:dev> wcreate occt -DHome=D:/occt
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  Note: $CASROOT is equal to D:/occt now

  Then you can work with this workbench using normal WOK functionality (wprocess, umake, etc.; see WOK User's Guide for details) or use it only for generation of derived sources and project files, and build OCCT with Visual Studio on Windows or make command on Linux, as described below.
  
@section wok3 Generation of building projects

  Use command wgenproj in WOK to generate derived headers, source and building projects files: 
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.tcl}
  LOC:dev> wokcd occt
  LOC:dev:occt> wgenproj [ -target=<TARGET> ] [ -no_wprocess ]
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TARGET:
  * vc8 - Visual Studio 2005
  * vc9 - Visual Studio 2008
  * vc10 - Visual Studio 2010
  * vc11 - Visual Studio 2012
  * cbp - CodeBlocks
  * cmake - CMake
  * amk - AutoMake
  * xcd - Xcode
-no_wprocess - skip generation of derived headers and source files

Note that this command takes several minutes to complete at the first call. 

Re-execute this step to generate derived headers, source and building projects files if some CDL files in OCCT have been modified (either by you directly, or due to updates in the repository). Note that in some cases WOK may fail to update correctly; in such case remove sub-directories drv and .adm and repeat the command. 

To regenerate derived headers and source files without regeneration of projects use command:
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.tcl}
  LOC:dev> wokcd occt
  LOC:dev:occt> wprocess -DGroups=Src,Xcpp
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The generated building project has been placed into $CASROOT/adm folder:
  * for vc8 - $CASROOT/adm/msvc/vc8
  * for vc9 - $CASROOT/adm/msvc/vc9
  * for vc10 - $CASROOT/adm/msvc/vc10
  * for vc11 - $CASROOT/adm/msvc/vc11
  * for cbp - $CASROOT/adm/\<OS\>/cbp
  * for cmake - $CASROOT/adm/cmake
  * for amk - $CASROOT/adm/lin/amk
  * xcd - $CASROOT/adm/\<OS\>/xcd

@section wok4  Generation of documentation

  Use command wgendoc in WOK to generate reference documentation: 
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.tcl}
  :LOC:dev> wokcd occt
  :LOC:dev:occt> wgendoc 
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The following options can be used: 
  * -wb=< workbench name >  the name of OCCT workbench (the current one by default);
  * -m=< list of modules > the list of modules that will be contained in the documentation;
  * -outdir=< path > the output directory for the documentation;
  * -chm  the option to generate CHM file;
  * -hhc=< path > the path to HTML Help Compiler (hhc.exe) or equivalent;
  * -qthelp=< path > the option to generate Qt Help file, it is necessary to specify the path to qthelpgenerator executable;
  * -doxygen=< path > the path to Doxygen executable
  * -dot=< path > the path to GraphViz dot executable