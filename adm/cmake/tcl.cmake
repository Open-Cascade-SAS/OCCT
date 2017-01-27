# tcl

if (NOT DEFINED INSTALL_TCL AND BUILD_SHARED_LIBS)
  set (INSTALL_TCL OFF CACHE BOOL "${INSTALL_TCL_DESCR}")
endif()

# tcl directory
if (NOT DEFINED 3RDPARTY_TCL_DIR)
  set (3RDPARTY_TCL_DIR "" CACHE PATH "The directory containing tcl")
endif()

# tcl include directory
if (NOT DEFINED 3RDPARTY_TCL_INCLUDE_DIR)
  set (3RDPARTY_TCL_INCLUDE_DIR "" CACHE FILEPATH "The directory containing headers of tcl")
endif()

if (BUILD_SHARED_LIBS)
  # tcl library file (with absolute path)
  if (NOT DEFINED 3RDPARTY_TCL_LIBRARY OR NOT 3RDPARTY_TCL_LIBRARY_DIR)
    set (3RDPARTY_TCL_LIBRARY "" CACHE FILEPATH "tcl library"  FORCE)
  endif()

  # tcl library directory
  if (NOT DEFINED 3RDPARTY_TCL_LIBRARY_DIR)
    set (3RDPARTY_TCL_LIBRARY_DIR "" CACHE FILEPATH "The directory containing tcl library")
  endif()

  # tcl shared library (with absolute path)
  if (WIN32)
    if (NOT DEFINED 3RDPARTY_TCL_DLL OR NOT 3RDPARTY_TCL_DLL_DIR)
      set (3RDPARTY_TCL_DLL "" CACHE FILEPATH "tcl shared library" FORCE)
    endif()
  endif()

  # tcl shared library directory
  if (WIN32 AND NOT DEFINED 3RDPARTY_TCL_DLL_DIR)
    set (3RDPARTY_TCL_DLL_DIR "" CACHE FILEPATH "The directory containing tcl shared library")
  endif()
endif()

# search for tcl in user defined directory
if (NOT 3RDPARTY_TCL_DIR AND 3RDPARTY_DIR)
  FIND_PRODUCT_DIR("${3RDPARTY_DIR}" tcl TCL_DIR_NAME)
  if (TCL_DIR_NAME)
    set (3RDPARTY_TCL_DIR "${3RDPARTY_DIR}/${TCL_DIR_NAME}" CACHE PATH "The directory containing tcl" FORCE)
  endif()
endif()

# define paths for default engine
if (3RDPARTY_TCL_DIR AND EXISTS "${3RDPARTY_TCL_DIR}")
  set (TCL_INCLUDE_PATH "${3RDPARTY_TCL_DIR}/include")
endif()

# check tcl include dir, library dir and shared library dir
COMPLIANCE_PRODUCT_CONSISTENCY(TCL)

# use default (CMake) TCL search
find_package(TCL QUIET)

# tcl include dir
if (NOT 3RDPARTY_TCL_INCLUDE_DIR)
  if (TCL_INCLUDE_PATH AND EXISTS "${TCL_INCLUDE_PATH}")
    set (3RDPARTY_TCL_INCLUDE_DIR "${TCL_INCLUDE_PATH}" CACHE FILEPATH "The directory containing headers of TCL" FORCE)
  endif()
endif()

