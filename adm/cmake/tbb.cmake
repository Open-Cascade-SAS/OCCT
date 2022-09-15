# tbb

if (NOT DEFINED 3RDPARTY_DIR)
  message (FATAL_ERROR "3RDPARTY_DIR is not defined.")
endif()

if ((NOT EXISTS "${3RDPARTY_DIR}") OR ("${3RDPARTY_DIR}" STREQUAL ""))
  message (FATAL_ERROR "Directory ${3RDPARTY_DIR} is not set.")
endif()

if (NOT DEFINED INSTALL_TBB AND BUILD_SHARED_LIBS)
  set (INSTALL_TBB OFF CACHE BOOL "${INSTALL_TBB_DESCR}")
endif()

# tbb directory
if (NOT DEFINED 3RDPARTY_TBB_DIR)
  set (3RDPARTY_TBB_DIR "" CACHE PATH "The directory containing tbb")
endif()

if (MSVC AND BUILD_SHARED_LIBS)
  add_definitions (-D__TBB_NO_IMPLICIT_LINKAGE)
  add_definitions (-D__TBBMALLOC_NO_IMPLICIT_LINKAGE)
endif()

# include occt macros. compiler_bitness, os_wiht_bit, compiler
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

# specify TBB folder in connectin with 3RDPARTY_DIR
if (3RDPARTY_DIR AND EXISTS "${3RDPARTY_DIR}")
  #CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_DIR 3RDPARTY_TBB_DIR PATH "The directory containing tbb")

  if (NOT 3RDPARTY_TBB_DIR OR NOT EXISTS "${3RDPARTY_TBB_DIR}")
    FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" TBB TBB_DIR_NAME)
    if (TBB_DIR_NAME)
      set (3RDPARTY_TBB_DIR "${3RDPARTY_DIR}/${TBB_DIR_NAME}" CACHE PATH "The directory containing tbb" FORCE)
    endif()
  endif()
else()
  #set (3RDPARTY_TBB_DIR "" CACHE PATH "The directory containing TBB" FORCE)
endif()

if (NOT DEFINED 3RDPARTY_TBB_INCLUDE_DIR)
  set (3RDPARTY_TBB_INCLUDE_DIR "" CACHE PATH "The directory containing headers of the TBB")
endif()

if (3RDPARTY_TBB_DIR AND EXISTS "${3RDPARTY_TBB_DIR}")
    # check 3RDPARTY_TBB_INCLUDE_DIR for consictency with specified 3RDPARTY_TBB_DIR
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_TBB_DIR 3RDPARTY_TBB_INCLUDE_DIR PATH "The directory containing headers of the TBB")
endif()

# tbb.h
if (NOT 3RDPARTY_TBB_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_TBB_INCLUDE_DIR}")

  set (HEADER_NAMES tbb.h tbb/tbb.h)

  # set 3RDPARTY_TBB_INCLUDE_DIR as notfound, otherwise find_library can't assign a new value to 3RDPARTY_TBB_INCLUDE_DIR
  set (3RDPARTY_TBB_INCLUDE_DIR "3RDPARTY_TBB_INCLUDE_DIR-NOTFOUND" CACHE PATH "the path to tbb.h" FORCE)

  if (3RDPARTY_TBB_DIR AND EXISTS "${3RDPARTY_TBB_DIR}")
    find_path (3RDPARTY_TBB_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                              PATHS ${3RDPARTY_TBB_DIR}
                                              PATH_SUFFIXES include
                                              CMAKE_FIND_ROOT_PATH_BOTH
                                              NO_DEFAULT_PATH)
  else()
    find_path (3RDPARTY_TBB_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                              PATH_SUFFIXES include
                                              CMAKE_FIND_ROOT_PATH_BOTH)
  endif()
endif()

