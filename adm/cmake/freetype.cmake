# freetype

if (NOT DEFINED INSTALL_FREETYPE)
  set (INSTALL_FREETYPE OFF CACHE BOOL "${INSTALL_FREETYPE_DESCR}")
endif()

if (NOT DEFINED 3RDPARTY_FREETYPE_DIR)
  set (3RDPARTY_FREETYPE_DIR "" CACHE PATH "The directory containing freetype")
endif()

# store ENV{FREETYPE_DIR}
SET (CACHED_FREETYPE_DIR $ENV{FREETYPE_DIR})

# include occt macros. compiler_bitness, os_wiht_bit, compiler
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

OCCT_MAKE_COMPILER_SHORT_NAME()
OCCT_MAKE_COMPILER_BITNESS()

if (NOT ENV{FREETYPE_DIR})
  # search for freetype in user defined directory
  if (NOT 3RDPARTY_FREETYPE_DIR AND 3RDPARTY_DIR)
    FIND_PRODUCT_DIR("${3RDPARTY_DIR}" FREETYPE FREETYPE_DIR_NAME)
    if (FREETYPE_DIR_NAME)
      set (3RDPARTY_FREETYPE_DIR "${3RDPARTY_DIR}/${FREETYPE_DIR_NAME}" CACHE PATH "The directory containing freetype" FORCE)
    endif()
  endif()

  if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
    set (ENV{FREETYPE_DIR} "${3RDPARTY_FREETYPE_DIR}")
  endif()
endif()

if (NOT DEFINED 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build)
  set (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build  "" CACHE FILEPATH "the path of ft2build.h")
endif()

if (NOT DEFINED 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2)
  set (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 "" CACHE FILEPATH "the path of freetype2")
endif()

if (NOT DEFINED 3RDPARTY_FREETYPE_LIBRARY OR NOT 3RDPARTY_FREETYPE_LIBRARY_DIR)
  set (3RDPARTY_FREETYPE_LIBRARY               "" CACHE FILEPATH "freetype library")
endif()

if (NOT DEFINED 3RDPARTY_FREETYPE_LIBRARY_DIR)
  set (3RDPARTY_FREETYPE_LIBRARY_DIR           "" CACHE FILEPATH "The directory containing freetype library")
endif()

if (WIN32)
  if (NOT DEFINED 3RDPARTY_FREETYPE_DLL OR NOT 3RDPARTY_FREETYPE_DLL_DIR)
    set (3RDPARTY_FREETYPE_DLL                   "" CACHE FILEPATH "freetype shared library")
  endif()
endif()

if (WIN32)
  if (NOT DEFINED 3RDPARTY_FREETYPE_DLL_DIR)
    set (3RDPARTY_FREETYPE_DLL_DIR               "" CACHE FILEPATH "The directory containing freetype shared library")
  endif()
endif()

find_package(Freetype)

# ft2build header
if (FREETYPE_INCLUDE_DIR_ft2build AND EXISTS "${FREETYPE_INCLUDE_DIR_ft2build}")
  if (NOT 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build)
    set (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build  "${FREETYPE_INCLUDE_DIR_ft2build}" CACHE FILEPATH "the path of ft2build.h" FORCE)
  endif()
endif()

if (NOT FREETYPE_INCLUDE_DIR_freetype2 OR NOT EXISTS "${FREETYPE_INCLUDE_DIR_freetype2}")
  # cmake (version is < 3.0) doesn't find ftheader.h of freetype (version is >= 2.5.1)
  # do search taking into account freetype structure of 2.5.1 version
  find_path (FREETYPE_INCLUDE_DIR_freetype2 NAMES
                                             freetype/config/ftheader.h
                                             config/ftheader.h
                                           HINTS
                                             ENV FREETYPE_DIR
                                           PATHS
                                             /usr/X11R6
                                             /usr/local/X11R6
                                             /usr/local/X11
                                             /usr/freeware
                                           PATH_SUFFIXES include/freetype2 include freetype2
                                           NO_DEFAULT_PATH)
  find_path (FREETYPE_INCLUDE_DIR_freetype2 NAMES freetype/config/ftheader.h config/ftheader.h)
