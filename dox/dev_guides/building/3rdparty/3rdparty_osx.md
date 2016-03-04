 Building 3rd-party libraries on MacOS X {#occt_dev_guides__building_3rdparty_osx}
==============================================
@tableofcontents 

@section dev_guides__building_3rdparty_osx_1 Introduction

This document presents additional guidelines for building third-party products 
used by Open CASCADE Technology and samples on Mac OS X platform (10.6.4  and later). 

The links for downloading the third-party products are available 
on the web site of OPEN CASCADE SAS at http://www.opencascade.com/content/3rd-party-components. 

There are two types of third-party products, which are  necessary to build OCCT: 
  
* Mandatory  products: 
	* Tcl/Tk 8.5 - 8.6;
	* FreeType 2.4.10 - 2.5.3.
* Optional  products: 
	* TBB 3.x - 4.x;
	* gl2ps  1.3.5 - 1.3.8;
	* FreeImage 3.14.1 - 3.16.0
  
@section dev_guides__building_3rdparty_osx_2 Building Mandatory Third-party Products

@subsection dev_guides__building_3rdparty_osx_2_1 Tcl/Tk 8.5 

Tcl/Tk is required for DRAW test harness. Version 8.5 or  8.6 can be used with OCCT. 

@subsubsection dev_guides__building_3rdparty_osx_2_1_2 Installation from sources: Tcl 8.5

Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 

1. Enter the *macosx* sub-directory of the directory where the Tcl source files are located <i>(TCL_SRC_DIR)</i>. 

       cd TCL_SRC_DIR/macosx   

2. Run the *configure* command 

       configure --enable-gcc  --enable-shared --enable-threads --prefix=TCL_INSTALL_DIR   

   For a 64 bit platform also add <i>--enable-64bit</i> option to the command line. 

3. If the *configure* command has finished successfully, start the building process 

       make   
  
4. If building is finished successfully, start the installation of Tcl. 
   All binary and service files of the product will be copied to the directory defined by *TCL_INSTALL_DIR*. 

       make install   

@subsubsection dev_guides__building_3rdparty_osx_2_1_3 Installation from sources: Tk 8.5

Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 

1. Enter the *macosx* sub-directory of the directory where the  source files of Tk are located <i>(TK_SRC_DIR)</i>. 

       cd TK_SRC_DIR/macosx   

2. Run the *configure* command, where TCL_LIB_DIR is  TCL_INSTALL_DIR/lib 

       configure --enable-gcc --enable-shared --enable-threads --with-tcl=TCL_LIB_DIR --prefix=TK_INSTALL_DIR   

	For a 64 bit platform also add <i>--enable-64bit</i> option to the  command line. 

3. If the *configure* command has finished successfully, start the building process:

       make   

4. If the building has finished successfully, start the  installation of Tk. All binary and service files of the product will be copied to the directory defined by *TK_INSTALL_DIR* (usually it is TCL_INSTALL_DIR) 

       make install   

@subsection dev_guides__building_3rdparty_osx_2_2 FreeType 2.4.10
  
FreeType is required for text display in the 3D viewer. 

Download the necessary archive from http://sourceforge.net/projects/freetype/files/ and unpack it. 

1. Enter the directory where the source files of FreeType  are located <i>(FREETYPE_SRC_DIR)</i>. 

       cd FREETYPE_SRC_DIR   

2. Run the *configure* command 
  
       configure  --prefix=FREETYPE_INSTALL_DIR   

   For a 64 bit platform also add <i>CFLAGS='-m64 -fPIC'  CPPFLAGS='-m64 -fPIC'</i> option to the command line. 

3. If the *configure* command has finished successfully, start  the building process 

       make   

4. If building has finished successfully, start the installation of FreeType. 
   All binary and service files of the product will be copied to the directory defined by *FREETYPE_INSTALL_DIR*.

       make install   

@section dev_guides__building_3rdparty_osx_3 Building Optional Third-party Products
    
@subsection dev_guides__building_3rdparty_osx_3_1 TBB 3.x or 4.x

This third-party product is installed with binaries from the archive 
that can be downloaded from http://threadingbuildingblocks.org/. 
Go to the **Download** page, find the release version you need (e.g. *tbb30_018oss*) 
and  pick the archive for Mac OS X platform. 
To install, unpack the downloaded archive of TBB 3.0 product (*tbb30_018oss_osx.tgz*).

@subsection dev_guides__building_3rdparty_osx_3_2 gl2ps 1.3.5

Download the necessary archive from http://geuz.org/gl2ps/ and unpack it. 

1. Install or build cmake product from the source file. 

2. Start cmake in GUI mode with the directory, where the source  files of *fl2ps* are located: 

       ccmake GL2PS_SRC_DIR   

   * Press <i>[c]</i> to make the  initial configuration; 
   * Define the necessary options in *CMAKE_INSTALL_PREFIX*; 
   * Press <i>[c]</i> to make the final  configuration; 
   * Press <i>[g]</i> to generate  Makefile and exit. 

   or just run the following command: 

       cmake  –DCMAKE_INSTALL_PREFIX=GL2PS_INSTALL_DIR –DCMAKE_BUILD_TYPE=Release   

3. Start the building of gl2ps 

       make   

4. Start the installation of gl2ps. Binaries will be  installed according to the *CMAKE_INSTALL_PREFIX* option 

       make install   

@subsection dev_guides__building_3rdparty_osx_3_3 FreeImage 3.14.1 or 3.15.x

Download the necessary archive from 
http://sourceforge.net/projects/freeimage/files/Source%20Distribution/   
and unpack it. The directory with unpacked sources is  further referred to as *FREEIMAGE_SRC_DIR*.  

Note that for building FreeImage on Mac OS X 10.7 you should replace *Makefile.osx* 
in *FREEIMAGE_SRC_DIR* by the corrected file, which you can find in attachment to issue #22811 in OCCT Mantis bug tracker 
(http://tracker.dev.opencascade.org/file_download.php?file_id=6937&type=bug). 

1. If you build FreeImage 3.15.x you can skip this  step. 
   Modify <i>FREEIMAGE_SRC_DIR/Source/OpenEXR/Imath/ImathMatrix.h:</i> 
  
   In line 60 insert the following: 

       #include string.h 

   Modify <i>FREEIMAGE_SRC_DIR/Source/FreeImage/PluginTARGA.cpp:</i> 
  
   In line 320 replace: 

       SwapShort(value); 

   with: 

       SwapShort(&value); 

2. Enter the directory where the source files of FreeImage  are located <i>(FREEIMAGE_SRC_DIR)</i>. 

       cd FREEIMAGE_SRC_DIR 

3. Run the building process 

       make   

4. Run the installation process 
   
   1. If you have the permission to write into <i>/usr/local/include</i>  and <i>/usr/local/lib</i> directories, run the following command: 

          make install   

   2. If you do not have this permission, you need to modify file *FREEIMAGE_SRC_DIR/Makefile.osx*: 

      Change line 49 from:     

          PREFIX ?= /usr/local

      to:
   
          PREFIX  ?= $(PREFIX) 

      Change lines 65-69 from: 

          install -d -m 755 -o  root -g wheel $(INCDIR) $(INSTALLDIR) 
          install  -m 644 -o root -g wheel $(HEADER) $(INCDIR) 
          install  -m 644 -o root -g wheel $(SHAREDLIB) $(STATICLIB) $(INSTALLDIR) 
          ranlib  -sf $(INSTALLDIR)/$(STATICLIB) 
          ln  -sf $(SHAREDLIB) $(INSTALLDIR)/$(LIBNAME) 
    
      to: 

          install  -d $(INCDIR) $(INSTALLDIR) 
          install  -m 755 $(HEADER) $(INCDIR) 
          install  -m 755 $(STATICLIB) $(INSTALLDIR) 
          install  -m 755 $(SHAREDLIB) $(INSTALLDIR) 
          ln  -sf $(SHAREDLIB) $(INSTALLDIR)/$(VERLIBNAME)  
          ln  -sf $(VERLIBNAME) $(INSTALLDIR)/$(LIBNAME)

      Then run the installation process by the following command: 
  
          make PREFIX=FREEIMAGE_INSTALL_DIR  install 
  
5. Clean temporary files 
  
       make clean 
