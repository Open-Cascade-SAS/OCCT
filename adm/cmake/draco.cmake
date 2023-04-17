# Draco - a library for a lossy vertex data compression, used as extension to glTF format.
# https://github.com/google/draco

OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

if (NOT DEFINED 3RDPARTY_DRACO_DIR)
  set (3RDPARTY_DRACO_DIR "" CACHE PATH "The directory containing Draco")
endif()

if (NOT DEFINED 3RDPARTY_DRACO_INCLUDE_DIR)
  set (3RDPARTY_DRACO_INCLUDE_DIR  "" CACHE PATH "The directory containing headers of the Draco")
endif()

if (NOT DEFINED 3RDPARTY_DRACO_LIBRARY)
  set (3RDPARTY_DRACO_LIBRARY "" CACHE FILEPATH "Draco library")
endif()

if (NOT DEFINED 3RDPARTY_DRACO_LIBRARY_DIR)
  set (3RDPARTY_DRACO_LIBRARY_DIR "" CACHE PATH "The directory containing Draco library")
endif()

if (WIN32)
  if (NOT DEFINED 3RDPARTY_DRACO_LIBRARY_DEBUG)
    set (3RDPARTY_DRACO_LIBRARY_DEBUG "" CACHE FILEPATH "Draco debug library")
  endif()
  
  if (NOT DEFINED 3RDPARTY_DRACO_LIBRARY_DIR_DEBUG)
    set (3RDPARTY_DRACO_LIBRARY_DIR_DEBUG "" CACHE PATH "The directory containing Draco debug library")
  endif()
endif()

if (3RDPARTY_DIR AND EXISTS "${3RDPARTY_DIR}")
  if (NOT 3RDPARTY_DRACO_DIR OR NOT EXISTS "${3RDPARTY_DRACO_DIR}")
    FIND_PRODUCT_DIR("${3RDPARTY_DIR}" draco DRACO_DIR_NAME)
    if (DRACO_DIR_NAME)
      set (3RDPARTY_DRACO_DIR "${3RDPARTY_DIR}/${DRACO_DIR_NAME}" CACHE PATH "The directory containing Draco" FORCE)
    endif()
  endif()
endif()

# header
if (NOT 3RDPARTY_DRACO_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_DRACO_INCLUDE_DIR}")
  set (HEADER_NAMES draco)

  # set 3RDPARTY_DRACO_INCLUDE_DIR as notfound, otherwise find_path can't assign a new value to 3RDPARTY_DRACO_INCLUDE_DIR
  set (3RDPARTY_DRACO_INCLUDE_DIR "3RDPARTY_DRACO_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "The directory containing headers of the Draco" FORCE)

  if (3RDPARTY_DRACO_DIR AND EXISTS "${3RDPARTY_DRACO_DIR}")
    find_path (3RDPARTY_DRACO_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                                 PATHS ${3RDPARTY_DRACO_DIR}
                                                 PATH_SUFFIXES "include"
                                                 CMAKE_FIND_ROOT_PATH_BOTH
                                                 NO_DEFAULT_PATH)
  endif()
endif()

if (3RDPARTY_DRACO_INCLUDE_DIR AND EXISTS "${3RDPARTY_DRACO_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_DRACO_INCLUDE_DIR}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_DRACO_INCLUDE_DIR)
endif()

if (3RDPARTY_DRACO_DIR AND EXISTS "${3RDPARTY_DRACO_DIR}")
  if (NOT 3RDPARTY_DRACO_LIBRARY OR NOT EXISTS "${3RDPARTY_DRACO_LIBRARY}")
    set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .a)
    set (3RDPARTY_DRACO_LIBRARY "3RDPARTY_DRACO_LIBRARY-NOTFOUND" CACHE FILEPATH "The path to Draco library" FORCE)

    find_library (3RDPARTY_DRACO_LIBRARY NAMES ${CSF_Draco}
                                         PATHS "${3RDPARTY_DRACO_DIR}"
                                         PATH_SUFFIXES lib
                                         CMAKE_FIND_ROOT_PATH_BOTH
                                         NO_DEFAULT_PATH)
    if (3RDPARTY_DRACO_LIBRARY AND EXISTS "${3RDPARTY_DRACO_LIBRARY}")
      get_filename_component (3RDPARTY_DRACO_LIBRARY_DIR "${3RDPARTY_DRACO_LIBRARY}" PATH)
      set (3RDPARTY_DRACO_LIBRARY_DIR "${3RDPARTY_DRACO_LIBRARY_DIR}" CACHE FILEPATH "The directory containing Draco library" FORCE)
    endif()
  endif()

  if (WIN32 AND (NOT 3RDPARTY_DRACO_LIBRARY_DEBUG OR NOT EXISTS "${3RDPARTY_DRACO_LIBRARY_DEBUG}"))
    set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .a)
    set (3RDPARTY_DRACO_LIBRARY_DEBUG "3RDPARTY_DRACO_LIBRARY_DEBUG-NOTFOUND" CACHE FILEPATH "The path to debug Draco library" FORCE)

    find_library (3RDPARTY_DRACO_LIBRARY_DEBUG NAMES ${CSF_Draco}
                                         PATHS "${3RDPARTY_DRACO_DIR}"
                                         PATH_SUFFIXES libd
                                         CMAKE_FIND_ROOT_PATH_BOTH
                                         NO_DEFAULT_PATH)
    if (3RDPARTY_DRACO_LIBRARY_DEBUG AND EXISTS "${3RDPARTY_DRACO_LIBRARY_DEBUG}")
      get_filename_component (3RDPARTY_DRACO_LIBRARY_DIR_DEBUG "${3RDPARTY_DRACO_LIBRARY_DEBUG}" PATH)
      set (3RDPARTY_DRACO_LIBRARY_DIR_DEBUG "${3RDPARTY_DRACO_LIBRARY_DIR_DEBUG}" CACHE FILEPATH "The directory containing debug Draco library" FORCE)
    endif()
  endif()
endif()
