Build OCCT {#build_upgrade__building_occt}
===================

@tableofcontents

Before building OCCT, make sure to have all required third-party libraries installed.
The list of required libraries depends on what OCCT modules will be used, and your preferences.
The typical minimum is **FreeType** (necessary for Visualization) and **Tcl/Tk** (for DRAW).
See @ref intro_req "requirements on 3rdparty libraries" for a full list.

The easiest way to install third-party libraries is to download archive with pre-built binaries, corresponding to your target configuration,
from [Development Portal](https://dev.opencascade.org/resources/download/3rd-party-components).
You can also build third-party libraries from their sources, see @ref build_upgrade_building_3rdparty for instructions.

On Linux and macOS we recommend using libraries maintained by distributive developers when possible.

@section build_requirements System Requirements

* **CMake** version 3.10 or later (3.16+ recommended for precompiled headers support)
* C++11 compliant compiler (required)
* Supported platforms and compilers:
  * Windows:
    - Visual Studio 2015 or later
    - MinGW-w64 7.3 or later
  * Linux:
    - GCC 5.0 or later
    - Clang 3.8 or later
  * macOS:
    - Apple Clang 9.0 or later
    - Xcode 9.0 or later

@section build_occt_cmake Building with CMake

This chapter describes the [CMake](https://cmake.org/download/)-based build process, which is the standard way to produce OCCT binaries from sources.

CMake is a tool that generates project files for the selected target build system (e.g. Unix makefiles) or IDE (e.g. Visual Studio).
Here we describe the build procedure using Windows platform with Visual Studio as an example.
However, CMake is cross-platform and can be used to build OCCT on Linux and macOS in essentially the same way.

CMake deals with three directories: source, build or binary and installation.

* The source directory is where the sources of OCCT are located in your file system;
* The build or binary directory is where all files created during CMake configuration and generation process will be located;
* The installation directory is where binaries will be installed after building the *INSTALL* project, along with header files and resources required for OCCT use in applications.

The good practice is not to use the source directory as a build one.
Different configurations should be built in different build directories to avoid conflicts.
For example:

    d:/occt/                   - the source directory
    d:/tmp/occt-build-vc14-x64 - the build directory with the generated
                                 solution and other intermediate files created during a CMake tool working
    d:/occt-install            - the installation directory that can
                                 contain several OCCT configurations

@subsection build_cmake_conf Configuration process

For inexperienced users we recommend starting with *cmake-gui* -- a cross-platform GUI tool provided by CMake on Windows, Mac and Linux.
A command-line alternative, *ccmake* can also be used.

If the command-line tool is used, run it from the build directory with a single argument indicating the source directory:

    cd d:/tmp/occt-build-vc14-x64
    ccmake d:/occt

Then press *c* to configure.

@figure{/build/build_occt/images/cmake_image000.png}

If the GUI tool is used, run this tool without additional arguments and specify the source directory by clicking **Browse Source** and the build directory by clicking **Browse Build**:

@figure{/build/build_occt/images/cmake_image001.png}

@note Each configuration of the project should be built in its own directory.
When building multiple configurations it is suggested to indicate in the name of build directories the system, bitness and compiler (e.g., <i>d:/occt/build/win32-vc14</i>).

Once the source and build directories are selected, "Configure" button should be pressed in order to start manual configuration process.
It begins with selection of a target configurator. It is "Visual Studio 14 2015 Win64" in our example.

@figure{/build/build_occt/images/cmake_image002.png}

@note To build OCCT for **Universal Windows Platform (UWP)** specify the path to toolchain file for cross-compiling <i>d:/occt/adm/templates/uwp.toolchain.config.cmake</i>.
Alternatively, if you are using CMake from the command line add options `-DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0`.
Universal Windows Platform (UWP) is supported only on "Visual Studio 14 2015".
File `CASROOT/samples/xaml/ReadMe.md` describes the building procedure of XAML (UWP) sample.

Once "Finish" button is pressed, the first pass of the configuration process is executed.
At the end of the process, CMake outputs the list of environment variables, which have to be properly specified for successful configuration.

@figure{/build/build_occt/images/cmake_image003.png}

The error message provides some information about these variables.
This message will appear after each pass of the process until all required variables are specified correctly.

The change of the state of some variables can lead to the appearance of new variables.
The new variables appeared after the pass of the configuration process are highlighted with red color by CMake GUI tool.

@note There is "grouped" option, which groups variables with a common prefix.

The following table gives the full list of environment variables used at the configuration stage:

| Variable | Type | Purpose |
|----------|------|---------|
| CMAKE_BUILD_TYPE | String | Specifies the build type on single-configuration generators (such as make). Possible values are Debug, Release and RelWithDebInfo |
| USE_FREETYPE  | Boolean | Indicates whether FreeType product should be used in OCCT for text rendering |
| USE_FREEIMAGE | Boolean | Indicates whether FreeImage product should be used in OCCT visualization module for support of popular graphics image formats (PNG, BMP, etc.) |
| USE_FFMPEG    | Boolean | Indicates whether FFmpeg framework should be used for video encoding/decoding support |
| USE_OPENVR    | Boolean | Indicates whether OpenVR product should be used in OCCT visualization module for VR support |
| USE_OPENGL    | Boolean | Indicates whether TKOpenGl graphic driver using OpenGL library (desktop) should be built |
| USE_GLES2     | Boolean | Indicates whether TKOpenGles graphic driver using OpenGL ES library should be built |
| USE_RAPIDJSON | Boolean | Indicates whether RapidJSON product should be used for JSON format support |
| USE_XLIB      | Boolean | Indicates whether Xlib should be supported by OCCT visualization module for window creation |
| USE_WAYLAND   | Boolean | Indicates whether Wayland should be supported by OCCT visualization module for window creation |
| USE_DRACO     | Boolean | Indicates whether Draco mesh compression library should be used |
| USE_TK        | Boolean | Indicates whether Tcl/Tk product should be used in Draw Harness for user interface |
| USE_TBB       | Boolean | Indicates whether TBB (Threading Building Blocks) should be used for parallel computations |
| USE_VTK       | Boolean | Indicates whether VTK bridge should be built |
| BUILD_USE_PCH | Boolean | Enable/disable use of precompiled headers. Requires CMake 3.16 or later |
| BUILD_USE_VCPKG | Boolean | Use vcpkg for managing third-party dependencies |
| BUILD_INCLUDE_SYMLINK | Boolean | Use symbolic links instead of copies for header files in build directory |
| BUILD_MODULE_<MODULE>| Boolean | Indicates whether the corresponding OCCT module should be built |
| BUILD_LIBRARY_TYPE | String | Specifies library type ("Shared" or "Static") |
| BUILD_CPP_STANDARD | String | Select C++ standard (C++11, C++14, C++17, C++20, C++23) |
| 3RDPARTY_DIR | Path | Defines the root directory where all required 3rd party products will be searched. Once you define this path it is very convenient to click "Configure" button in order to let CMake automatically detect all necessary products|
| 3RDPARTY_FREETYPE_* | Path | Path to FreeType binaries |
| 3RDPARTY_TCL_* 3RDPARTY_TK_* | Path | Path to Tcl/Tk binaries |
| 3RDPARTY_FREEIMAGE* | Path | Path to FreeImage binaries |
| 3RDPARTY_TBB*  | Path | Path to TBB binaries |
| 3RDPARTY_VTK_* | Path | Path to VTK binaries |
| BUILD_ADDITIONAL_TOOLKITS | String | Semicolon-separated individual toolkits to include into build process. If you want to build some particular libraries (toolkits) only, then you may uncheck all modules in the corresponding *BUILD_MODUE_\<MODULE\>* options and provide the list of necessary libraries here. Of course, all dependencies will be resolved automatically |
| BUILD_YACCLEX | Boolean | Enables Flex/Bison lexical analyzers. OCCT source files relating to STEP reader and ExprIntrp functionality are generated automatically with Flex/Bison. Checking this option leads to automatic search of Flex/Bison binaries and regeneration of the mentioned files |
| BUILD_SAMPLES_MFC | Boolean | Indicates whether MFC samples should be built together with OCCT. This option is only relevant to Windows platforms |
| BUILD_SAMPLES_QT  | Boolean | Indicates whether QT samples should be built together with OCCT. |
| BUILD_Inspector | Boolean | Indicates whether Inspector should be built together with OCCT. |
| BUILD_DOC_Overview | Boolean | Indicates whether OCCT overview documentation project should be created together with OCCT. It is not built together with OCCT. Checking this option leads to automatic search of Doxygen binaries. Its building calls Doxygen command to generate the documentation in HTML format |
| BUILD_PATCH | Path | Points to the directory recognized as a "patch" for OCCT. If specified, the files from this directory take precedence over the corresponding native OCCT sources. This way you are able to introduce patches to Open CASCADE Technology not affecting the original source distribution |
| BUILD_WITH_DEBUG | Boolean | Enables extended messages of many OCCT algorithms, usually printed to cout. These include messages on internal errors and special cases encountered, timing, etc. |
| BUILD_ENABLE_FPE_SIGNAL_HANDLER | Boolean | Enable/Disable the floating point exceptions (FPE) during DRAW execution only. Corresponding environment variable (CSF_FPE) can be changed manually in custom.bat/sh scripts without regeneration by CMake. |
| CMAKE_CONFIGURATION_TYPES | String | Semicolon-separated CMake configurations |
| INSTALL_DIR          | Path | Points to the installation directory. *INSTALL_DIR* is a synonym of *CMAKE_INSTALL_PREFIX*. The user can specify both *INSTALL_DIR* or *CMAKE_INSTALL_PREFIX* |
| INSTALL_DIR_BIN      | Path | Relative path to the binaries installation directory (absolute path is ${INSTALL_DIR}/${INSTALL_DIR_BIN}) |
| INSTALL_DIR_SCRIPT   | Path | Relative path to the scripts installation directory (absolute path is ${INSTALL_DIR}/${INSTALL_DIR_SCRIPT}) |
| INSTALL_DIR_LIB      | Path | Relative path to the libraries installation directory (absolute path is ${INSTALL_DIR}/${INSTALL_DIR_LIB}) |
| INSTALL_DIR_INCLUDE  | Path | Relative path to the includes installation directory (absolute path is ${INSTALL_DIR}/${INSTALL_DIR_INCLUDE}) |
| INSTALL_DIR_RESOURCE | Path | Relative path to the resources installation directory (absolute path is ${INSTALL_DIR}/${INSTALL_DIR_RESOURCE}) |
| INSTALL_DIR_LAYOUT   | String | Defines the structure of OCCT files (binaries, resources, headers, etc.) for the install directory. Two variants are predefined: for Windows (standard OCCT layout) and for Unix operating systems (standard Linux layout). If needed, the layout can be customized with INSTALL_DIR_* variables |
| INSTALL_DIR_DATA     | Path | Relative path to the data files installation directory (absolute path is ${INSTALL_DIR}/${INSTALL_DIR_DATA}) |
| INSTALL_DIR_SAMPLES  | Path | Relative path to the samples installation directory. Note that only "samples/tcl" folder will be installed. (absolute path is ${INSTALL_DIR}/${INSTALL_DIR_SAMPLES}) |
| INSTALL_DIR_TESTS    | Path | Relative path to the tests installation directory (absolute path is ${INSTALL_DIR}/${INSTALL_DIR_TESTS}) |
| INSTALL_DIR_DOC      | Path | Relative path to the documentation installation directory (absolute path is ${INSTALL_DIR}/${INSTALL_DIR_DOC}) |
| INSTALL_FREETYPE     | Boolean | Indicates whether FreeType binaries should be installed into the installation directory |
| INSTALL_FREEIMAGE    | Boolean | Indicates whether FreeImage binaries should be installed into the installation directory |
| INSTALL_TBB          | Boolean | Indicates whether TBB binaries should be installed into the installation directory |
| INSTALL_VTK          | Boolean | Indicates whether VTK binaries should be installed into the installation directory |
| INSTALL_TCL          | Boolean | Indicates whether TCL binaries should be installed into the installation directory |
| INSTALL_TEST_CASES   | Boolean | Indicates whether non-regression OCCT test scripts should be installed into the installation directory |
| INSTALL_DOC_Overview | Boolean | Indicates whether OCCT overview documentation should be installed into the installation directory |

@note Only the forward slashes ("/") are acceptable in the CMake options defining paths.

@subsubsection build_cmake_3rdparty 3rd party search mechanism

If `3RDPARTY_DIR` directory is defined, then required 3rd party binaries are sought in it, and default system folders are ignored.
The procedure expects to find binary and header files of each 3rd party product in its own sub-directory: *bin*, *lib* and *include*.
The results of the search (achieved on the next pass of the configuration process) are recorded in the corresponding variables:

* `3RDPARTY_<PRODUCT>_DIR` -- path to the 3rdparty directory (with directory name) (e.g. <i>D:/3rdparty/tcltk-86-32</i>);
* `3RDPARTY_<PRODUCT>_LIBRARY_DIR` -- path to the directory containing a library (e.g. <i>D:/3rdparty/tcltk-86-32/lib</i>);
* `3RDPARTY_<PRODUCT>_INCLUDE_DIR` -- path to the directory containing a header file (e.g., <i>D:/3rdparty/tcltk-86-32/include</i>);
* `3RDPARTY_<PRODUCT>_DLL_DIR` -- path to the directory containing a shared library (e.g., <i>D:/3rdparty/tcltk-86-32/bin</i>) This variable is only relevant to Windows platforms.

@note Each library and include directory should be children of the product directory if the last one is defined.

The search process is as follows:

1. Common path: `3RDPARTY_DIR`
2. Path to a particular 3rd-party library: `3RDPARTY_<PRODUCT>_DIR`
3. Paths to headers and binaries:
   1. `3RDPARTY_<PRODUCT>_INCLUDE_DIR`
   2. `3RDPARTY_<PRODUCT>_LIBRARY_DIR`
   3. `3RDPARTY_<PRODUCT>_DLL_DIR`

If a variable of any level is not defined (empty or `<variable name>-NOTFOUND`) and the upper level variable is defined, the content of the non-defined variable will be sought at the next configuration step.
If the search process at level 3 does not find the required files, it seeks in default places.

If a search result (include path, or library path, or dll path) does not meet your expectations, you can change `3RDPARTY_<PRODUCT>_*_DIR` variable,
clear (if they are not empty) `3RDPARTY_<PRODUCT>_DLL_DIR`, `3RDPARTY_<PRODUCT>_INCLUDE_DIR` and `3RDPARTY_<PRODUCT>_LIBRARY_DIR` variables (or clear one of them) and run the configuration process again.

At this time the search will be performed in the newly identified directory and the result will be recorded to corresponding variables (replace old value if it is necessary).
For example, `3RDPARTY_FREETYPE_DIR` variable

    d:/3rdparty/freetype-2.4.10

can be changed to

    d:/3rdparty/freetype-2.5.3

During the configuration process the related variables (`3RDPARTY_FREETYPE_DLL_DIR`, `3RDPARTY_FREETYPE_INCLUDE_DIR` and `3RDPARTY_FREETYPE_LIBRARY_DIR`) will be filled with new found values.

@note The names of searched libraries and header files are hard-coded.
If there is the need to change their names, change appropriate CMake variables (edit CMakeCache.txt file or edit in cmake-gui in advance mode) without reconfiguration:
`3RDPARTY_<PRODUCT>_INCLUDE` for include, `3RDPARTY_<PRODUCT>_LIB` for library and `3RDPARTY_<PRODUCT>_DLL` for shared library.

@subsection build_cmake_gen Projects generation

Once the configuration process is done, the "Generate" button is used to prepare project files for the target IDE.
In our exercise the Visual Studio solution will be automatically created in the build directory.

@subsection build_cmake_build Building

Go to the build folder, start the Visual Studio solution *OCCT.sln* and build it by clicking **Build -> Build Solution**.

@figure{/build/build_occt/images/cmake_image004.png}

By default, the build solution process skips the building of the INSTALL and Overview projects.
When the building process is finished build:
* *Overview* project to generate OCCT overview documentation (if `BUILD_DOC_Overview` variable is checked)
* the *INSTALL* project to run the **installation process**

For this, right-click on the *Overview/INSTALL* project and select **Project Only -> Build Only** -> *Overview/INSTALL* in the solution explorer.

@subsection build_cmake_install Installation

Installation is a process of extracting redistributable resources (binaries, include files etc) from the build directory into the installation one.
The installation directory will be free of project files, intermediate object files and any other information related to the build routines.

Normally you use the installation directory of OCCT to link against your specific application.
The directory structure is as follows:

    data            - data files for OCCT (brep, iges, stp)
    doc             - OCCT overview documentation in HTML format
    inc             - header files
    samples         - samples
    src             - all required source files for OCCT
    tests           - OCCT test suite
    win32\vc14\bind - binary files (installed 3rdparties and occt)
              \libd - libraries (installed 3rdparties and occt)

@note The above example is given for debug configuration.
However, it is generally safe to use the same installation directory for the release build.
In the latter case the contents of install directory will be enriched with subdirectories and files related to the release configuration.
In particular, the binaries directory win64 will be expanded as follows:

    \win32\vc14\bind
               \libd
               \bin
               \lib

If CMake installation flags are enabled for the 3rd party products (e.g. `INSTALL_FREETYPE`), then the corresponding binaries will be copied to the same bin(d) and lib(d) directories together with the native binaries of OCCT.
Such organization of libraries can be especially helpful if your OCCT-based software does not use itself the 3rd parties of Open CASCADE Technology (thus, there is no sense to pack them into dedicated directories).

The installation folder contains the scripts to run *DRAWEXE* (*draw.bat* or *draw.sh*), samples (if they were installed) and overview.html (short-cut for installed OCCT overview documentation).

@subsection build_occt_crossplatform_cmake Cross-compiling (Android)

This section describes the steps to build OCCT libraries for Android from a complete source package with GNU make (makefiles).
The steps on Windows 7 and Ubuntu 15.10 are similar. There is the only one difference: makefiles are built with mingw32-make on Windows and native GNU make on Ubuntu.

Required tools (download and install if it is required):
  - CMake 3.10+ (3.16+ recommended)
  - Android NDK r19+
  - Android SDK API level 21+
  - For Windows: MinGW-w64 7.3+ or Visual Studio 2015+
  - For Linux/macOS: GNU Make 4.0+

Run GUI tool provided by CMake and:
  - Specify the root folder of OCCT (`$CASROOT`, which contains *CMakelists.txt* file) by clicking **Browse Source**.
  - Specify the location (build folder) for CMake generated project files by clicking **Browse Build**.

@figure{/build/build_occt/images/android_image001.png}

Click **Configure** button. It opens the window with a drop-down list of generators supported by CMake project.
Select "MinGW MakeFiles" item from the list
  - Choose "Specify toolchain file for cross-compiling", and click "Next".
@figure{/build/build_occt/images/android_image002.png}

  - Specify a toolchain file at the next dialog to `android.toolchain.cmake`, and click "Finish".
@figure{/build/build_occt/images/android_image003.png}

If `ANDROID_NDK` environment variable is not defined in current OS, add cache entry `ANDROID_NDK` (entry type is `PATH`) -- path to the NDK folder ("Add Entry" button):
@figure{/build/build_occt/images/android_image004.png}

If on Windows the message is appeared:
  "CMake Error: CMake was unable to find a build program corresponding to "MinGW Makefiles" CMAKE_MAKE_PROGRAM is not set. You probably need to select a different build tool.",
specify `CMAKE_MAKE_PROGRAM` to mingw32-make executable.
@figure{/build/build_occt/images/android_image005.png}

How to configure OCCT, see @ref build_cmake_conf "Configure" section taking into account the specific configuration variables for Android:
  - `ANDROID_ABI` = `armeabi-v7a`, `arm64-v8a`, `x86`, or `x86_64`
  - `ANDROID_PLATFORM` = `android-21` (minimum supported API level)
  - `ANDROID_STL` = `c++_shared` (recommended)
  - `BUILD_MODULE_Draw` = `OFF`

@figure{/build/build_occt/images/android_image006.png}

Click **Generate** button and wait until the generation process is finished.
Then makefiles will appear in the build folder (e.g. <i> D:/tmp/occt-android </i>).

Open console and go to the build folder. Type "mingw32-make" (Windows) or "make" (Ubuntu) to start build process:
> mingw32-make
or
> make

Parallel building can be started with using `-jN` argument of "mingw32-make/make", where `N` is the number of building threads:
> mingw32-make -j4
or
> make -j4

Type "mingw32-make/make" with argument "install" to place the libraries to the install folder:
> mingw32-make install
or
> make install
