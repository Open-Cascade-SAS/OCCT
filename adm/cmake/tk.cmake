# tk

if (NOT DEFINED INSTALL_TK AND BUILD_SHARED_LIBS)
  set (INSTALL_TK OFF CACHE BOOL "${INSTALL_TK_DESCR}")
endif()

# tk directory
if (NOT DEFINED 3RDPARTY_TK_DIR)
  set (3RDPARTY_TK_DIR "" CACHE PATH "The directory containing tk")
endif ()

if (NOT 3RDPARTY_TK_DIR AND 3RDPARTY_TCLTK_DIR)
  set (3RDPARTY_TK_DIR "${3RDPARTY_TCLTK_DIR}" CACHE PATH "The directory containing tk" FORCE)
endif()

# tk include directory
if (NOT DEFINED 3RDPARTY_TK_INCLUDE_DIR)
  set (3RDPARTY_TK_INCLUDE_DIR "" CACHE FILEPATH "The directory containing headers of tk")
endif()

if (BUILD_SHARED_LIBS)
  # tk library file (with absolute path)
  if (NOT DEFINED 3RDPARTY_TK_LIBRARY OR NOT 3RDPARTY_TK_LIBRARY_DIR)
    set (3RDPARTY_TK_LIBRARY "" CACHE FILEPATH "tk library" FORCE)
  endif()

  # tk library directory
  if (NOT DEFINED 3RDPARTY_TK_LIBRARY_DIR)
    set (3RDPARTY_TK_LIBRARY_DIR "" CACHE FILEPATH "The directory containing tk library")
  endif()

  # tk shared library (with absolute path)
  if (WIN32)
    if (NOT DEFINED 3RDPARTY_TK_DLL OR NOT 3RDPARTY_TK_DLL_DIR)
      set (3RDPARTY_TK_DLL "" CACHE FILEPATH "tk shared library" FORCE)
    endif()
  endif()

  # tk shared library directory
  if (WIN32 AND NOT DEFINED 3RDPARTY_TK_DLL_DIR)
    set (3RDPARTY_TK_DLL_DIR "" CACHE FILEPATH "The directory containing tk shared library")
  endif()
endif()

# search for tk in user defined directory
if (NOT 3RDPARTY_TK_DIR AND 3RDPARTY_DIR)
  FIND_PRODUCT_DIR("${3RDPARTY_DIR}" tk TK_DIR_NAME)
  if (TK_DIR_NAME)
    set (3RDPARTY_TK_DIR "${3RDPARTY_DIR}/${TK_DIR_NAME}" CACHE PATH "The directory containing tk" FORCE)
  endif()
endif()

# define paths for default engine
if (3RDPARTY_TK_DIR AND EXISTS "${3RDPARTY_TK_DIR}")
  set (TK_INCLUDE_PATH "${3RDPARTY_TK_DIR}/include")
endif()

# check tk include dir, library dir and shared library dir
COMPLIANCE_PRODUCT_CONSISTENCY(TK)

# use default (CMake) TCL search
find_package(TCL QUIET)

# tk include dir
if (NOT 3RDPARTY_TK_INCLUDE_DIR)
  if (TK_INCLUDE_PATH AND EXISTS "${TK_INCLUDE_PATH}")
    set (3RDPARTY_TK_INCLUDE_DIR "${TK_INCLUDE_PATH}" CACHE FILEPATH "The directory containing headers of TK" FORCE)
  endif()
endif()

if (BUILD_SHARED_LIBS)
  # tk dir and library
  if (NOT 3RDPARTY_TK_LIBRARY)
    if (TK_LIBRARY AND EXISTS "${TK_LIBRARY}")
      set (3RDPARTY_TK_LIBRARY "${TK_LIBRARY}" CACHE FILEPATH "TK library" FORCE)

      if (NOT 3RDPARTY_TK_LIBRARY_DIR)
        get_filename_component (3RDPARTY_TK_LIBRARY_DIR "${3RDPARTY_TK_LIBRARY}" PATH)
        set (3RDPARTY_TK_LIBRARY_DIR "${3RDPARTY_TK_LIBRARY_DIR}" CACHE FILEPATH "The directory containing TK library" FORCE)
      endif()
    endif()
  endif()


  if (WIN32)
    if (NOT 3RDPARTY_TK_DLL)
        set (CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll" ".a")

        set (DLL_FOLDER_FOR_SEARCH "")
        if (3RDPARTY_TK_DLL_DIR)
          set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TK_DLL_DIR}")
        elseif (3RDPARTY_TK_DIR)
          set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TK_DIR}/bin")
        elseif (3RDPARTY_TK_LIBRARY_DIR)
          get_filename_component (3RDPARTY_TK_LIBRARY_DIR_PARENT "${3RDPARTY_TK_LIBRARY_DIR}" PATH)
          set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TK_LIBRARY_DIR_PARENT}/bin")
        endif()

        set (3RDPARTY_TK_DLL "3RDPARTY_TK_DLL-NOTFOUND" CACHE FILEPATH "TK shared library" FORCE)
        find_library (3RDPARTY_TK_DLL NAMES ${CSF_TclTkLibs}
                                            PATHS "${DLL_FOLDER_FOR_SEARCH}"
                                            NO_DEFAULT_PATH)
    endif()
  endif()
