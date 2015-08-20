# tcl

if (NOT DEFINED INSTALL_TCL)
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

# tk include directory
if (NOT DEFINED 3RDPARTY_TK_INCLUDE_DIR)
  set (3RDPARTY_TK_INCLUDE_DIR "" CACHE FILEPATH "The directory containing headers of tk")
endif()

# tcl library file (with absolute path)
if (NOT DEFINED 3RDPARTY_TCL_LIBRARY OR NOT 3RDPARTY_TCL_LIBRARY_DIR)
  set (3RDPARTY_TCL_LIBRARY "" CACHE FILEPATH "tcl library"  FORCE)
endif()

# tcl library directory
if (NOT DEFINED 3RDPARTY_TCL_LIBRARY_DIR)
  set (3RDPARTY_TCL_LIBRARY_DIR "" CACHE FILEPATH "The directory containing tcl library")
endif()

# tk library file (with absolute path)
if (NOT DEFINED 3RDPARTY_TK_LIBRARY OR NOT 3RDPARTY_TK_LIBRARY_DIR)
  set (3RDPARTY_TK_LIBRARY "" CACHE FILEPATH "tk library" FORCE)
endif()

# tk library directory
if (NOT DEFINED 3RDPARTY_TK_LIBRARY_DIR)
  set (3RDPARTY_TK_LIBRARY_DIR "" CACHE FILEPATH "The directory containing tk library")
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

# check tcl/tk include dir, library dir and shared library dir
macro (DIR_SUBDIR_FILE_FIT LIBNAME)
  if (3RDPARTY_TCL_DIR AND EXISTS "${3RDPARTY_TCL_DIR}")
    # tcl/tk include dir
    if (3RDPARTY_${LIBNAME}_INCLUDE_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_INCLUDE_DIR}")
      string (REGEX MATCH "${3RDPARTY_TCL_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_INCLUDE_DIR}")
      if (NOT DOES_PATH_CONTAIN)
        set (3RDPARTY_${LIBNAME}_INCLUDE_DIR "" CACHE FILEPATH "The directory containing headers of ${LIBNAME}" FORCE)
      endif()
    else()
      set (3RDPARTY_${LIBNAME}_INCLUDE_DIR "" CACHE FILEPATH "The directory containing headers of ${LIBNAME}" FORCE)
    endif()

    # tcl/tk library dir
    if (3RDPARTY_${LIBNAME}_LIBRARY_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}")

      string (REGEX MATCH "${3RDPARTY_TCL_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}")
      if (NOT DOES_PATH_CONTAIN)
        set (3RDPARTY_${LIBNAME}_LIBRARY_DIR "" CACHE FILEPATH "The directory containing ${LIBNAME} library" FORCE)
      endif()
    else()
      set (3RDPARTY_${LIBNAME}_LIBRARY_DIR "" CACHE FILEPATH "The directory containing ${LIBNAME} library" FORCE)
    endif()

    # tcl/tk shared library dir
    if (WIN32)
      if (3RDPARTY_${LIBNAME}_DLL_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_DLL_DIR}")
        string (REGEX MATCH "${3RDPARTY_TCL_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_DLL_DIR}")
        if (NOT DOES_PATH_CONTAIN)
          set (3RDPARTY_${LIBNAME}_DLL_DIR "" CACHE FILEPATH "The directory containing ${LIBNAME} shared library" FORCE)
        endif()
      else()
        set (3RDPARTY_${LIBNAME}_DLL_DIR "" CACHE FILEPATH "The directory containing ${LIBNAME} shared library" FORCE)
      endif()
    endif()
  endif()

  # check tcl/tk library
  if (3RDPARTY_${LIBNAME}_LIBRARY_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}")
    if (3RDPARTY_${LIBNAME}_LIBRARY AND EXISTS "${3RDPARTY_${LIBNAME}_LIBRARY}")
      string (REGEX MATCH "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_LIBRARY}")

      if (NOT DOES_PATH_CONTAIN)
        set (3RDPARTY_${LIBNAME}_LIBRARY "" CACHE FILEPATH "${LIBNAME} library" FORCE)
      endif()
    else()
      set (3RDPARTY_${LIBNAME}_LIBRARY "" CACHE FILEPATH "${LIBNAME} library" FORCE)
    endif()
  else()
    set (3RDPARTY_${LIBNAME}_LIBRARY "" CACHE FILEPATH "${LIBNAME} library" FORCE)
  endif()

  # check tcl/tk shared library
  if (WIN32)
    if (3RDPARTY_${LIBNAME}_DLL_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_DLL_DIR}")
      if (3RDPARTY_${LIBNAME}_DLL AND EXISTS "${3RDPARTY_${LIBNAME}_DLL}")
        string (REGEX MATCH "${3RDPARTY_${LIBNAME}_DLL_DIR}" DOES_PATH_CONTAIN "${3RDPARTY_${LIBNAME}_DLL}")

        if (NOT DOES_PATH_CONTAIN)
          set (3RDPARTY_${LIBNAME}_DLL "" CACHE FILEPATH "${LIBNAME} shared library" FORCE)
        endif()
      else()
        set (3RDPARTY_${LIBNAME}_DLL "" CACHE FILEPATH "${LIBNAME} shared library" FORCE)
      endif()
    else()
      set (3RDPARTY_${LIBNAME}_DLL "" CACHE FILEPATH "${LIBNAME} shared library" FORCE)
    endif()
  endif()
