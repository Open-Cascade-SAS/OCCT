# variable description

# 
set (BUILD_PATCH_DESCR 
"Points to the directory recognized as a 'patch' for OCCT. If specified,
the files from this directory take precedence over the corresponding native
OCCT sources. This way you are able to introduce patches to Open CASCADE
Technology not affecting the original source distribution")


set (BUILD_LIBRARY_TYPE_DESCR 
"Specifies the type of library to be created. 'Shared' libraries
are linked dynamically and loaded at runtime. 'Static' libraries
are archives of object files for use when linking other targets")

set (BUILD_YACCLEX_DESCR 
"Enables Flex/Bison lexical analyzers. OCCT source files relating to STEP reader and
ExprIntrp functionality are generated automatically with Flex/Bison. Checking this options
leads to automatic search of Flex/Bison binaries and regeneration of the mentioned files")

set (BUILD_WITH_DEBUG_DESCR
"Enables extended messages of many OCCT algorithms, usually printed to cout. 
These include messages on internal errors and special cases encountered, timing etc")

# install variables
set (INSTALL_DIR_DESCR 
"The place where built OCCT libraries, headers, test cases (INSTALL_TEST_CASES variable),
samples (INSTALL_SAMPLES_DESCR variable) and certain 3rdparties (INSTALL_GL2PS, INSTALL_TBB and
other similar variables) will be placed during the installation process (building INSTALL project)")

macro (INSTALL_MESSAGE INSTALL_TARGET_VARIABLE INSTALL_TARGET_STRING)
set (${INSTALL_TARGET_VARIABLE}_DESCR
"Indicates whether ${INSTALL_TARGET_STRING} should be installed (building INSTALL
project) into the installation directory (INSTALL_DIR variable)")
endmacro()

INSTALL_MESSAGE (INSTALL_SAMPLES          "OCCT samples")
INSTALL_MESSAGE (INSTALL_TEST_CASES       "non-regression OCCT test scripts")
INSTALL_MESSAGE (INSTALL_DOC_OcctOverview "OCCT overview documentation (HTML format)")
INSTALL_MESSAGE (INSTALL_FREEIMAGE        "FreeImage binaries")
INSTALL_MESSAGE (INSTALL_FREEIMAGEPLUS    "FreeImagePlus binaries")
INSTALL_MESSAGE (INSTALL_FREETYPE         "FreeType binaries")
INSTALL_MESSAGE (INSTALL_GL2PS            "GL2PS binaries")
INSTALL_MESSAGE (INSTALL_TBB              "TBB binaries")
INSTALL_MESSAGE (INSTALL_TCL              "TCL binaries")
INSTALL_MESSAGE (INSTALL_TK               "TK binaries")
#INSTALL_MESSAGE (INSTALL_VTK              "VTK binaries ")

# build variables
macro (BUILD_MODULE_MESSAGE BUILD_MODULE_TARGET_VARIABLE BUILD_MODULE_TARGET_STRING)
set (${BUILD_MODULE_TARGET_VARIABLE}_DESCR
"Indicates whether ${BUILD_MODULE_TARGET_STRING} module should be built or not.
It should be noted that some toolkits of the module can be built even if this module
is not checked (this happens if some other modules depend on these toolkits)")
endmacro()

BUILD_MODULE_MESSAGE (BUILD_MODULE_ApplicationFramework "ApplicationFramework")
BUILD_MODULE_MESSAGE (BUILD_MODULE_DataExchange         "DataExchange")
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

set (BUILD_MODULE_OcctMfcSamples_DESCR
"Indicates whether OCCT MFC samples should be built together with OCCT.
These samples show some possibilities of using OCCT and they can be executed
with script samples.bat from the installation directory (INSTALL_DIR)")

set (BUILD_DOC_OcctOverview_DESCR
"Indicates whether OCCT overview documentation project (Markdown format) should be
created together with OCCT. It is not built together with OCCT. Checking this options
leads to automatic search of Doxygen binaries. Building of it will be call Doxygen command
to generate the documentation in HTML format. The documentation will be available in the
installation directory (overview.bat script) if INSTALL_DOC_OcctOverview variable is checked")

set (3RDPARTY_DIR_DESCR
"The root directory where all required 3-rd party products will be searched. If a
3-rd party product have been found - corresponding CMake variables will be specified
(VTK: 3RDPARTY_VTK_DIR, 3RDPARTY_VTK_INCLUDE_DIR, 3RDPARTY_VTK_LIBRARY_DIR)")

set (USE_FREEIMAGE_DESCR
"Indicates whether Freeimage product should be used in OCCT visualization
module for support of popular graphics image formats (PNG, BMP etc)")

set (USE_GL2PS_DESCR
"Indicates whether GL2PS product should be used in OCCT visualization
module for support of vector image formats (PS, EPS etc)")

set (USE_TBB_DESCR
"Indicates whether TBB 3-rd party is used or not. TBB stands for Threading Building Blocks,
the technology of Intel Corp, which comes with different mechanisms and patterns for
injecting parallelism into your application. OCCT remains parallel even without TBB product")

set (USE_VTK_DESCR
"Indicates whether VTK 3-rd party is used or not. VTK stands for Visualization
ToolKit, the technology of Kitware Inc intended for general-purpose scientific
visualization. OCCT comes with a bridge between CAD data representation and
VTK by means of its dedicated VIS component (VTK Integration Services).")

set (USE_GLX_DESCR "Indicates whether X11 OpenGl on OSX is used or not")

set (USE_D3D_DESCR "Indicates whether optional Direct3D wrapper in OCCT visualization module should be build or not")

macro (BUILD_MODULE MODULE_NAME)
  set (BUILD_MODULE_${MODULE_NAME} ON CACHE BOOL "${BUILD_MODULE_${MODULE_NAME}_DESCR}")
endmacro()
