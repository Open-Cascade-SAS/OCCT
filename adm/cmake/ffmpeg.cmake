# FFmpeg

if (NOT DEFINED INSTALL_FFMPEG)
  set (INSTALL_FFMPEG OFF CACHE BOOL "${INSTALL_FFMPEG_DESCR}")
endif()

if (NOT DEFINED 3RDPARTY_FFMPEG_DIR)
  set (3RDPARTY_FFMPEG_DIR "" CACHE PATH "The directory containing FFmpeg")
endif()

# include occt macros. compiler_bitness, os_wiht_bit, compiler
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

# specify FFMPEG folder in connection with 3RDPARTY_DIR
if (3RDPARTY_DIR AND EXISTS "${3RDPARTY_DIR}")
  if (NOT 3RDPARTY_FFMPEG_DIR OR NOT EXISTS "${3RDPARTY_FFMPEG_DIR}")
    FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" FFMPEG FFMPEG_DIR_NAME)
    if (FFMPEG_DIR_NAME)
      set (3RDPARTY_FFMPEG_DIR "${3RDPARTY_DIR}/${FFMPEG_DIR_NAME}" CACHE PATH "The directory containing FFmpeg" FORCE)
    endif()
  endif()
else()
endif()

# define required FFMPEG variables
if (NOT DEFINED 3RDPARTY_FFMPEG_INCLUDE_DIR)
  set (3RDPARTY_FFMPEG_INCLUDE_DIR  "" CACHE PATH "the path of headers directory")
endif()

if (NOT DEFINED 3RDPARTY_FFMPEG_LIBRARY OR NOT 3RDPARTY_FFMPEG_LIBRARY_DIR OR NOT EXISTS "${3RDPARTY_FFMPEG_LIBRARY_DIR}")
  set (3RDPARTY_FFMPEG_LIBRARY               "" CACHE FILEPATH "FFmpeg framework" FORCE)
endif()

if (NOT DEFINED 3RDPARTY_FFMPEG_LIBRARY_DIR)
  set (3RDPARTY_FFMPEG_LIBRARY_DIR           "" CACHE PATH "The directory containing FFmpeg framework")
endif()

if (WIN32)
  if (NOT DEFINED 3RDPARTY_FFMPEG_DLL OR NOT 3RDPARTY_FFMPEG_DLL_DIR OR NOT EXISTS "${3RDPARTY_FFMPEG_DLL_DIR}")
    set (3RDPARTY_FFMPEG_DLL                 "" CACHE FILEPATH "FFmpeg shared libraries" FORCE)
  endif()
endif()

if (WIN32)
  if (NOT DEFINED 3RDPARTY_FFMPEG_DLL_DIR)
    set (3RDPARTY_FFMPEG_DLL_DIR               "" CACHE PATH "The directory containing FFmpeg shared libraries")
  endif()
endif()

# check 3RDPARTY_${PRODUCT_NAME}_ paths for consistency with specified 3RDPARTY_${PRODUCT_NAME}_DIR
if (3RDPARTY_FFMPEG_DIR AND EXISTS "${3RDPARTY_FFMPEG_DIR}")
  CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_INCLUDE_DIR PATH "the path to FFmpeg")
  CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_LIBRARY FILEPATH "the path to FFmpeg framework")

  if (3RDPARTY_FFMPEG_LIBRARY AND EXISTS "${3RDPARTY_FFMPEG_LIBRARY}")
    get_filename_component (3RDPARTY_FFMPEG_LIBRARY_DIR "${3RDPARTY_FFMPEG_LIBRARY}" PATH)
    set (3RDPARTY_FFMPEG_LIBRARY_DIR "${3RDPARTY_FFMPEG_LIBRARY_DIR}" CACHE PATH "The directory containing FFmpeg libraries" FORCE)
  else()
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_LIBRARY_DIR PATH "The directory containing FFmpeg libraries")
  endif()

  if (WIN32)
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_DLL FILEPATH "the path to FFmpeg shared libraries")

    if (3RDPARTY_FFMPEG_DLL AND EXISTS "${3RDPARTY_FFMPEG_DLL}")
      get_filename_component (3RDPARTY_FFMPEG_DLL_DIR "${3RDPARTY_FFMPEG_DLL}" PATH)
      set (3RDPARTY_FFMPEG_DLL_DIR "${3RDPARTY_FFMPEG_DLL_DIR}" CACHE PATH "The directory containing FFmpeg shared libraries" FORCE)
    else()
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_DLL_DIR PATH "The directory containing FFmpeg shared libraries")
    endif()
  endif()
endif()

