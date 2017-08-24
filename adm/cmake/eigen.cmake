# eigen

if (NOT DEFINED INSTALL_EIGEN)
  set (INSTALL_EIGEN OFF CACHE BOOL "${INSTALL_EIGEN_DESCR}")
endif()

# eigen directory
if (NOT DEFINED 3RDPARTY_EIGEN_DIR)
  set (3RDPARTY_EIGEN_DIR "" CACHE PATH "The directory containing eigen")
endif()

# search for eigen in user defined directory
if (3RDPARTY_DIR AND EXISTS "${3RDPARTY_DIR}")
  if (NOT 3RDPARTY_EIGEN_DIR OR NOT EXISTS "${3RDPARTY_EIGEN_DIR}")
    FIND_PRODUCT_DIR("${3RDPARTY_DIR}" Eigen EIGEN_DIR_NAME)
    if (EIGEN_DIR_NAME)
      set (3RDPARTY_EIGEN_DIR "${3RDPARTY_DIR}/${EIGEN_DIR_NAME}" CACHE PATH "The directory containing eigen" FORCE)
    endif()
  endif()
endif()

if (NOT DEFINED 3RDPARTY_EIGEN_INCLUDE_DIR)
  set (3RDPARTY_EIGEN_INCLUDE_DIR  "" CACHE FILEPATH "The directory containing headers of the EIGEN")
endif()

if (NOT 3RDPARTY_EIGEN_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_EIGEN_INCLUDE_DIR}")

  set (HEADER_NAMES Eigen)

  set (3RDPARTY_EIGEN_INCLUDE_DIR "3RDPARTY_EIGEN_INCLUDE_DIR-NOTFOUND" CACHE PATH "the path to Eigen header file" FORCE)

  if (3RDPARTY_EIGEN_DIR AND EXISTS "${3RDPARTY_EIGEN_DIR}")
    find_path (3RDPARTY_EIGEN_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                              PATHS ${3RDPARTY_EIGEN_DIR}
                                              PATH_SUFFIXES include eigen3 include/eigen3
                                              CMAKE_FIND_ROOT_PATH_BOTH
                                              NO_DEFAULT_PATH)
  else()
    find_path (3RDPARTY_EIGEN_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                              PATH_SUFFIXES include eigen3 include/eigen3
                                              CMAKE_FIND_ROOT_PATH_BOTH)
  endif()

  # use default (CMake) EIGEN search
  if (NOT 3RDPARTY_EIGEN_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_EIGEN_INCLUDE_DIR}")
    # use 3RDPARTY_EIGEN_DIR if it is specified for eigen search
    if (3RDPARTY_EIGEN_DIR AND EXISTS "${3RDPARTY_EIGEN_DIR}")
      set (CACHED_EIGEN_DIR $ENV{Eigen3_DIR})
      set (ENV{Eigen3_DIR} "${3RDPARTY_EIGEN_DIR}")
    endif()

    find_package(Eigen3 QUIET)

    # restore ENV{Eigen3_DIR}
    if (3RDPARTY_EIGEN_DIR AND EXISTS "${3RDPARTY_EIGEN_DIR}")
      set (ENV{Eigen3_DIR} ${CACHED_EIGEN_DIR})
    endif()

    if (${EIGEN3_FOUND})
      set (3RDPARTY_EIGEN_INCLUDE_DIR "${EIGEN3_INCLUDE_DIR}" CACHE PATH "the path to Eigen header file" FORCE)
      set (3RDPARTY_EIGEN_DIR "${EIGEN3_ROOT_DIR}"    CACHE PATH "The directory containing eigen" FORCE)
    endif()
  endif()
endif()

if (3RDPARTY_EIGEN_INCLUDE_DIR AND EXISTS "${3RDPARTY_EIGEN_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_EIGEN_INCLUDE_DIR}")

  # Install header files
  if (INSTALL_EIGEN)
    file(GLOB EIGEN_SUBDIRS "${3RDPARTY_EIGEN_INCLUDE_DIR}/*")
    foreach(SUBDIR ${EIGEN_SUBDIRS})
      if(IS_DIRECTORY "${SUBDIR}")
        install (DIRECTORY "${SUBDIR}" DESTINATION "${INSTALL_DIR_INCLUDE}")
      else()
        install (FILES "${SUBDIR}" DESTINATION "${INSTALL_DIR_INCLUDE}")
      endif()
    endforeach()
  endif()
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_EIGEN_INCLUDE_DIR)

  set (3RDPARTY_EIGEN_INCLUDE_DIR "" CACHE PATH "the path to Eigen header file" FORCE)
endif()

# unset all redundant variables
OCCT_CHECK_AND_UNSET(Eigen3_DIR)
