# freetype

if (NOT DEFINED INSTALL_FREETYPE AND BUILD_SHARED_LIBS)
  set (INSTALL_FREETYPE OFF CACHE BOOL "${INSTALL_FREETYPE_DESCR}")
endif()

if (NOT DEFINED 3RDPARTY_FREETYPE_DIR)
  set (3RDPARTY_FREETYPE_DIR "" CACHE PATH "The directory containing freetype")
endif()

# include occt macros. compiler_bitness, os_wiht_bit, compiler
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

OCCT_MAKE_COMPILER_SHORT_NAME()
OCCT_MAKE_COMPILER_BITNESS()

# specify freetype folder in connectin with 3RDPARTY_DIR
if (3RDPARTY_DIR AND EXISTS "${3RDPARTY_DIR}")
  #CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_DIR 3RDPARTY_FREETYPE_DIR PATH "The directory containing freetype")

  if (NOT 3RDPARTY_FREETYPE_DIR OR NOT EXISTS "${3RDPARTY_FREETYPE_DIR}")
    FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" FREETYPE FREETYPE_DIR_NAME)
    if (FREETYPE_DIR_NAME)
      set (3RDPARTY_FREETYPE_DIR "${3RDPARTY_DIR}/${FREETYPE_DIR_NAME}" CACHE PATH "The directory containing freetype" FORCE)
    endif()
  endif()
else()
  #set (3RDPARTY_FREETYPE_DIR "" CACHE PATH "The directory containing freetype" FORCE)
endif()

# define required freetype variables
if (NOT DEFINED 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build)
  set (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build  "" CACHE FILEPATH "the path of ft2build.h")
endif()

if (NOT DEFINED 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2)
  set (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 "" CACHE FILEPATH "the path of freetype2")
endif()

if (BUILD_SHARED_LIBS)
  if (NOT DEFINED 3RDPARTY_FREETYPE_LIBRARY OR NOT 3RDPARTY_FREETYPE_LIBRARY_DIR OR NOT EXISTS "${3RDPARTY_FREETYPE_LIBRARY_DIR}")
    set (3RDPARTY_FREETYPE_LIBRARY               "" CACHE FILEPATH "freetype library" FORCE)
  endif()

  if (NOT DEFINED 3RDPARTY_FREETYPE_LIBRARY_DIR)
    set (3RDPARTY_FREETYPE_LIBRARY_DIR           "" CACHE PATH "The directory containing freetype library")
  endif()

  if (WIN32)
    if (NOT DEFINED 3RDPARTY_FREETYPE_DLL OR NOT 3RDPARTY_FREETYPE_DLL_DIR OR NOT EXISTS "${3RDPARTY_FREETYPE_DLL_DIR}")
      set (3RDPARTY_FREETYPE_DLL                 "" CACHE FILEPATH "freetype shared library" FORCE)
    endif()
  endif()

  if (WIN32)
    if (NOT DEFINED 3RDPARTY_FREETYPE_DLL_DIR)
      set (3RDPARTY_FREETYPE_DLL_DIR               "" CACHE PATH "The directory containing freetype shared library")
    endif()
  endif()
endif()