endmacro()


DIR_SUBDIR_FILE_FIT(TCL)
DIR_SUBDIR_FILE_FIT(TK)


# use default (CMake) TCL search
find_package(TCL)

foreach (LIBNAME TCL TK)
  string (TOLOWER "${LIBNAME}" LIBNAME_L)

  # tcl/tk include dir
  if (NOT 3RDPARTY_${LIBNAME}_INCLUDE_DIR)
    if (${LIBNAME}_INCLUDE_PATH AND EXISTS "${${LIBNAME}_INCLUDE_PATH}")
      set (3RDPARTY_${LIBNAME}_INCLUDE_DIR "${${LIBNAME}_INCLUDE_PATH}" CACHE FILEPATH "The directory containing headers of ${LIBNAME}" FORCE)
    endif()
  endif()

  # tcl/tk dir and library
  if (NOT 3RDPARTY_${LIBNAME}_LIBRARY)
    if (${LIBNAME}_LIBRARY AND EXISTS "${${LIBNAME}_LIBRARY}")
      set (3RDPARTY_${LIBNAME}_LIBRARY "${${LIBNAME}_LIBRARY}" CACHE FILEPATH "${LIBNAME} library" FORCE)

      if (NOT 3RDPARTY_${LIBNAME}_LIBRARY_DIR)
        get_filename_component (3RDPARTY_${LIBNAME}_LIBRARY_DIR "${3RDPARTY_${LIBNAME}_LIBRARY}" PATH)
        set (3RDPARTY_${LIBNAME}_LIBRARY_DIR "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}" CACHE FILEPATH "The directory containing ${LIBNAME} library" FORCE)
      endif()
    endif()
  endif()


  if (WIN32)
    if (NOT 3RDPARTY_${LIBNAME}_DLL)
        set (CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")

        set (DLL_FOLDER_FOR_SEARCH "")
        if (3RDPARTY_${LIBNAME}_DLL_DIR)
          set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_${LIBNAME}_DLL_DIR}")
        elseif (3RDPARTY_TCL_DIR)
          set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TCL_DIR}/bin")
        elseif (3RDPARTY_${LIBNAME}_LIBRARY_DIR)
          get_filename_component (3RDPARTY_${LIBNAME}_LIBRARY_DIR_PARENT "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}" PATH)
          set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_${LIBNAME}_LIBRARY_DIR_PARENT}/bin")
        endif()

        set (3RDPARTY_${LIBNAME}_DLL "3RDPARTY_${LIBNAME}_DLL-NOTFOUND" CACHE FILEPATH "${LIBNAME} shared library" FORCE)
        find_library (3RDPARTY_${LIBNAME}_DLL NAMES ${LIBNAME_L}86 ${LIBNAME_L}85
                                              PATHS "${DLL_FOLDER_FOR_SEARCH}"
                                              NO_DEFAULT_PATH)
    endif()
  endif()

  DIR_SUBDIR_FILE_FIT(${LIBNAME})


  # tcl/tk dir and library
  if (NOT 3RDPARTY_${LIBNAME}_LIBRARY)
    set (3RDPARTY_${LIBNAME}_LIBRARY "3RDPARTY_${LIBNAME}_LIBRARY-NOTFOUND" CACHE FILEPATH "${LIBNAME} library" FORCE)
    find_library (3RDPARTY_${LIBNAME}_LIBRARY NAMES ${LIBNAME_L}8.6 ${LIBNAME_L}86 ${LIBNAME_L}8.5 ${LIBNAME_L}85
                                              PATHS "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}"
                                              NO_DEFAULT_PATH)

    # search in another place if previous search doesn't find anything
    find_library (3RDPARTY_${LIBNAME}_LIBRARY NAMES ${LIBNAME_L}8.6 ${LIBNAME_L}86 ${LIBNAME_L}8.5 ${LIBNAME_L}85
                                              PATHS "${3RDPARTY_TCL_DIR}/lib"
                                              NO_DEFAULT_PATH)


    if (NOT 3RDPARTY_${LIBNAME}_LIBRARY OR NOT EXISTS "${3RDPARTY_${LIBNAME}_LIBRARY}")
      set (3RDPARTY_${LIBNAME}_LIBRARY "" CACHE FILEPATH "${LIBNAME} library" FORCE)
    endif()

    if (NOT 3RDPARTY_${LIBNAME}_LIBRARY_DIR AND 3RDPARTY_${LIBNAME}_LIBRARY)
      get_filename_component (3RDPARTY_${LIBNAME}_LIBRARY_DIR "${3RDPARTY_${LIBNAME}_LIBRARY}" PATH)
      set (3RDPARTY_${LIBNAME}_LIBRARY_DIR "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}" CACHE FILEPATH "The directory containing ${LIBNAME} library" FORCE)
    endif()
  endif()

  set (3RDPARTY_${LIBNAME}_LIBRARY_VERSION "")
  if (3RDPARTY_${LIBNAME}_LIBRARY AND EXISTS "${3RDPARTY_${LIBNAME}_LIBRARY}")
    get_filename_component (${LIBNAME}_LIBRARY_NAME "${3RDPARTY_${LIBNAME}_LIBRARY}" NAME)
    string(REGEX REPLACE "^.*${LIBNAME_L}([0-9]\\.*[0-9]).*$" "\\1" ${LIBNAME}_LIBRARY_VERSION "${${LIBNAME}_LIBRARY_NAME}")

    if (NOT "${${LIBNAME}_LIBRARY_VERSION}" STREQUAL "${${LIBNAME}_LIBRARY_NAME}")
      set (3RDPARTY_${LIBNAME}_LIBRARY_VERSION "${${LIBNAME}_LIBRARY_VERSION}")
    else() # if the version isn't found - seek other library with 8.6 or 8.5 version in the same dir
      message (STATUS "Info: ${LIBNAME} version isn't found")
    endif()
  endif()

  set (3RDPARTY_${LIBNAME}_LIBRARY_VERSION_WITH_DOT "")
  if (3RDPARTY_${LIBNAME}_LIBRARY_VERSION)
    string (REGEX REPLACE "^.*([0-9])[^0-9]*[0-9].*$" "\\1" 3RDPARTY_${LIBNAME}_MAJOR_VERSION "${3RDPARTY_${LIBNAME}_LIBRARY_VERSION}")
    string (REGEX REPLACE "^.*[0-9][^0-9]*([0-9]).*$" "\\1" 3RDPARTY_${LIBNAME}_MINOR_VERSION "${3RDPARTY_${LIBNAME}_LIBRARY_VERSION}")
    set (3RDPARTY_${LIBNAME}_LIBRARY_VERSION_WITH_DOT "${3RDPARTY_${LIBNAME}_MAJOR_VERSION}.${3RDPARTY_${LIBNAME}_MINOR_VERSION}")
  endif()

  if (WIN32)
    if (NOT 3RDPARTY_${LIBNAME}_DLL)
        set (CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")

        set (DLL_FOLDER_FOR_SEARCH "")
        if (3RDPARTY_${LIBNAME}_DLL_DIR)
          set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_${LIBNAME}_DLL_DIR}")
        elseif (3RDPARTY_TCL_DIR)
          set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_TCL_DIR}/bin")
        else()
          get_filename_component (3RDPARTY_${LIBNAME}_LIBRARY_DIR_PARENT "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}" PATH)
          set (DLL_FOLDER_FOR_SEARCH "${3RDPARTY_${LIBNAME}_LIBRARY_DIR_PARENT}/bin")
        endif()

        set (3RDPARTY_${LIBNAME}_DLL "3RDPARTY_${LIBNAME}_DLL-NOTFOUND" CACHE FILEPATH "${LIBNAME} shared library" FORCE)
        find_library (3RDPARTY_${LIBNAME}_DLL NAMES ${LIBNAME_L}${3RDPARTY_${LIBNAME}_LIBRARY_VERSION}
                                              PATHS "${DLL_FOLDER_FOR_SEARCH}"
                                              NO_DEFAULT_PATH)

        if (NOT 3RDPARTY_${LIBNAME}_DLL OR NOT EXISTS "${3RDPARTY_${LIBNAME}_DLL}")
          set (3RDPARTY_${LIBNAME}_DLL "" CACHE FILEPATH "${LIBNAME} shared library" FORCE)
        endif()

        if (NOT 3RDPARTY_${LIBNAME}_DLL_DIR AND 3RDPARTY_${LIBNAME}_DLL)
          get_filename_component (3RDPARTY_${LIBNAME}_DLL_DIR "${3RDPARTY_${LIBNAME}_DLL}" PATH)
          set (3RDPARTY_${LIBNAME}_DLL_DIR "${3RDPARTY_${LIBNAME}_DLL_DIR}" CACHE FILEPATH "The directory containing ${LIBNAME} shared library" FORCE)
        endif()
    endif()
  endif()

  # include found paths to common variables
  if (3RDPARTY_${LIBNAME}_INCLUDE_DIR AND EXISTS "${3RDPARTY_${LIBNAME}_INCLUDE_DIR}")
    list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_${LIBNAME}_INCLUDE_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${LIBNAME}_INCLUDE_DIR)
  endif()

  if (3RDPARTY_${LIBNAME}_LIBRARY AND EXISTS "${3RDPARTY_${LIBNAME}_LIBRARY}")
    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_${LIBNAME}_LIBRARY_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${LIBNAME}_LIBRARY_DIR})
  endif()

  if (WIN32)
    if (3RDPARTY_${LIBNAME}_DLL OR EXISTS "${3RDPARTY_${LIBNAME}_DLL}")
      list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_${LIBNAME}_DLL_DIR}")
    else()
      list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${LIBNAME}_DLL_DIR)
    endif()
  endif()