# header
if (NOT 3RDPARTY_FFMPEG_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_FFMPEG_INCLUDE_DIR}")
  set (HEADER_NAMES avutil.h libavutil/avutil.h)

  # set 3RDPARTY_FFMPEG_INCLUDE_DIR as notfound, otherwise find_library can't assign a new value to 3RDPARTY_FFMPEG_INCLUDE_DIR
  set (3RDPARTY_FFMPEG_INCLUDE_DIR "3RDPARTY_FFMPEG_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "the path to header directory" FORCE)

  if (3RDPARTY_FFMPEG_DIR AND EXISTS "${3RDPARTY_FFMPEG_DIR}")
    find_path (3RDPARTY_FFMPEG_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                                 PATHS ${3RDPARTY_FFMPEG_DIR}
                                                 PATH_SUFFIXES include
                                                 CMAKE_FIND_ROOT_PATH_BOTH
                                                 NO_DEFAULT_PATH)
  else()
    find_path (3RDPARTY_FFMPEG_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                                 PATH_SUFFIXES include
                                                 CMAKE_FIND_ROOT_PATH_BOTH)
  endif()
endif()

if (3RDPARTY_FFMPEG_INCLUDE_DIR AND EXISTS "${3RDPARTY_FFMPEG_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_FFMPEG_INCLUDE_DIR}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FFMPEG_INCLUDE_DIR)

  set (3RDPARTY_FFMPEG_INCLUDE_DIR "" CACHE FILEPATH "the path to avutil.h" FORCE)
endif()

# library
if (NOT 3RDPARTY_FFMPEG_LIBRARY OR NOT EXISTS "${3RDPARTY_FFMPEG_LIBRARY}")
  set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .so .dylib .a)

  set (FFMPEG_PATH_SUFFIXES lib)
  if (ANDROID)
    set (FFMPEG_PATH_SUFFIXES ${FFMPEG_PATH_SUFFIXES} libs/${ANDROID_ABI})
  elseif(APPLE)
    set (FFMPEG_PATH_SUFFIXES ${FFMPEG_PATH_SUFFIXES} Frameworks)
  endif()

  # set 3RDPARTY_FFMPEG_LIBRARY as notfound, otherwise find_library can't assign a new value to 3RDPARTY_FFMPEG_LIBRARY
  set (3RDPARTY_FFMPEG_LIBRARY "3RDPARTY_FFMPEG_LIBRARY-NOTFOUND" CACHE FILEPATH "The path to FFmpeg library" FORCE)

  if (3RDPARTY_FFMPEG_DIR AND EXISTS "${3RDPARTY_FFMPEG_DIR}")
    find_library (3RDPARTY_FFMPEG_LIBRARY NAMES avutil
                  PATHS "${3RDPARTY_FFMPEG_LIBRARY_DIR}" "${3RDPARTY_FFMPEG_DIR}"
                  PATH_SUFFIXES ${FFMPEG_PATH_SUFFIXES}
                  CMAKE_FIND_ROOT_PATH_BOTH
                  NO_DEFAULT_PATH)
  else()
    find_library (3RDPARTY_FFMPEG_LIBRARY NAMES avutil
                  PATH_SUFFIXES ${FFMPEG_PATH_SUFFIXES}
                  CMAKE_FIND_ROOT_PATH_BOTH)
  endif()

  if (3RDPARTY_FFMPEG_LIBRARY AND EXISTS "${3RDPARTY_FFMPEG_LIBRARY}")
    get_filename_component (3RDPARTY_FFMPEG_LIBRARY_DIR "${3RDPARTY_FFMPEG_LIBRARY}" PATH)
    set (3RDPARTY_FFMPEG_LIBRARY_DIR "${3RDPARTY_FFMPEG_LIBRARY_DIR}" CACHE PATH "The directory containing FFmpeg library" FORCE)
  else()
    set (3RDPARTY_FFMPEG_LIBRARY_DIR "" CACHE PATH "The directory containing FFmpeg library" FORCE)
  endif()
endif()

if (3RDPARTY_FFMPEG_LIBRARY_DIR AND EXISTS "${3RDPARTY_FFMPEG_LIBRARY_DIR}")
  list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_FFMPEG_LIBRARY_DIR}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FFMPEG_LIBRARY_DIR)

  set (3RDPARTY_FFMPEG_LIBRARY "" CACHE FILEPATH "The path to FFmpeg library" FORCE)
endif()

