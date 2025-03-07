# variable description

set (BUILD_LIBRARY_TYPE_DESCR 
"Specifies the type of library to be created. 'Shared' libraries
are linked dynamically and loaded at runtime. 'Static' libraries
are archives of object files for use when linking other targets")

set (BUILD_YACCLEX_DESCR 
"Enables Flex/Bison lexical analyzers. OCCT source files relating to STEP reader and
ExprIntrp functionality are generated automatically with Flex/Bison. Checking this options
leads to automatic search of Flex/Bison binaries and regeneration of the mentioned files")

set (BUILD_RESOURCES_DESCR "Enables regeneration of OCCT resource files")

set (BUILD_WITH_DEBUG_DESCR
"Enables extended messages of many OCCT algorithms, usually printed to cout.
These include messages on internal errors and special cases encountered, timing etc.
Applies only for Debug configuration.")

set (BUILD_SHARED_LIBRARY_NAME_POSTFIX_DESCR
"Append the postfix to names of output libraries")

set (BUILD_SOVERSION_NUMBERS_DESCR
"Version numbers to put into SONAME: 0 - for empty, 1 - for major, 2 - for major.minor, 3 - for major.minor.maintenance")

set (BUILD_RELEASE_DISABLE_EXCEPTIONS_DESCR
"Disables exceptions like Standard_OutOfRange in Release builds.
Defines No_Exception macros for Release builds when enabled (default).
These exceptions are always enabled in Debug builds, but disable in Release for better performance")

set (BUILD_ENABLE_FPE_SIGNAL_HANDLER_DESCR
"Enable/Disable the floating point exceptions (FPE) during DRAW execution only.
Corresponding environment variable (CSF_FPE) can be changed manually
in custom.bat/sh scripts without regeneration by CMake.")

set (BUILD_FORCE_RelWithDebInfo_DESCR
"Generate PDB files within normal Release build.")

set (BUILD_USE_PCH_DESCR
"Use precompiled headers to accelerate the build.
Precompiled headers are generated automatically by Cotire tool.")

set (BUILD_INCLUDE_SYMLINK_DESCR
"Include symbolic links to the source files in the build directory.
OFF - using a reference file with direct include to the origin,
ON - symbolic link to the origin file are created")

# install variables
set (INSTALL_DIR_DESCR 
"The place where built OCCT libraries, headers, test cases (INSTALL_TEST_CASES variable),
samples (INSTALL_SAMPLES_DESCR variable) and certain 3rdparties (INSTALL_TBB and
other similar variables) will be placed during the installation process (building INSTALL project)")

set (INSTALL_DIR_WITH_VERSION_DESCR
"Use OCCT version number as suffix for names of directories")

set (INSTALL_DIR_LAYOUT_DESCR
"Defines structure of OCCT files (binaries, resources, headers etc.) for the install directory.
Two variants are predefined: for Windows (standard OCCT layout) and for Unix operating systems (standard Linux layout).
If needed, layout can be customized with INSTALL_DIR_* variables.")

set (INSTALL_DIR_BIN_DESCR 
"Subdirectory of INSTALL_DIR where binaries will be installed")
set (INSTALL_DIR_INCLUDE_DESCR 
"Subdirectory of INSTALL_DIR where OCCT headers will be installed")
set (INSTALL_DIR_DATA_DESCR 
"Subdirectory of INSTALL_DIR where sample data files will be installed")
set (INSTALL_DIR_DOC_DESCR 
"Subdirectory of INSTALL_DIR where documentation will be installed")
set (INSTALL_DIR_LIB_DESCR 
"Subdirectory of INSTALL_DIR where libraries (.so on Linux, .lib on Windows) will be installed")
set (INSTALL_DIR_RESOURCE_DESCR 
"Subdirectory of INSTALL_DIR where OCCT resource files will be installed")
set (INSTALL_DIR_SAMPLES_DESCR 
"Subdirectory of INSTALL_DIR where samples will be installed")
set (INSTALL_DIR_TESTS_DESCR 
"Subdirectory of INSTALL_DIR where test scripts will be installed")
set (INSTALL_DIR_SCRIPT_DESCR 
"Subdirectory of INSTALL_DIR where scripts will be installed")
set (INSTALL_DIR_CMAKE_DESCR 
"Subdirectory of INSTALL_DIR where CMake configuration files will be installed.
Must be three levels below INSTALL_DIR")

macro (INSTALL_MESSAGE INSTALL_TARGET_VARIABLE INSTALL_TARGET_STRING)
set (${INSTALL_TARGET_VARIABLE}_DESCR
"Indicates whether ${INSTALL_TARGET_STRING} should be installed (building INSTALL
project) into the installation directory (INSTALL_DIR variable)")
endmacro()

INSTALL_MESSAGE (INSTALL_SAMPLES          "OCCT samples")
INSTALL_MESSAGE (INSTALL_TEST_CASES       "non-regression OCCT test scripts")
INSTALL_MESSAGE (INSTALL_DOC_Overview     "OCCT overview documentation (HTML format)")
INSTALL_MESSAGE (INSTALL_FFMPEG           "FFmpeg binaries")
INSTALL_MESSAGE (INSTALL_FREEIMAGE        "FreeImage binaries")
INSTALL_MESSAGE (INSTALL_OPENVR           "OpenVR binaries")
INSTALL_MESSAGE (INSTALL_EIGEN            "EIGEN header files")
INSTALL_MESSAGE (INSTALL_EGL              "EGL binaries")
INSTALL_MESSAGE (INSTALL_GLES2            "OpenGL ES 2.0 binaries")
INSTALL_MESSAGE (INSTALL_FREETYPE         "FreeType binaries")
INSTALL_MESSAGE (INSTALL_TBB              "TBB binaries")
INSTALL_MESSAGE (INSTALL_RAPIDJSON        "RapidJSON header files")
INSTALL_MESSAGE (INSTALL_TCL              "TCL binaries")
INSTALL_MESSAGE (INSTALL_TK               "TK binaries")
INSTALL_MESSAGE (INSTALL_VTK              "VTK binaries ")

# build variables
macro (BUILD_MODULE_MESSAGE BUILD_MODULE_TARGET_VARIABLE BUILD_MODULE_TARGET_STRING)
set (${BUILD_MODULE_TARGET_VARIABLE}_DESCR
"Indicates whether ${BUILD_MODULE_TARGET_STRING} module should be built or not.
It should be noted that some toolkits of the module can be built even if this module
is not checked (this happens if some other modules depend on these toolkits)")
endmacro()

BUILD_MODULE_MESSAGE (BUILD_MODULE_ApplicationFramework "ApplicationFramework")
BUILD_MODULE_MESSAGE (BUILD_MODULE_DataExchange         "DataExchange")
BUILD_MODULE_MESSAGE (BUILD_MODULE_DETools              "DETools")
BUILD_MODULE_MESSAGE (BUILD_MODULE_Draw                 "Draw")
BUILD_MODULE_MESSAGE (BUILD_MODULE_FoundationClasses    "FoundationClasses")
BUILD_MODULE_MESSAGE (BUILD_MODULE_ModelingAlgorithms   "ModelingAlgorithms")
BUILD_MODULE_MESSAGE (BUILD_MODULE_ModelingData         "ModelingData")
BUILD_MODULE_MESSAGE (BUILD_MODULE_Visualization        "Visualization")


set (BUILD_ADDITIONAL_TOOLKITS_DESCR
"Semicolon-separated individual toolkits to include into build process. If you
want to build some particular libraries (toolkits) only, then you may uncheck
all modules in the corresponding BUILD_MODUE_* options and provide the list of
necessary libraries here. Of course, all dependencies will be resolved automatically")

set (BUILD_SAMPLES_MFC_DESCR
"Indicates whether OCCT MFC samples should be built together with OCCT.
These samples show some possibilities of using OCCT and they can be executed
with script samples.bat from the installation directory (INSTALL_DIR)")

set (BUILD_SAMPLES_QT_DESCR
"Indicates whether OCCT Qt samples should be built together with OCCT.
These samples show some possibilities of using OCCT and they can be executed
with script samples.bat from the installation directory (INSTALL_DIR)")

set (BUILD_Inspector_DESCR
"Indicates whether OCCT inspector should be built together with OCCT.
This inspector provides functionality to interactively inspect low-level content
of the OCAF data model, OCCT viewer, etc. have been introduced in OCCT.
It can be executed with script inspector.bat from the installation directory (INSTALL_DIR) or
using 'tinspector' command in DRAW interpretator")

set (BUILD_MODULE_UwpSample_DESCR
"Indicates whether OCCT UWP sample should be built together with OCCT.")

set (BUILD_DOC_Overview_DESCR
"Indicates whether OCCT overview documentation project (Markdown format) should be
created together with OCCT. It is not built together with OCCT. Checking this options
leads to automatic search of Doxygen binaries. Building of it will be call Doxygen command
to generate the documentation in HTML format. The documentation will be available in the
installation directory (overview.bat script) if INSTALL_DOC_Overview variable is checked")

set (3RDPARTY_DIR_DESCR
"The root directory where all required third-party products will be searched. If a
third-party product have been found - corresponding CMake variables will be specified
(VTK: 3RDPARTY_VTK_DIR, 3RDPARTY_VTK_INCLUDE_DIR, 3RDPARTY_VTK_LIBRARY_DIR)")

set (USE_TK_DESCR
"Indicates whether Tk product should be used by Draw Harness for user interface")

set (USE_FREETYPE_DESCR
"Indicates whether FreeType product should be used in OCCT for text rendering using external font files")

set (USE_FFMPEG_DESCR
"Indicates whether FFmpeg framework is used or not. FFmpeg stands for
multimedia data handling, open-source software libraries used for video encoding and decoding.")

set (USE_FREEIMAGE_DESCR
"Indicates whether FreeImage product should be used in OCCT visualization
module for support of popular graphics image formats (PNG, BMP etc)")

set (USE_OPENVR_DESCR
"Indicates whether OpenVR should be used in OCCT visualization module for VR support")

set (USE_RAPIDJSON_DESCR
"Indicates whether RapidJSON product should be used in OCCT DataExchange
module for support of JSON-based formats like glTF")

set (USE_DRACO_DESCR
"Indicates whether Draco mesh decoding library should be used by glTF reader")

set (USE_EGL_DESCR
"Indicates whether EGL should be used in OCCT visualization
module instead of conventional OpenGL context creation APIs")

set (USE_OPENGL_DESCR
"Indicates whether OpenGL desktop should be used in OCCT visualization module")
set (USE_GLES2_DESCR
"Indicates whether OpenGL ES 2.0 should be used in OCCT visualization module")

set (USE_TBB_DESCR
"Indicates whether TBB is used or not. TBB stands for Threading Building Blocks,
the technology of Intel Corp, which comes with different mechanisms and patterns for
injecting parallelism into your application. OCCT remains parallel even without TBB product")

set (USE_VTK_DESCR
"Indicates whether VTK is used or not. VTK stands for Visualization
ToolKit, the technology of Kitware Inc intended for general-purpose scientific
visualization. OCCT comes with a bridge between CAD data representation and
VTK by means of its dedicated VIS component (VTK Integration Services).")

set (USE_XLIB_DESCR "Indicates whether X11 is used or not")

set (USE_D3D_DESCR "Indicates whether optional Direct3D wrapper in OCCT visualization module should be build or not")

macro (BUILD_MODULE MODULE_NAME)
  set (ENABLE_MODULE TRUE)
  set (BUILD_MODULE_${MODULE_NAME} ${ENABLE_MODULE} CACHE BOOL "${BUILD_MODULE_${MODULE_NAME}_DESCR}")
endmacro()
