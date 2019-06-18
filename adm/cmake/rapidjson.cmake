# RapidJSON

if (NOT DEFINED INSTALL_RAPIDJSON)
  set (INSTALL_RAPIDJSON OFF CACHE BOOL "${INSTALL_RAPIDJSON_DESCR}")
endif()

# RapidJSON directory
if (NOT DEFINED 3RDPARTY_RAPIDJSON_DIR)
  set (3RDPARTY_RAPIDJSON_DIR "" CACHE PATH "The directory containing RapidJSON")
endif()

# search for RapidJSON in user defined directory
if (3RDPARTY_DIR AND EXISTS "${3RDPARTY_DIR}")
  if (NOT 3RDPARTY_RAPIDJSON_DIR OR NOT EXISTS "${3RDPARTY_RAPIDJSON_DIR}")
    FIND_PRODUCT_DIR("${3RDPARTY_DIR}" RapidJSON RAPIDJSON_DIR_NAME)
    if (RAPIDJSON_DIR_NAME)
      set (3RDPARTY_RAPIDJSON_DIR "${3RDPARTY_DIR}/${RAPIDJSON_DIR_NAME}" CACHE PATH "The directory containing RapidJSON" FORCE)
    endif()
  endif()
endif()

if (NOT DEFINED 3RDPARTY_RAPIDJSON_INCLUDE_DIR)
  set (3RDPARTY_RAPIDJSON_INCLUDE_DIR  "" CACHE FILEPATH "The directory containing headers of the RAPIDJSON")
endif()

if (NOT 3RDPARTY_RAPIDJSON_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_RAPIDJSON_INCLUDE_DIR}")

  set (HEADER_NAMES rapidjson/rapidjson.h)

  set (3RDPARTY_RAPIDJSON_INCLUDE_DIR "3RDPARTY_RAPIDJSON_INCLUDE_DIR-NOTFOUND" CACHE PATH "the path to RapidJSON header file" FORCE)

  if (3RDPARTY_RAPIDJSON_DIR AND EXISTS "${3RDPARTY_RAPIDJSON_DIR}")
    find_path (3RDPARTY_RAPIDJSON_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                              PATHS ${3RDPARTY_RAPIDJSON_DIR}
                                              PATH_SUFFIXES include rapidjson
                                              CMAKE_FIND_ROOT_PATH_BOTH
                                              NO_DEFAULT_PATH)
  else()
    find_path (3RDPARTY_RAPIDJSON_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                              PATH_SUFFIXES include rapidjson
                                              CMAKE_FIND_ROOT_PATH_BOTH)
  endif()

  # use default (CMake) RapidJSON search
  if (NOT 3RDPARTY_RAPIDJSON_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_RAPIDJSON_INCLUDE_DIR}")
    if (3RDPARTY_RAPIDJSON_DIR AND EXISTS "${3RDPARTY_RAPIDJSON_DIR}")
      set (CACHED_RAPIDJSON_DIR $ENV{RapidJSON_DIR})
      set (ENV{RapidJSON_DIR} "${3RDPARTY_RAPIDJSON_DIR}")
    endif()

    find_package(RapidJSON QUIET)

    # restore ENV{RapidJSON_DIR}
    if (3RDPARTY_RAPIDJSON_DIR AND EXISTS "${3RDPARTY_RAPIDJSON_DIR}")
      set (ENV{RapidJSON_DIR} ${CACHED_RAPIDJSON_DIR})
    endif()

    if (${RAPIDJSON_FOUND})
      set (3RDPARTY_RAPIDJSON_INCLUDE_DIR "${RAPIDJSON_INCLUDE_DIR}" CACHE PATH "the path to RapidJSON header file" FORCE)
      set (3RDPARTY_RAPIDJSON_DIR         "${RAPIDJSON_ROOT_DIR}"    CACHE PATH "The directory containing RapidJSON" FORCE)
    endif()
  endif()
endif()

if (3RDPARTY_RAPIDJSON_INCLUDE_DIR AND EXISTS "${3RDPARTY_RAPIDJSON_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_RAPIDJSON_INCLUDE_DIR}")

  # Install header files
  if (INSTALL_RAPIDJSON)
    file(GLOB RAPIDJSON_SUBDIRS "${3RDPARTY_RAPIDJSON_INCLUDE_DIR}/*")
    foreach(SUBDIR ${RAPIDJSON_SUBDIRS})
      if(IS_DIRECTORY "${SUBDIR}")
        install (DIRECTORY "${SUBDIR}" DESTINATION "${INSTALL_DIR_INCLUDE}")
      else()
        install (FILES "${SUBDIR}" DESTINATION "${INSTALL_DIR_INCLUDE}")
      endif()
    endforeach()
  endif()
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_RAPIDJSON_INCLUDE_DIR)

  set (3RDPARTY_RAPIDJSON_INCLUDE_DIR "" CACHE PATH "the path to RapidJSON header file" FORCE)
endif()

# unset all redundant variables
OCCT_CHECK_AND_UNSET(RapidJSON_DIR)