# shared library
if (WIN32)
  if (NOT 3RDPARTY_FFMPEG_DLL OR NOT EXISTS "${3RDPARTY_FFMPEG_DLL}")

    set (CMAKE_FIND_LIBRARY_SUFFIXES .dll)
    set (3RDPARTY_FFMPEG_DLL "3RDPARTY_FFMPEG_DLL-NOTFOUND" CACHE FILEPATH "The path to FFmpeg shared library" FORCE)

    # find FFmpeg shared library
    file (GLOB 3RDPARTY_FFMPEG_DLL "${3RDPARTY_FFMPEG_DIR}/bin/avutil[-][0-9]*")

    if (3RDPARTY_FFMPEG_DLL AND EXISTS "${3RDPARTY_FFMPEG_DLL}")
      set (3RDPARTY_FFMPEG_DLL "${3RDPARTY_FFMPEG_DLL}" CACHE FILEPATH "FFmpeg shared library" FORCE)
      get_filename_component (3RDPARTY_FFMPEG_DLL_DIR "${3RDPARTY_FFMPEG_DLL}" PATH)
      set (3RDPARTY_FFMPEG_DLL_DIR "${3RDPARTY_FFMPEG_DLL_DIR}" CACHE PATH "The directory containing FFmpeg library" FORCE)
    else()
      set (3RDPARTY_FFMPEG_DLL_DIR "" CACHE PATH "The directory containing FFmpeg shared library" FORCE)
    endif()
  endif()

  if (3RDPARTY_FFMPEG_DLL_DIR OR EXISTS "${3RDPARTY_FFMPEG_DLL_DIR}")
    list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_FFMPEG_DLL_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FFMPEG_DLL_DIR)
  endif()
endif()

# install instructions
if (INSTALL_FFMPEG)
  OCCT_MAKE_OS_WITH_BITNESS()
  OCCT_MAKE_COMPILER_SHORT_NAME()

  if (WIN32)
    if (DEFINED INSTALL_BIN_DIR)
      install (FILES "${3RDPARTY_FFMPEG_DLL}" DESTINATION "${INSTALL_BIN_DIR}")
    else()
      install (FILES "${3RDPARTY_FFMPEG_DLL}"
               CONFIGURATIONS Release
               DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
      install (FILES "${3RDPARTY_FFMPEG_DLL}"
               CONFIGURATIONS RelWithDebInfo
               DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bini")
      install (FILES "${3RDPARTY_FFMPEG_DLL}"
               CONFIGURATIONS Debug
               DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")
    endif()
  else()
    get_filename_component(3RDPARTY_FFMPEG_LIBRARY_ABS ${3RDPARTY_FFMPEG_LIBRARY} REALPATH)
    get_filename_component(3RDPARTY_FFMPEG_LIBRARY_NAME ${3RDPARTY_FFMPEG_LIBRARY} NAME)

    if (DEFINED INSTALL_LIB_DIR)
      install (FILES "${3RDPARTY_FFMPEG_LIBRARY_ABS}"
               DESTINATION "${INSTALL_LIB_DIR}"
               RENAME ${3RDPARTY_FFMPEG_LIBRARY_NAME}.6)
    else()
      install (FILES "${3RDPARTY_FFMPEG_LIBRARY_ABS}"
               CONFIGURATIONS Release
               DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib"
               RENAME ${3RDPARTY_FFMPEG_LIBRARY_NAME}.6)
      install (FILES "${3RDPARTY_FFMPEG_LIBRARY_ABS}"
               CONFIGURATIONS RelWithDebInfo
               DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libi"
               RENAME ${3RDPARTY_FFMPEG_LIBRARY_NAME}.6)
      install (FILES "${3RDPARTY_FFMPEG_LIBRARY_ABS}"
               CONFIGURATIONS Debug
               DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libd"
               RENAME ${3RDPARTY_FFMPEG_LIBRARY_NAME}.6)
    endif()
  endif()

  set (USED_3RDPARTY_FFMPEG_DIR "")
else()
  # the library directory for using by the executable
  if (WIN32)
    set (USED_3RDPARTY_FFMPEG_DIR ${3RDPARTY_FFMPEG_DLL_DIR})
  else()
    set (USED_3RDPARTY_FFMPEG_DIR ${3RDPARTY_FFMPEG_LIBRARY_DIR})
  endif()
endif()

# unset all redundant variables
OCCT_CHECK_AND_UNSET (FFMPEG_INCLUDE_DIRS)
OCCT_CHECK_AND_UNSET (FFMPEG_LIBRARY_DIRS)
OCCT_CHECK_AND_UNSET (FFMPEG_DIR)

mark_as_advanced (3RDPARTY_FFMPEG_LIBRARY 3RDPARTY_FFMPEG_DLL)
