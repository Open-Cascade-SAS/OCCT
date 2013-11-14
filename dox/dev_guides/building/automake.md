Building with Automake {#dev_guides__building__automake}
======================

This file describes steps to build OCCT libraries from complete source
archive on Linux with GNU build system (Autotools).

If you are building OCCT from bare sources (as in Git repository), or do some 
changes affecting CDL files, you need to use WOK to re-generate header files
and build scripts / projects. See paragraph 1 \ref dev_guides__building__wok for instructions.

Before building OCCT, you need to install required third-party libraries; see paragraph 1 of 
\ref dev_guides__building for instructions.

Note that during compilation by makefiles on some Linux OS on a station with 
NVIDIA video card you may experience problems because the installation 
procedure of NVIDIA video driver removes library libGL.so included in package 
libMesaGL from directory /usr/X11R6/lib and places this library libGL.so in 
directory /usr/lib. However, libtool expects to find the library in directory 
/usr/X11R6/lib, which causes compilation crash (See /usr/X11R6/lib/libGLU.la). 

To prevent this, suggest making links: 

 ln -s /usr/lib/libGL.so /usr/X11R6/lib/libGL.so 
 ln -s /usr/lib/libGL.la /usr/X11R6/lib/libGL.la 

  1.In OCCT root folder, launch build_configure script 

   This will generate files configure and Makefile.in for your system.

  2.Go to the directory where OCCT will be built, and run configure to generate
   makefiles.

   $CASROOT/configure \<FLAGS\>

   Where \<FLAGS\> is a set of options.
   The following flags are mandatory:

   * --with-tcl=  defines location of tclConfig.sh
   * --with-tk=  defines location of tkConfig.sh
   * --with-freetype=  defines location of installed FreeType product
   * --prefix= defines location for the installation of OCCT binaries

   Additional flags:

   * --with-gl2ps=  defines location of installed gl2ps product
   * --with-freeimage=  defines location of installed FreeImage product
   * --with-tbb-include= defines location of tbb.h
   * --with-tbb-library=  defines location of libtbb.so
   * --with-opencl-include= defines location of cl.h
   * --with-opencl-library= defines location of libOpenCL.so
   * --enable-debug=       yes: includes debug information, no: does not include debug information
   * --enable-production=   yes: switches code optimization, no: switches off code optimization
   * --disable-draw - allows OCCT building without Draw.

   If location of FreeImage, TBB, gl2ps or OpenCL is not specified, OCCT will be
   built without these optional libraries.

   
   Attention: 64-bit platforms are detected automatically.

   Example:

   > ./configure -prefix=/PRODUCTS/occt-6.5.5 --with-tcl=/PRODUCTS/tcltk-8.5.8/lib --with-tk=/PRODUCTS/tcltk-8.5.8/lib --with-freetype=/PRODUCTS/freetype-2.4.10 --with-gl2ps=/PRODUCTS/gl2ps-1.3.5 --with-freeimage=/PRODUCTS/freeimage-3.14.1 --with-tbb-include=/PRODUCTS/tbb30_018oss/include --with-tbb-library=/PRODUCTS/tbb30_018oss/lib/ia32/cc4.1.0_libc2.4_kernel2.6.16.21 --with-opencl-include=/PRODUCTS/opencl-icd-1.2.11.0/include --with-opencl-library=/PRODUCTS/opencl-icd-1.2.11.0/lib

  3.If configure exits successfully, you can build OCCT with make command.

   > make -j8 install

To start DRAW, launch

   > draw.sh