if (BUILD_SHARED_LIBS)
  # tcl dir and library
  if (NOT 3RDPARTY_TCL_LIBRARY)
    if (TCL_LIBRARY AND EXISTS "${TCL_LIBRARY}")
      set (3RDPARTY_TCL_LIBRARY "${TCL_LIBRARY}" CACHE FILEPATH "TCL library" FORCE)

      if (NOT 3RDPARTY_TCL_LIBRARY_DIR)
        get_filename_component (3RDPARTY_TCL_LIBRARY_DIR "${3RDPARTY_TCL_LIBRARY}" PATH)
        set (3RDPARTY_TCL_LIBRARY_DIR "${3RDPARTY_TCL_LIBRARY_DIR}" CACHE FILEPATH "The directory containing TCL library" FORCE)
      endif()
    endif()
  endif()


  if (WIN32)
    if (NOT 3RDPARTY_TCL_DLL)
      set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll .a)

      set (DLL_FOLDER_FOR_SEARCH "")
      if (3RDPARTY_TCL_DLL_DIR)
        set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TCL_DLL_DIR}")
      elseif (3RDPARTY_TCL_DIR)
        set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TCL_DIR}/bin")
      elseif (3RDPARTY_TCL_LIBRARY_DIR)
        get_filename_component (3RDPARTY_TCL_LIBRARY_DIR_PARENT "${3RDPARTY_TCL_LIBRARY_DIR}" PATH)
        set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TCL_LIBRARY_DIR_PARENT}/bin")
      endif()

      set (3RDPARTY_TCL_DLL "3RDPARTY_TCL_DLL-NOTFOUND" CACHE FILEPATH "TCL shared library" FORCE)
      find_library (3RDPARTY_TCL_DLL NAMES ${CSF_TclLibs}
                                           PATHS "${DLL_FOLDER_FOR_SEARCH}"
                                           NO_DEFAULT_PATH)
    endif()
  endif()
endif()

COMPLIANCE_PRODUCT_CONSISTENCY(TCL)

