 Building 3rd-party libraries on Windows {#occt_dev_guides__building_3rdparty_windows}
==============================================
@tableofcontents

@section dev_guides__building_3rdparty_win_1 Introduction

This document presents guidelines for building third-party products used by Open CASCADE Technology (OCCT) and samples on Windows platform. It is assumed that you are already familiar with MS Visual Studio / Visual C++. 

You need to use the same version of MS Visual Studio for building all third-party products and OCCT itself, in order to receive a consistent set of run-time binaries. 

The links for downloading the third-party products are available on the web site of OPEN CASCADE SAS at http://www.opencascade.com/content/3rd-party-components.

There are two types of third-party products used  by OCCT: 

* Mandatory  products: 
	* Tcl/Tk 8.5 -- 8.6;
	* FreeType 2.4.10 -- 2.5.3.
* Optional  products:
	* TBB 3.x -- 4.x;
	* gl2ps 1.3.5 -- 1.3.8;
	* FreeImage 3.14.1 -- 3.16.0;
	* VTK 6.1.0.

It is recommended to create a separate new folder on your workstation, where you will unpack the downloaded archives of the third-party  products, and where you will build these products (for example, *c:\\occ3rdparty*). 

Further in this document, this folder is referred to as *3rdparty*. 

@section dev_guides__building_3rdparty_win_2 Building Mandatory Third-party Products

@subsection dev_guides__building_3rdparty_win_2_1 Tcl/Tk

Tcl/Tk is required for DRAW test harness.

@subsubsection dev_guides__building_3rdparty_win_2_1_1 Installation from sources: Tcl
  
Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 
  
1. In the *win* sub-directory, edit file *buildall.vc.bat*:

   * Edit the line "call ... vcvars32.bat" to have correct path to the version of Visual Studio to be used for building, for instance:

         call "%VS80COMNTOOLS%\vsvars32.bat"

     If you are building 64-bit version, set environment accordingly, e.g.:

         call "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64
     
   * Define variable *INSTALLDIR* pointing to directory where Tcl/Tk will be installed, e.g.:

         set INSTALLDIR=D:\OCCT\3rdparty\tcltk-86-32

   * Add option *install* to the first command line calling *nmake*:

         nmake -nologo -f makefile.vc release htmlhelp install %1

   * Remove second call to *nmake* (building statically linked executable)

2. Edit file *rules.vc* replacing line 

       SUFX	    = tsgx

   by

       SUFX	    = sgx

   This is to avoid extra prefix 't' in the library name, which is not recognized by default by OCCT build tools.


3. By default, Tcl uses dynamic version of run-time library (MSVCRT), which must be installed on the system where Tcl will be used.
   You may wish to link Tcl library with static version of run-time to avoid this dependency.
   For that:

   * Edit file *makefile.vc* replacing strings "crt = -MD" by "crt = -MT"

   * Edit source file *tclMain.c* (located in folder *generic*) commenting out forward declaration of function *isatty()*.


4. In the command prompt, run *buildall.vc.bat*

   You might need to run this script twice to have *tclsh* executable installed; check subfolder *bin* of specified installation path to verify this.

5. For convenience of use, we recommend making a copy of *tclsh* executable created in subfolder *bin* of *INSTALLDIR* and named with Tcl version number suffix, as *tclsh.exe* (with no suffix)

       > cd D:\OCCT\3rdparty\tcltk-86-32\bin
       > cp tclsh86.exe tclsh.exe

@subsubsection dev_guides__building_3rdparty_win_2_1_2 Installation from sources: Tk
  
Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 

Apply the same steps as described for building Tcl above, with the same INSTALLDIR.
Note that Tk produces its own executable, called *wish*. 

You might need to edit default value of *TCLDIR* variable defined in *buildall.vc.bat* (should be not necessary if you unpack both Tcl and Tk sources in the same folder).

@subsection dev_guides__building_3rdparty_win_2_2 FreeType

FreeType is required for text display in a 3D viewer. You can download its sources from http://sourceforge.net/projects/freetype/files/   

### The building procedure

1. Unpack the downloaded archive of FreeType product into the *3rdparty*  folder. As a result, you will get a folder named, for example, *3rdparty\\freetype-2.4.10*.  Further in this document, this folder is referred to as *freetype*. 
   
2. Open the solution file *freetype\\builds\\win32\\vc20xx\\freetype.sln*  in Visual Studio. Here *vc20xx* stands for your version of Visual Studio. 

3. Select the configuration to build: either Debug or Release. 

4. Build  the *freetype* project. 

   As a result, you will get a  freetype import library (.lib) in the *freetype\\obj\\win32\\vc20xx*  folder.  


5. If you build FreeType for a 64 bit platform, select in the main menu **Build - Configuration Manager**  and add *x64* platform to the solution configuration by copying the settings from Win32 platform: 

   @figure{/dev_guides/building/3rdparty/images/3rdparty_image001.png}

   Update the value of the Output File for  x64 configuration: 

   @figure{/dev_guides/building/3rdparty/images/3rdparty_image003.png}

   Build the *freetype* project. 

   As a result, you will obtain a 64 bit import library (.lib) file in the *freetype\\x64\\vc20xx*  folder. 

   To build FreeType as a dynamic library (.dll) follow steps 6, 7 and 8 of this procedure. 

6. Open menu Project-> Properties-> Configuration  Properties-> General and change option **Configuration Type** to *Dynamic  Library (.dll)*. 
7. Edit file *freetype\\include\\freetype\\config\\ftoption.h*:  
  
   in line 255, uncomment the definition of macro *FT_EXPORT* and change it as follows: 

       #define FT_EXPORT(x)   __declspec(dllexport) x 

8. Build  the *freetype* project. 

   As a result, you will obtain the files of the import  library (.lib) and the dynamic library (.dll)   in folders <i>freetype  \\objs\\release</i> or <i>\\objs\\debug </i>. 
   
   If you build for a 64 bit platform, follow step 5 of the procedure. 

   To facilitate the use of FreeType libraries in OCCT with minimal adjustment of build procedures, it is recommended to copy the include files and libraries of FreeType into a separate folder, named according to the pattern: *freetype-compiler-bitness-building mode*, where: 
   * **compiler** is *vc8* or *vc9* or *vc10* or *vc11*; 
   * **bitness**  is *32* or *64*;
   * **building mode** is *opt* (for Release) or *deb* (for Debug). 
    
   The *include* subfolder should be copied as is, while libraries should be renamed to *freetype.lib* and *freetype.dll* (suffixes removed) and placed to subdirectories *lib *and  *bin*, respectively. If the Debug configuration is built, the Debug libraries should be put into subdirectories *libd* and *bind*. 

@section dev_guides__building_3rdparty_win_3 Building Optional Third-party Products

@subsection dev_guides__building_3rdparty_win_3_1 TBB

This third-party product is installed with binaries 
from the archive that can be downloaded from http://threadingbuildingblocks.org/. 
Go to the **Download** page, find the  release version you need  (e.g. *tbb30_018oss*) and pick the archive for Windows  platform. 

Unpack the downloaded  archive of TBB product into the *3rdparty* folder. 

Further in this document,  this folder is referred to as *tbb*. 

@subsection dev_guides__building_3rdparty_win_3_2  gl2ps

This third-party product should be built as a dynamically loadable library (dll file). 
You can download its sources from  http://geuz.org/gl2ps/src/. 

### The building procedure

1. Unpack the downloaded  archive of gl2ps product (e.g. *gl2ps-1.3.5.tgz*) into the *3rdparty*  folder. 

   As a result, you will get a folder named, for example, *3rdparty\\gl2ps-1.3.5-source*. 

   Rename it into <i>gl2ps-platform-compiler-building mode</i>, where 
   * **platform** -- *win32*  or *win64*; 
   * **compiler** -- *vc8*, *vc9* or *vc10*; 
   * **building mode** -- *opt*  (for release) or *deb* (for debug). 
   
   For example, <i>gl2ps-win64-vc10-deb</i>

   Further in this document,  this folder is referred to as *gl2ps*. 

2. Download (from http://www.cmake.org/cmake/resources/software.html) 
   and install the *CMake* build system.  

3. Edit the file *gl2ps\\CMakeLists.txt*. 

   After line 113 in *CMakeLists.txt*: 

       set_target_properties(shared PROPERTIES  COMPILE_FLAGS \"-DGL2PSDLL -DGL2PSDLL_EXPORTS\")
    
   add the following line:   
    
       add_definitions(-D_USE_MATH_DEFINES)  

   Attention: If Cygwin was installed on  your computer, make sure that there is no path to it in the *PATH* variable to avoid possible conflicts during the configuration. 

4. Launch CMake <i>(cmake-gui.exe)</i> using  the Program menu. 

   In CMake: 
  
   * Define where the source code is. 
     This path must point to *gl2ps*  folder. 

   * Define where to build the  binaries. 
     This path must point to the folder where generated gl2ps project binaries will be placed 
     (for example, *gl2ps\\bin*). 
     Further in this document, this folder is referred to as *gl2ps_bin*.

   * Press  **Configure** button. 
   
     @figure{/dev_guides/building/3rdparty/images/3rdparty_image004.png}

   * Select the generator (the compiler  and the target platform -- 32 or 64 bit) in the pop-up window. 
   
     @figure{/dev_guides/building/3rdparty/images/3rdparty_image005.png}

   * Press **Finish** button to  return to the main CMake window. 
     Expand the ENABLE group and uncheck  ENABLE_PNG and ENABLE_ZLIB check boxes. 
	 
     @figure{/dev_guides/building/3rdparty/images/3rdparty_image006.png}

   * Expand the CMAKE group and define *CMAKE_INSTALL_PREFIX* which is the path where you want to install the build results, for example, *c:\\occ3rdparty\\gl2ps-1.3.5*. 
   
     @figure{/dev_guides/building/3rdparty/images/3rdparty_image007.png}

   * Press **Configure** button again, then press **Generate** button to generate Visual Studio projects. After completion, close CMake application. 

5. Open the solution file *gl2ps_bin\\gl2ps.sln* in Visual Studio. 

   * Select a configuration to build 
     * Choose **Release** to build Release binaries. 
     * Choose **Debug** to build Debug binaries. 
   * Select a platform to build. 
     * Choose **Win32** to build for a 32 bit platform. 
     * Choose **x64** to build for a 64 bit platform. 
   * Build the solution. 
   * Build the *INSTALL* project. 
    
As a  result, you should have the installed gl2ps product in the *CMAKE_INSTALL_PREFIX* path. 

@subsection dev_guides__building_3rdparty_win_3_3 FreeImage

This third-party product should be built as a dynamically loadable library (.dll file). 
You can download its sources from 
http://sourceforge.net/projects/freeimage/files/Source%20Distribution/

### The building procedure: 

1. Unpack the  downloaded archive of FreeImage product into *3rdparty* folder. 
  
   As a result, you should have a folder named *3rdparty\\FreeImage*. 
  
   Rename it according to the rule: *freeimage-platform-compiler-building  mode*, where 
   
   * **platform**  is *win32* or *win64*; 
   * **compiler**  is *vc8* or *vc9* or *vc10* or *vc11*; 
   * **building  mode** is *opt* (for release) or *deb* (for debug) 

   Further in this  document, this folder is referred to as *freeimage*. 

2. Open the solution file *freeimage\\FreeImage.*.sln* in your Visual Studio. 
  
   If you use a Visual Studio version higher than VC++ 2008, apply conversion of the workspace. 
   Such conversion should be suggested automatically by Visual  Studio. 
    
3. Select a  configuration to build. 

   - Choose **Release**  if you are building Release binaries. 
   - Choose **Debug** if you are building  Debug binaries. 

   *Note:* 

   If you want to  build a debug version of FreeImage binaries then you need to rename the following files in FreeImage and FreeimagePlus projects:
   
   Project -> Properties -> Configuration Properties -> Linker -> General -> Output File

       FreeImage*d*.dll  to FreeImage.dll 
       FreeImagePlus*d*.dll to FreeImagePlus.dll 

   Project -> Properties -> Configuration Properties -> Linker -> Debugging-> Generate Program Database File

       FreeImage*d*.pdb  to FreeImage.pdb 
       FreeImagePlus*d*.pdb to FreeImagePlus.pdb 

   Project -> Properties -> Configuration Properties -> Linker -> Advanced-Import Library

       FreeImage*d*.lib  to FreeImage.lib 
       FreeImagePlus*d*.lib  to FreeImagePlus.lib 

   Project -> Properties -> Configuration Properties -> Build Events -> Post -> Build Event -> Command Line 

       FreeImage*d*.dll     to FreeImage.dll 
       FreeImage*d*.lib     to FreeImage.lib 
       FreeImagePlus*d*.dll to FreeImagePlus.dll 
       FreeImagePlus*d*.lib to FreeImagePlus.lib 

   Additionally, rename in project FreeImagePlus 
   
   Project -> Properties -> Configuration  Properties -> Linker -> Input -> Additional Dependencies 

       from FreeImage*d*.lib to FreeImage.lib 

4. Select a platform to build. 

   - Choose *Win32* if you are building for a 32 bit platform. 
   - Choose *x64* if you are building for a 64 bit platform. 

5. Start the building process. 

   As a result, you should have the  library files of FreeImage product  in *freeimage\\Dist* folder (*FreeImage.dll* and *FreeImage.lib*) and in *freeimage\\Wrapper\\FreeImagePlus\\dist* folder (*FreeImagePlus.dll* and *FreeImagePlus.lib*).

@subsection dev_guides__building_3rdparty_win_3_4 VTK

VTK is an open-source, freely available software system for 3D computer graphics, image processing and visualization. VTK Integration Services component provides adaptation functionality for visualization of OCCT topological shapes by means of VTK library.

### The building procedure:

1. Download the necessary archive from http://www.vtk.org/VTK/resources/software.html and unpack it into *3rdparty* folder.

   As a result, you will get a folder named, for example, <i>3rdparty\VTK-6.1.0.</i>

   Further in this document, this folder is referred to as *VTK*.

2. Use CMake to generate VS projects for building the library:
   - Start CMake-GUI and select VTK folder as source path, and the folder of your choice for VS project and intermediate build data.
   - Click **Configure**.
   - Select the VS version to be used from the ones you have installed (we recommend using VS 2010) and the architecture (32 or 64-bit).
   - Generate VS projects with default CMake options. The open solution *VTK.sln* will be generated in the build folder.

3. Build project VTK in Release mode.
