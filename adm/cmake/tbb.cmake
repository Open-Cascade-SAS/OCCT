# tbb

if (MSVC AND BUILD_SHARED_LIBS)
  add_definitions (-D__TBB_NO_IMPLICIT_LINKAGE)
  add_definitions (-D__TBBMALLOC_NO_IMPLICIT_LINKAGE)
endif()

if (NOT DEFINED INSTALL_TBB AND BUILD_SHARED_LIBS)
  set (INSTALL_TBB OFF CACHE BOOL "${INSTALL_TBB_DESCR}")
endif()

# Initialize tbb directory.
if (NOT DEFINED 3RDPARTY_TBB_DIR)
  set (3RDPARTY_TBB_DIR "" CACHE PATH "The directory containing tbb")
endif()

if (WIN32)
  if (NOT DEFINED 3RDPARTY_DIR)
    message (FATAL_ERROR "3RDPARTY_DIR is not defined.")
  endif()
  if ("${3RDPARTY_DIR}" STREQUAL "")
    message (FATAL_ERROR "3RDPARTY_DIR is empty string.")
  endif()
  if (NOT EXISTS "${3RDPARTY_DIR}")
    message (FATAL_ERROR "3RDPARTY_DIR is not exist.")
  endif()

  # Below, we have correct 3RDPARTY_DIR.

  # Initialize TBB folder in connectin with 3RDPARTY_DIR.
  if (("${3RDPARTY_TBB_DIR}" STREQUAL "") OR (NOT EXISTS "${3RDPARTY_TBB_DIR}"))
    FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" TBB TBB_DIR_NAME)
    if (TBB_DIR_NAME)
      set (3RDPARTY_TBB_DIR "${3RDPARTY_DIR}/${TBB_DIR_NAME}" CACHE PATH "The directory containing tbb" FORCE)
    endif()
  endif()

  # Here we have full path name to installation directory of TBB.
  # Employ it.
  if (EXISTS "${3RDPARTY_TBB_DIR}")
    find_package (
      TBB 2021.5
      PATHS "${3RDPARTY_TBB_DIR}" NO_DEFAULT_PATH
      REQUIRED
      CONFIG)

    # Achive include directory
    get_target_property (TBB_INCLUDE_DIR TBB::tbb INTERFACE_INCLUDE_DIRECTORIES)
    if (NOT DEFINED 3RDPARTY_TBB_INCLUDE_DIR)
      set (3RDPARTY_TBB_INCLUDE_DIR "" CACHE PATH "The directory containing headers of the TBB")
    endif()
    if (EXISTS "${TBB_INCLUDE_DIR}")
      set (3RDPARTY_TBB_INCLUDE_DIR "${TBB_INCLUDE_DIR}" CACHE PATH "The directory containing headers of the TBB" FORCE)
      list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_TBB_INCLUDE_DIR}")
    else()
      list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TBB_INCLUDE_DIR)
    endif()

    separate_arguments (CSF_TBB)
    foreach (LIB IN LISTS CSF_TBB)
      string(TOLOWER "${LIB}" LIB_LOWER)
      string(TOUPPER "${LIB}" LIB_UPPER)

      # Achive *.lib files and directory containing it.
      get_target_property (TBB_LIB_FILE "TBB::${LIB_LOWER}" IMPORTED_IMPLIB_RELEASE)
      # Reserve cache variable for *.lib.
      if (NOT DEFINED 3RDPARTY_${LIB_UPPER}_LIBRARY)
        set (3RDPARTY_${LIB_UPPER}_LIBRARY "" CACHE FILEPATH "${LIB_UPPER} library (*.lib)")
      endif()
      # Reserve cache variable for directory containing *.lib file.
      if (NOT DEFINED 3RDPARTY_${LIB_UPPER}_LIBRARY_DIR)
        set (3RDPARTY_${LIB_UPPER}_LIBRARY_DIR "" CACHE PATH "The directory containing ${LIB_UPPER} library (*.lib)")
      endif()
      if (EXISTS "${TBB_LIB_FILE}")
        set (3RDPARTY_${LIB_UPPER}_LIBRARY
             "${TBB_LIB_FILE}"
             CACHE FILEPATH
             "${LIB_UPPER} library (*.lib)"
             FORCE)
        get_filename_component (TBB_LIB_FILE_DIRECTORY "${TBB_LIB_FILE}" DIRECTORY)
        set (3RDPARTY_${LIB_UPPER}_LIBRARY_DIR
             "${TBB_LIB_FILE_DIRECTORY}"
             CACHE PATH
             "The directory containing ${LIB_UPPER} library (*.lib)"
             FORCE)
        list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_${LIB_UPPER}_LIBRARY_DIR}")
      else()
        list (APPEND 3RDPARTY_NO_LIBS 3RDPARTY_${LIB_UPPER}_LIBRARY_DIR)
      endif()

      # Achive *.dll files and directory containing it.
      get_target_property (TBB_DLL_FILE "TBB::${LIB_LOWER}" IMPORTED_LOCATION_RELEASE)
      # Reserve cache variable for *.dll.
      if (NOT DEFINED 3RDPARTY_${LIB_UPPER}_DLL)
        set (3RDPARTY_${LIB_UPPER}_DLL "" CACHE FILEPATH "${LIB_UPPER} library (*.dll)")
      endif()
      # Reserve cache variable for directory containing *.dll file.
      if (NOT DEFINED 3RDPARTY_${LIB_UPPER}_DLL_DIR)
        set (3RDPARTY_${LIB_UPPER}_DLL_DIR "" CACHE PATH "The directory containing ${LIB_UPPER} library (*.dll)")
      endif()
      if (EXISTS "${TBB_DLL_FILE}")
        set (3RDPARTY_${LIB_UPPER}_DLL
             "${TBB_DLL_FILE}"
             CACHE FILEPATH
             "${LIB_UPPER} library (*.dll)"
             FORCE)
        get_filename_component (TBB_DLL_FILE_DIRECTORY "${TBB_DLL_FILE}" DIRECTORY)
        set (3RDPARTY_${LIB_UPPER}_DLL_DIR
             "${TBB_DLL_FILE_DIRECTORY}"
             CACHE PATH
             "The directory containing ${LIB_UPPER} library (*.dll)"
             FORCE)
        list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_${LIB_UPPER}_DLL_DIR}")
      else()
        list (APPEND 3RDPARTY_NO_DLLS 3RDPARTY_${LIB_UPPER}_DLL_DIR)
      endif()

      # install *.dll (tbb & tbbmalloc)
      if (INSTALL_TBB)
        OCCT_MAKE_OS_WITH_BITNESS()
        OCCT_MAKE_COMPILER_SHORT_NAME()

        if (SINGLE_GENERATOR)
          install (FILES ${3RDPARTY_${LIB_UPPER}_DLL} DESTINATION "${INSTALL_DIR_BIN}")
        else()
          install (FILES ${3RDPARTY_${LIB_UPPER}_DLL} CONFIGURATIONS Release DESTINATION "${INSTALL_DIR_BIN}")
          install (FILES ${3RDPARTY_${LIB_UPPER}_DLL} CONFIGURATIONS RelWithDebInfo DESTINATION "${INSTALL_DIR_BIN}i")
          install (FILES ${3RDPARTY_${LIB_UPPER}_DLL} CONFIGURATIONS Debug DESTINATION "${INSTALL_DIR_BIN}d")
        endif()
      endif()
      mark_as_advanced (3RDPARTY_${LIB_UPPER}_LIBRARY 3RDPARTY_${LIB_UPPER}_DLL)
    endforeach()
    if (INSTALL_TBB)
      set (USED_3RDPARTY_TBB_DIR "")
    else()
      # the *.dll/*.so* directory for using by the executable
      set (USED_3RDPARTY_TBB_DIR ${3RDPARTY_TBB_DLL_DIR})
    endif()
  else()
    message (FATAL_ERROR "Installation directory with TBB is not exist.")
  endif()
