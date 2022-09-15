 Build 3rd-parties {#build_upgrade_building_3rdparty}
==============================================
@tableofcontents

On Windows, the easiest way to install third-party libraries is to download archive with pre-built binaries from https://dev.opencascade.org/resources/download/3rd-party-components.
On Linux and macOS, it is recommended to use the version installed in the system natively.

@section dev_guides__building_3rdparty_win_1 Windows

This section presents guidelines for building third-party products used by Open CASCADE Technology (OCCT) and samples on Windows platform.
It is assumed that you are already familiar with MS Visual Studio / Visual C++.

You need to use the same version of MS Visual Studio for building all third-party products and OCCT itself, in order to receive a consistent set of runtime binaries.

It is recommended to create a separate new folder on your workstation, where you will unpack the downloaded archives of the third-party products, and where you will build these products (for example, `c:/occ3rdparty`).
Further in this document, this folder is referred to as `3rdparty`.

@subsection dev_guides__building_3rdparty_win_2 Tcl/Tk

Tcl/Tk is required for DRAW test harness.

**Installation from sources: Tcl**

Download the necessary archive from https://www.tcl.tk/software/tcltk/download.html and unpack it.
  
1. In the `win` sub-directory, edit file `buildall.vc.bat`:

   * Edit the line `"call ... vcvars32.bat"` to have correct path to the version of Visual Studio to be used for building, for instance:

         call "%VS80COMNTOOLS%\vsvars32.bat"

     If you are building 64-bit version, set environment accordingly, e.g.:

         call "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64

   * Define variable `INSTALLDIR` pointing to directory where Tcl/Tk will be installed, e.g.:

         set INSTALLDIR=D:\OCCT\3rdparty\tcltk-86-32

   * Add option `install` to the first command line calling `nmake`:

         nmake -nologo -f makefile.vc release htmlhelp install %1

   * Remove second call to `nmake` (building statically linked executable)

2. Edit file `rules.vc` replacing line

       SUFX	    = tsgx

   by

       SUFX	    = sgx

   This is to avoid extra prefix 't' in the library name, which is not recognized by default by OCCT build tools.

3. By default, Tcl uses dynamic version of run-time library (MSVCRT), which must be installed on the system where Tcl will be used.
   You may wish to link Tcl library with static version of run-time to avoid this dependency.
   For that:

   * Edit file `makefile.vc` replacing strings `"crt = -MD"` by `"crt = -MT"`

   * Edit source file `tclMain.c` (located in folder `generic`) commenting out forward declaration of function `isatty()`.

4. In the command prompt, run `buildall.vc.bat`<br>
   You might need to run this script twice to have `tclsh` executable installed; check subfolder `bin` of specified installation path to verify this.

5. For convenience of use, we recommend making a copy of `tclsh` executable created in subfolder `bin` of `INSTALLDIR` and named with Tcl version number suffix, as `tclsh.exe` (with no suffix)

       > cd D:\OCCT\3rdparty\tcltk-86-32\bin
       > cp tclsh86.exe tclsh.exe

**Installation from sources: Tk**
  
Download the necessary archive from https://www.tcl.tk/software/tcltk/download.html and unpack it.
Apply the same steps as described for building Tcl above, with the same `INSTALLDIR`.
Note that Tk produces its own executable, called `wish`.

You might need to edit default value of `TCLDIR` variable defined in `buildall.vc.bat` (should be not necessary if you unpack both Tcl and Tk sources in the same folder).

@subsection dev_guides__building_3rdparty_win_2_2 FreeType

FreeType is required for text display in a 3D viewer.
You can download its sources from https://freetype.org/

1. Unpack the downloaded archive of FreeType product into the `3rdparty` folder.
   As a result, you will get a folder named, for example, `3rdparty/freetype-2.4.10`.
   Further in this document, this folder is referred to as `freetype`.

2. Open the solution file `freetype/builds/win32/vc20xx/freetype.sln` in Visual Studio.
   Here `vc20xx` stands for your version of Visual Studio.

3. Select the configuration to build: either `Debug` or `Release`.

4. Build the `freetype` project.<br>
   As a result, you will get a `freetype` import library (`.lib`) in the `freetype/obj/win32/vc20xx` folder.

5. If you build FreeType for a 64 bit platform, select in the main menu `Build - Configuration Manager`
   and add `x64` platform to the solution configuration by copying the settings from `Win32` platform:

   @figure{/build/build_3rdparty/images/3rdparty_image001.png}

   Update the value of the Output File for `x64` configuration:

   @figure{/build/build_3rdparty/images/3rdparty_image003.png}

   Build the `freetype` project.<br>
   As a result, you will obtain a 64 bit import library (`.lib`) file in the `freetype/x64/vc20xx` folder.
   To build FreeType as a dynamic library (`.dll`) follow steps 6, 7 and 8 of this procedure.

6. Open menu Project-> Properties-> Configuration  Properties-> General and change option `Configuration Type` to `Dynamic Library (.dll)`.
7. Edit file `freetype/include/freetype/config/ftoption.h`:<br>
   in line 255, uncomment the definition of macro `FT_EXPORT` and change it as follows:

       #define FT_EXPORT(x)   __declspec(dllexport) x

8. Build the `freetype` project.<br>
   As a result, you will obtain the files of the import library (`.lib`) and the dynamic library (`.dll`) in folders `freetype/objs/release` or `freetype/objs/debug`.
   If you build for a 64 bit platform, follow step 5 of the procedure.

   To facilitate the use of FreeType libraries in OCCT with minimal adjustment of build procedures,
   it is recommended to copy the include files and libraries of FreeType into a separate folder, named according to the pattern `freetype-compiler-bitness-building mode`, where:
   * `compiler` is `vc8` or `vc9` or `vc10` or `vc11`;
   * `bitness`  is `32` or `64`;
   * `building mode` is `opt` (for `Release`) or `deb` (for `Debug`).

   The `include` subfolder should be copied as is, while libraries should be renamed to `freetype.lib` and `freetype.dll` (suffixes removed) and placed to subdirectories `lib` and `bin`, respectively.
   If the `Debug` configuration is built, the Debug libraries should be put into subdirectories `libd` and `bind`.

@subsection dev_guides__building_3rdparty_win_3_1 TBB

This third-party product is installed with binaries from the archive that can be downloaded from https://github.com/oneapi-src/oneTBB/releases/tag/v2021.5.0.
Go to the **Download** page, find the release version you need (e.g. `oneTBB 2021.5.0`) and pick the archive for Windows platform.
To install, unpack the downloaded archive of TBB product (`oneapi-tbb-2021.5.0-win.zip`)

Unpack the downloaded archive of TBB product into the `3rdparty` folder.

Further in this document, this folder is referred to as `tbb`.

@subsection dev_guides__building_3rdparty_win_3_3 FreeImage

This third-party product should be built as a dynamically loadable library (`.dll` file).
You can download its sources from
https://sourceforge.net/projects/freeimage/files/Source%20Distribution/

1. Unpack the downloaded archive of FreeImage product into `3rdparty` folder.<br>
   As a result, you should have a folder named `3rdparty/FreeImage`.
   Rename it according to the rule: `freeimage-platform-compiler-building mode`, where

   * `platform`  is `win32` or `win64`;
   * `compiler`  is `vc8` or `vc9` or `vc10` or `vc11`;
   * `building mode` is *opt* (for release) or `deb` (for debug)

   Further in this document, this folder is referred to as `freeimage`.

2. Open the solution file `freeimage/FreeImage.*.sln` in your Visual Studio.<br>
   If you use a Visual Studio version higher than VC++ 2008, apply conversion of the workspace.
   Such conversion should be suggested automatically by Visual Studio.

3. Select a configuration to build.
   - Choose `Release` if you are building Release binaries.
   - Choose `Debug` if you are building Debug binaries.

   *Note:*

   If you want to build a debug version of FreeImage binaries then you need to rename the following files in FreeImage projects:

   Project -> Properties -> Configuration Properties -> Linker -> General -> Output File

       FreeImage*d*.dll  to FreeImage.dll

   Project -> Properties -> Configuration Properties -> Linker -> Debugging-> Generate Program Database File

       FreeImage*d*.pdb  to FreeImage.pdb

   Project -> Properties -> Configuration Properties -> Linker -> Advanced-Import Library

       FreeImage*d*.lib  to FreeImage.lib

   Project -> Properties -> Configuration Properties -> Build Events -> Post -> Build Event -> Command Line

       FreeImage*d*.dll     to FreeImage.dll
       FreeImage*d*.lib     to FreeImage.lib

   Additionally, rename in project FreeImagePlus

   Project -> Properties -> Configuration  Properties -> Linker -> Input -> Additional Dependencies

       from FreeImage*d*.lib to FreeImage.lib

4. Select a platform to build.
   - Choose `Win32` if you are building for a 32 bit platform.
   - Choose `x64` if you are building for a 64 bit platform.

5. Start the building process.<br>
   As a result, you should have the library files of FreeImage product in `freeimage/Dist` folder (`FreeImage.dll` and `FreeImage.lib`).

@subsection dev_guides__building_3rdparty_win_3_4 VTK

VTK Integration Services component provides adaptation functionality for visualization of OCCT topological shapes by means of VTK library.

1. Download the necessary archive from https://www.vtk.org/VTK/resources/software.html and unpack it into `3rdparty` folder.<br>
   As a result, you will get a folder named, for example, `3rdparty/VTK-6.1.0`.
   Further in this document, this folder is referred to as `VTK`.

2. Use CMake to generate VS projects for building the library:
   - Start CMake-GUI and select `VTK` folder as source path, and the folder of your choice for VS project and intermediate build data.
   - Click **Configure**.
   - Select the VS version to be used from the ones you have installed (we recommend using VS 2015) and the architecture (32 or 64-bit).
   - Generate VS projects with default CMake options. The open solution `VTK.sln` will be generated in the build folder.

3. Build project VTK in Release mode.

@section build_3rdparty_linux Linux

This section presents additional guidelines for building third-party products used by Open CASCADE Technology and samples on Linux platform.

@subsection dev_guides__building_3rdparty_linux_4 Installation From Official Repositories

**Debian-based distributives**

All 3rd-party products required for building of OCCT could be installed from official repositories.
You may install them from console using apt-get utility:

    sudo apt-get install tcllib tklib tcl-dev tk-dev libfreetype-dev libx11-dev libgl1-mesa-dev libfreeimage-dev
    sudo apt-get install rapidjson-dev libdraco-dev

Building is possible with C++ compliant compiler:

    sudo apt-get install g++

@subsection dev_guides__building_3rdparty_linux_2_1 Tcl/Tk

Tcl/Tk is required for DRAW test harness.

**Installation from sources: Tcl**

Download the necessary archive from https://www.tcl.tk/software/tcltk/download.html and unpack it.

1. Enter the `unix` sub-directory of the directory where the Tcl source files are located (`TCL_SRC_DIR`).

       cd TCL_SRC_DIR/unix

2. Run the `configure` command:

       configure --enable-gcc  --enable-shared --enable-threads --prefix=TCL_INSTALL_DIR

   For a 64 bit platform also add `--enable-64bit` option to the command line.

3. If the configure command has finished successfully, start the building process:

       make

4. If building is finished successfully, start the installation of Tcl.
   All binary and service files of the product will be copied to the directory defined by `TCL_INSTALL_DIR`

       make install

**Installation from sources: Tk**

Download the necessary archive from https://www.tcl.tk/software/tcltk/download.html and unpack it.
  
1. Enter the `unix` sub-directory of the directory where the Tk source files are located (`TK_SRC_DIR`)

       cd TK_SRC_DIR/unix

2. Run the `configure` command, where `TCL_LIB_DIR` is `TCL_INSTALL_DIR/lib`.

       configure --enable-gcc  --enable-shared --enable-threads --with-tcl=TCL_LIB_DIR  --prefix=TK_INSTALL_DIR

   For a 64 bit platform also add `--enable-64bit` option to the command line.

3. If the configure command has finished successfully, start the building process:

       make

4. If the building has finished successfully, start the installation of Tk.
   All binary and service files of the product will be copied
   to the directory defined by `TK_INSTALL_DIR` (usually it is `TCL_INSTALL_DIR`)

       make install

@subsection dev_guides__building_3rdparty_linux_2_2 FreeType

FreeType is required for text display in the 3D viewer.
Download the necessary archive from https://freetype.org/ and unpack it.

1. Enter the directory where the source files of FreeType are located (`FREETYPE_SRC_DIR`).

       cd FREETYPE_SRC_DIR   

2. Run the `configure` command:

       configure  --prefix=FREETYPE_INSTALL_DIR

   For a 64 bit platform also add `CFLAGS='-m64 -fPIC'  CPPFLAGS='-m64 -fPIC'` option to the command line.

3. If the `configure` command has finished successfully, start the building process:

       make

4. If the building has finished successfully, start the installation of FreeType.
   All binary and service files of the product will be copied to the directory defined by `FREETYPE_INSTALL_DIR`

       make install

@subsection dev_guides__building_3rdparty_linux_3_1 TBB

This third-party product is installed with binaries from the archive that can be downloaded from https://github.com/oneapi-src/oneTBB/releases/tag/v2021.5.0.
Go to the **Download** page, find the release version you need (e.g. `oneTBB 2021.5.0`) and pick the archive for Linux platform.
To install, unpack the downloaded archive of TBB product (`oneapi-tbb-2021.5.0-lin.tgz`).

@subsection dev_guides__building_3rdparty_linux_3_3 FreeImage

Download the necessary archive from https://sourceforge.net/projects/freeimage/files/Source%20Distribution/ and unpack it.
The directory with unpacked sources is  further referred to as `FREEIMAGE_SRC_DIR`.

1. Modify `FREEIMAGE_SRC_DIR/Source/OpenEXR/Imath/ImathMatrix.h`:<br>
   In line 60 insert the following:

       #include string.h

2. Enter the directory where the source files of FreeImage are located (`FREEIMAGE_SRC_DIR`).

       cd FREEIMAGE_SRC_DIR

3. Run the building process

       make

4. Run the installation process

   a. If you have the permission to write into directories `/usr/include` and `/usr/lib`, run the following command:

          make install

   b. If you do not have this permission, you need to modify file `FREEIMAGE_SRC_DIR/Makefile.gnu`:

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

Download the necessary archive from https://www.vtk.org/VTK/resources/software.html and unpack it.

1. Install or build `cmake` product from the source file.
2. Start `cmake` in GUI mode with the directory where the source files of *VTK* are located:

       ccmake VTK_SRC_DIR

   * Press `[c]` to make the initial configuration
   * Define the necessary options in `VTK_INSTALL_PREFIX`
   * Press `[c]` to make the final configuration
   * Press `[g]` to generate `Makefile` and exit

3. Start the building of VTK:

       make

4. Start the installation of VTK. Binaries will be installed according to the `VTK_INSTALL_PREFIX` option.

       make install

@section build_3rdparty_macos Mac OS X

This section presents additional guidelines for building third-party products
used by Open CASCADE Technology and samples on Mac OS X platform (10.6.4 and later).

@subsection dev_guides__building_3rdparty_osx_2_1 Tcl/Tk

Tcl/Tk is required for DRAW test harness.

**Installation from sources: Tcl**

Download the necessary archive from https://www.tcl.tk/software/tcltk/download.html and unpack it.

1. Enter the `macosx` sub-directory of the directory where the Tcl source files are located (`TCL_SRC_DIR`).

       cd TCL_SRC_DIR/macosx

2. Run the `configure` command

       configure --enable-gcc  --enable-shared --enable-threads --prefix=TCL_INSTALL_DIR

   For a 64 bit platform also add `--enable-64bit` option to the command line.

3. If the `configure` command has finished successfully, start the building process

       make

4. If building is finished successfully, start the installation of Tcl.
   All binary and service files of the product will be copied to the directory defined by `TCL_INSTALL_DIR`.

       make install

**Installation from sources: Tk**

Download the necessary archive from https://www.tcl.tk/software/tcltk/download.html and unpack it.

1. Enter the `macosx` sub-directory of the directory where the source files of Tk are located (`TK_SRC_DIR`).

       cd TK_SRC_DIR/macosx

2. Run the `configure` command, where `TCL_LIB_DIR` is `TCL_INSTALL_DIR/lib`

       configure --enable-gcc --enable-shared --enable-threads --with-tcl=TCL_LIB_DIR --prefix=TK_INSTALL_DIR

   For a 64 bit platform also add `--enable-64bit` option to the command line.

3. If the `configure` command has finished successfully, start the building process:

       make

4. If the building has finished successfully, start the installation of Tk.
   All binary and service files of the product will be copied to the directory defined by `TK_INSTALL_DIR` (usually it is `TCL_INSTALL_DIR`).

       make install

@subsection dev_guides__building_3rdparty_osx_2_2 FreeType

FreeType is required for text display in the 3D viewer.
Download the necessary archive from https://freetype.org/ and unpack it.

1. Enter the directory where the source files of FreeType are located (`FREETYPE_SRC_DIR`).

       cd FREETYPE_SRC_DIR

2. Run the `configure` command

       configure  --prefix=FREETYPE_INSTALL_DIR

   For a 64 bit platform also add `CFLAGS='-m64 -fPIC'  CPPFLAGS='-m64 -fPIC'` option to the command line.

3. If the `configure` command has finished successfully, start the building process

       make

4. If building has finished successfully, start the installation of FreeType.
   All binary and service files of the product will be copied to the directory defined by `FREETYPE_INSTALL_DIR`.

       make install

@subsection dev_guides__building_3rdparty_osx_3_1 TBB

This third-party product is installed with binaries from the archive that can be downloaded from https://github.com/oneapi-src/oneTBB/releases/tag/v2021.5.0.
Go to the **Download** page, find the release version you need (e.g. `oneTBB 2021.5.0`) and pick the archive for Mac OS X platform.
To install, unpack the downloaded archive of TBB product (`oneapi-tbb-2021.5.0-mac.tgz`).

@subsection dev_guides__building_3rdparty_osx_3_3 FreeImage

Download the necessary archive from
https://sourceforge.net/projects/freeimage/files/Source%20Distribution/
and unpack it. The directory with unpacked sources is further referred to as `FREEIMAGE_SRC_DIR`.

Note that for building FreeImage on Mac OS X 10.7 you should replace `Makefile.osx`
in `FREEIMAGE_SRC_DIR` by the corrected file, which you can find in attachment to issue [`#22811`](https://tracker.dev.opencascade.org/file_download.php?file_id=6937&type=bug) in OCCT Mantis bug tracker.

1. If you build FreeImage 3.15.x you can skip this step.

   Modify `FREEIMAGE_SRC_DIR/Source/OpenEXR/Imath/ImathMatrix.h:`<br>
   In line 60 insert the following:

       #include string.h

   Modify `FREEIMAGE_SRC_DIR/Source/FreeImage/PluginTARGA.cpp`:<br>
   In line 320 replace:

       SwapShort(value);

   with:

       SwapShort(&value);

2. Enter the directory where the source files of FreeImage are located (`FREEIMAGE_SRC_DIR`).

       cd FREEIMAGE_SRC_DIR

3. Run the building process 

       make

4. Run the installation process

   1. If you have the permission to write into `/usr/local/include` and `/usr/local/lib` directories, run the following command:

          make install

   2. If you do not have this permission, you need to modify file `FREEIMAGE_SRC_DIR/Makefile.osx`:<br>
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