if (BUILD_SHARED_LIBS)
  # tcl dir and library
  if (NOT 3RDPARTY_TCL_LIBRARY)
    set (3RDPARTY_TCL_LIBRARY "3RDPARTY_TCL_LIBRARY-NOTFOUND" CACHE FILEPATH "TCL library" FORCE)
    find_library (3RDPARTY_TCL_LIBRARY NAMES ${CSF_TclLibs}
                                             PATHS "${3RDPARTY_TCL_LIBRARY_DIR}"
                                             NO_DEFAULT_PATH)

    # search in another place if previous search doesn't find anything
    find_library (3RDPARTY_TCL_LIBRARY NAMES ${CSF_TclLibs}
                                             PATHS "${3RDPARTY_TCL_DIR}/lib"
                                             NO_DEFAULT_PATH)


    if (NOT 3RDPARTY_TCL_LIBRARY OR NOT EXISTS "${3RDPARTY_TCL_LIBRARY}")
      set (3RDPARTY_TCL_LIBRARY "" CACHE FILEPATH "TCL library" FORCE)
    endif()

    if (NOT 3RDPARTY_TCL_LIBRARY_DIR AND 3RDPARTY_TCL_LIBRARY)
      get_filename_component (3RDPARTY_TCL_LIBRARY_DIR "${3RDPARTY_TCL_LIBRARY}" PATH)
      set (3RDPARTY_TCL_LIBRARY_DIR "${3RDPARTY_TCL_LIBRARY_DIR}" CACHE FILEPATH "The directory containing TCL library" FORCE)
    endif()
  endif()

  set (3RDPARTY_TCL_LIBRARY_VERSION "")
  if (3RDPARTY_TCL_LIBRARY AND EXISTS "${3RDPARTY_TCL_LIBRARY}")
    get_filename_component (TCL_LIBRARY_NAME "${3RDPARTY_TCL_LIBRARY}" NAME)
    string(REGEX REPLACE "^.*tcl([0-9]\\.*[0-9]).*$" "\\1" TCL_LIBRARY_VERSION "${TCL_LIBRARY_NAME}")

    if (NOT "${TCL_LIBRARY_VERSION}" STREQUAL "${TCL_LIBRARY_NAME}")
      set (3RDPARTY_TCL_LIBRARY_VERSION "${TCL_LIBRARY_VERSION}")
    else() # if the version isn't found - seek other library with 8.6 or 8.5 version in the same dir
      message (STATUS "Info: TCL version isn't found")
    endif()
  endif()

  set (3RDPARTY_TCL_LIBRARY_VERSION_WITH_DOT "")
  if (3RDPARTY_TCL_LIBRARY_VERSION)
    string (REGEX REPLACE "^.*([0-9])[^0-9]*[0-9].*$" "\\1" 3RDPARTY_TCL_MAJOR_VERSION "${3RDPARTY_TCL_LIBRARY_VERSION}")
    string (REGEX REPLACE "^.*[0-9][^0-9]*([0-9]).*$" "\\1" 3RDPARTY_TCL_MINOR_VERSION "${3RDPARTY_TCL_LIBRARY_VERSION}")
    set (3RDPARTY_TCL_LIBRARY_VERSION_WITH_DOT "${3RDPARTY_TCL_MAJOR_VERSION}.${3RDPARTY_TCL_MINOR_VERSION}")
  endif()

  if (WIN32)
    if (NOT 3RDPARTY_TCL_DLL)
      set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll .a)

      set (DLL_FOLDER_FOR_SEARCH "")
      if (3RDPARTY_TCL_DLL_DIR)
        set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TCL_DLL_DIR}")
      elseif (3RDPARTY_TCL_DIR)
        set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TCL_DIR}/bin")
      else()
        get_filename_component (3RDPARTY_TCL_LIBRARY_DIR_PARENT "${3RDPARTY_TCL_LIBRARY_DIR}" PATH)
        set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TCL_LIBRARY_DIR_PARENT}/bin")
      endif()

      set (3RDPARTY_TCL_DLL "3RDPARTY_TCL_DLL-NOTFOUND" CACHE FILEPATH "TCL shared library" FORCE)
      find_library (3RDPARTY_TCL_DLL NAMES tcl${3RDPARTY_TCL_LIBRARY_VERSION}
                                           PATHS "${DLL_FOLDER_FOR_SEARCH}"
                                           NO_DEFAULT_PATH)

      if (NOT 3RDPARTY_TCL_DLL OR NOT EXISTS "${3RDPARTY_TCL_DLL}")
        set (3RDPARTY_TCL_DLL "" CACHE FILEPATH "TCL shared library" FORCE)
      endif()
    endif()
    if (NOT 3RDPARTY_TCL_DLL_DIR AND 3RDPARTY_TCL_DLL)
      get_filename_component (3RDPARTY_TCL_DLL_DIR "${3RDPARTY_TCL_DLL}" PATH)
      set (3RDPARTY_TCL_DLL_DIR "${3RDPARTY_TCL_DLL_DIR}" CACHE FILEPATH "The directory containing TCL shared library" FORCE)
    endif()
  endif()

  # include found paths to common variables
  if (3RDPARTY_TCL_INCLUDE_DIR AND EXISTS "${3RDPARTY_TCL_INCLUDE_DIR}")
    list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_TCL_INCLUDE_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TCL_INCLUDE_DIR)
  endif()

  if (3RDPARTY_TCL_LIBRARY AND EXISTS "${3RDPARTY_TCL_LIBRARY}")
    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_TCL_LIBRARY_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TCL_LIBRARY_DIR)
  endif()

  if (WIN32)
    if (3RDPARTY_TCL_DLL OR EXISTS "${3RDPARTY_TCL_DLL}")
      list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_TCL_DLL_DIR}")
    else()
      list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TCL_DLL_DIR)
    endif()
  endif()

  # install tcl
  if (INSTALL_TCL)
    # include occt macros. compiler_bitness, os_wiht_bit, compiler
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

    OCCT_MAKE_OS_WITH_BITNESS()
    OCCT_MAKE_COMPILER_SHORT_NAME()

    if (WIN32)
      # tcl 8.6 requires zlib. install all dlls from tcl bin folder that may contain zlib also

      # collect and install all dlls from tcl dll dirs
      file (GLOB TCL_DLLS "${3RDPARTY_TCL_DLL_DIR}/*.dll")

      if (SINGLE_GENERATOR)
        install (FILES ${TCL_DLLS} DESTINATION "${INSTALL_DIR_BIN}")
      else()
        install (FILES ${TCL_DLLS}
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_BIN}")
        install (FILES ${TCL_DLLS}
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_BIN}i")
        install (FILES ${TCL_DLLS}
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_BIN}d")
      endif()
    else()
      get_filename_component(3RDPARTY_TCL_LIBRARY_REALPATH ${3RDPARTY_TCL_LIBRARY} REALPATH)

      if (SINGLE_GENERATOR)
        install (FILES ${3RDPARTY_TCL_LIBRARY_REALPATH} DESTINATION "${INSTALL_DIR_LIB}")
      else()
        install (FILES ${3RDPARTY_TCL_LIBRARY_REALPATH}
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_LIB}")
        install (FILES ${3RDPARTY_TCL_LIBRARY_REALPATH}
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_LIB}i")
        install (FILES ${3RDPARTY_TCL_LIBRARY_REALPATH}
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_LIB}d")
      endif()
    endif()

    if (TCL_TCLSH_VERSION)
      # tcl is required to install in lib folder (without)
      install (DIRECTORY "${3RDPARTY_TCL_LIBRARY_DIR}/tcl8"                    DESTINATION "${INSTALL_DIR_LIB}")
      install (DIRECTORY "${3RDPARTY_TCL_LIBRARY_DIR}/tcl${TCL_TCLSH_VERSION}" DESTINATION "${INSTALL_DIR_LIB}")
    else()
      message (STATUS "\nWarning: tclX.X subdir won't be copyied during the installation process.")
      message (STATUS "Try seeking tcl within another folder by changing 3RDPARTY_TCL_DIR variable.")
    endif()

    set (USED_3RDPARTY_TCL_DIR "")
  else()
    # the library directory for using by the executable
    if (WIN32)
      set (USED_3RDPARTY_TCL_DIR ${3RDPARTY_TCL_DLL_DIR})
    else()
      set (USED_3RDPARTY_TCL_DIR ${3RDPARTY_TCL_LIBRARY_DIR})
    endif()
  endif()

  mark_as_advanced (3RDPARTY_TCL_LIBRARY 3RDPARTY_TCL_DLL)
