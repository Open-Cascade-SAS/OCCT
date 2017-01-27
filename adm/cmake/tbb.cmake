# tbb

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

# common steps for tbb and tbbmalloc
macro (TBB_PRODUCT_SEARCH PRODUCT_LIBRARY_NAME)

  string (TOUPPER ${PRODUCT_LIBRARY_NAME} upper_PRODUCT_LIBRARY_NAME)

  # define required tbb/tbbmalloc variables
  if (NOT DEFINED 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY OR NOT 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR OR NOT EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR}")
    set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY "" CACHE FILEPATH "${upper_PRODUCT_LIBRARY_NAME} library" FORCE)
  endif()

  if (NOT DEFINED 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR)
    set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR "" CACHE PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} library")
  endif()

  if (WIN32)
    if (NOT DEFINED 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL OR NOT 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR OR NOT EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR}")
      set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL "" CACHE FILEPATH "${upper_PRODUCT_LIBRARY_NAME} shared library" FORCE)
    endif()
  endif()

  if (WIN32 AND NOT DEFINED 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR)
    set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR "" CACHE PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} shared library")
  endif()

  # check 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_ paths for consistency with specified 3RDPARTY_TBB_DIR
  if (3RDPARTY_TBB_DIR AND EXISTS "${3RDPARTY_TBB_DIR}")
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_TBB_DIR 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY FILEPATH "the path to ${upper_PRODUCT_LIBRARY_NAME} library")

    if (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY AND EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY}")
      get_filename_component (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY}" PATH)
      set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR}" CACHE PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} library" FORCE)
    else()
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_TBB_DIR 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} library")
    endif()

    if (WIN32)
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_TBB_DIR 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL FILEPATH "the path to ${upper_PRODUCT_LIBRARY_NAME} shared library")

      if (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL AND EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL}")
        get_filename_component (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL}" PATH)
        set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR}" CACHE PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} shared library" FORCE)
      else()

      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_TBB_DIR 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} shared library")
      endif()
    endif()
  endif()

  OCCT_MAKE_COMPILER_SHORT_NAME()
  OCCT_MAKE_COMPILER_BITNESS()

  if (${COMPILER_BITNESS} EQUAL 32)
    set (${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME ia32)
  else()
    set (${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME intel64)
  endif()

  # tbb/tbbmalloc library
  if (NOT 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY OR NOT EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY}")
    
    set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .so .dylib .a)
    set (PRODUCT_PATH_SUFFIXES lib ${PRODUCT_LIBRARY_NAME})

    # set 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY as notfound, otherwise find_library can't assign a new value to 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY
    set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY "3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY-NOTFOUND" CACHE FILEPATH "The path to ${upper_PRODUCT_LIBRARY_NAME} library" FORCE)

    if (3RDPARTY_TBB_DIR AND EXISTS "${3RDPARTY_TBB_DIR}")
      if (NOT EXISTS "${3RDPARTY_TBB_DIR}/lib/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}/${COMPILER}")
        if (EXISTS "${3RDPARTY_TBB_DIR}/lib/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}")
          file (GLOB ${upper_PRODUCT_LIBRARY_NAME}_COMPILER_LIST "${3RDPARTY_TBB_DIR}/lib/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}/*")
          if (${upper_PRODUCT_LIBRARY_NAME}_COMPILER_LIST)
            list (GET ${upper_PRODUCT_LIBRARY_NAME}_COMPILER_LIST -1 THE_MOST_FRESH_COMPILER_VERSION)
            if (THE_MOST_FRESH_COMPILER_VERSION)
              get_filename_component (THE_MOST_FRESH_COMPILER_VERSION_NAME "${THE_MOST_FRESH_COMPILER_VERSION}" NAME)
              set (PRODUCT_PATH_SUFFIXES lib ${PRODUCT_LIBRARY_NAME} lib/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}/${THE_MOST_FRESH_COMPILER_VERSION_NAME})
            endif()
          endif()
        endif()
      else()
        set (PRODUCT_PATH_SUFFIXES lib ${PRODUCT_LIBRARY_NAME} lib/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}/${COMPILER})
      endif()

      find_library (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY ${PRODUCT_LIBRARY_NAME}
                                         PATHS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR}" "${3RDPARTY_TBB_DIR}"
                                         PATH_SUFFIXES ${PRODUCT_PATH_SUFFIXES}
                                         CMAKE_FIND_ROOT_PATH_BOTH
                                         NO_DEFAULT_PATH)
    else()
      find_library (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY ${PRODUCT_LIBRARY_NAME}
                                         PATH_SUFFIXES ${PRODUCT_PATH_SUFFIXES}
                                         CMAKE_FIND_ROOT_PATH_BOTH)
    endif()

    if (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY AND EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY}")
      get_filename_component (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY}" PATH)
      set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR}" CACHE PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} library" FORCE)
    else()
      set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR "" CACHE PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} library" FORCE)
    endif()
  endif()

  if (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR AND EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR}")
    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY_DIR)

    set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY "" CACHE FILEPATH "The path to ${upper_PRODUCT_LIBRARY_NAME} library" FORCE)
  endif()

  # tbb/tbbmalloc shared library
  if (WIN32)
    if (NOT 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL OR NOT EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL}")
      set (CMAKE_FIND_LIBRARY_SUFFIXES .dll)
      set (PRODUCT_PATH_SUFFIXES bin)

      # set 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL as notfound, otherwise find_library can't assign a new value to 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL
      set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL "3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL-NOTFOUND" CACHE FILEPATH "${upper_PRODUCT_LIBRARY_NAME} shared library" FORCE)

      if (3RDPARTY_TBB_DIR AND EXISTS "${3RDPARTY_TBB_DIR}")
        if (NOT EXISTS "${3RDPARTY_TBB_DIR}/bin/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}/${COMPILER}")
          if (EXISTS "${3RDPARTY_TBB_DIR}/bin/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}")
            file (GLOB ${upper_PRODUCT_LIBRARY_NAME}_COMPILER_LIST "${3RDPARTY_TBB_DIR}/bin/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}/*")
            if (${upper_PRODUCT_LIBRARY_NAME}_COMPILER_LIST)
              list (GET ${upper_PRODUCT_LIBRARY_NAME}_COMPILER_LIST -1 THE_MOST_FRESH_COMPILER_VERSION)
              if (THE_MOST_FRESH_COMPILER_VERSION)
                get_filename_component (THE_MOST_FRESH_COMPILER_VERSION_NAME "${THE_MOST_FRESH_COMPILER_VERSION}" NAME)
                set (PRODUCT_PATH_SUFFIXES bin bin/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}/${THE_MOST_FRESH_COMPILER_VERSION_NAME})
              endif()
            endif()
          endif()
        else()
          set (PRODUCT_PATH_SUFFIXES bin bin/${${upper_PRODUCT_LIBRARY_NAME}_ARCH_NAME}/${COMPILER})
        endif()

        if (3RDPARTY_TBB_DIR AND EXISTS "${3RDPARTY_TBB_DIR}")
          find_library (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL ${PRODUCT_LIBRARY_NAME}
                                         PATHS "${3RDPARTY_TBB_DIR}"
                                         PATH_SUFFIXES ${PRODUCT_PATH_SUFFIXES}
                                         NO_DEFAULT_PATH)
        else()
          find_library (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL ${PRODUCT_LIBRARY_NAME} PATH_SUFFIXES ${PRODUCT_PATH_SUFFIXES})
        endif()

        if (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL AND EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL}")
          get_filename_component (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL}" PATH)
          set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR}" CACHE PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} library" FORCE)
        else()
          set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR "" CACHE PATH "The directory containing ${upper_PRODUCT_LIBRARY_NAME} shared library" FORCE)

          set (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL "" CACHE FILEPATH "${upper_PRODUCT_LIBRARY_NAME} shared library" FORCE)
        endif()
      endif()
    endif()

    if (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR OR EXISTS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR}")
      list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR}")
    else()
      list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL_DIR)
    endif()
  endif()

  # install tbb/tbbmalloc
  if (INSTALL_TBB)
    OCCT_MAKE_OS_WITH_BITNESS()
    OCCT_MAKE_COMPILER_SHORT_NAME()

    if (WIN32)
      if (SINGLE_GENERATOR)
        install (FILES ${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL} DESTINATION "${INSTALL_DIR_BIN}")
      else()
        install (FILES ${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL}
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_BIN}")
        install (FILES ${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL}
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_BIN}i")
        install (FILES ${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL}
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_BIN}d")
      endif()
    else()
      get_filename_component (PRODUCT_LIBRARY_NAME ${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY} NAME)

      if (SINGLE_GENERATOR)
        install (FILES ${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY}.2
                 DESTINATION "${INSTALL_DIR_LIB}"
                 RENAME ${PRODUCT_LIBRARY_NAME}.2)
      else()
        install (FILES ${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY}.2
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_LIB}"
                 RENAME ${PRODUCT_LIBRARY_NAME}.2)
        install (FILES ${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY}.2
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_LIB}i"
                 RENAME ${PRODUCT_LIBRARY_NAME}.2)
        install (FILES ${3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY}.2
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_LIB}d"
                 RENAME ${PRODUCT_LIBRARY_NAME}.2)
      endif()
    endif()
  endif()
  mark_as_advanced (3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_LIBRARY 3RDPARTY_${upper_PRODUCT_LIBRARY_NAME}_DLL)
endmacro()

if (BUILD_SHARED_LIBS)
  separate_arguments (CSF_TBB)
  foreach (LIB IN LISTS CSF_TBB)
    TBB_PRODUCT_SEARCH (${LIB})
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
endif()