# check 3RDPARTY_FREETYPE_ paths for consistency with specified 3RDPARTY_FREETYPE_DIR
if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
  CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FREETYPE_DIR 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build FILEPATH "the path to ft2build.h")
  CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FREETYPE_DIR 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 FILEPATH "the path to ftheader.h")
  if (BUILD_SHARED_LIBS)
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FREETYPE_DIR 3RDPARTY_FREETYPE_LIBRARY FILEPATH "the path to freetype library")

    if (3RDPARTY_FREETYPE_LIBRARY AND EXISTS "${3RDPARTY_FREETYPE_LIBRARY}")
      get_filename_component (3RDPARTY_FREETYPE_LIBRARY_DIR "${3RDPARTY_FREETYPE_LIBRARY}" PATH)
      set (3RDPARTY_FREETYPE_LIBRARY_DIR "${3RDPARTY_FREETYPE_LIBRARY_DIR}" CACHE PATH "The directory containing freetype library" FORCE)
    else()
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FREETYPE_DIR 3RDPARTY_FREETYPE_LIBRARY_DIR PATH "The directory containing freetype library")
    endif()

    if (WIN32)
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FREETYPE_DIR 3RDPARTY_FREETYPE_DLL FILEPATH "the path to freetype shared library")

      if (3RDPARTY_FREETYPE_DLL AND EXISTS "${3RDPARTY_FREETYPE_DLL}")
        get_filename_component (3RDPARTY_FREETYPE_DLL_DIR "${3RDPARTY_FREETYPE_DLL}" PATH)
        set (3RDPARTY_FREETYPE_DLL_DIR "${3RDPARTY_FREETYPE_DLL_DIR}" CACHE PATH "The directory containing freetype shared library" FORCE)
      else()
        CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FREETYPE_DIR 3RDPARTY_FREETYPE_DLL_DIR PATH "The directory containing freetype shared library")
      endif()
    endif()
  endif()
endif()

# the FIRST step in search for freetype library and header folders (built-in search engine)

# execute built-in search engine to seek freetype
set (IS_BUILTIN_SEARCH_REQUIRED OFF)
if (NOT 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build OR NOT EXISTS "${3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build}")
  set (IS_BUILTIN_SEARCH_REQUIRED ON)
elseif (NOT 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 OR NOT EXISTS "${3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2}")
  set (IS_BUILTIN_SEARCH_REQUIRED ON)
elseif (NOT 3RDPARTY_FREETYPE_LIBRARY OR NOT EXISTS "${3RDPARTY_FREETYPE_LIBRARY}")
  set (IS_BUILTIN_SEARCH_REQUIRED ON)
#elseif (WIN32)
  #if (NOT 3RDPARTY_FREETYPE_DLL OR NOT EXISTS "${3RDPARTY_FREETYPE_DLL}")
  #  set (IS_BUILTIN_SEARCH_REQUIRED ON)
  #endif()
endif()

if (IS_BUILTIN_SEARCH_REQUIRED)

  # use 3RDPARTY_FREETYPE_DIR if it is specified for freetype search
  if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
    set (CACHED_FREETYPE_DIR $ENV{FREETYPE_DIR})
    set (ENV{FREETYPE_DIR} "${3RDPARTY_FREETYPE_DIR}")
  endif()

  find_package(Freetype)

  # restore ENV{FREETYPE_DIR}
  if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
    set (ENV{FREETYPE_DIR} ${CACHED_FREETYPE_DIR})
  endif()

  # check the found paths for consistency with specified 3RDPARTY_FREETYPE_DIR
  if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FREETYPE_DIR FREETYPE_INCLUDE_DIR_ft2build FILEPATH "the path to ft2build.h")
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FREETYPE_DIR FREETYPE_INCLUDE_DIR_freetype2 FILEPATH "the path to ftheader.h")
    if (BUILD_SHARED_LIBS)
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FREETYPE_DIR FREETYPE_LIBRARY FILEPATH "freetype library")
    endif()
  endif()

  # assign the found paths to corresponding 3RDPARTY_FREETYPE_ variables
  if (NOT 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build OR NOT EXISTS "${3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build}")
    if (FREETYPE_INCLUDE_DIR_ft2build AND EXISTS "${FREETYPE_INCLUDE_DIR_ft2build}")
      set (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build  "${FREETYPE_INCLUDE_DIR_ft2build}" CACHE FILEPATH "the path to ft2build.h" FORCE)
    endif()
  endif()

  if (NOT 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 OR NOT EXISTS "${3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2}")
    if (FREETYPE_INCLUDE_DIR_freetype2 AND EXISTS "${FREETYPE_INCLUDE_DIR_freetype2}")
      set (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2  "${FREETYPE_INCLUDE_DIR_freetype2}" CACHE FILEPATH "the path to ftheader.h" FORCE)
    endif()
  endif()

  if (BUILD_SHARED_LIBS)
    if (NOT 3RDPARTY_FREETYPE_LIBRARY OR NOT EXISTS "${3RDPARTY_FREETYPE_LIBRARY}")
      if (FREETYPE_LIBRARY AND EXISTS "${FREETYPE_LIBRARY}")
        set (3RDPARTY_FREETYPE_LIBRARY  "${FREETYPE_LIBRARY}" CACHE FILEPATH "The path to freetype library" FORCE)
      endif()
    endif()

    if (3RDPARTY_FREETYPE_LIBRARY AND EXISTS "${3RDPARTY_FREETYPE_LIBRARY}")
      get_filename_component (3RDPARTY_FREETYPE_LIBRARY_DIR "${3RDPARTY_FREETYPE_LIBRARY}" PATH)
      set (3RDPARTY_FREETYPE_LIBRARY_DIR "${3RDPARTY_FREETYPE_LIBRARY_DIR}" CACHE PATH "The directory containing freetype library" FORCE)
    else()
      set (3RDPARTY_FREETYPE_LIBRARY_DIR "" CACHE PATH "The directory containing freetype library" FORCE)
    endif()
  endif()