endif()

COMPLIANCE_PRODUCT_CONSISTENCY(TK)

if (BUILD_SHARED_LIBS)
  # tk dir and library
  if (NOT 3RDPARTY_TK_LIBRARY)
    set (3RDPARTY_TK_LIBRARY "3RDPARTY_TK_LIBRARY-NOTFOUND" CACHE FILEPATH "TK library" FORCE)
    find_library (3RDPARTY_TK_LIBRARY NAMES ${CSF_TclTkLibs}
                                            PATHS "${3RDPARTY_TK_LIBRARY_DIR}"
                                            NO_DEFAULT_PATH)

    # search in another place if previous search doesn't find anything
    find_library (3RDPARTY_TK_LIBRARY NAMES ${CSF_TclTkLibs}
                                            PATHS "${3RDPARTY_TK_DIR}/lib"
                                            NO_DEFAULT_PATH)


    if (NOT 3RDPARTY_TK_LIBRARY OR NOT EXISTS "${3RDPARTY_TK_LIBRARY}")
      set (3RDPARTY_TK_LIBRARY "" CACHE FILEPATH "TK library" FORCE)
    endif()

    if (NOT 3RDPARTY_TK_LIBRARY_DIR AND 3RDPARTY_TK_LIBRARY)
      get_filename_component (3RDPARTY_TK_LIBRARY_DIR "${3RDPARTY_TK_LIBRARY}" PATH)
      set (3RDPARTY_TK_LIBRARY_DIR "${3RDPARTY_TK_LIBRARY_DIR}" CACHE FILEPATH "The directory containing TK library" FORCE)
    endif()
  endif()

  set (3RDPARTY_TK_LIBRARY_VERSION "")
  if (3RDPARTY_TK_LIBRARY AND EXISTS "${3RDPARTY_TK_LIBRARY}")
    get_filename_component (TK_LIBRARY_NAME "${3RDPARTY_TK_LIBRARY}" NAME)
    string(REGEX REPLACE "^.*tk([0-9]\\.*[0-9]).*$" "\\1" TK_LIBRARY_VERSION "${TK_LIBRARY_NAME}")

    if (NOT "${TK_LIBRARY_VERSION}" STREQUAL "${TK_LIBRARY_NAME}")
      set (3RDPARTY_TK_LIBRARY_VERSION "${TK_LIBRARY_VERSION}")
    else() # if the version isn't found - seek other library with 8.6 or 8.5 version in the same dir
      message (STATUS "Info: TK version isn't found")
    endif()
  endif()

  set (3RDPARTY_TK_LIBRARY_VERSION_WITH_DOT "")
  if (3RDPARTY_TK_LIBRARY_VERSION)
    string (REGEX REPLACE "^.*([0-9])[^0-9]*[0-9].*$" "\\1" 3RDPARTY_TK_MAJOR_VERSION "${3RDPARTY_TK_LIBRARY_VERSION}")
    string (REGEX REPLACE "^.*[0-9][^0-9]*([0-9]).*$" "\\1" 3RDPARTY_TK_MINOR_VERSION "${3RDPARTY_TK_LIBRARY_VERSION}")
    set (3RDPARTY_TK_LIBRARY_VERSION_WITH_DOT "${3RDPARTY_TK_MAJOR_VERSION}.${3RDPARTY_TK_MINOR_VERSION}")
  endif()

  if (WIN32)
    if (NOT 3RDPARTY_TK_DLL)
      set (CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll" ".a")

      set (DLL_FOLDER_FOR_SEARCH "")
      if (3RDPARTY_TK_DLL_DIR)
        set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TK_DLL_DIR}")
      elseif (3RDPARTY_TK_DIR)
        set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TK_DIR}/bin")
      else()
        get_filename_component (3RDPARTY_TK_LIBRARY_DIR_PARENT "${3RDPARTY_TK_LIBRARY_DIR}" PATH)
        set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TK_LIBRARY_DIR_PARENT}/bin")
      endif()

      set (3RDPARTY_TK_DLL "3RDPARTY_TK_DLL-NOTFOUND" CACHE FILEPATH "TK shared library" FORCE)
      find_library (3RDPARTY_TK_DLL NAMES tk${3RDPARTY_TK_LIBRARY_VERSION}
                                          PATHS "${DLL_FOLDER_FOR_SEARCH}"
                                          NO_DEFAULT_PATH)

      if (NOT 3RDPARTY_TK_DLL OR NOT EXISTS "${3RDPARTY_TK_DLL}")
        set (3RDPARTY_TK_DLL "" CACHE FILEPATH "TK shared library" FORCE)
      endif()
    endif()
    if (NOT 3RDPARTY_TK_DLL_DIR AND 3RDPARTY_TK_DLL)
      get_filename_component (3RDPARTY_TK_DLL_DIR "${3RDPARTY_TK_DLL}" PATH)
      set (3RDPARTY_TK_DLL_DIR "${3RDPARTY_TK_DLL_DIR}" CACHE FILEPATH "The directory containing TK shared library" FORCE)
    endif()
  endif()
