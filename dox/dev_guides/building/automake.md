Building with Automake {#occt_dev_guides__building_automake}
======================

This file describes steps to build OCCT libraries from a complete source
archive on Linux with **Autotools** GNU build system.

If you build OCCT from bare sources (as in Git repository), or do some 
changes affecting CDL files, you need to use WOK to re-generate header files
and build scripts / projects. See paragraph 1 \ref occt_dev_guides__building_wok for instructions.

Before building OCCT, you need to install the required third-party libraries; see paragraph 1 of 
\ref occt_dev_guides__building for instructions.

Note that during compilation by makefiles on Linux station with 
NVIDIA video card you may experience problems because the installation 
procedure of NVIDIA video driver removes library *libGL.so* included in package 
*libMesaGL* from directory <i>/usr/X11R6/lib</i> and places this library *libGL.so* in 
directory <i>/usr/lib</i>. However, *libtool* expects to find the library in directory 
<i>/usr/X11R6/lib</i>, which causes compilation crash (See <i>/usr/X11R6/lib/libGLU.la </i>). 

To prevent this, it is suggested to make links: 


	ln -s /usr/lib/libGL.so /usr/X11R6/lib/libGL.so 
	ln -s /usr/lib/libGL.la /usr/X11R6/lib/libGL.la 


  1.In OCCT root folder, launch *build_configure* script to generate files *configure* and *Makefile.in* for your system.

  2.Go to the directory, where OCCT will be built, and run *configure* to generate makefiles.

 
	$CASROOT/configure \<FLAGS\>

   Where <i> \<FLAGS\> </i> is a set of options.
   The following flags are mandatory:

   * <i> --with-tcl= </i> defines the location of *tclConfig.sh*;
   * <i> --with-tk= </i> defines location of *tkConfig.sh*;
   * <i> --with-freetype= </i> defines location of installed **FreeType** product
   * <i> --prefix= </i> defines the location for installation of OCCT binaries

   Additional flags:

   * <i> --with-gl2ps= </i> defines the location of installed **gl2ps** product;
   * <i> --with-freeimage= </i> defines the location of installed **FreeImage** product;
   * <i> --with-tbb-include= </i> defines the location of *tbb.h*;
   * <i> --with-tbb-library= </i> defines the location of *libtbb.so*;
   * <i> --with-vtk-include= </i> defines the location of VTK includes;
   * <i> --with-vtk-library= </i> defines the location of VTK libraries;
   * <i> --enable-debug=   yes: </i>  includes debug information, no: does not include debug information;
   * <i> --enable-production=   yes: </i> switches code optimization, no: switches off code optimization;
   * <i> --disable-draw </i> allows OCCT building without Draw.

   If location of **FreeImage, TBB, gl2ps** or **VTK** is not specified, OCCT will be built without these optional libraries.

   
  Attention: 64-bit platforms are detected automatically.

  Example:
   

  \>  ./configure -prefix=/PRODUCTS/occt-6.5.5 --with-tcl=/PRODUCTS/tcltk-8.5.8/lib --with-tk=/PRODUCTS/tcltk-8.5.8/lib --with-freetype=/PRODUCTS/freetype-2.4.10 --with-gl2ps=/PRODUCTS/gl2ps-1.3.5 --with-freeimage=/PRODUCTS/freeimage-3.14.1 --with-tbb-include=/PRODUCTS/tbb30_018oss/include --with-tbb-library=/PRODUCTS/tbb30_018oss/lib/ia32/cc4.1.0_libc2.4_kernel2.6.16.21 -–with-vtk-include=/PRODUCTS/VTK-6.1.0/include/vtk-6.1 –with-vtk-library=/PRODUCTS/ /VTK-6.1.0//lib


  3.If configure exits successfully, you can build OCCT with *make* command.

  \> make -j8 install

  4.To start *DRAW*, launch

  \> draw.sh