endif()

# the SECOND step in search for freetype library and header folders (additional search algorithms)

# ft2build.h
if (NOT 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build OR NOT EXISTS "${3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build}")
  set (FT2BUILD_NAMES ft2build.h config/ft2build.h freetype/config/ft2build.h)

  # set 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build as notfound, otherwise find_library can't assign a new value to 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build
  set (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build "3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build-NOTFOUND" CACHE FILEPATH "the path to ft2build.h" FORCE)

  # cmake (version < 3.0) doesn't find ft2build.h of freetype (version is >= 2.5.1)
  # do search taking into account freetype structure of 2.5.1 version
  if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
    find_path (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build NAMES ${FT2BUILD_NAMES}
                                                      PATHS ${3RDPARTY_FREETYPE_DIR}
                                                      PATH_SUFFIXES include freetype2 include/freetype2
                                                      CMAKE_FIND_ROOT_PATH_BOTH
                                                      NO_DEFAULT_PATH)
  else()
    find_path (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build NAMES ${FT2BUILD_NAMES}
                                                      PATHS /usr/X11R6 /usr/local/X11R6 /usr/local/X11 /usr/freeware
                                                      PATH_SUFFIXES include/freetype2 include freetype2
                                                      CMAKE_FIND_ROOT_PATH_BOTH)
  endif()
endif()

