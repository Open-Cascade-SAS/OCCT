Building with MS Visual C++ {#occt_dev_guides__building_msvc}
===========================

This file describes steps to build OCCT libraries from a complete source
archive on Windows with <b>MS Visual C++</b>. 

If you build OCCT from bare sources (as in Git repository) or do some 
changes affecting CDL files, you need to use WOK to re-generate header files
and build scripts / projects. See \ref occt_dev_guides__building_wok for instructions.

Before building OCCT, you need to install the required third-party libraries; see
paragraph 1 of \ref occt_dev_guides__building for instructions.

1. Edit file *custom.bat* to define the environment: 

   - *VCVER* - version of Visual Studio (vc8, vc9, vc10, vc11 or vc12), 
             and relevant *VCVARS* path
   - *ARCH* - architecture (32 or 64), affects only *PATH* variable for execution
   - <i>HAVE_*</i> - flags to enable or disable use of optional third-party products
   - CSF_OPT_* - paths to search for includes and binaries of all used 
                 third-party products

2. Launch *msvc.bat* to start Visual Studio with all necessary environment 
   variables defined.

   Note: the MSVC project files are located in folders <i>adm\\msvc\\vc[9-12]</i>.
   Binaries are produced in *win32* or *win64* folders.

3. Build with Visual Studio

   Note: If VTK was not installed on you computer and you are not interested in usage of 
            OCCT VTK Integration Services (VIS) component you should exclude TKIVtk and TKIVtkDraw 
            projects from process of compilation in the main menu  <b>Build / Configuration Manager</b>. 

To start DRAW, launch *draw.bat*.
