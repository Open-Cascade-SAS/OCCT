#

# include occt macros. compiler_bitness, os_wiht_bit, compiler and build_postfix
OCCT_INCLUDE_CMAKE_FILE ("adm/templates/occt_macros")

macro (THIRDPARTY_PRODUCT PRODUCT_NAME HEADER_NAME LIBRARY_NAME LIBRARY_NAME_DEBUG)

  OCCT_MAKE_BUILD_POSTFIX()

  # define 3RDPARTY_${PRODUCT_NAME}_DIR variable is it isn't defined
  if (NOT DEFINED 3RDPARTY_${PRODUCT_NAME}_DIR)
    set (3RDPARTY_${PRODUCT_NAME}_DIR "" CACHE PATH "The directory containing ${PRODUCT_NAME}")
  endif()

  # search for product directory inside 3RDPARTY_DIR directory
  if (NOT 3RDPARTY_${PRODUCT_NAME}_DIR AND 3RDPARTY_DIR)
    FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" "${PRODUCT_NAME}" ${PRODUCT_NAME}_DIR_NAME)
    if (${PRODUCT_NAME}_DIR_NAME)
      message (STATUS "Info: ${PRODUCT_NAME}: ${${PRODUCT_NAME}_DIR_NAME} folder is used")
      set (3RDPARTY_${PRODUCT_NAME}_DIR "${3RDPARTY_DIR}/${${PRODUCT_NAME}_DIR_NAME}" CACHE PATH "The directory containing ${PRODUCT_NAME}" FORCE)
    endif()
  endif()

  if (NOT DEFINED INSTALL_${PRODUCT_NAME})
    set (INSTALL_${PRODUCT_NAME} OFF CACHE BOOL "Is ${PRODUCT_NAME} required to be copied into install directory")
  endif()

  # search for include directory
  if (NOT 3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR}")
    set (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR "3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "The directory containing the headers of the ${PRODUCT_NAME}" FORCE)
    find_path (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR ${HEADER_NAME} PATHS
                                                                    "${3RDPARTY_${PRODUCT_NAME}_DIR}/include"
                                                                    ${3RDPARTY_${PRODUCT_NAME}_ADDITIONAL_PATH_FOR_HEADER}
                                                                   NO_DEFAULT_PATH)
    find_path (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR ${HEADER_NAME})
  endif()
  
  if (NOT 3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR}")
    set (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR "" CACHE FILEPATH "The directory containing the headers of the ${PRODUCT_NAME}" FORCE)
  endif()

  if (NOT 3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR)
    set (3RDPARTY_${PRODUCT_NAME}_LIBRARY "" CACHE FILEPATH "${PRODUCT_NAME} library" FORCE)
  elseif (3RDPARTY_${PRODUCT_NAME}_LIBRARY AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY}")
    get_filename_component(3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_TMP "${3RDPARTY_${PRODUCT_NAME}_LIBRARY}" PATH)
    if (NOT "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR}" STREQUAL "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR_TMP}")
      set (3RDPARTY_${PRODUCT_NAME}_LIBRARY "" CACHE FILEPATH "${PRODUCT_NAME} library" FORCE)
    endif()
  endif()

  # search for library
  if (NOT 3RDPARTY_${PRODUCT_NAME}_LIBRARY OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY}")
    set (3RDPARTY_${PRODUCT_NAME}_LIBRARY "3RDPARTY_${PRODUCT_NAME}_LIBRARY-NOTFOUND" CACHE FILEPATH "${PRODUCT_NAME} library" FORCE)

    # first of all, search for debug version of a library if build type is debug
    if (DEFINED IS_BUILD_DEBUG)
      find_library (3RDPARTY_${PRODUCT_NAME}_LIBRARY ${LIBRARY_NAME_DEBUG}
                                                     PATHS 
                                                      "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR}"
                                                      "${3RDPARTY_${PRODUCT_NAME}_DIR}/lib"
                                                      "${3RDPARTY_${PRODUCT_NAME}_DIR}/libd"
                                                      ${3RDPARTY_${PRODUCT_NAME}_ADDITIONAL_PATH_FOR_LIB}
                                                     NO_DEFAULT_PATH)
      # second search if previous one do not find anything
      find_library (3RDPARTY_${PRODUCT_NAME}_LIBRARY ${LIBRARY_NAME_DEBUG})
    endif()

    # if build type is release or debug version of library isn't found - search for release version of one
    if (NOT 3RDPARTY_${PRODUCT_NAME}_LIBRARY OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY}")
      set (3RDPARTY_${PRODUCT_NAME}_LIBRARY "3RDPARTY_${PRODUCT_NAME}_LIBRARY-NOTFOUND" CACHE FILEPATH "${PRODUCT_NAME} library" FORCE)

      if (DEFINED IS_BUILD_DEBUG)
        message (STATUS "Warning: debug version of ${PRODUCT_NAME} library isn't found (${LIBRARY_NAME_DEBUG}) in ${3RDPARTY_${PRODUCT_NAME}_DIR}/lib(d). Search for release one")
      endif()

      find_library (3RDPARTY_${PRODUCT_NAME}_LIBRARY ${LIBRARY_NAME} PATHS 
                                                                      "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR}"
                                                                      "${3RDPARTY_${PRODUCT_NAME}_DIR}/lib"
                                                                      ${3RDPARTY_${PRODUCT_NAME}_ADDITIONAL_PATH_FOR_LIB}
                                                                     NO_DEFAULT_PATH)
      # second search if previous one do not find anything
      find_library (3RDPARTY_${PRODUCT_NAME}_LIBRARY ${LIBRARY_NAME})
    endif()
  endif()
  
  if (NOT DEFINED 3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR)
    set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR "" CACHE FILEPATH "The directory containing ${PRODUCT_NAME} library" FORCE)
  endif()
  
  # library path
  if (3RDPARTY_${PRODUCT_NAME}_LIBRARY AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY}")
    get_filename_component (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR "${3RDPARTY_${PRODUCT_NAME}_LIBRARY}" PATH)
    set (3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR}" CACHE FILEPATH "The directory containing ${PRODUCT_NAME} library" FORCE)
  endif()
  
  # search for shared library (just for win case)
  if (WIN32)
    set (CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
    
    if (NOT 3RDPARTY_${PRODUCT_NAME}_DLL_DIR)
      set (3RDPARTY_${PRODUCT_NAME}_DLL "" CACHE FILEPATH "${PRODUCT_NAME} shared library" FORCE)
    elseif (3RDPARTY_${PRODUCT_NAME}_DLL AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL}")
      get_filename_component(3RDPARTY_${PRODUCT_NAME}_DLL_DIR_TMP "${3RDPARTY_${PRODUCT_NAME}_DLL}" PATH)
      if (NOT "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR}" STREQUAL "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR_TMP}")
        set (3RDPARTY_${PRODUCT_NAME}_DLL "" CACHE FILEPATH "${PRODUCT_NAME} shared library" FORCE)
      endif()
    endif()

    if (NOT 3RDPARTY_${PRODUCT_NAME}_DLL OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL}")
      set (3RDPARTY_${PRODUCT_NAME}_DLL "3RDPARTY_${PRODUCT_NAME}_DLL-NOTFOUND" CACHE FILEPATH "${PRODUCT_NAME} shared library" FORCE)

      if (DEFINED IS_BUILD_DEBUG)
        find_library (3RDPARTY_${PRODUCT_NAME}_DLL "${LIBRARY_NAME_DEBUG}"
                                                   PATHS
                                                    "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR}"
                                                    "${3RDPARTY_${PRODUCT_NAME}_DIR}/bin"
                                                    "${3RDPARTY_${PRODUCT_NAME}_DIR}/bind"
                                                    ${3RDPARTY_${PRODUCT_NAME}_ADDITIONAL_PATH_FOR_DLL}
                                                   NO_DEFAULT_PATH)
        
        # second search if previous one do not find anything
        find_library (3RDPARTY_${PRODUCT_NAME}_DLL "${LIBRARY_NAME_DEBUG}")
      endif()

      if (NOT 3RDPARTY_${PRODUCT_NAME}_DLL OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL}")
        set (3RDPARTY_${PRODUCT_NAME}_DLL "3RDPARTY_${PRODUCT_NAME}_DLL-NOTFOUND" CACHE FILEPATH "${PRODUCT_NAME} shared library" FORCE)

        if (DEFINED IS_BUILD_DEBUG)
          message (STATUS "Warning: debug version of ${PRODUCT_NAME} dll isn't found (${LIBRARY_NAME_DEBUG}) in ${3RDPARTY_${PRODUCT_NAME}_DIR}/bin(d). Search for release one")
        endif()

        find_library (3RDPARTY_${PRODUCT_NAME}_DLL "${LIBRARY_NAME}" PATHS
                                                                      "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR}"
                                                                      "${3RDPARTY_${PRODUCT_NAME}_DIR}/bin"
                                                                      ${3RDPARTY_${PRODUCT_NAME}_ADDITIONAL_PATH_FOR_DLL}
                                                                     NO_DEFAULT_PATH)
        
        # second search if previous one do not find anything
        find_library (3RDPARTY_${PRODUCT_NAME}_DLL "${LIBRARY_NAME}")
      endif()
    endif()
    
    if (NOT DEFINED 3RDPARTY_${PRODUCT_NAME}_DLL_DIR)
      set (3RDPARTY_${PRODUCT_NAME}_DLL_DIR "" CACHE FILEPATH "The directory containing ${PRODUCT_NAME} shared library" FORCE)
    endif()
    
    # shared library path
    if (3RDPARTY_${PRODUCT_NAME}_DLL AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL}")
      get_filename_component (3RDPARTY_${PRODUCT_NAME}_DLL_DIR "${3RDPARTY_${PRODUCT_NAME}_DLL}" PATH)
      set (3RDPARTY_${PRODUCT_NAME}_DLL_DIR "${3RDPARTY_${PRODUCT_NAME}_DLL_DIR}" CACHE FILEPATH "The directory containing ${PRODUCT_NAME} shared library" FORCE)
    endif()
  endif()
  
  if (3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR}")
    list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${PRODUCT_NAME}_INCLUDE_DIR)
  endif()

  if (3RDPARTY_${PRODUCT_NAME}_LIBRARY AND EXISTS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY}")
    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR}")
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${PRODUCT_NAME}_LIBRARY_DIR)
  endif()

  if (WIN32)
    if (NOT 3RDPARTY_${PRODUCT_NAME}_DLL OR NOT EXISTS "${3RDPARTY_${PRODUCT_NAME}_DLL}")
      list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_${PRODUCT_NAME}_DLL_DIR)
    endif()
  endif()

  if (INSTALL_${PRODUCT_NAME})
    OCCT_MAKE_OS_WITH_BITNESS()
    OCCT_MAKE_COMPILER_SHORT_NAME()
    OCCT_MAKE_BUILD_POSTFIX()

    if (WIN32)
      install (FILES "${3RDPARTY_${PRODUCT_NAME}_DLL}"     DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin${BUILD_POSTFIX}")
    else()
      install (FILES "${3RDPARTY_${PRODUCT_NAME}_LIBRARY}" DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib${BUILD_POSTFIX}")
    endif()
  endif()
  
  mark_as_advanced (3RDPARTY_${PRODUCT_NAME}_LIBRARY 3RDPARTY_${PRODUCT_NAME}_DLL)
endmacro()