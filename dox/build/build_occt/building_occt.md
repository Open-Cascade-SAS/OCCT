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

On Linux and macOS we recommend to use libraries maintained by distributive developers, when possible.

@section build_occt_win_cmake Building with CMake tool

This chapter describes the [CMake](https://cmake.org/download/)-based build process, which is now suggested as a standard way to produce the binaries of Open CASCADE Technology from sources.
OCCT requires CMake version 3.1 or later.

CMake is a tool that generates the actual project files for the selected target build system (e.g. Unix makefiles) or IDE (e.g. Visual Studio 2010).
Here we describe the build procedure on the example of Windows platform with Visual Studio 2010.
However, CMake is cross-platform and can be used to build OCCT on Linux and macOS in essentially the same way.

CMake deals with three directories: source, build or binary and installation.

* The source directory is where the sources of OCCT are located in your file system;
* The build or binary directory is where all files created during CMake configuration and generation process will be located. The mentioned process will be described below.
* The installation directory is where binaries will be installed after building the *INSTALL* project that is created by CMake generation process, along with header files and resources required for OCCT use in applications. 

The good practice is not to use the source directory as a build one.
Different configurations should be built in different build directories to avoid conflicts. 
It is however possible to choose one installation directory for several configurations of OCCT (differentiated by platform, bitness, compiler and build type), for example:

    d:/occt/                   - the source directory
    d:/tmp/occt-build-vc10-x64 - the build directory with the generated
                                 solution and other intermediate files created during a CMake tool working
    d:/occt-install            - the installation directory that is
                                 able to contain several OCCT configurations

@subsection build_cmake_conf Configuration process

For unexperienced users we recommend to start with *cmake-gui* -- a cross-platform GUI tool provided by CMake on Windows, Mac and Linux.
A command-line alternative, *ccmake* can also be used.

If the command-line tool is used, run the tool from the build directory with a single argument indicating the source (relative or absolute path) directory, and press *c* to configure:

    cd d:/tmp/occt-build-vc10-x64
    ccmake d:/occt

@figure{/build/build_occt/images/cmake_image000.png}

If the GUI tool is used, run this tool without additional arguments and after that specify the source directory by clicking **Browse Source** and the build (binary) one by clicking **Browse Build**:

@figure{/build/build_occt/images/cmake_image001.png}

@note Each configuration of the project should be built in its own directory.
When building multiple configurations it is suggested to indicate in the name of build directories the system, bitness and compiler (e.g., <i>d:/occt/build/win32-vc10</i>).

Once the source and build directories are selected, "Configure" button should be pressed in order to start manual configuration process.
It begins with selection of a target configurator. It is "Visual Studio 10 2010 Win64" in our example.

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
| USE_FREETYPE  | Boolean | Indicates whether FreeType  product should be used in OCCT for text rendering |
| USE_FREEIMAGE | Boolean | Indicates whether FreeImage product should be used in OCCT visualization module for support of popular graphics image formats (PNG, BMP, etc.) |
| USE_OPENVR    | Boolean | Indicates whether OpenVR    product should be used in OCCT visualization module for support of Virtual Reality |
| USE_OPENGL    | Boolean | Indicates whether TKOpenGl   graphic driver using OpenGL library (desktop) should be built within OCCT visualization module |
| USE_GLES2     | Boolean | Indicates whether TKOpenGles graphic driver using OpenGL ES library (embedded OpenGL) should be built within OCCT visualization module |
| USE_RAPIDJSON | Boolean | Indicates whether RapidJSON product should be used in OCCT Data Exchange module for support of glTF mesh file format |
| USE_DRACO     | Boolean | Indicates whether Draco     product should be used in OCCT Data Exchange module for support of Draco compression in glTF mesh file format |
| USE_TK        | Boolean | Indicates whether Tcl/Tk product should be used in OCCT Draw Harness module for user interface (in addition to Tcl, which is mandatory for Draw Harness) |
| USE_TBB       | Boolean | Indicates whether TBB (Threading Building Blocks) 3rd party is used or not. Note that OCCT remains parallel even without TBB product |
| USE_VTK       | Boolean | Indicates whether VTK 3rd party is used or not. OCCT comes with a bridge between CAD data representation and VTK by means of its dedicated VIS component (VTK Integration Services). You may skip this 3rd party unless you are planning to use VTK visualization for OCCT geometry. See the official documentation @ref occt_user_guides__vis for the details on VIS |
| 3RDPARTY_DIR | Path | Defines the root directory where all required 3rd party products will be searched. Once you define this path it is very convenient to click "Configure" button in order to let CMake automatically detect all necessary products|
| 3RDPARTY_FREETYPE_* | Path | Path to FreeType binaries |
| 3RDPARTY_TCL_* 3RDPARTY_TK_* | Path | Path to Tcl/Tk binaries |
| 3RDPARTY_FREEIMAGE* | Path | Path to FreeImage binaries |
| 3RDPARTY_TBB*  | Path | Path to TBB binaries |
| 3RDPARTY_VTK_* | Path | Path to VTK binaries |
| BUILD_MODULE_<MODULE>| Boolean | Indicates whether the corresponding OCCT module should be built or not. It should be noted that some toolkits of a module can be built even if this module is not checked (this happens if some other modules depend on these toolkits). The main modules and their descriptions can be found in @ref user_guides |
| BUILD_LIBRARY_TYPE | String |  Specifies the type of library to be created. "Shared" libraries are linked dynamically and loaded at runtime. "Static" libraries are archives of object files used when linking other targets. Note that Draw Harness plugin system is incompatible with "Static" builds, and therefore it is disabled for these builds.|
| BUILD_ADDITIONAL_TOOLKITS | String | Semicolon-separated individual toolkits to include into build process. If you want to build some particular libraries (toolkits) only, then you may uncheck all modules in the corresponding *BUILD_MODUE_\<MODULE\>* options and provide the list of necessary libraries here. Of course, all dependencies will be resolved automatically |
| BUILD_YACCLEX | Boolean | Enables Flex/Bison lexical analyzers. OCCT source files relating to STEP reader and ExprIntrp functionality are generated automatically with Flex/Bison. Checking this option leads to automatic search of Flex/Bison binaries and regeneration of the mentioned files |
| BUILD_SAMPLES_MFC | Boolean | Indicates whether MFC samples should be built together with OCCT. This option is only relevant to Windows platforms |
| BUILD_SAMPLES_QT  | Boolean | Indicates whether QT samples should be built together with OCCT. |
| BUILD_Inspector | Boolean | Indicates whether Inspector should be built together with OCCT. |
| BUILD_DOC_Overview | Boolean | Indicates whether OCCT overview documentation project should be created together with OCCT. It is not built together with OCCT. Checking this option leads to automatic search of Doxygen binaries. Its building calls Doxygen command to generate the documentation in HTML format |
| BUILD_PATCH | Path | Points to the directory recognized as a "patch" for OCCT. If specified, the files from this directory take precedence over the corresponding native OCCT sources. This way you are able to introduce patches to Open CASCADE Technology not affecting the original source distribution |
| BUILD_WITH_DEBUG | Boolean | Enables extended messages of many OCCT algorithms, usually printed to cout. These include messages on internal errors and special cases encountered, timing, etc. |
| BUILD_ENABLE_FPE_SIGNAL_HANDLER | Boolean | Enable/Disable the floating point exceptions (FPE) during DRAW execution only. Corresponding environment variable (CSF_FPE) can be changed manually in custom.bat/sh scripts without regeneration by CMake. |
| BUILD_CPP_STANDARD | String | Employ corresponding c++ standard (C++11, C++14, ..C++23) for building OCCT |
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
    win32\vc10\bind - binary files (installed 3rdparties and occt)
              \libd - libraries (installed 3rdparties and occt)

@note The above example is given for debug configuration.
However, it is generally safe to use the same installation directory for the release build.
In the latter case the contents of install directory will be enriched with subdirectories and files related to the release configuration.
In particular, the binaries directory win64 will be expanded as follows:

    \win32\vc10\bind
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
  - CMake 3.0+
  - [Cross-compilation toolchain for CMake](https://github.com/taka-no-me/android-cmake)
  - [Android NDK r12+](https://developer.android.com/ndk/downloads)
  - GNU Make: MinGW v4.82+ for [Windows](https://www.mingw-w64.org/), GNU Make 4.0 for Ubuntu.

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
  - `ANDROID_ABI` = `armeabi-v7a`
  - `ANDROID_NATIVE_API_LEVEL` = `15`
  - `ANDROID_NDK_LAYOUT` is equal to `CMAKE_BUILD_TYPE` variable
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

@section build_occt_genproj Building with Genproj tool

**genproj** is a legacy tool (originated from command "wgenproj" in WOK) for generation of Visual Studio, Code::Blocks, Qt Creator (qmake), and XCode project files for building Open CASCADE Technology.
These project files are placed inside OCCT directory (in *adm* subfolder) and use relative paths, thus can be moved together with sources.
The project files included in official distribution of OCCT are generated by this tool.

@note If you have official distribution with project files included, you can use them directly without a need to call **genproj**.

**genproj** is a less flexible alternative to use of CMake build system (see @ref build_occt_win_cmake), but still has some small features useful for OCCT development.

@subsection build_genproj Configuration process

The environment is defined in the file *custom.sh* (on Linux and macOS) or *custom.bat* (on Windows) which can be edited directly:

* `ARCH` -- architecture (32 or 64), affects only `PATH` variable for execution
* `HAVE_*` -- flags to enable or disable use of optional third-party products
* `CSF_OPT_*` -- paths to search for includes and binaries of all used  third-party products
* `SHORTCUT_HEADERS` -- defines method for population of folder *inc* by header files. Supported methods are:
  * *Copy* - headers will be copied from *src*;
  * *ShortCut* - short-cut header files will be created, redirecting to same-named header located in *src*;
  * *HardLink* - hard links to headers located in *src* will be created.
* `VCVER` -- specification of format of project files, defining also version of Visual Studio to be used, and default name of the sub-folder for binaries:
* Add paths to includes of used third-party libraries in variable `CSF_OPT_INC`.
* Add paths to their binary libraries in variable `CSF_OPT_LIB64`.
* For optional third-party libraries, set corresponding environment variable `HAVE_<LIBRARY_NAME>` to either *false*,  e.g. `export HAVE_FREEIMAGE=false`.

| VCVER     | Visual Studio version | Windows Platform                 | Binaries folder name |
|-----------|-----------------------|----------------------------------|----------------------|
| vc10      | 2010 (10)             | Desktop (Windows API)            | vc10 |
| vc11      | 2012 (11)             | Desktop (Windows API)            | vc11 |
| vc12      | 2013 (12)             | Desktop (Windows API)            | vc12 |
| vc14      | 2015 (14)             | Desktop (Windows API)            | vc14 |
| vc14-uwp  | 2015 (14)             | UWP (Universal Windows Platform) | vc14-uwp |
| vc141     | 2017 (15)             | Desktop (Windows API)            | vc14 |
| vc141-uwp | 2017 (15)             | UWP (Universal Windows Platform) | vc14-uwp |
| vc142     | 2019 (16)             | Desktop (Windows API)            | vc14 |
| vc142-uwp | 2019 (16)             | UWP (Universal Windows Platform) | vc14-uwp |
| vc143     | 2022 (17)             | Desktop (Windows API)            | vc14 |

Alternatively, you can launch **genconf**, a GUI tool allowing to configure build options interactively.
That tool will analyze your environment and propose you to choose available options:

* Type and version of project files to generate (from the list of installed ones, detected by presence of environment variables like `VS100COMNTOOLS` on Windows platform).
* Method to populate folder *inc* (short-cuts by default).
* Location of third-party libraries (usually downloaded from OCCT web site, see above).
* Path to common directory where third-party libraries are located (optional).
* Paths to headers and binaries of the third-party libraries (found automatically basing on previous options; click button "Reset" to update).
* Generation of PDB files within Release build ("Release with Debug info", false by default).

Below are screenshots of **genconf** tool on various platforms (Windows and Linux):
@figure{/build/build_occt/images/genconf_windows.png}
@figure{/build/build_occt/images/genconf_linux.png}

Click "Save" to store the specified configuration in *custom.bat* (Windows) or *custom.sh* (other systems) file.

@subsection build_genproj_generate Projects generation

Launch **genproj** to update content of *inc* folder and generate project files after changes in OCCT code affecting layout or composition of source files.

@note To use **genproj** and **genconf** tools you need to have Tcl installed and accessible by `PATH`.
If Tcl is not found, the tool may prompt you to enter the path to directory where Tcl can be found.

~~~~
  $ genproj.bat
~~~~

Note that if *custom.bat* is not present, **genproj** will start **genconf** to configure environment.

@subsection build_genproj_build Building

@subsubsection build_msvc_build Visual Studio

Launch *msvc.bat* to start Visual Studio with all necessary environment variables defined, and build the whole solution or required toolkits.

The MSVC project files are located in folders <i>adm\\msvc\\vc...</i>.
Binaries are produced in *win32* or *win64* folders.

To start DRAW, launch *draw.bat*.

@subsubsection build_codeblocks_build Code::Blocks

Code::Blocks is a cross-platform IDE which can be used for building OCCT on Linux, macOS and Windows platforms.
The generated Code::Blocks project could be found within subfolder *adm/&lt;OS&gt;/cbp*.

To start **Code::Blocks**, launch script *codeblocks.sh*.
To build all toolkits, click **Build->Build workspace** in the menu bar.

To start *DRAWEXE*, which has been built with **Code::Blocks** on Mac OS X, run the script
~~~~
   ./draw.sh cbp [d]
~~~~
Option *d* is used if OCCT has been built in **Debug** mode.

@subsubsection build_occt_macos_xcode XCode

XCode is an IDE for development on macOS platform and targeting macOS and iOS platforms.
**genproj** tool comes with a legacy XCode project files generator, but CMake is a preferred way for building OCCT on macOS platform.

To start **XCode**, launch script *xcode.sh*.
To build a certain toolkit, select it in **Scheme** drop-down list in XCode toolbar, press **Product** in the menu and click **Build** button.

To build the entire OCCT:
* Create a new empty project (select **File -> New -> Project -> Empty project** in the menu; input the project name, e.g. *OCCT*; then click **Next** and **Create**).
* Drag and drop the *OCCT* folder in the created *OCCT* project in the Project navigator.
* Select **File -> New -> Target -> Aggregate** in the menu. 
* Enter the project name (e.g. *OCCT*) and click **Finish**. The **Build Phases** tab will open.
* Click "+" button to add the necessary toolkits to the target project. It is possible to select all toolkits by pressing **Command+A** combination.

To start *DRAWEXE*, which has been built with XCode on Mac OS X, perform the following steps:

1.Open Terminal application
2.Enter `<OCCT_ROOT_DIR>`:
~~~~
   cd \<OCCT_ROOT_DIR\>
~~~~
3.Run the script
~~~~
   ./draw.sh xcd [d]
~~~~

Option *d* is used if OCCT has been built in **Debug** mode.