if (3RDPARTY_TBB_INCLUDE_DIR AND EXISTS "${3RDPARTY_TBB_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_TBB_INCLUDE_DIR}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TBB_INCLUDE_DIR)

  set (3RDPARTY_TBB_INCLUDE_DIR "" CACHE PATH "the path to tbb.h" FORCE)
endif()

# Throw execution if 3RDPARTY_TBB_DIR is equal to void string.
if ("${3RDPARTY_TBB_DIR}" STREQUAL "")
  message (FATAL_ERROR "Directory with one TBB have not found.")
endif()

# Searching TBBConfig.cmake and TBBTargets-release.cmake in 3RDPARTY_TBB_DIR
# TBBConfig.cmake - is required, TBBTargets-release.cmake is optional.
file (GLOB_RECURSE TBB_CONFIG_CMAKE_FILE "${3RDPARTY_TBB_DIR}/*TBBConfig.cmake")
if (NOT EXISTS "${TBB_CONFIG_CMAKE_FILE}")
  message (FATAL_ERROR "TBBConfig.cmake has not been found.")
endif()
include ("${TBB_CONFIG_CMAKE_FILE}")

file (GLOB_RECURSE TBB_TARGET_CMAKE_FILE "${3RDPARTY_TBB_DIR}/*TBBTargets-release.cmake")
if (EXISTS "${TBB_TARGET_CMAKE_FILE}")
  include ("${TBB_TARGET_CMAKE_FILE}")
endif()

# We do not know, full path to file is pointed, or local.
# So, we should check it and output FULL PATH to FILE.
macro (TBB_FILE_NAME_TO_FILEPATH FL_NAME FL_PATH)
  if (EXISTS "${FL_NAME}")
    # FL_NAME is full path.
    set (${FL_PATH} "${FL_NAME}")
  else()
    # Here we deal with local path, so assign to var full path to file.
    # Acquire full path.
    set (${FL_PATH} "${3RDPARTY_TBB_DIR}${FL_NAME}")
    if (NOT EXISTS "${${FL_PATH}}")
      message (FATAL_ERROR "TBB: needed file not found (${FL_PATH}).")
    endif()
  endif()
endmacro()

# TARGET_NAME - is target name from oneTBB cmake file
# it is either "TBB::tbb", or "TBB::tbbmalloc"
# LIB_NAME_UC - is library id (TBB or TBBMALLOC)
# PROPERTY_TO_SET - LIBRARY or DLL
macro (WIN_TBB_PARSE TARGET_NAME LIB_NAME PROPERTY_TO_SET)
  set (FILE_NAME "")
  set (FILE_PATH "")
  set (FILE_DIR "")

  if ("${PROPERTY_TO_SET}" STREQUAL "LIBRARY")
    get_target_property (FILE_NAME "${TARGET_NAME}" IMPORTED_IMPLIB_RELEASE)
  else()
    get_target_property (FILE_NAME "${TARGET_NAME}" IMPORTED_LOCATION_RELEASE)
  endif()

  # acquire full path
  TBB_FILE_NAME_TO_FILEPATH("${FILE_NAME}" FILE_PATH)

  get_filename_component (FILE_NAME "${FILE_PATH}" NAME)
  get_filename_component (FILE_DIR "${FILE_PATH}" DIRECTORY)

  if (NOT EXISTS "${FILE_DIR}/${FILE_NAME}")
    set (3RDPARTY_${LIB_NAME}_${PROPERTY_TO_SET} "" CACHE FILEPATH "${LIB_NAME} library" FORCE)
    set (3RDPARTY_${LIB_NAME}_${PROPERTY_TO_SET}_DIR "" CACHE PATH "The directory containing ${LIB_NAME} shared library")

    if ("${PROPERTY_TO_SET}" STREQUAL "LIBRARY")
      list (APPEND 3RDPARTY_NO_LIBS 3RDPARTY_${LIB_NAME}_${PROPERTY_TO_SET}_DIR)
    else()
      list (APPEND 3RDPARTY_NO_DLLS 3RDPARTY_${LIB_NAME}_${PROPERTY_TO_SET}_DIR)
    endif()
  else()
    set (3RDPARTY_${LIB_NAME}_${PROPERTY_TO_SET} "${FILE_DIR}/${FILE_NAME}" CACHE FILEPATH "${LIB_NAME} library" FORCE)
    set (3RDPARTY_${LIB_NAME}_${PROPERTY_TO_SET}_DIR "${FILE_DIR}" CACHE PATH "The directory containing ${LIB_NAME} shared library")

    if ("${PROPERTY_TO_SET}" STREQUAL "LIBRARY")
      list (APPEND 3RDPARTY_LIBRARY_DIRS "${FILE_DIR}")
    else()
      list (APPEND 3RDPARTY_DLL_DIRS "${FILE_DIR}")
    endif()
  endif()
endmacro()

# TARGET_NAME - is target name from oneTBB cmake file
# it is either "TBB::tbb", or "TBB::tbbmalloc"
# LIB_NAME_UC - is library id (TBB or TBBMALLOC)
macro (LIN_TBB_PARSE TARGET_NAME LIB_NAME)
  set (FILE_NAME "")
  set (FILE_PATH "")
  set (FILE_DIR "")

  get_target_property (FILE_NAME "${TARGET_NAME}" IMPORTED_LOCATION_RELEASE)

  # acquire full path
  TBB_FILE_NAME_TO_FILEPATH("${FILE_NAME}" FILE_PATH)

  get_filename_component (FILE_NAME "${FILE_PATH}" NAME)
  get_filename_component (FILE_DIR "${FILE_PATH}" DIRECTORY)

  if (NOT EXISTS "${FILE_DIR}/${FILE_NAME}")
    set (3RDPARTY_${LIB_NAME}_LIBRARY "" CACHE FILEPATH "${LIB_NAME} library" FORCE)
    set (3RDPARTY_${LIB_NAME}_LIBRARY_DIR "" CACHE PATH "The directory containing ${LIB_NAME} shared library")

    list (APPEND 3RDPARTY_NO_LIBS 3RDPARTY_${LIB_NAME}_LIBRARY_DIR)
  else()
    set (3RDPARTY_${LIB_NAME}_LIBRARY "${FILE_DIR}/${FILE_NAME}" CACHE FILEPATH "${LIB_NAME} library" FORCE)
    set (3RDPARTY_${LIB_NAME}_LIBRARY_DIR "${FILE_DIR}" CACHE PATH "The directory containing ${LIB_NAME} shared library")

    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_${LIB_NAME}_LIBRARY_DIR}")
  endif()
