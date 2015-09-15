#

macro (OCCT_CHECK_AND_UNSET VARNAME)
  if (DEFINED ${VARNAME})
    unset (${VARNAME} CACHE)
  endif()
endmacro()

macro (OCCT_CHECK_AND_UNSET_GROUP VARNAME)
  OCCT_CHECK_AND_UNSET ("${VARNAME}_DIR")

  OCCT_CHECK_AND_UNSET ("${VARNAME}_INCLUDE_DIR")

  OCCT_CHECK_AND_UNSET ("${VARNAME}_LIBRARY")
  OCCT_CHECK_AND_UNSET ("${VARNAME}_LIBRARY_DIR")

  OCCT_CHECK_AND_UNSET ("${VARNAME}_DLL")
  OCCT_CHECK_AND_UNSET ("${VARNAME}_DLL_DIR")
endmacro()

# BUILD_POSTFIX, IS_BUILD_DEBUG variables
macro (OCCT_MAKE_BUILD_POSTFIX)
  if ("${BUILD_CONFIGURATION}" STREQUAL "Debug")
    set (BUILD_POSTFIX "d")
    set (IS_BUILD_DEBUG "")
  else()
    set (BUILD_POSTFIX "")
    OCCT_CHECK_AND_UNSET (IS_BUILD_DEBUG)
  endif()
endmacro()

# COMPILER_BITNESS variable
macro (OCCT_MAKE_COMPILER_BITNESS)
  math (EXPR COMPILER_BITNESS "32 + 32*(${CMAKE_SIZEOF_VOID_P}/8)")
endmacro()

# OS_WITH_BIT
macro (OCCT_MAKE_OS_WITH_BITNESS)

  OCCT_MAKE_COMPILER_BITNESS()

  if (WIN32)
    set (OS_WITH_BIT "win${COMPILER_BITNESS}")
  elseif(APPLE)
    set (OS_WITH_BIT "mac${COMPILER_BITNESS}")
  else()
    set (OS_WITH_BIT "lin${COMPILER_BITNESS}")
  endif()
endmacro()

# COMPILER variable
macro (OCCT_MAKE_COMPILER_SHORT_NAME)
  if (MSVC)
    if (MSVC70)
      set (COMPILER vc7)
    elseif (MSVC80)
      set (COMPILER vc8)
    elseif (MSVC90)
      set (COMPILER vc9)
    elseif (MSVC10)
      set (COMPILER vc10)
    elseif (MSVC11)
      set (COMPILER vc11)
    elseif (MSVC12)
      set (COMPILER vc12)
    elseif (MSVC14)
      set (COMPILER vc14)
    endif()
  elseif (DEFINED CMAKE_COMPILER_IS_GNUCC)
    set (COMPILER gcc)
  elseif (DEFINED CMAKE_COMPILER_IS_GNUCXX)
    set (COMPILER gxx)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (COMPILER clang)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set (COMPILER icc)
  else()
    set (COMPILER ${CMAKE_GENERATOR})
    string (REGEX REPLACE " " "" COMPILER ${COMPILER})
  endif()
endmacro()

function (SUBDIRECTORY_NAMES MAIN_DIRECTORY RESULT)
  file (GLOB SUB_ITEMS "${MAIN_DIRECTORY}/*")

  foreach (ITEM ${SUB_ITEMS})
    if (IS_DIRECTORY "${ITEM}")
      get_filename_component (ITEM_NAME "${ITEM}" NAME)
      list (APPEND LOCAL_RESULT "${ITEM_NAME}")
    endif()
  endforeach()
  set (${RESULT} ${LOCAL_RESULT} PARENT_SCOPE)
endfunction()

