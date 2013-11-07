 Building 3rd-party libraries on Windows {#dev_guides__building_3rdparty_windows}
==============================================
@tableofcontents

@section dev_guides__building_3rdparty_win_1 Introduction

  This document presents guidelines for building third-party products 
  used by Open CASCADE Technology (OCCT) and samples on Windows platform. 

  In order to understand these guidelines, you need to be  familiar with MS Visual Studio / Visual C++. 

  You need to use the same version of MS Visual Studio for building 
  all third-party products and OCCT itself, in order to receive a consistent set of run-time binaries. 

  The links for downloading the third-party products are available on the web site 
  of OPEN CASCADE SAS at http://www.opencascade.org/getocc/require/. 
  There are two types of third-party products which are used  by OCCT: 

  * Mandatory  products: Tcl 8.5, Tk 8.5 and  FreeType 2.4.10 
  * Optional  products: TBB 3.x or 4.x, gl2ps * 1.3.5, FreeImage 3.14.1 

  It is recommended to create a separate new folder on your workstation where 
  you will unpack the downloaded archives of the third-party  products, 
  and where you will build these products (for example, *c:\\occ3rdparty*). 

  Further in this document, this folder is referred to as *3rdparty*. 

@section dev_guides__building_3rdparty_win_2 Building Mandatory Third-party Products

@subsection dev_guides__building_3rdparty_win_2_1 Tcl/Tk 8.5

  Tcl/Tk is required for DRAW test harness. Version 8.5 or  * 8.6 can be used for OCCT. 
  We recommend installing a binary distribution that could be downloaded from 
  http://www.activestate.com/activetcl.

  Go to \"Free Downloads\" and pick the version of the Install Wizard 
  that matches your target platform – 32 bit (x86) or 64 bit (x64). 
  The version of Visual Studio you use is irrelevant when choosing the Install Wizard. 

  Run the Install Wizard you  downloaded, and install Tcl/Tk products 
  to 3rdparty\\tcltk-win32 folder (for 32-bit platform) or 
  to 3rdparty\\tcltk-win64 folder (for 64-bit platform). 

  Further in this document,  this folder is referred to as *tcltk*. 

@subsection dev_guides__building_3rdparty_win_2_2 FreeType 2.4.10

  FreeType is required for display of text in 3D viewer. 
  You can download its sources from http://sourceforge.net/projects/freetype/files/   

  The building process is the following:

    * 1. Unpack the  downloaded archive of FreeType 2.4.10 product into the *3rdparty*  folder. 

      As a result, you should have a folder named *3rdparty\\freetype-2.4.10*.  Further in this document, this folder is referred to as *freetype*. 

    * 2. Open the  solution file *freetype\\builds\\win32\\vc20xx\\freetype.sln*  in Visual Studio, where vc20xx stands for the version of Visual Studio you are  using. 
    * 3. Select a  configuration to build: either Debug or Release. 
    * 4. Build  the *freetype* project. 

      As a result, you will get a  freetype import library (.lib) in the *freetype\\obj\\win32\\vc20xx*  folder. 

    * 5. If you are  building for 64 bit platform, start the Configuration Manager (Build - Configuration Manager), 
    and add *x64* platform to the solution configuration by copying the  settings from Win32 platform: 

@image html /dev_guides/building/3rdparty/images/3rdparty_image001.png 
@image latex /dev_guides/building/3rdparty/images/3rdparty_image001.png 

  Update the value of the Output File for  x64 configuration: 

@image html /dev_guides/building/3rdparty/images/3rdparty_image003.png 
@image latex /dev_guides/building/3rdparty/images/3rdparty_image003.png 

  Build the *freetype* project. 

   As a result, you should obtain a 64  bit import library (.lib) file in the *freetype\\x64\\vc20xx*  folder. 
   If you want to build freetype as an import library (.lib) and a dynamic library (.dll) you should follow items 6, 7 and 8 of this list. 

  * 6.  Open Project-Properties-Configuration  Properties-General and change option 'Configuration Type' to \"*Dynamic  Library (.dll)*\". 
  * 7.  Edit file *freetype\\include\\freetype\\config\\ftoption.h*:  
  
    in line 255, uncomment the definition of macro FT_EXPORT and change it as follows: 

  @verbatim
    #define FT_EXPORT(x)   __declspec(dllexport) x 
  @endverbatim

  * 8. Build  the *freetype* project. 

    As a result, you should obtain import  library (.lib) and dynamic library (.dll) 
    files in *freetype  \\objs\\release or \\objs\\debug folders.* 
    If you are building for a 64 bit  platform, follow item 5 of this list. 

    In order to facilitate use of the  FreeType libraries in OCCT with minimal adjustment of its build procedures, 
    it is recommended to copy the include files and libraries of FreeType to a separate folder, named according to the pattern: 
    *freetype-compiler-bitness-building  mode*  
    where 
    
    * compiler  is vc8 or vc9 or vc10 or vc11; 
    * bitness  is 32 or 64; 
    * building  mode is opt (for Release) or deb (for Debug) 
    
    The include subfolder should be copied as is, while libraries should be renamed to 
    *freetype.lib* and *freetype.dll* (suffixes removed) and placed to subdirectories 
    *lib *and  *bin*, respectively. If Debug configuration is built, 
    the Debug libraries should be put in subdirectories *libd* and *bind*. 

@section dev_guides__building_3rdparty_win_3 Building Optional Third-party Products

@subsection dev_guides__building_3rdparty_win_3_1 TBB 3.x or 4.x

  This third-party product is  installed with binaries 
  from the archive that can be downloaded from http://threadingbuildingblocks.org/. 
  Go to \"Downloads page\", find the  release version you need (e.g. tbb30_018oss) and pick the archive for Windows  platform. 
  Unpack the downloaded  archive of TBB product into the *3rdparty* folder. 
  Further in this document,  this folder is referred to as *tbb*. 

@subsection dev_guides__building_3rdparty_win_3_2  gl2ps * 1.3.5

  This third-party product should be built as a dynamically loadable library (dll file). 
  You can download its sources from  http://geuz.org/gl2ps/src/ 

  The building process is the following: 

  * 1. Unpack the downloaded  archive of gl2ps * 1.3.5 product (*gl2ps-* 1.3.5.tgz*) into the *3rdparty*  folder. 
    As a result, you should  have a folder named *3rdparty\\gl2ps-* 1.3.5-source*. 
    Rename it according to the  rule: gl2ps-platform-compiler-building mode, where 
    platform is win32  or win64; 
    compiler is vc8 or  vc9 or vc10; 
    building mode - opt  (for release) or deb (for debug) 
    Further in this document,  this folder is referred to as *gl2ps*. 

  * 2. Download (from http://www.cmake.org/cmake/resources/software.html) 
    and install the *CMake* build system.  

  * 3. Edit the file *gl2ps\\CMakeLists.txt*. 
   After line 113 in CMakeLists.txt: 

  @verbatim
    set_target_properties(shared PROPERTIES  COMPILE_FLAGS \"-DGL2PSDLL -DGL2PSDLL_EXPORTS\")
  @endverbatim
    
   add the following line:   
    
  @verbatim
    add_definitions(-D_USE_MATH_DEFINES)  
  @endverbatim

  Attention: If cygwin was installed on  your computer make sure that there is no path 
  to the latter in the PATH variable in order to avoid possible conflicts during  the configuration. 

  * 4. Launch CMake (cmake-gui.exe) using  the Program menu. 
  In CMake: 
  
     * Define where the source code is. 
       This path must point to *gl2ps*  folder. 
     * Define where to build the  binaries. 
       This path must point to the folder where generated gl2ps project binaries will be placed 
       (for example, *gl2ps\\bin*). 
       Further in this document, this folder is referred to as *gl2ps_bin*.
     * Press the \"Configure\" button. 
@image html /dev_guides/building/3rdparty/images/3rdparty_image004.png 
@image latex /dev_guides/building/3rdparty/images/3rdparty_image004.png 
     * Select the generator (the compiler  and the target platform - 32 or 64 bit) in the pop-up window. 
@image html /dev_guides/building/3rdparty/images/3rdparty_image005.png 
@image latex /dev_guides/building/3rdparty/images/3rdparty_image005.png 
     * Then press the \"Finish\" button to  return to the main CMake window. 
       Expand the ENABLE group and uncheck  ENABLE_PNG and ENABLE_ZLIB check boxes. 
@image html /dev_guides/building/3rdparty/images/3rdparty_image006.png 
@image latex /dev_guides/building/3rdparty/images/3rdparty_image006.png 
     * Expand the CMAKE group and define CMAKE_INSTALL_PREFIX 
       (path where you want to install the build results, for example, *c:\\occ3rdparty\\gl2ps\-1.3.5*). 
@image html /dev_guides/building/3rdparty/images/3rdparty_image007.png 
@image latex /dev_guides/building/3rdparty/images/3rdparty_image007.png 
     * Press the  \"Configure\" button again, and then the \"Generate\" button in order to generate 
       Visual Studio projects. After completion, close CMake application. 

  * 5. Open the solution file *gl2ps_bin\\gl2ps.sln* in Visual Studio. 
    * Select a  configuration to build 
        * Choose \"*Release*\" if you are building Release binaries. 
        * Choose \"*Debug*\" if you are building Debug binaries. 
    * Select  a platform to build. 
        * Choose \"*Win32*\" if you are building for a 32 bit  platform. 
        * Choose \"*x64*\" if you are building for a 64 bit  platform. 
    * Build the solution. 
    * Build the *INSTALL* project. 
    
    As a  result, you should have the installed gl2ps product in the *CMAKE_INSTALL_PREFIX*  path. 

@subsection dev_guides__building_3rdparty_win_3_3 FreeImage 3.14.1

  This third-party product should be built as a dynamically loadable library (.dll file). 
  You can download its sources from 
  http://sourceforge.net/projects/freeimage/files/Source%20Distribution/

  The building process is the following: 

  * 1. Unpack the  downloaded archive of FreeImage 3.14.1 product (*FreeImage314* 1.zip*) into *3rdparty* folder. 
  
    As a result,  you should have a folder named *3rdparty\\FreeImage*. 
    Rename it  according to the rule: freeimage-platform-compiler-building  mode, where 
    platform  is win32 or win64; 
    compiler  is vc8 or vc9 or vc10 or vc11; 
    building  mode is opt (for release) or deb (for debug) 
    Further in this  document, this folder is referred to as *freeimage*. 

  * 2. Open the  solution file *freeimage\\FreeImage.*.sln* in Visual Studio  that corresponds to the version of Visual Studio you use. 
  
    Since the  version of Visual Studio you use is higher than VC++ 2008, apply conversion of the workspace. 
    Such conversion should be suggested automatically by Visual  Studio. 
    
  * 3. Select a  configuration to build. 
   Choose \" *Release* \"  if you are building Release binaries. 
   Choose \" *Debug* \" if you are building  Debug binaries. 
   *Note:* 
   If you want to  build a debug version of FreeImage binaries then you must rename 
   the following  files for projects FreeImage and FreeimagePlus:

  Project-Properties-Configuration Properties-Linker-General-Output File

  @verbatim
    from FreeImage*d*.dll  to FreeImage.dll 
    from  FreeImagePlus*d*.dll to FreeImagePlus.dll 
  @endverbatim

  Project-Properties-Configuration Properties-Linker-Debugging-Generate Program Database File

  @verbatim
    from FreeImage*d*.pdb  to FreeImage.pdb 
    from  FreeImagePlus*d*.pdb to FreeImagePlus.pdb 
  @endverbatim

  Project-Properties-Configuration Properties-Linker-Advanced-Import Library

  @verbatim
    from FreeImage*d*.lib  to FreeImage.lib 
    from FreeImagePlus*d*.lib  to FreeImagePlus.lib 
  @endverbatim

  Project-Properties-Configuration Properties-Build Events-Post-Build Event-Comand Line 

  @verbatim
    from FreeImage*d*.dll     to FreeImage.dll 
    from FreeImage*d*.lib     to FreeImage.lib 
    from FreeImagePlus*d*.dll to FreeImagePlus.dll 
    from FreeImagePlus*d*.lib to FreeImagePlus.lib 
  @endverbatim

  Additionally, for project FreeImagePlus rename: 
  Project-Properties-Configuration  Properties-Linker-Input-Additional Dependencies 

  @verbatim
    from FreeImage*d*.lib to FreeImage.lib 
  @endverbatim

  * 4. Select a platform to build. 

    Choose \" *Win32* \" if you are building for a 32 bit platform. 
    Choose \" *x64* \" if you are building for a 64 bit platform. 

  * 5. Start the building process. 

  As a result, you should have the  library files of FreeImage product in the *freeimage\\Dist*

  @verbatim
    folder (FreeImage.dll and FreeImage.lib files) and in the *freeimage\\Wrapper\\FreeImagePlus\\dist*
    folder (FreeImagePlus.dll and FreeImagePlus.lib files). 
  @endverbatim

@see http://www.opencascade.org for details
