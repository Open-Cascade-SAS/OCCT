Building with CMake {#dev_guides__building__cmake}
===================

@tableofcontents

This file describes steps to build OCCT libraries from complete source package
with CMake. CMake is free software that can create GNU Makefiles, KDevelop, 
XCode, and Visual Studio project files. Version 2.6 or above of CMake is 
required.

If you are building OCCT from bare sources (as in Git repository), or do some 
changes affecting CDL files, you need to use WOK to re-generate header files
and build scripts / projects. See \ref dev_guides__building__wok for instructions.

Before building OCCT, you need to install required third-party libraries; see
instructions for your platform in @ref dev_guides__building.

## Decide on location of build and install directories.

The build directory is the one where intermediate files will be created (projects / makefiles, objects, binaries).
The install directory is the one where binaries will be installed after build, 
along with header files and resources required for OCCT use in applications.

OCCT CMake scripts assume use of separate build and one install directories 
for each configuration (Debug or Release).

It is recommended to separate build and install directories from OCCT source directory, for example:

       /user/home/occt/ - sources
       /user/home/tmp/occt-build-release - intermediate files (release)
       /user/home/occt-install-release - installed binaries (release)

## CMake usage

Run CMake indicating path to OCCT sources ($CASROOT; in previous example 
CASROOT equal to /user/home/occt in lin case, and d:/occt in windows case) 
and selected build directory (in prev example build directory is 
/user/home/tmp/occt-build-release). 

It is recommended to use GUI tools provided by CMake: cmake-gui on Windows 
and Mac, ccmake on Linux.

### Windows:

@image html /dev_guides/building/cmake/images/cmake_image001.png
@image latex /dev_guides/building/cmake/images/cmake_image001.png

* Specify "main" CMakelists.txt meta-project location by clicking Browse Source (e.g., $CASROOT)
* Specify location (build folder) for Cmake generated project files by clicking Browse Build (e.g., d:/occt/build/win32-vc9-debug) (each cmake configuration of the project uses a specific build directory and a specific directory for installed files. It is recommended to compose names of the binary and install directory from system, bitness, compiler and build type.)
* Configure opens the window with a drop-down list of generators supported by CMake project. Select the required generator (e.g., Visual Studio 2008) and click Finish)

@image html /dev_guides/building/cmake/images/cmake_image002.png
@image latex /dev_guides/building/cmake/images/cmake_image002.png

### Linux:

In the console, change to the build directory and call ccmake with the path to the source directory of the project:

       > cd ~/occt/build/debug
       > ccmake ~/occt

@image html /dev_guides/building/cmake/images/cmake_image003.png
@image latex /dev_guides/building/cmake/images/cmake_image003.png

Press "c" to configure.

### Mac OS:

Use cmake-gui (Applications -> CMake 2.8-10.app) to generate project files for the chosen build environment (e.g., XCode).

@image html /dev_guides/building/cmake/images/cmake_image004.png
@image latex /dev_guides/building/cmake/images/cmake_image004.png

## OCCT Configuration

The error message which appears at the end of configuration process, informs you about the required variables 
which need to be defined. This error will appear until all required variables are defined correctly.
Note: In cmake-gui there is "grouped" option, which groups variables with a common prefix.

### Selection of components to be built

The variables with "BUILD_" prefix allow specifying OCCT components and
configuration to be built:

* BUILD_CONFIGURATION - defines configuration to be built (Release by default).
* BUILD_<MODULE> - specify whether corresponding OCCT module should be 
                   built (all toolkits). Note that even if whole module is not 
                   selected for build, its toolkits used by other toolkits 
                   selected for build will be included automatically.
* BUILD_TOOLKITS - allows including additional toolkits from non-selected 
                   modules (should be list of toolkit names separated by a 
                   space or a semicolon).
* BUILD_SAMPLES - specify whether OCCT MFC samples should be built.

Check variables with "USE_" prefix (USE_FREEIMAGE, USE_GL2PS, USE_TBB, and 
USE_OPENCL) if you want to enable use of the corresponding optional 3rd-party 
library.

### 3rd-party configuration

### 3rd-party configuration (The variables with 3RDPARTY_ prefix)

If you have 3rd-party libraries in a non-default location 
(e.g., on Windows, binaries downloaded from "http://www.opencascade.org/getocc/download/3rdparty/"), 
specify 3RDPARTY_DIR variable that points to the folders of 3rdparty products (some or all). 
At the next configuration 3rd-party product paths stored in 3RDPARTY_\<PRODUCT\>_DIR variable 
will be searched for in 3RDPARTY_DIR directory. If the structure of 3RDPARTY_DIR directory 
is the same as adopted in the OCCT, the directory will contain product dir, lib and header files. 

Press "Configure" ("c" key for ccmake).

The result of the 3rdparty product search will be recorded in the corresponding variables:

* 3RDPARTY_\<PRODUCT\>_DIR - path to the product directory (with directory name) (e.g., D:/3rdparty/Tcl-8.5.12.0-32)
* 3RDPARTY_\<PRODUCT\>_LIBRARY - path to the .lib libraries (with the library name) (e.g., D:/3rdparty/Tcl-8.5.12.0-32/lib/tcl85.lib). In non-windows case, this variable is the same as 3RDPARTY_\<PRODUCT\>_DLL.
* 3RDPARTY_\<PRODUCT\>_INCLUDE - path to the include directory that contains the required header file (with "include" name) (e.g., D:/3rdparty/Tcl-8.5.12.0-32/include including tcl.h)
* 3RDPARTY_\<PRODUCT\>_DLL - path to the .dll/.so/.dylib library  (with the library name) (e.g., D:/3rdparty/Tcl-8.5.12.0-32/bin/tcl85.dll)