else ()
  # NOT WIN32 branch
  if ((DEFINED 3RDPARTY_DIR) AND (NOT "${3RDPARTY_DIR}" STREQUAL "") AND (EXISTS "${3RDPARTY_DIR}"))
    # Here, we have correct 3RDPARTY_DIR.
    # Trying to specify TBB folder in connection with 3RDPARTY_DIR
    if (("${3RDPARTY_TBB_DIR}" STREQUAL "") OR (NOT EXISTS "${3RDPARTY_TBB_DIR}"))
      FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" TBB TBB_DIR_NAME)
      if (TBB_DIR_NAME)
        set (3RDPARTY_TBB_DIR "${3RDPARTY_DIR}/${TBB_DIR_NAME}" CACHE PATH "The directory containing tbb" FORCE)
      endif()
    endif()
    if ((NOT "${3RDPARTY_TBB_DIR}" STREQUAL "") AND (EXISTS "${3RDPARTY_TBB_DIR}"))
      # Find TBB 2021.5 in existing directory.
      find_package (
      TBB 2021.5
      PATHS "${3RDPARTY_TBB_DIR}" NO_DEFAULT_PATH
      REQUIRED
      CONFIG)
    else()
      # Find TBB 2021.5 in system directory.
      find_package (
      TBB 2021.5
      REQUIRED
      CONFIG)
    endif()
  else()
    # Find TBB 2021.5 in system directory.
    find_package (
    TBB 2021.5
    REQUIRED
    CONFIG)
  endif()
  # TBB has been configured (in other case FATAL_ERROR occures).

  # Achive include directory.
  get_target_property (TBB_INCLUDE_DIR TBB::tbb INTERFACE_INCLUDE_DIRECTORIES)
  if (NOT DEFINED 3RDPARTY_TBB_INCLUDE_DIR)
    set (3RDPARTY_TBB_INCLUDE_DIR "" CACHE PATH "The directory containing headers of the TBB")
  endif()
  if (EXISTS "${TBB_INCLUDE_DIR}")
    set (3RDPARTY_TBB_INCLUDE_DIR "${TBB_INCLUDE_DIR}" CACHE PATH "The directory containing headers of the TBB" FORCE)
    list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_TBB_INCLUDE_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_TBB_INCLUDE_DIR)
  endif()

  separate_arguments (CSF_TBB)
  foreach (LIB IN LISTS CSF_TBB)
    string(TOLOWER "${LIB}" LIB_LOWER)
    string(TOUPPER "${LIB}" LIB_UPPER)

    # Achive *.so files and directory containing it.
    get_target_property (TBB_SO_FILE "TBB::${LIB_LOWER}" IMPORTED_LOCATION_RELEASE)
    # Reserve cache variable for *.so.
    if (NOT DEFINED 3RDPARTY_${LIB_UPPER}_LIBRARY)
      set (3RDPARTY_${LIB_UPPER}_LIBRARY "" CACHE FILEPATH "${LIB_UPPER} library (*.so)")
    endif()
    # Reserve cache variable for directory containing *.so file.
    if (NOT DEFINED 3RDPARTY_${LIB_UPPER}_LIBRARY_DIR)
      set (3RDPARTY_${LIB_UPPER}_LIBRARY_DIR "" CACHE PATH "The directory containing ${LIB_UPPER} library (*.so)")
    endif()
    if (EXISTS "${TBB_SO_FILE}")
      set (3RDPARTY_${LIB_UPPER}_LIBRARY
           "${TBB_SO_FILE}"
           CACHE FILEPATH
           "${LIB_UPPER} library (*.so)"
           FORCE)
      get_filename_component (TBB_SO_FILE_DIRECTORY "${TBB_SO_FILE}" DIRECTORY)
      set (3RDPARTY_${LIB_UPPER}_LIBRARY_DIR
           "${TBB_SO_FILE_DIRECTORY}"
           CACHE PATH
           "The directory containing ${LIB_UPPER} library (*.so)"
           FORCE)
      list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_${LIB_UPPER}_LIBRARY_DIR}")
    else()
      list (APPEND 3RDPARTY_NO_LIBS 3RDPARTY_${LIB_UPPER}_LIBRARY_DIR)
    endif()

    # install *.so* (tbb & tbbmalloc)
    if (INSTALL_TBB)
      OCCT_MAKE_OS_WITH_BITNESS()
      OCCT_MAKE_COMPILER_SHORT_NAME()

      if (SINGLE_GENERATOR)
        install (FILES ${3RDPARTY_${LIB_UPPER}_LIBRARY} DESTINATION "${INSTALL_DIR_LIB}")
      else()
        install (FILES ${3RDPARTY_${LIB_UPPER}_LIBRARY} CONFIGURATIONS Release DESTINATION "${INSTALL_DIR_LIB}")
        install (FILES ${3RDPARTY_${LIB_UPPER}_LIBRARY} CONFIGURATIONS RelWithDebInfo DESTINATION "${INSTALL_DIR_LIB}i")
        install (FILES ${3RDPARTY_${LIB_UPPER}_LIBRARY} CONFIGURATIONS Debug DESTINATION "${INSTALL_DIR_LIB}d")
      endif()
    endif()
  endforeach()
  if (INSTALL_TBB)
    set (USED_3RDPARTY_TBB_DIR "")
  else()
    # the *.so* directory for using by the executable
    set (USED_3RDPARTY_TBB_DIR ${3RDPARTY_TBB_LIBRARY_DIR})
  endif()
endif()
