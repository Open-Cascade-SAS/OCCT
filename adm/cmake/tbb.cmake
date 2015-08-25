# tbb

if (NOT DEFINED INSTALL_TBB)
  set (INSTALL_TBB OFF CACHE BOOL "${INSTALL_TBB_DESCR}")
endif()

# tbb directory
if (NOT DEFINED 3RDPARTY_TBB_DIR)
  set (3RDPARTY_TBB_DIR "" CACHE PATH "The directory containing tbb")
endif()

# tbb include directory
if (NOT DEFINED 3RDPARTY_TBB_INCLUDE_DIR)
  set (3RDPARTY_TBB_INCLUDE_DIR "" CACHE FILEPATH "The directory containing headers of the tbb")
endif()

# tbb library file (with absolute path)
if (NOT DEFINED 3RDPARTY_TBB_LIBRARY OR NOT 3RDPARTY_TBB_LIBRARY_DIR)
  set (3RDPARTY_TBB_LIBRARY "" CACHE FILEPATH "tbb library" FORCE)
endif()

# tbb library directory
if (NOT DEFINED 3RDPARTY_TBB_LIBRARY_DIR)
  set (3RDPARTY_TBB_LIBRARY_DIR "" CACHE FILEPATH "The directory containing tbb library")
endif()

# tbb malloc library file (with absolute path)
if (NOT DEFINED 3RDPARTY_TBBMALLOC_LIBRARY OR NOT 3RDPARTY_TBBMALLOC_LIBRARY_DIR)
  set (3RDPARTY_TBBMALLOC_LIBRARY "" CACHE FILEPATH "tbb malloc library" FORCE)
endif()

# tbb malloc library directory
if (NOT DEFINED 3RDPARTY_TBBMALLOC_LIBRARY_DIR)
  set (3RDPARTY_TBBMALLOC_LIBRARY_DIR "" CACHE FILEPATH "The directory containing tbb malloc library")
endif()

# tbb shared library (with absolute path)
if (WIN32)
  if (NOT DEFINED 3RDPARTY_TBB_DLL OR NOT 3RDPARTY_TBB_DLL_DIR)
    set (3RDPARTY_TBB_DLL "" CACHE FILEPATH "tbb shared library" FORCE)
  endif()
endif()

# tbb shared library directory
if (WIN32 AND NOT DEFINED 3RDPARTY_TBB_DLL_DIR)
  set (3RDPARTY_TBB_DLL_DIR "" CACHE FILEPATH "The directory containing tbb shared library")
endif()

# tbb malloc shared library (with absolute path)
if (WIN32)
  if (NOT DEFINED 3RDPARTY_TBBMALLOC_DLL OR NOT 3RDPARTY_TBBMALLOC_DLL_DIR)
    set (3RDPARTY_TBBMALLOC_DLL "" CACHE FILEPATH "tbb malloc shared library" FORCE)
  endif()
endif()

# tbb malloc shared library directory
if (WIN32 AND NOT DEFINED 3RDPARTY_TBBMALLOC_DLL_DIR)
  set (3RDPARTY_TBBMALLOC_DLL_DIR "" CACHE FILEPATH "The directory containing tbb malloc shared library")
endif()

if (MSVC)
  add_definitions (-D__TBB_NO_IMPLICIT_LINKAGE)
  add_definitions (-D__TBBMALLOC_NO_IMPLICIT_LINKAGE)
endif()

# include occt macros. compiler_bitness, os_wiht_bit, compiler
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

# search for product directory inside 3RDPARTY_DIR directory
if (NOT 3RDPARTY_TBB_DIR AND 3RDPARTY_DIR)
  FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" "TBB" TBB_DIR_NAME)
  if (TBB_DIR_NAME)
    message (STATUS "Info: TBB: ${TBB_DIR_NAME} folder is used")
    set (3RDPARTY_TBB_DIR "${3RDPARTY_DIR}/${TBB_DIR_NAME}" CACHE PATH "The directory containing tbb" FORCE)
  endif()
endif()

OCCT_MAKE_COMPILER_BITNESS()

if (${COMPILER_BITNESS} STREQUAL 32)
  set (TBB_ARCH_NAME ia32)
else()
  set (TBB_ARCH_NAME intel64)
endif()