The search process is as follows:

1. Common path: 3RDPARTY_DIR
2. Path to particular 3rd-party library: 3RDPARTY_\<PRODUCT\>_DIR
3. Paths to headers and binaries:
   1. 3RDPARTY_\<PRODUCT\>_INCLUDE
   2. 3RDPARTY_\<PRODUCT\>_LIBRARY
   3. 3RDPARTY_\<PRODUCT\>_DLL

If a variable of any level is not defined (empty or \<variable name\>-NOTFOUND) 
and the upper level variable is defined, the content of the non-defined variable 
will be searched for at the next configuration step. If search process in level 3 
does not find the required files, it searches in default places also.

**Note**: the names of searched libraries and header files are hardcoded.
Freetype search process tries to find ft2build.h file in 3RDPARTY_FREETYPE INCLUDE dir 
and after that adds "3RDPARTY_FREETYPE_INCLUDE /freetype2" path to common includes if it exists. 

Important: If BUILD_CONFIGURATION variable is changed - at the next configuration 
3RDPARTY_ variables will be replaced by the search process result, except for the 3RDPARTY_DIR variable.

*Note*: CMake will produce an error after the configuration step until all required variables are defined correctly.
If the search result (include path, or library path, or dll path) does not meet your expectations - 
you can  change 3RDPARTY_\<PRODUCT\>_DIR variable, clear (if they are not empty) 
3RDPARTY_\<PRODUCT\>_DLL, 3RDPARTY_\<PRODUCT\>_INCLUDE_DIR and 3RDPARTY_\<PRODUCT\>_LIBRARY variables 
(or clear one of them) and run the configuration process again. 

At this time the search will be performed in the new identified directory 
and the result will be recorded to empty variables (non-empty variables will not be replaced).

For example, (Linux case) 3RDPARTY_FREETYPE_DIR variable 

    /PRODUCTS/maintenance/Mandriva2010/freetype-2.3.7

can be changed to 

    /PRODUCTS/maintenance/Mandriva2010/freetype-2.4.10

and the related variables: 3RDPARTY_FREETYPE_DLL, 3RDPARTY_FREETYPE_INCLUDE_DIR and  3RDPARTY_FREETYPE_LIBRARY will be cleared.

@image html /dev_guides/building/cmake/images/cmake_image005.png
@image latex /dev_guides/building/cmake/images/cmake_image005.png

During configuration process the cleaned variables will be filled with new found values.

###The variables with INSTALL_ prefix:

Define in INSTALL_DIR variable the path where will be placed built OCCT files (libraries, executables and headers).
If INSTALL_\<PRODUCT\> variable is checked - 3rd-party products will be copied to the install directory.

#### At the end of the configuration process "configuring done" message will be shown and the generation process can be started.

## OCCT Generation

This will create makefiles or project files for your build system.

### Windows

Click Generate button and wait until the generation process is finished. 
Then the project files will appear in the build folder (e.g., d:/occt/build/win32-vc9-release). 

### Linux

When the configuration is complete, start the generation process by pressing "g".

@image html /dev_guides/building/cmake/images/cmake_image006.png
@image latex /dev_guides/building/cmake/images/cmake_image006.png

### Mac OS X

Click Generate button and wait until the generation process is finished. 
Then the project files will appear in the build folder (e.g., /Developer/occt/build/XCode).

## OCCT Building

The install folder contains bin, inc, lib and res folders and a script to run DRAWEXE (draw.bat or draw.sh).
"bin" contains executables, DLL (Windows) style shared libraries and pdb-files in OCCT debug version,.
"lib" contains the import parts of DLL libraries.
"inc" contains header files.
"res" contains all required source files for OCCT.

### Windows (Visual studio)

Go to the build folder, start the Visual Studio solution (OCCT.sln) and build it by clicking Build - Build Solution.
When the building process finished, build the INSTALL project 
(by default the build solution process skips the building of the INSTALL project) to move the above files to INSTALL_DIR. 
For this in the solution explorer right click on the INSTALL project and select Project Only - Build Only INSTALL. 

### Linux (make)
Change directory to binary dir and run make command

       > make 

To copy all libraries, executables and chosen 3rd-party libraries run "make" command with "install" argument

       > make install

This command will move the above files to INSTALL_DIR.

### Mac OS X (XCode)

Go to the build folder, start the XCode solution (OCCT.xcodeproj) 
and build it by clicking Build -> Build. 
Please notice that XCode may have worst responsibility to user actions 
due to sources processing at first start.

When the building process finished, build the INSTALL project 
(by default the build solution process skips the building of the INSTALL project) 
to move the above files to INSTALL_DIR. 
Notice that env.sh (configure PATH and DYLD_LIBRARY_PATH environment variables 
as well as Draw Harness extra variables) and draw.sh (to launch DRAWEXE) will be created in target directory. 

## OCCT project debugging for Visual Studio
Run OCCT.bat from the build directory to start Visual Studio with required environment for debugging.