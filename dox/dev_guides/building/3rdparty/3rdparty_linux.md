 Building 3rd-party libraries on Linux {#dev_guides__building_3rdparty_linux}
============================================
@tableofcontents 

@section dev_guides__building_3rdparty_linux_1 Introduction

  This document presents additional guidelines for building third-party 
  products used by Open CASCADE Technology and samples on Linux platform. 

  The links for downloading the third-party products are available on the web site 
  of OPEN CASCADE SAS at 
  http://www.opencascade.org/getocc/require/. 

  There are two types of third-party products, which are  necessary to build OCCT: 
  
  * Mandatory  products: Tcl 8.5, Tk 8.5, FreeType 2.4.10 
  * Optional  products: TBB 3.x or 4.x, gl2ps 1.3.5, FreeImage 3.14.1 

@section dev_guides__building_3rdparty_linux_2 Building Mandatory Third-party Products

@subsection dev_guides__building_3rdparty_linux_2_1 Tcl/Tk 8.5 

  Tcl/Tk is required for DRAW test harness. Version 8.5 or 8.6 can be used with OCCT. 

@subsubsection dev_guides__building_3rdparty_linux_2_1_1 Installation from binaries

  It is possible to download ready-to-install binaries from 
  http://www.activestate.com/activetcl/downloads

  * 1. Download the binaries archive and unpack them to some  TCL_SRC_DIR. 
  * 2. Enter the directory TCL_SRC_DIR.

  @verbatim
    cd TCL_SRC_DIR
  @endverbatim

  * 3. Run the install command 

  @verbatim
    install.sh
  @endverbatim
    
  and follow instructions. 

@subsubsection dev_guides__building_3rdparty_linux_2_1_2 Installation from sources: Tcl 8.5
  
  Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 
  
  * 1. Enter the unix sub-directory of the directory where the source  files of Tcl are located (TCL_SRC_DIR). 

  @verbatim
    cd TCL_SRC_DIR/unix   
  @endverbatim

  * 2. Run the configure command

  @verbatim
    configure --enable-gcc  --enable-shared --enable-threads --prefix=TCL_INSTALL_DIR   
  @endverbatim

  For a 64 bit platform also add --enable-64bit option to the  command line. 
  
  * 3. If the configure command has finished successfully, start  the building process 
  
  @verbatim
    make   
  @endverbatim

  * 4. If building is finished successfully, start the installation of Tcl. 
  All binary and service files of the product will be copied to the directory defined by TCL_INSTALL_DIR 

  @verbatim
    make install   
  @endverbatim

@subsubsection dev_guides__building_3rdparty_linux_2_1_3 Installation from sources: Tk 8.5
  
  Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 
  
  * 1. Enter the unix sub-directory of the directory where the source  files of Tk are located (TK_SRC_DIR). 

  @verbatim
    cd TK_SRC_DIR/unix   
  @endverbatim

  * 2. Run the configure command, where TCL_LIB_DIR is  TCL_INSTALL_DIR/lib 

  @verbatim
    configure --enable-gcc  --enable-shared --enable-threads --with-tcl=TCL_LIB_DIR  --prefix=TK_INSTALL_DIR   
  @endverbatim

  where TCL_LIB_DIR is TCL_INSTALL_DIR/lib 

  For a 64 bit platform also add --enable-64bit option to the  command line. 

  * 3. If the configure command has finished successfully, start  the building process 

  @verbatim
    make   
  @endverbatim

  * 4. If building has finished successfully, start the installation of Tk. 
       All binary and service files of the product will be copied 
       to the directory defined by TK_INSTALL_DIR (usually  TK_INSTALL_DIR is TCL_INSTALL_DIR) 

  @verbatim
    make install   
  @endverbatim

@subsection dev_guides__building_3rdparty_linux_2_2 FreeType 2.4.10

  FreeType is required for display of text in 3D viewer. 
  Download the necessary archive from http://sourceforge.net/projects/freetype/files/ and unpack it. 
  
  * 1. Enter the directory where the source files of FreeType  are located (FREETYPE_SRC_DIR). 

  @verbatim
    cd FREETYPE_SRC_DIR   
  @endverbatim

  * 2. Run the configure command 
  
  @verbatim
    configure  --prefix=FREETYPE_INSTALL_DIR   
  @endverbatim

  For a 64 bit platform also add CFLAGS='-m64 -fPIC'  CPPFLAGS='-m64 -fPIC' option to the command line. 
    
  * 3. If the configure command has finished successfully, start  the building process 

  @verbatim
    make   
  @endverbatim

  * 4. If building has finished successfully, start the installation of FreeType. 
       All binary and service files of the product will be copied to the directory defined by FREETYPE_INSTALL_DIR 
    
  @verbatim
    make install   
  @endverbatim

@section dev_guides__building_3rdparty_linux_3 Building Optional Third-party Products
    
@subsection dev_guides__building_3rdparty_linux_3_1 TBB 3.x or 4.x

  This third-party product is  installed with binaries from the archive that can be downloaded from http://threadingbuildingblocks.org. 
  Go to \"Downloads page\", find the  release version you need (e.g. tbb30_018oss) and pick the archive for Linux  platform. 
  To install, unpack the downloaded archive of TBB 3.0 product (*tbb30_018oss_lin.tgz*).

@subsection dev_guides__building_3rdparty_linux_3_2 gl2ps 1.3.5

  Download the necessary archive from http://geuz.org/gl2ps/ and unpack it.
  
  * 1. Install or build cmake product from source file. 
  * 2. Start cmake in GUI mode with the directory where the source files of gl2ps are located:
  @verbatim
    ccmake GL2PS_SRC_DIR   
  @endverbatim
     * 2.1. Press [c] to make the  initial configuration 
     * 2.2. Define the necessary options  CMAKE_INSTALL_PREFIX 
     * 2.3. Press [c] to make the final  configuration 
     * 2.4. Press [g] to generate  Makefile and exit 

  or just run the following command: 

  @verbatim
    cmake  –DCMAKE_INSTALL_PREFIX=GL2PS_INSTALL_DIR –DCMAKE_BUILD_TYPE=Release   
  @endverbatim

  * 3. Start building of gl2ps 

  @verbatim
    make   
  @endverbatim

  * 4. Start the installation of gl2ps. Binaries will be  installed according to the CMAKE_INSTALL_PREFIX option 

  @verbatim
    make install   
  @endverbatim

@subsection dev_guides__building_3rdparty_linux_3_3 FreeImage 3.14.1

  Download the necessary archive from http://sourceforge.net/projects/freeimage/files/Source%20Distribution/
  and unpack it. The directory with unpacked sources is  further referred to as FREEIMAGE_SRC_DIR. 
  
  * 1. Modify FREEIMAGE_SRC_DIR/Source/OpenEXR/Imath/ImathMatrix.h: 
   In line 60 insert the following: 

  @verbatim
    #include string.h   
  @endverbatim

  * 2. Enter the directory where the source files of FreeImage are located (FREEIMAGE_SRC_DIR). 
  
  @verbatim
    cd FREEIMAGE_SRC_DIR   
  @endverbatim

  * 3. Run the building process 
  
  @verbatim
    make   
  @endverbatim

  * 4. Run the installation process 
     * 4.1. If you have permissions to write to /usr/include and /usr/lib directories then run the following command: 
  @verbatim
    make install   
  @endverbatim
     * 4.2. If you don’t have permissions to write to /usr/include and 
       /usr/lib directories then you need to modify the file FREEIMAGE_SRC_DIR/Makefile.gnu: 
  
  Change lines 7-9 from:
  
  @verbatim
    DESTDIR ?= /   
    INCDIR  ?= $(DESTDIR)/usr/include   
    INSTALLDIR  ?= $(DESTDIR)/usr/lib   
  @endverbatim

  to: 

  @verbatim
    DESTDIR  ?= $(DESTDIR)   
    INCDIR  ?= $(DESTDIR)/include   
    INSTALLDIR  ?= $(DESTDIR)/lib   
  @endverbatim

  Change lines 65-67 from: 

  @verbatim
    install  -m 644 -o root -g root $(HEADER) $(INCDIR)   
    install  -m 644 -o root -g root $(STATICLIB) $(INSTALLDIR)   
    install  -m 755 -o root -g root $(SHAREDLIB) $(INSTALLDIR)   
  @endverbatim
  
  to:

  @verbatim
    install  -m 755 $(HEADER) $(INCDIR)   
    install  -m 755 $(STATICLIB) $(INSTALLDIR)   
    install  -m 755 $(SHAREDLIB) $(INSTALLDIR)
  @endverbatim

  Change line 70 from: 

  @verbatim
    ldconfig
  @endverbatim

  to:
  
  @verbatim
    \#ldconfig   
  @endverbatim
    
  Then run the installation process by the following command: 

  @verbatim
    make DESTDIR=FREEIMAGE_INSTALL_DIR  install   
  @endverbatim

  * 5. Clean the temporary files
  
  @verbatim
    make clean   
  @endverbatim

@section dev_guides__building_3rdparty_linux_4 Installation From Official Repositories

@subsection dev_guides__building_3rdparty_linux_4_1 Debian-based distributives

  All 3rd-party products required for building of  OCCT could be installed 
  from official repositories. You may install them from  console using apt-get utility: 

  @verbatim
    sudo apt-get install \   
    tcllib tklib tcl-dev tk-dev \   
    libfreetype-dev \   
    libxt-dev libxmu-dev \   
    libgl1-mesa-dev \   
    libfreeimage-dev \   
    libtbb-dev \   
    libgl2ps-dev   
  @endverbatim
  
  To launch WOK-prebuilt binaries you need install C shell and  32-bit libraries on x86_64 distributives: 
  
  @verbatim
    sudo apt-get install \   
    csh \   
    libstdc++5:i386 libxt6:i386   
  @endverbatim

  Any compliant C++ compiler is required for building anyway: 
  
  @verbatim
    sudo apt-get install \   
    g++ \   
  @endverbatim

@see http://www.opencascade.org for details
