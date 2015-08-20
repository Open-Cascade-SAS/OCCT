# variable description

# 
set (APPLY_OCCT_PATCH_DIR_DESCR "The directory contaning patched OCCT sources that should be used")
set (BUILD_LIBRARY_TYPE_DESCR "The type of further OCCT libraries: shared or static")

set (REBUILD_PLATFORM_DEPENDENT_CODE_DESCR "OCCT has already compiled platform-depending code and it can be rebult once again (*.yacc/*.lex)")

# install variables
set (INSTALL_DIR_DESCR "The place where built OCCT libraries, headers, test cases (INSTALL_OCCT_TEST_CASES variable), \n samples (INSTALL_OCCT_SAMPLES_DESCR variable) and certain 3rdparties (INSTALL_GL2PS, INSTALL_TBB and other similar variables) \n will be placed during the installation process (building INSTALL project)")

set (INSTALL_OCCT_SAMPLES_DESCR "The installation process (building INSTALL project) also places OCCT samples into installation directory (INSTALL_DIR variable)")
set (INSTALL_OCCT_TEST_CASES_DESCR "The installation process (building INSTALL project) also places OCCT test cases into installation directory (INSTALL_DIR variable)")
set (INSTALL_DOC_OcctOverview_DESCR "The installation process (building INSTALL project) also places OCCT overview documentation (HTML format) \n into installation directory (INSTALL_DIR variable)") 

set (INSTALL_FREEIMAGE_DESCR "The installation process (building INSTALL project) also places FreeImage library \n into installation directory (INSTALL_DIR variable). After that OCCT can use this library in portable mode")
set (INSTALL_FREEIMAGEPLUS_DESCR "The installation process (building INSTALL project) also places FreeImagePlus library into \n installation directory (INSTALL_DIR variable). After that OCCT can use this library in portable mode")
set (INSTALL_FREETYPE_DESCR "The installation process (building INSTALL project) also places FreeType library into \n installation directory (INSTALL_DIR variable). After that OCCT can use this library in portable mode")
set (INSTALL_GL2PS_DESCR "The installation process (building INSTALL project) also places GL2PS library into \n installation directory (INSTALL_DIR variable). After that OCCT can use this library in portable mode")
set (INSTALL_TBB_DESCR "The installation process (building INSTALL project) also places tbb and tbbmalloc libraries into \n installation directory (INSTALL_DIR variable). After that OCCT can use these libraries in portable mode")
set (INSTALL_TCL_DESCR "The installation process (building INSTALL project) also places Tcl (library and its nessecary sources) into \n installation directory (INSTALL_DIR variable). After that OCCT can use Tcl in portable mode")
set (INSTALL_VTK_DESCR "The installation process (building INSTALL project) also places Vtk libraries into \n installation directory (INSTALL_DIR variable). After that OCCT can use these libraries in portable mode")

# build variables
set (BUILD_MODULE_ApplicationFramework_DESCR "Build all toolkits of ApplicationFramework module in the solution")
set (BUILD_MODULE_DataExchange_DESCR "Build all toolkits of DataExchange module in the solution")
set (BUILD_MODULE_Draw_DESCR "Build all toolkits of Draw module in the solution")
set (BUILD_MODULE_FoundationClasses_DESCR "Build all toolkits of FoundationClasses module in the solution")
set (BUILD_MODULE_ModelingAlgorithms_DESCR "Build all toolkits of ModelingAlgorithms module in the solution")
set (BUILD_MODULE_ModelingData_DESCR "Build all toolkits of ModelingData module in the solution")
set (BUILD_MODULE_Visualization_DESCR "Build all toolkits of Visualization module in the solution")

set (BUILD_ADDITIONAL_TOOLKITS_DESCR "Build additional OCCT toolkits (including dependent ones) in the solution")

set (BUILD_MODULE_OcctMfcSamples_DESCR "Build several OCCT MFC sample projects in the solution. \n These samples show some possibilities of using OCCT and they can \n be executed with script samples.bat from the installation directory (INSTALL_DIR)")
set (BUILD_DOC_OcctOverview_DESCR "Build a project containing OCCT overview documentation (Markdown format) \n in the solution. The building of the project executes doxygen command that generates \n the documentation in HTML format. The documentation will be available in the installation \n directory (overview.bat script) if INSTALL_DOC_OcctOverview variable is checked")

set (3RDPARTY_DIR_DESCR "The directory that contain several or all used 3rdparties. \n If the 3rdpartyies have been found - corresponding CMake variables \n will be specified (VTK: 3RDPARTY_VTK_DIR, 3RDPARTY_VTK_INCLUDE_DIR, 3RDPARTY_VTK_LIBRARY_DIR)")

set (USE_FREEIMAGE_DESCR "The solution can use FreeImage")
set (USE_GL2PS_DESCR "The solution can use GL2PS")
set (USE_TBB_DESCR "The solution can use tbb")
set (USE_VTK_DESCR "The solution can use VTK")
set (USE_GLX_DESCR "The solution can use X11 OpenGl on OSX")

macro (BUILD_MODULE MODULE_NAME)
  set (BUILD_MODULE_${MODULE_NAME} ON CACHE BOOL "${BUILD_MODULE_${MODULE_NAME}_DESCR}")
endmacro()