endif()

if (TK_FOUND AND 3RDPARTY_TCL_DIR)

  get_filename_component (TK_WISH_ABSOLUTE          "${TK_WISH}"          ABSOLUTE)
  get_filename_component (3RDPARTY_TCL_DIR_ABSOLUTE "${3RDPARTY_TCL_DIR}" ABSOLUTE)

  string (FIND "${TK_WISH_ABSOLUTE}" "${3RDPARTY_TCL_DIR_ABSOLUTE}" THE_SAME_FOLDER)

  if (${THE_SAME_FOLDER} EQUAL 0)
    set (3RDPARTY_TCLTK_DIR "${3RDPARTY_TCL_DIR}")
    message (STATUS "Info: TK is used from TCL folder: ${3RDPARTY_TCLTK_DIR}")
  endif()
endif()

# unset all redundant variables
#TCL
OCCT_CHECK_AND_UNSET (TCL_LIBRARY)
OCCT_CHECK_AND_UNSET (TCL_INCLUDE_PATH)
OCCT_CHECK_AND_UNSET (TCL_TCLSH)
#TK
OCCT_CHECK_AND_UNSET (TK_LIBRARY)
OCCT_CHECK_AND_UNSET (TK_INCLUDE_PATH)
OCCT_CHECK_AND_UNSET (TK_WISH)

if (NOT BUILD_SHARED_LIBS)
  OCCT_CHECK_AND_UNSET (3RDPARTY_TCL_LIBRARY)
  OCCT_CHECK_AND_UNSET (3RDPARTY_TCL_LIBRARY_DIR)
  OCCT_CHECK_AND_UNSET (3RDPARTY_TCL_DLL)
  OCCT_CHECK_AND_UNSET (3RDPARTY_TCL_DLL_DIR)
  OCCT_CHECK_AND_UNSET (INSTALL_TCL)
endif()