endif()

# include found paths to common variables
if (3RDPARTY_TK_INCLUDE_DIR AND EXISTS "${3RDPARTY_TK_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_TK_INCLUDE_DIR}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TK_INCLUDE_DIR)
endif()

if (BUILD_SHARED_LIBS)
  if (3RDPARTY_TK_LIBRARY AND EXISTS "${3RDPARTY_TK_LIBRARY}")
    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_TK_LIBRARY_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TK_LIBRARY_DIR)
  endif()

  if (WIN32)
    if (3RDPARTY_TK_DLL OR EXISTS "${3RDPARTY_TK_DLL}")
      list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_TK_DLL_DIR}")
    else()
      list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TK_DLL_DIR)
    endif()
  endif()

  # install tk
  if (INSTALL_TK)
    # include occt macros. compiler_bitness, os_wiht_bit, compiler
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

    OCCT_MAKE_OS_WITH_BITNESS()
    OCCT_MAKE_COMPILER_SHORT_NAME()

    if (WIN32)
      # tk 8.6 requires zlib. install all dlls from tk bin folder that may contain zlib also

      # collect and install all dlls from tk dll dirs
      file (GLOB TK_DLLS  "${3RDPARTY_TK_DLL_DIR}/*.dll")

      if (SINGLE_GENERATOR)
        install (FILES ${TK_DLLS} DESTINATION "${INSTALL_DIR_BIN}")
      else()
        install (FILES ${TK_DLLS}
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_BIN}")
        install (FILES ${TK_DLLS}
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_BIN}i")
        install (FILES ${TK_DLLS}
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_BIN}d")
      endif()
    else()
      get_filename_component(3RDPARTY_TK_LIBRARY_REALPATH ${3RDPARTY_TK_LIBRARY} REALPATH)

      if (SINGLE_GENERATOR)
        install (FILES ${3RDPARTY_TK_LIBRARY_REALPATH} DESTINATION "${INSTALL_DIR_LIB}")
      else()
        install (FILES ${3RDPARTY_TK_LIBRARY_REALPATH}
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_LIB}/")
        install (FILES ${3RDPARTY_TK_LIBRARY_REALPATH}
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_LIB}/i")
        install (FILES ${3RDPARTY_TK_LIBRARY_REALPATH}
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_LIB}d")
      endif()
    endif()

    if (TCL_TCLSH_VERSION)
      # tk is required to install in lib folder (without)
      install (DIRECTORY "${3RDPARTY_TK_LIBRARY_DIR}/tk${TCL_TCLSH_VERSION}"  DESTINATION "${INSTALL_DIR_LIB}")
    else()
      message (STATUS "\nWarning: tkX.X subdir won't be copyied during the installation process.")
      message (STATUS "Try seeking tk within another folder by changing 3RDPARTY_TK_DIR variable.")
    endif()

    set (USED_3RDPARTY_TK_DIR "")
  else()
    # the library directory for using by the executable
    if (WIN32)
      set (USED_3RDPARTY_TK_DIR ${3RDPARTY_TK_DLL_DIR})
    else()
      set (USED_3RDPARTY_TK_DIR ${3RDPARTY_TK_LIBRARY_DIR})
    endif()
  endif()

  mark_as_advanced (3RDPARTY_TK_LIBRARY 3RDPARTY_TK_DLL)
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
  OCCT_CHECK_AND_UNSET (3RDPARTY_TK_LIBRARY)
  OCCT_CHECK_AND_UNSET (3RDPARTY_TK_LIBRARY_DIR)
  OCCT_CHECK_AND_UNSET (3RDPARTY_TK_DLL)
  OCCT_CHECK_AND_UNSET (3RDPARTY_TK_DLL_DIR)
  OCCT_CHECK_AND_UNSET (INSTALL_TK)
endif()