if (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build AND EXISTS "${3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build)

  set (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build "" CACHE FILEPATH "the path to ft2build.h" FORCE)
endif()

# ftheader.h
if (NOT 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 OR NOT EXISTS "${3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2}")
  set (FTHEADER_NAMES ftheader.h config/ftheader.h freetype/config/ftheader.h)

  # set 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 as notfound, otherwise find_library can't assign a new value to 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2
  set (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 "3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2-NOTFOUND" CACHE FILEPATH "the path to ftheader.h" FORCE)

  if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
    find_path (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 NAMES ${FTHEADER_NAMES}
                                                       HINTS ${3RDPARTY_FREETYPE_DIR}
                                                       PATH_SUFFIXES include/freetype2 include freetype2
                                                       CMAKE_FIND_ROOT_PATH_BOTH
                                                       NO_DEFAULT_PATH)
  else()
    find_path (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 NAMES ${FTHEADER_NAMES}
                                                       PATHS /usr/X11R6 /usr/local/X11R6 /usr/local/X11 /usr/freeware
                                                       PATH_SUFFIXES include/freetype2 include freetype2
                                                       CMAKE_FIND_ROOT_PATH_BOTH)
  endif()
endif()

if (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 AND EXISTS "${3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2)

  set (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 "" CACHE FILEPATH "the path to ftheader.h" FORCE)
endif()

# freetype library
if (BUILD_SHARED_LIBS)
  if (NOT 3RDPARTY_FREETYPE_LIBRARY OR NOT EXISTS "${3RDPARTY_FREETYPE_LIBRARY}")
    set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .so .dylib .a)

    set (FREETYPE_PATH_SUFFIXES lib)
    if (ANDROID)
      set (FREETYPE_PATH_SUFFIXES ${FREETYPE_PATH_SUFFIXES} libs/${ANDROID_ABI})
    endif()

    # set 3RDPARTY_FREETYPE_LIBRARY as notfound, otherwise find_library can't assign a new value to 3RDPARTY_FREETYPE_LIBRARY
    set (3RDPARTY_FREETYPE_LIBRARY "3RDPARTY_FREETYPE_LIBRARY-NOTFOUND" CACHE FILEPATH "The path to freetype library" FORCE)

    if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
      find_library (3RDPARTY_FREETYPE_LIBRARY ${CSF_FREETYPE}
                                              PATHS "${3RDPARTY_FREETYPE_LIBRARY_DIR}" "${3RDPARTY_FREETYPE_DIR}"
                                              PATH_SUFFIXES ${FREETYPE_PATH_SUFFIXES}
                                              CMAKE_FIND_ROOT_PATH_BOTH
                                              NO_DEFAULT_PATH)
    else()
      find_library (3RDPARTY_FREETYPE_LIBRARY ${CSF_FREETYPE}
                                              PATH_SUFFIXES ${FREETYPE_PATH_SUFFIXES}
                                              CMAKE_FIND_ROOT_PATH_BOTH)
    endif()

    if (3RDPARTY_FREETYPE_LIBRARY AND EXISTS "${3RDPARTY_FREETYPE_LIBRARY}")
      get_filename_component (3RDPARTY_FREETYPE_LIBRARY_DIR "${3RDPARTY_FREETYPE_LIBRARY}" PATH)
      set (3RDPARTY_FREETYPE_LIBRARY_DIR "${3RDPARTY_FREETYPE_LIBRARY_DIR}" CACHE PATH "The directory containing freetype library" FORCE)
    else()
      set (3RDPARTY_FREETYPE_LIBRARY_DIR "" CACHE PATH "The directory containing freetype library" FORCE)
    endif()
  endif()

  if (3RDPARTY_FREETYPE_LIBRARY_DIR AND EXISTS "${3RDPARTY_FREETYPE_LIBRARY_DIR}")
    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_FREETYPE_LIBRARY_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FREETYPE_LIBRARY_DIR)

    set (3RDPARTY_FREETYPE_LIBRARY "" CACHE FILEPATH "The path to freetype library" FORCE)
  endif()

  # freetype shared library
  if (WIN32)
    if (NOT 3RDPARTY_FREETYPE_DLL OR NOT EXISTS "${3RDPARTY_FREETYPE_DLL}")

      set (CMAKE_FIND_LIBRARY_SUFFIXES .dll)

      # set 3RDPARTY_FREETYPE_DLL as notfound, otherwise find_library can't assign a new value to 3RDPARTY_FREETYPE_DLL
      set (3RDPARTY_FREETYPE_DLL "3RDPARTY_FREETYPE_DLL-NOTFOUND" CACHE FILEPATH "The path to freetype shared library" FORCE)

      if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
        find_library (3RDPARTY_FREETYPE_DLL ${CSF_FREETYPE}
                                            PATHS "${3RDPARTY_FREETYPE_DIR}"
                                            PATH_SUFFIXES bin
                                            NO_DEFAULT_PATH)
      else()
        find_library (3RDPARTY_FREETYPE_DLL ${CSF_FREETYPE}
                                            PATH_SUFFIXES bin)
      endif()

      if (3RDPARTY_FREETYPE_DLL AND EXISTS "${3RDPARTY_FREETYPE_DLL}")
        get_filename_component (3RDPARTY_FREETYPE_DLL_DIR "${3RDPARTY_FREETYPE_DLL}" PATH)
        set (3RDPARTY_FREETYPE_DLL_DIR "${3RDPARTY_FREETYPE_DLL_DIR}" CACHE PATH "The directory containing freetype library" FORCE)
      else()
        set (3RDPARTY_FREETYPE_DLL_DIR "" CACHE PATH "The directory containing freetype shared library" FORCE)

        set (3RDPARTY_FREETYPE_DLL "" CACHE FILEPATH "freetype shared library" FORCE)
      endif()
    endif()

    if (3RDPARTY_FREETYPE_DLL_DIR OR EXISTS "${3RDPARTY_FREETYPE_DLL_DIR}")
      list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_FREETYPE_DLL_DIR}")
    else()
      list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FREETYPE_DLL_DIR)
    endif()
  endif()

  # install instructions
  if (INSTALL_FREETYPE)
    OCCT_MAKE_OS_WITH_BITNESS()

    if (WIN32)
      if (SINGLE_GENERATOR)
        install (FILES "${3RDPARTY_FREETYPE_DLL}" DESTINATION "${INSTALL_DIR_BIN}")
      else()
        install (FILES "${3RDPARTY_FREETYPE_DLL}"
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_BIN}")
        install (FILES "${3RDPARTY_FREETYPE_DLL}"
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_BIN}i")
        install (FILES "${3RDPARTY_FREETYPE_DLL}"
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_BIN}d")
      endif()
    else()
      get_filename_component(3RDPARTY_FREETYPE_LIBRARY_ABS ${3RDPARTY_FREETYPE_LIBRARY} REALPATH)
      get_filename_component(3RDPARTY_FREETYPE_LIBRARY_NAME ${3RDPARTY_FREETYPE_LIBRARY} NAME)

      if (SINGLE_GENERATOR)
        install (FILES "${3RDPARTY_FREETYPE_LIBRARY_ABS}"
                 DESTINATION "${INSTALL_DIR_LIB}"
                 RENAME ${3RDPARTY_FREETYPE_LIBRARY_NAME}.6)
      else()
        install (FILES "${3RDPARTY_FREETYPE_LIBRARY_ABS}"
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_LIB}"
                 RENAME ${3RDPARTY_FREETYPE_LIBRARY_NAME}.6)
        install (FILES "${3RDPARTY_FREETYPE_LIBRARY_ABS}"
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_LIB}i"
                 RENAME ${3RDPARTY_FREETYPE_LIBRARY_NAME}.6)
        install (FILES "${3RDPARTY_FREETYPE_LIBRARY_ABS}"
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_LIB}d"
                 RENAME ${3RDPARTY_FREETYPE_LIBRARY_NAME}.6)
      endif()
    endif()

    set (USED_3RDPARTY_FREETYPE_DIR "")
  else()
    # the library directory for using by the executable
    if (WIN32)
      set (USED_3RDPARTY_FREETYPE_DIR ${3RDPARTY_FREETYPE_DLL_DIR})
    else()
      set (USED_3RDPARTY_FREETYPE_DIR ${3RDPARTY_FREETYPE_LIBRARY_DIR})
    endif()
  endif()
endif()

# unset all redundant variables
OCCT_CHECK_AND_UNSET(FREETYPE_INCLUDE_DIR_ft2build)
OCCT_CHECK_AND_UNSET(FREETYPE_INCLUDE_DIR_freetype2)
OCCT_CHECK_AND_UNSET(FREETYPE_LIBRARY)

if (BUILD_SHARED_LIBS)
  mark_as_advanced (3RDPARTY_FREETYPE_LIBRARY 3RDPARTY_FREETYPE_DLL)
else()
  OCCT_CHECK_AND_UNSET(3RDPARTY_FREETYPE_DLL)
  OCCT_CHECK_AND_UNSET(3RDPARTY_FREETYPE_DLL_DIR)
  OCCT_CHECK_AND_UNSET(3RDPARTY_FREETYPE_LIBRARY)
  OCCT_CHECK_AND_UNSET(3RDPARTY_FREETYPE_LIBRARY_DIR)
  OCCT_CHECK_AND_UNSET(INSTALL_FREETYPE)
endif()