elseif (FREETYPE_INCLUDE_DIR_freetype2 OR EXISTS "${FREETYPE_INCLUDE_DIR_freetype2}")
  if (3RDPARTY_FREETYPE_DIR AND EXISTS "${3RDPARTY_FREETYPE_DIR}")
    get_filename_component (3RDPARTY_FREETYPE_DIR_ABS "${3RDPARTY_FREETYPE_DIR}" ABSOLUTE)
    get_filename_component (FREETYPE_INCLUDE_DIR_freetype2_ABS "${FREETYPE_INCLUDE_DIR_freetype2}" ABSOLUTE)

    string (REGEX MATCH "${3RDPARTY_FREETYPE_DIR_ABS}" DOES_PATH_CONTAIN "${FREETYPE_INCLUDE_DIR_freetype2_ABS}")

    if (NOT DOES_PATH_CONTAIN) # if cmake found freetype2 at different place from 3RDPARTY_FREETYPE_DIR
      # search for freetype2 in 3RDPARTY_FREETYPE_DIR and if it will be found - replace freetyp2 path by new one
      set (TMP_FREETYPE2 "TMP_FREETYPE2-NOTFOUND" CACHE FILEPATH "" FORCE)
      find_path (TMP_FREETYPE2 NAMES freetype/config/ftheader.h config/ftheader.h
                               PATHS "${3RDPARTY_FREETYPE_DIR}"
                               PATH_SUFFIXES include/freetype2 include freetype2
                               NO_DEFAULT_PATH)

      if (TMP_FREETYPE2 OR NOT EXISTS "${TMP_FREETYPE2}")
        set (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 "${TMP_FREETYPE2}" CACHE FILEPATH "the path of freetype2" FORCE)

        # hide and remove TMP_FREETYPE2
        mark_as_advanced (TMP_FREETYPE2)
        unset (TMP_FREETYPE2)
      endif()
    endif()
  endif()
endif()

# return ENV{FREETYPE_DIR}
SET (ENV{FREETYPE_DIR} ${CACHED_FREETYPE_DIR})

# freetype2 header
if (FREETYPE_INCLUDE_DIR_freetype2 AND EXISTS "${FREETYPE_INCLUDE_DIR_freetype2}")
  if (NOT 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2)
    set (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2 "${FREETYPE_INCLUDE_DIR_freetype2}" CACHE FILEPATH "the path of freetype2" FORCE)
  endif()
endif()

if (NOT 3RDPARTY_FREETYPE_LIBRARY_DIR)
  set (3RDPARTY_FREETYPE_LIBRARY "" CACHE FILEPATH "freetype library" FORCE)
elseif (3RDPARTY_FREETYPE_LIBRARY AND EXISTS "${3RDPARTY_FREETYPE_LIBRARY}")
  get_filename_component(3RDPARTY_FREETYPE_LIBRARY_DIR_TMP "${3RDPARTY_FREETYPE_LIBRARY}" PATH)
  if (NOT "${3RDPARTY_FREETYPE_LIBRARY_DIR}" STREQUAL "${3RDPARTY_FREETYPE_LIBRARY_DIR_TMP}")
    set (3RDPARTY_FREETYPE_LIBRARY "" CACHE FILEPATH "freetype library" FORCE)
  endif()
endif()

if (WIN32)
  if (NOT 3RDPARTY_FREETYPE_DLL_DIR)
    set (3RDPARTY_FREETYPE_DLL "" CACHE FILEPATH "freetype shared library" FORCE)
  elseif (3RDPARTY_FREETYPE_DLL AND EXISTS "${3RDPARTY_FREETYPE_DLL}")
    get_filename_component(3RDPARTY_FREETYPE_DLL_DIR_TMP "${3RDPARTY_FREETYPE_DLL}" PATH)
    if (NOT "${3RDPARTY_FREETYPE_DLL_DIR}" STREQUAL "${3RDPARTY_FREETYPE_DLL_DIR_TMP}")
      set (3RDPARTY_FREETYPE_DLL "" CACHE FILEPATH "freetype shared library" FORCE)
    endif()
  endif()
endif()