function (FIND_PRODUCT_DIR ROOT_DIR PRODUCT_NAME RESULT)
  OCCT_MAKE_COMPILER_SHORT_NAME()
  OCCT_MAKE_COMPILER_BITNESS()

  string (TOLOWER "${PRODUCT_NAME}" lower_PRODUCT_NAME)

  list (APPEND SEARCH_TEMPLATES "${lower_PRODUCT_NAME}.*${COMPILER}.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "${lower_PRODUCT_NAME}.*[0-9.]+.*${COMPILER}.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "${lower_PRODUCT_NAME}.*[0-9.]+.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "${lower_PRODUCT_NAME}.*[0-9.]+")
  list (APPEND SEARCH_TEMPLATES "${lower_PRODUCT_NAME}")

  SUBDIRECTORY_NAMES ("${ROOT_DIR}" SUBDIR_NAME_LIST)

  foreach (SEARCH_TEMPLATE ${SEARCH_TEMPLATES})
    if (LOCAL_RESULT)
      BREAK()
    endif()

    foreach (SUBDIR_NAME ${SUBDIR_NAME_LIST})
      string (TOLOWER "${SUBDIR_NAME}" lower_SUBDIR_NAME)

      string (REGEX MATCH "${SEARCH_TEMPLATE}" DUMMY_VAR "${lower_SUBDIR_NAME}")
      if (DUMMY_VAR)
        list (APPEND LOCAL_RESULT ${SUBDIR_NAME})
      endif()
    endforeach()
  endforeach()

  if (LOCAL_RESULT)
    list (LENGTH "${LOCAL_RESULT}" LOC_LEN)
    math (EXPR LAST_ELEMENT_INDEX "${LOC_LEN}-1")
    list (GET LOCAL_RESULT ${LAST_ELEMENT_INDEX} DUMMY)
    set (${RESULT} ${DUMMY} PARENT_SCOPE)
  endif()
endfunction()

macro (OCCT_INSTALL_FILE_OR_DIR BEING_INSTALLED_OBJECT DESTINATION_PATH)
  if (BUILD_PATCH_DIR AND EXISTS "${BUILD_PATCH_DIR}/${BEING_INSTALLED_OBJECT}")
    if (IS_DIRECTORY "${BUILD_PATCH_DIR}/${BEING_INSTALLED_OBJECT}")
      # first of all, install original files
      install (DIRECTORY "${CMAKE_SOURCE_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")

      # secondly, rewrite original files with patched ones
      install (DIRECTORY "${BUILD_PATCH_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
    else()
      install (FILES     "${BUILD_PATCH_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
    endif()
  else()
    if (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${BEING_INSTALLED_OBJECT}")
      install (DIRECTORY "${CMAKE_SOURCE_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
    else()
      install (FILES     "${CMAKE_SOURCE_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
    endif()
  endif()
endmacro()

macro (OCCT_CONFIGURE_AND_INSTALL BEING_CONGIRUGED_FILE FINAL_NAME DESTINATION_PATH)
  if (BUILD_PATCH_DIR AND EXISTS "${BUILD_PATCH_DIR}/${BEING_CONGIRUGED_FILE}")
    configure_file("${BUILD_PATCH_DIR}/${BEING_CONGIRUGED_FILE}" "${FINAL_NAME}" @ONLY)
  else()
    configure_file("${CMAKE_SOURCE_DIR}/${BEING_CONGIRUGED_FILE}" "${FINAL_NAME}" @ONLY)
  endif()

  install(FILES "${OCCT_BINARY_DIR}/${FINAL_NAME}" DESTINATION  "${DESTINATION_PATH}")
endmacro()

function (OCCT_IS_PRODUCT_REQUIRED CSF_VAR_NAME USE_PRODUCT)
  set (${USE_PRODUCT} OFF PARENT_SCOPE)
  
  if (NOT USED_TOOLKITS)
    message(STATUS "Warning: the list of being used toolkits is empty")
  else()
    foreach (USED_TOOLKIT ${USED_TOOLKITS})
      if (BUILD_PATCH_DIR AND EXISTS "${BUILD_PATCH_DIR}/src/${USED_TOOLKIT}/EXTERNLIB")
        file (READ "${BUILD_PATCH_DIR}/src/${USED_TOOLKIT}/EXTERNLIB" FILE_CONTENT)
      elseif (EXISTS "${CMAKE_SOURCE_DIR}/src/${USED_TOOLKIT}/EXTERNLIB")
        file (READ "${CMAKE_SOURCE_DIR}/src/${USED_TOOLKIT}/EXTERNLIB" FILE_CONTENT)
      endif()

      string (REGEX MATCH "${CSF_VAR_NAME}" DOES_FILE_CONTAIN "${FILE_CONTENT}")

      if (DOES_FILE_CONTAIN)
        set (${USE_PRODUCT} ON PARENT_SCOPE)
        break()
      endif()
    endforeach()
  endif()
endfunction()




