endforeach()

# install tcltk
if (INSTALL_TCL)
  # include occt macros. compiler_bitness, os_wiht_bit, compiler
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

  OCCT_MAKE_OS_WITH_BITNESS()
  OCCT_MAKE_COMPILER_SHORT_NAME()

  if (WIN32)
    # tcl 8.6 requires zlib. install all dlls from tcl bin folder that may contain zlib also

    # collect and install all dlls from tcl/tk dll dirs
    file (GLOB TCL_DLLS "${3RDPARTY_TCL_DLL_DIR}/*.dll")
    file (GLOB TK_DLLS  "${3RDPARTY_TK_DLL_DIR}/*.dll")
    install (FILES ${TCL_DLLS} ${TK_DLLS}
             CONFIGURATIONS Release
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
    install (FILES ${TCL_DLLS} ${TK_DLLS}
             CONFIGURATIONS RelWithDebInfo
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bini")
    install (FILES ${TCL_DLLS} ${TK_DLLS}
             CONFIGURATIONS Debug
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")
  else()
    get_filename_component(3RDPARTY_TCL_LIBRARY_REALPATH ${3RDPARTY_TCL_LIBRARY} REALPATH)
    install (FILES ${3RDPARTY_TCL_LIBRARY_REALPATH}
             CONFIGURATIONS Release
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib")
    install (FILES ${3RDPARTY_TCL_LIBRARY_REALPATH}
             CONFIGURATIONS RelWithDebInfo
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libi")
    install (FILES ${3RDPARTY_TCL_LIBRARY_REALPATH}
             CONFIGURATIONS Debug
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libd")

    get_filename_component(3RDPARTY_TK_LIBRARY_REALPATH ${3RDPARTY_TK_LIBRARY} REALPATH)
    install (FILES ${3RDPARTY_TK_LIBRARY_REALPATH}
             CONFIGURATIONS Release
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib")
    install (FILES ${3RDPARTY_TK_LIBRARY_REALPATH}
             CONFIGURATIONS RelWithDebInfo
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libi")
    install (FILES ${3RDPARTY_TK_LIBRARY_REALPATH}
             CONFIGURATIONS Debug
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libd")
  endif()

  if (TCL_TCLSH_VERSION)
    # tcl is required to install in lib folder (without)
    install (DIRECTORY "${3RDPARTY_TCL_LIBRARY_DIR}/tcl8"                    DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib")
    install (DIRECTORY "${3RDPARTY_TCL_LIBRARY_DIR}/tcl${TCL_TCLSH_VERSION}" DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib")
    install (DIRECTORY "${3RDPARTY_TCL_LIBRARY_DIR}/tk${TCL_TCLSH_VERSION}"  DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib")
  else()
    message (STATUS "\nWarning: tclX.X and tkX.X subdirs won't be copyied during the installation process.")
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

mark_as_advanced (3RDPARTY_TCL_LIBRARY 3RDPARTY_TK_LIBRARY 3RDPARTY_TCL_DLL 3RDPARTY_TK_DLL)

# unset all redundant variables
#TCL
OCCT_CHECK_AND_UNSET (TCL_LIBRARY)
OCCT_CHECK_AND_UNSET (TCL_INCLUDE_PATH)
OCCT_CHECK_AND_UNSET (TCL_TCLSH)
#TK
OCCT_CHECK_AND_UNSET (TK_LIBRARY)
OCCT_CHECK_AND_UNSET (TK_INCLUDE_PATH)
OCCT_CHECK_AND_UNSET (TK_WISH)