endmacro()

if (WIN32)
  # Here we should set:
  #  - 3RDPARTY_*_LIBRARY
  #  - 3RDPARTY_*_LIBRARY_DIR
  #  - 3RDPARTY_*_DLL
  #  - 3RDPARTY_*_DLL_DIR
  # where * - is TBB or TBBMALLOC

  separate_arguments (CSF_TBB)
  foreach (LIB IN LISTS CSF_TBB)
    string(TOLOWER "${LIB}" LIB_LOWER)
    string(TOUPPER "${LIB}" LIB_UPPER)
    WIN_TBB_PARSE("TBB::${LIB_LOWER}" "${LIB_UPPER}" "LIBRARY")
    WIN_TBB_PARSE("TBB::${LIB_LOWER}" "${LIB_UPPER}" "DLL")
  endforeach()
else()
  # Here we should set:
  #  - 3RDPARTY_*_LIBRARY
  #  - 3RDPARTY_*_LIBRARY_DIR

  separate_arguments (CSF_TBB)
  foreach (LIB IN LISTS CSF_TBB)
    string(TOLOWER "${LIB}" LIB_LOWER)
    string(TOUPPER "${LIB}" LIB_UPPER)
    LIN_TBB_PARSE("TBB::${LIB_LOWER}" "${LIB_UPPER}")
  endforeach()
endif()

# install tbb/tbbmalloc
if (INSTALL_TBB)
  OCCT_MAKE_OS_WITH_BITNESS()
  OCCT_MAKE_COMPILER_SHORT_NAME()

  if (WIN32)
    if (SINGLE_GENERATOR)
      foreach (LIB IN LISTS CSF_TBB)
        string(TOUPPER "${LIB}" LIB_UPPER)
        install (FILES ${3RDPARTY_${LIB_UPPER}_DLL} DESTINATION "${INSTALL_DIR_BIN}")
      endforeach()
    else()
      foreach (LIB IN LISTS CSF_TBB)
        string(TOUPPER "${LIB}" LIB_UPPER)
        install (FILES ${3RDPARTY_${LIB_UPPER}_DLL} CONFIGURATIONS Release DESTINATION "${INSTALL_DIR_BIN}")
        install (FILES ${3RDPARTY_${LIB_UPPER}_DLL} CONFIGURATIONS RelWithDebInfo DESTINATION "${INSTALL_DIR_BIN}i")
        install (FILES ${3RDPARTY_${LIB_UPPER}_DLL} CONFIGURATIONS Debug DESTINATION "${INSTALL_DIR_BIN}d")
      endforeach()
    endif()
  else()
    if (SINGLE_GENERATOR)
      foreach (LIB IN LISTS CSF_TBB)
        string(TOUPPER "${LIB}" LIB_UPPER)
        install (FILES ${3RDPARTY_${LIB_UPPER}_LIBRARY} DESTINATION "${INSTALL_DIR_LIB}")
      endforeach()
    else()
      foreach (LIB IN LISTS CSF_TBB)
        string(TOUPPER "${LIB}" LIB_UPPER)
        install (FILES ${3RDPARTY_${LIB_UPPER}_LIBRARY} CONFIGURATIONS Release DESTINATION "${INSTALL_DIR_LIB}")
        install (FILES ${3RDPARTY_${LIB_UPPER}_LIBRARY} CONFIGURATIONS RelWithDebInfo DESTINATION "${INSTALL_DIR_LIB}i")
        install (FILES ${3RDPARTY_${LIB_UPPER}_LIBRARY} CONFIGURATIONS Debug DESTINATION "${INSTALL_DIR_LIB}d")
      endforeach()
    endif()
  endif()
endif()
foreach (LIB IN LISTS CSF_TBB)
  string(TOUPPER "${LIB}" LIB_UPPER)
  mark_as_advanced (3RDPARTY_${LIB_UPPER}_LIBRARY 3RDPARTY_${LIB_UPPER}_DLL)
endforeach()

if (INSTALL_TBB)
  set (USED_3RDPARTY_TBB_DIR "")
else()
  # the library directory for using by the executable
  if (WIN32)
    set (USED_3RDPARTY_TBB_DIR ${3RDPARTY_TBB_DLL_DIR})
  else()
    set (USED_3RDPARTY_TBB_DIR ${3RDPARTY_TBB_LIBRARY_DIR})
  endif()
endif()