# search for include directory in defined 3rdparty directory
if (NOT 3RDPARTY_TBB_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_TBB_INCLUDE_DIR}")
  set (3RDPARTY_TBB_INCLUDE_DIR "3RDPARTY_TBB_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "The directory containing the headers of tbb" FORCE)
  find_path (3RDPARTY_TBB_INCLUDE_DIR tbb/tbb.h PATHS "${3RDPARTY_TBB_DIR}/include" NO_DEFAULT_PATH)
  find_path (3RDPARTY_TBB_INCLUDE_DIR tbb/tbb.h PATHS "${3RDPARTY_TBB_DIR}/include")
endif()

if (NOT 3RDPARTY_TBB_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_TBB_INCLUDE_DIR}")
  set (3RDPARTY_TBB_INCLUDE_DIR "" CACHE FILEPATH "The directory containing the headers of tbb" FORCE)
endif()

OCCT_MAKE_COMPILER_SHORT_NAME()

# TBB_COMPILER_FOLER
#if (WIN32)
  set (TBB_COMPILER_FOLER ${COMPILER})
#else()
#  set (TBB_COMPILER_FOLER ${COMPILER})
#endif()

# search for tbb and tbb malloc library in defined 3rdparty directory
foreach (LIBRARY_NAME TBB TBBMALLOC)
  if (NOT WIN32)
    file (GLOB TBB_SUBDIRS RELATIVE ${3RDPARTY_TBB_DIR}/lib/${TBB_ARCH_NAME}/ ${3RDPARTY_TBB_DIR}/lib/${TBB_ARCH_NAME}/*)
    list (SORT TBB_SUBDIRS)
    list (REVERSE TBB_SUBDIRS)
    list (LENGTH TBB_SUBDIRS TBB_SUBDIRS_LENGTH)
    set (TBB_SUBDIR_FIRST "")
    if (${TBB_SUBDIRS_LENGTH})
      list (GET TBB_SUBDIRS 0 TBB_SUBDIR_FIRST)
      set (3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR "${3RDPARTY_TBB_DIR}/lib/${TBB_ARCH_NAME}/${TBB_SUBDIR_FIRST}")
    endif()
  endif()
  if (NOT 3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR)
    set (3RDPARTY_${LIBRARY_NAME}_LIBRARY "" CACHE FILEPATH "${LIBRARY_NAME} library" FORCE)
  elseif (3RDPARTY_${LIBRARY_NAME}_LIBRARY AND EXISTS "${3RDPARTY_${LIBRARY_NAME}_LIBRARY}")
    get_filename_component(3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR_TMP "${3RDPARTY_${LIBRARY_NAME}_LIBRARY}" PATH)
    if (NOT "${3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR}" STREQUAL "${3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR_TMP}")
      set (3RDPARTY_${LIBRARY_NAME}_LIBRARY "" CACHE FILEPATH "${LIBRARY_NAME} library" FORCE)
    endif()
  endif()

  if (NOT 3RDPARTY_${LIBRARY_NAME}_LIBRARY OR NOT EXISTS "${3RDPARTY_${LIBRARY_NAME}_LIBRARY}")
    set (3RDPARTY_${LIBRARY_NAME}_LIBRARY "3RDPARTY_${LIBRARY_NAME}_LIBRARY-NOTFOUND" CACHE FILEPATH "Path to library of ${LIBRARY_NAME}" FORCE)

    string (TOLOWER "${LIBRARY_NAME}" lower_LIBRARY_NAME)
    find_library (3RDPARTY_${LIBRARY_NAME}_LIBRARY ${lower_LIBRARY_NAME} PATHS
                                                                         "${3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR}"
                                                                         "${3RDPARTY_TBB_DIR}/lib/${TBB_ARCH_NAME}/${TBB_COMPILER_FOLER}"
                                                                         NO_DEFAULT_PATH)

      # second search if previous one do not find anything
      find_library (3RDPARTY_${LIBRARY_NAME}_LIBRARY ${LIBRARY_NAME})
  endif()

  if (NOT 3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR OR NOT EXISTS "${3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR}")
    get_filename_component(3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR "${3RDPARTY_${LIBRARY_NAME}_LIBRARY}" PATH)
  endif()
  set (3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR "${3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR}" CACHE FILEPATH "The directory containing ${LIBRARY_NAME} library" FORCE)

  # search for dll in defined 3rdparty directory (just for win case)
  if (WIN32)
    set (CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")

    if (NOT 3RDPARTY_${LIBRARY_NAME}_DLL_DIR)
      set (3RDPARTY_${LIBRARY_NAME}_DLL "" CACHE FILEPATH "${LIBRARY_NAME} shared library" FORCE)
    elseif (3RDPARTY_${LIBRARY_NAME}_DLL AND EXISTS "${3RDPARTY_${LIBRARY_NAME}_DLL}")
      get_filename_component(3RDPARTY_${LIBRARY_NAME}_DLL_DIR_TMP "${3RDPARTY_${LIBRARY_NAME}_DLL}" PATH)
      if (NOT "${3RDPARTY_${LIBRARY_NAME}_DLL_DIR}" STREQUAL "${3RDPARTY_${LIBRARY_NAME}_DLL_DIR_TMP}")
        set (3RDPARTY_${LIBRARY_NAME}_DLL "" CACHE FILEPATH "${LIBRARY_NAME} shared library" FORCE)
      endif()
    endif()

    if (NOT 3RDPARTY_${LIBRARY_NAME}_DLL OR NOT EXISTS "${3RDPARTY_${LIBRARY_NAME}_DLL}")
      set (3RDPARTY_${LIBRARY_NAME}_DLL "3RDPARTY_${LIBRARY_NAME}_DLL-NOTFOUND" CACHE FILEPATH "Path to shared library of ${LIBRARY_NAME}" FORCE)

      find_library (3RDPARTY_${LIBRARY_NAME}_DLL ${LIBRARY_NAME} PATHS
                                                                 "${3RDPARTY_${LIBRARY_NAME}_DLL_DIR}"
                                                                 "${3RDPARTY_TBB_DIR}/bin/${TBB_ARCH_NAME}/${TBB_COMPILER_FOLER}"
                                                                 NO_DEFAULT_PATH)

        # second search if previous one do not find anything
        find_library (3RDPARTY_${LIBRARY_NAME}_DLL ${LIBRARY_NAME})
    endif()

    if (NOT 3RDPARTY_${LIBRARY_NAME}_DLL_DIR OR NOT EXISTS "${3RDPARTY_${LIBRARY_NAME}_DLL_DIR}")
      get_filename_component(3RDPARTY_${LIBRARY_NAME}_DLL_DIR "${3RDPARTY_${LIBRARY_NAME}_DLL}" PATH)
      set (3RDPARTY_${LIBRARY_NAME}_DLL_DIR "${3RDPARTY_${LIBRARY_NAME}_DLL_DIR}" CACHE FILEPATH "The directory containing ${LIBRARY_NAME} shared library" FORCE)
    endif()
  endif() # end dll search
endforeach() # end tbb / tbbmalloc

# include found paths to common variables
if (3RDPARTY_TBB_INCLUDE_DIR AND EXISTS "${3RDPARTY_TBB_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_TBB_INCLUDE_DIR}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TBB_INCLUDE_DIR)
endif()

foreach (LIBRARY_NAME TBB TBBMALLOC)
  if (3RDPARTY_${LIBRARY_NAME}_LIBRARY AND EXISTS "${3RDPARTY_${LIBRARY_NAME}_LIBRARY}")
    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${LIBRARY_NAME}_LIBRARY_DIR)
  endif()

  if (WIN32)
    if (3RDPARTY_${LIBRARY_NAME}_DLL OR EXISTS "${3RDPARTY_${LIBRARY_NAME}_DLL}")
      list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_${LIBRARY_NAME}_DLL_DIR}")
    else()
      list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${LIBRARY_NAME}_DLL_DIR)
    endif()
  endif()
endforeach()

# install tbb
if (INSTALL_TBB)
  OCCT_MAKE_OS_WITH_BITNESS()
  OCCT_MAKE_COMPILER_SHORT_NAME()

  if (WIN32)
    install (FILES ${3RDPARTY_TBB_DLL} ${3RDPARTY_TBBMALLOC_DLL}
             CONFIGURATIONS Release
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
    install (FILES ${3RDPARTY_TBB_DLL} ${3RDPARTY_TBBMALLOC_DLL}
             CONFIGURATIONS RelWithDebInfo
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bini")
    install (FILES ${3RDPARTY_TBB_DLL} ${3RDPARTY_TBBMALLOC_DLL}
             CONFIGURATIONS Debug
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")
  else()
    get_filename_component(TBBLIB ${3RDPARTY_TBB_LIBRARY} NAME)
    get_filename_component(TBBMALLOCLIB ${3RDPARTY_TBBMALLOC_LIBRARY} NAME)

    install (FILES ${3RDPARTY_TBB_LIBRARY}.2
             CONFIGURATIONS Release
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib"
             RENAME ${TBBLIB}.2)
    install (FILES ${3RDPARTY_TBB_LIBRARY}.2
             CONFIGURATIONS RelWithDebInfo
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libi"
             RENAME ${TBBLIB}.2)
    install (FILES ${3RDPARTY_TBB_LIBRARY}.2
             CONFIGURATIONS Debug
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libd"
             RENAME ${TBBLIB}.2)

    install (FILES ${3RDPARTY_TBBMALLOC_LIBRARY}.2
             CONFIGURATIONS Release
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib"
             RENAME ${TBBMALLOCLIB}.2)
    install (FILES ${3RDPARTY_TBBMALLOC_LIBRARY}.2
             CONFIGURATIONS RelWithDebInfo
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libi"
             RENAME ${TBBMALLOCLIB}.2)
    install (FILES ${3RDPARTY_TBBMALLOC_LIBRARY}.2
             CONFIGURATIONS Debug
             DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libd"
             RENAME ${TBBMALLOCLIB}.2)
  endif()

  set (USED_3RDPARTY_TBB_DIR "")
else()
  # the library directory for using by the executable
  if (WIN32)
    set (USED_3RDPARTY_TBB_DIR ${3RDPARTY_TBB_DLL_DIR})
  else()
    set (USED_3RDPARTY_TBB_DIR ${3RDPARTY_TBB_LIBRARY_DIR})
  endif()
endif()

mark_as_advanced (3RDPARTY_TBB_LIBRARY 3RDPARTY_TBBMALLOC_LIBRARY 3RDPARTY_TBB_DLL 3RDPARTY_TBBMALLOC_DLL)