# freetype library
if (FREETYPE_LIBRARY AND EXISTS "${FREETYPE_LIBRARY}")
  if (NOT 3RDPARTY_FREETYPE_LIBRARY)
    set (3RDPARTY_FREETYPE_LIBRARY "${FREETYPE_LIBRARY}" CACHE FILEPATH "freetype library" FORCE)
  endif()

  if (3RDPARTY_FREETYPE_LIBRARY AND EXISTS "${3RDPARTY_FREETYPE_LIBRARY}")
    get_filename_component (3RDPARTY_FREETYPE_LIBRARY_DIR "${3RDPARTY_FREETYPE_LIBRARY}" PATH)
    set (3RDPARTY_FREETYPE_LIBRARY_DIR "${3RDPARTY_FREETYPE_LIBRARY_DIR}" CACHE FILEPATH "The directory containing freetype library" FORCE)
  endif()

  if (WIN32)
    set (CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")

    if (NOT 3RDPARTY_FREETYPE_DLL OR NOT EXISTS "${3RDPARTY_FREETYPE_DLL}")
      get_filename_component (FREETYPE_LIBRARY_PARENT_DIR "${3RDPARTY_FREETYPE_LIBRARY_DIR}" PATH) # parent of the library directory

      set (3RDPARTY_FREETYPE_DLL "3RDPARTY_FREETYPE_DLL-NOTFOUND" CACHE FILEPATH "freetype shared library" FORCE)
      find_library (3RDPARTY_FREETYPE_DLL freetype PATHS "${FREETYPE_LIBRARY_PARENT_DIR}/bin" NO_DEFAULT_PATH)
    endif()

    if (3RDPARTY_FREETYPE_DLL AND EXISTS "${3RDPARTY_FREETYPE_DLL}")
      get_filename_component (3RDPARTY_FREETYPE_DLL_DIR "${3RDPARTY_FREETYPE_DLL}" PATH)
      set (3RDPARTY_FREETYPE_DLL_DIR "${3RDPARTY_FREETYPE_DLL_DIR}" CACHE FILEPATH "The directory containing freetype shared library" FORCE)
    endif()
  endif()
endif()


if (NOT 3RDPARTY_FREETYPE_LIBRARY_DIR OR NOT EXISTS "${3RDPARTY_FREETYPE_LIBRARY_DIR}")
  set (3RDPARTY_FREETYPE_LIBRARY_DIR "" CACHE FILEPATH "The directory containing freetype library" FORCE)
endif()

if (WIN32)
  if (NOT 3RDPARTY_FREETYPE_DLL_DIR OR NOT EXISTS "${3RDPARTY_FREETYPE_DLL_DIR}")
    set (3RDPARTY_FREETYPE_DLL_DIR "" CACHE FILEPATH "The directory containing shared freetype library" FORCE)
  endif()
endif()

# include found paths to common variables
if (3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build)
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build)
endif()

if (3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2)
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2)
endif()

if (3RDPARTY_FREETYPE_LIBRARY)
  list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_FREETYPE_LIBRARY_DIR}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FREETYPE_LIBRARY_DIR)
endif()

if (WIN32)
  if (3RDPARTY_FREETYPE_DLL OR EXISTS "${3RDPARTY_FREETYPE_DLL}")
    list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_FREETYPE_DLL_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FREETYPE_DLL_DIR)
  endif()
endif()

if (INSTALL_FREETYPE)

  OCCT_MAKE_OS_WITH_BITNESS()
  if (WIN32)
    install (FILES "${3RDPARTY_FREETYPE_DLL}"
             CONFIGURATIONS Release
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
    install (FILES "${3RDPARTY_FREETYPE_DLL}"
             CONFIGURATIONS RelWithDebInfo
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bini")
    install (FILES "${3RDPARTY_FREETYPE_DLL}"
             CONFIGURATIONS Debug
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")
  else()
    get_filename_component(ABS_PATH ${3RDPARTY_FREETYPE_LIBRARY} REALPATH)
    get_filename_component(FREETYPELIB ${3RDPARTY_FREETYPE_LIBRARY} NAME)

    install (FILES "${ABS_PATH}"
             CONFIGURATIONS Release
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib"
             RENAME ${FREETYPELIB}.6)
    install (FILES "${ABS_PATH}"
             CONFIGURATIONS RelWithDebInfo
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libi"
             RENAME ${FREETYPELIB}.6)
    install (FILES "${ABS_PATH}"
             CONFIGURATIONS Debug
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libd"
             RENAME ${FREETYPELIB}.6)
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

# unset all redundant variables
OCCT_CHECK_AND_UNSET(FREETYPE_INCLUDE_DIR_ft2build)
OCCT_CHECK_AND_UNSET(FREETYPE_INCLUDE_DIR_freetype2)
OCCT_CHECK_AND_UNSET(FREETYPE_LIBRARY)

mark_as_advanced (3RDPARTY_FREETYPE_LIBRARY 3RDPARTY_FREETYPE_DLL)