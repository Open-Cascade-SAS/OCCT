 Building 3rd-party libraries on Linux {#occt_dev_guides__building_3rdparty_linux}
=========

@tableofcontents 

@section dev_guides__building_3rdparty_linux_1 Introduction

This document presents additional guidelines for building third-party 
products used by Open CASCADE Technology and samples on Linux platform. 

The links for downloading the third-party products are available on the web site 
of OPEN CASCADE SAS at 
http://www.opencascade.com/content/3rd-party-components. 

There are two types of third-party products, which are  necessary to build OCCT: 
  
* Mandatory  products: 
	* Tcl/Tk 8.5 - 8.6;  
	* FreeType 2.4.10 - 2.5.3;
* Optional  products: 
	* TBB 3.x - 4.x;
	* gl2ps 1.3.5 - 1.3.8;
	* FreeImage 3.14.1 - 3.16.0;
	* VTK 6.1.0.

@section dev_guides__building_3rdparty_linux_2 Building Mandatory Third-party Products

@subsection dev_guides__building_3rdparty_linux_2_1 Tcl/Tk 

Tcl/Tk is required for DRAW test harness. 

@subsubsection dev_guides__building_3rdparty_linux_2_1_2 Installation from sources: Tcl
  
Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 
  
1. Enter the unix sub-directory of the directory where the Tcl source  files are located <i>(TCL_SRC_DIR)</i>. 

       cd TCL_SRC_DIR/unix   

2. Run the *configure* command:

       configure --enable-gcc  --enable-shared --enable-threads --prefix=TCL_INSTALL_DIR   

   For a 64 bit platform also add <i>--enable-64bit</i> option to the command line. 
  
3. If the configure command has finished successfully, start  the building process:
  
       make   

4. If building is finished successfully, start the installation of Tcl. 
   All binary and service files of the product will be copied to the directory defined by *TCL_INSTALL_DIR* 

       make install   

@subsubsection dev_guides__building_3rdparty_linux_2_1_3 Installation from sources: Tk
  
Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 
  
1. Enter the unix sub-directory of the directory where the Tk source files are located <i>(TK_SRC_DIR)</i> 

       cd TK_SRC_DIR/unix   

2. Run the configure command, where <i>TCL_LIB_DIR</i> is *TCL_INSTALL_DIR/lib*.

       configure --enable-gcc  --enable-shared --enable-threads --with-tcl=TCL_LIB_DIR  --prefix=TK_INSTALL_DIR   

   For a 64 bit platform also add <i>--enable-64bit</i> option to the  command line. 

3. If the configure command has finished successfully, start the building process: 

       make   

4. If the building has finished successfully, start the installation of Tk. 
   All binary and service files of the product will be copied 
   to the directory defined by *TK_INSTALL_DIR* (usually it is *TCL_INSTALL_DIR*) 

       make install   

@subsection dev_guides__building_3rdparty_linux_2_2 FreeType

FreeType is required for text display in the 3D viewer. 
Download the necessary archive from http://sourceforge.net/projects/freetype/files/ and unpack it. 
  
1. Enter the directory where the source files of FreeType  are located <i>(FREETYPE_SRC_DIR)</i>. 

       cd FREETYPE_SRC_DIR   

2. Run the *configure* command: 
  
       configure  --prefix=FREETYPE_INSTALL_DIR   

   For a 64 bit platform also add <i>CFLAGS='-m64 -fPIC'  CPPFLAGS='-m64 -fPIC'</i> option to the command line. 
    
3. If the *configure* command has finished successfully, start  the building process: 

       make   

4. If the building has finished successfully, start the installation of FreeType. 
   All binary and service files of the product will be copied to the directory defined by *FREETYPE_INSTALL_DIR* 
    
       make install   

@section dev_guides__building_3rdparty_linux_3 Building Optional Third-party Products
    
@subsection dev_guides__building_3rdparty_linux_3_1 TBB

This third-party product is  installed with binaries from the archive that can be downloaded from http://threadingbuildingblocks.org. 
Go to the **Download** page, find the  release version you need and pick the archive for Linux platform.
To install, unpack the downloaded archive of TBB product.

@subsection dev_guides__building_3rdparty_linux_3_2 gl2ps

Download the necessary archive from http://geuz.org/gl2ps/ and unpack it.
  
1. Install or build *cmake* product from the source file. 
2. Start *cmake* in GUI mode with the directory where the source files of gl2ps are located:

       ccmake GL2PS_SRC_DIR   

   * Press <i>[c]</i> to make the  initial configuration; 
   * Define the necessary options in *CMAKE_INSTALL_PREFIX* 
   * Press <i>[c]</i> to make the final  configuration 
   * Press <i>[g]</i> to generate  Makefile and exit 

   or just run the following command: 

       cmake  –DCMAKE_INSTALL_PREFIX=GL2PS_INSTALL_DIR –DCMAKE_BUILD_TYPE=Release   

3. Start the building of gl2ps: 

       make   

4. Start the installation of gl2ps. Binaries will be  installed according to the *CMAKE_INSTALL_PREFIX* option. 

       make install   

@subsection dev_guides__building_3rdparty_linux_3_3 FreeImage

Download the necessary archive from http://sourceforge.net/projects/freeimage/files/Source%20Distribution/
and unpack it. The directory with unpacked sources is  further referred to as *FREEIMAGE_SRC_DIR*. 
  
1. Modify *FREEIMAGE_SRC_DIR/Source/OpenEXR/Imath/ImathMatrix.h*: 
   In line 60 insert the following: 

       #include string.h   

2. Enter the directory where the source files of FreeImage are located <i>(FREEIMAGE_SRC_DIR)</i>. 
  
       cd FREEIMAGE_SRC_DIR   

3. Run the building process 
  
       make   

4. Run the installation process 

   a. If you have the permission to write into directories <i>/usr/include</i> and <i>/usr/lib</i>, run the following command: 

          make install   
   b. If you do not have this permission, you need to modify file *FREEIMAGE_SRC_DIR/Makefile.gnu*: 
  
      Change lines 7-9 from:
  
          DESTDIR ?= /   
          INCDIR  ?= $(DESTDIR)/usr/include   
          INSTALLDIR  ?= $(DESTDIR)/usr/lib   

      to: 

          DESTDIR  ?= $(DESTDIR)   
          INCDIR  ?= $(DESTDIR)/include   
          INSTALLDIR  ?= $(DESTDIR)/lib   

      Change lines 65-67 from: 

          install  -m 644 -o root -g root $(HEADER) $(INCDIR)   
          install  -m 644 -o root -g root $(STATICLIB) $(INSTALLDIR)   
          install  -m 755 -o root -g root $(SHAREDLIB) $(INSTALLDIR)   
  
      to:

          install  -m 755 $(HEADER) $(INCDIR)   
          install  -m 755 $(STATICLIB) $(INSTALLDIR)   
          install  -m 755 $(SHAREDLIB) $(INSTALLDIR)

      Change line 70 from: 

          ldconfig

      to:
  
          \#ldconfig   
    
   Then run the installation process by the following command: 

        make DESTDIR=FREEIMAGE_INSTALL_DIR  install   

5. Clean temporary files

        make clean

@subsection dev_guides__building_3rdparty_linux_3_4 VTK

You can download VTK sources from http://www.vtk.org/VTK/resources/software.html

### The building procedure:

Download the necessary archive from http://www.vtk.org/VTK/resources/software.html and unpack it.

1. Install or build *cmake* product from the source file.
2. Start *cmake* in GUI mode with the directory where the source files of *VTK* are located:

       ccmake VTK_SRC_DIR

   * Press <i>[c]</i> to make the  initial configuration
   * Define the necessary options in *VTK_INSTALL_PREFIX*
   * Press <i>[c]</i> to make the final  configuration
   * Press <i>[g]</i> to generate  Makefile and exit

3. Start the building of VTK:

       make

4. Start the installation of gl2ps. Binaries will be  installed according to the *VTK_INSTALL_PREFIX* option.

       make install

@section dev_guides__building_3rdparty_linux_4 Installation From Official Repositories

@subsection dev_guides__building_3rdparty_linux_4_1 Debian-based distributives

All 3rd-party products required for building of  OCCT could be installed
from official repositories. You may install them from  console using apt-get utility:

    sudo apt-get install tcllib tklib tcl-dev tk-dev libfreetype-dev libxt-dev libxmu-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libfreeimage-dev libtbb-dev libgl2ps-dev

To launch binaries built with WOK you need to install C shell and 32-bit libraries on x86_64 distributives:

    # you may need to add i386 if not done already by command "dpkg --add-architecture i386"
    sudo apt-get install csh libstdc++6:i386 libxt6:i386 libxext6:i386 libxmu6:i386

Building is possible with C++ compliant compiler:

    sudo apt-get install g++
