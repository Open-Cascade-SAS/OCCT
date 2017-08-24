##

if(OCCT_MACROS_ALREADY_INCLUDED)
  return()
endif()
set(OCCT_MACROS_ALREADY_INCLUDED 1)


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

  if (WIN32)
    OCCT_CHECK_AND_UNSET ("${VARNAME}_DLL")
    OCCT_CHECK_AND_UNSET ("${VARNAME}_DLL_DIR")
  endif()
endmacro()

macro (OCCT_CHECK_AND_UNSET_INSTALL_DIR_SUBDIRS)
  OCCT_CHECK_AND_UNSET (INSTALL_DIR_BIN)
  OCCT_CHECK_AND_UNSET (INSTALL_DIR_SCRIPT)
  OCCT_CHECK_AND_UNSET (INSTALL_DIR_LIB)
  OCCT_CHECK_AND_UNSET (INSTALL_DIR_INCLUDE)
  OCCT_CHECK_AND_UNSET (INSTALL_DIR_RESOURCE)
  OCCT_CHECK_AND_UNSET (INSTALL_DIR_DATA)
  OCCT_CHECK_AND_UNSET (INSTALL_DIR_SAMPLES)
  OCCT_CHECK_AND_UNSET (INSTALL_DIR_TESTS)
  OCCT_CHECK_AND_UNSET (INSTALL_DIR_DOC)
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
    elseif (MSVC15)
      # Since Visual Studio 15 (2017), its version diverged from version of
      # compiler which is 14.1; as that compiler uses the same run-time as 14.0,
      # we keep its id as "vc14" to be compatibille
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

function (FIND_SUBDIRECTORY ROOT_DIRECTORY DIRECTORY_SUFFIX SUBDIRECTORY_NAME)
  #message("Trying to find directory with suffix ${DIRECTORY_SUFFIX} in ${ROOT_DIRECTORY}")
  SUBDIRECTORY_NAMES ("${ROOT_DIRECTORY}" SUBDIR_NAME_LIST)
  #message("Subdirectories: ${SUBDIR_NAME_LIST}")

  #set(${SUBDIRECTORY_NAME} "${SUBDIR_NAME_LIST}" PARENT_SCOPE)

  foreach (SUBDIR_NAME ${SUBDIR_NAME_LIST})
    #message("Subdir: ${SUBDIR_NAME}, ${DIRECTORY_SUFFIX}")
    # REGEX failed if the directory name contains '++' combination, so we replace it
    string(REPLACE "+" "\\+" SUBDIR_NAME_ESCAPED ${SUBDIR_NAME})
    string (REGEX MATCH "${SUBDIR_NAME_ESCAPED}" DOES_PATH_CONTAIN "${DIRECTORY_SUFFIX}")
    if (DOES_PATH_CONTAIN)
      set(${SUBDIRECTORY_NAME} "${ROOT_DIRECTORY}/${SUBDIR_NAME}" PARENT_SCOPE)
      #message("Subdirectory is found: ${SUBDIRECTORY_NAME}")
      BREAK()
    else()
      #message("Check directory: ${ROOT_DIRECTORY}/${SUBDIR_NAME}")
      FIND_SUBDIRECTORY ("${ROOT_DIRECTORY}/${SUBDIR_NAME}" "${DIRECTORY_SUFFIX}" SUBDIR_REC_NAME)
      if (NOT "${SUBDIR_REC_NAME}" STREQUAL "")
        set(${SUBDIRECTORY_NAME} "${SUBDIR_REC_NAME}" PARENT_SCOPE)
        #message("Subdirectory is found: ${SUBDIRECTORY_NAME}")
        BREAK()
      endif()
    endif()
  endforeach()
endfunction()

function (OCCT_ORIGIN_AND_PATCHED_FILES RELATIVE_PATH SEARCH_TEMPLATE RESULT)

  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/${RELATIVE_PATH}")
    file (GLOB FOUND_FILES "${BUILD_PATCH}/${RELATIVE_PATH}/${SEARCH_TEMPLATE}")
  endif()

  file (GLOB ORIGIN_FILES "${CMAKE_SOURCE_DIR}/${RELATIVE_PATH}/${SEARCH_TEMPLATE}")
  foreach (ORIGIN_FILE ${ORIGIN_FILES})
    # check for existence of patched version of current file
    if (NOT BUILD_PATCH OR NOT EXISTS "${BUILD_PATCH}/${RELATIVE_PATH}")
      list (APPEND FOUND_FILES ${ORIGIN_FILE})
    else()
      get_filename_component (ORIGIN_FILE_NAME "${ORIGIN_FILE}" NAME)
      if (NOT EXISTS "${BUILD_PATCH}/${RELATIVE_PATH}/${ORIGIN_FILE_NAME}")
        list (APPEND FOUND_FILES ${ORIGIN_FILE})
      endif()
    endif()
  endforeach()

  set (${RESULT} ${FOUND_FILES} PARENT_SCOPE)
endfunction()

function (FIND_PRODUCT_DIR ROOT_DIR PRODUCT_NAME RESULT)
  OCCT_MAKE_COMPILER_SHORT_NAME()
  OCCT_MAKE_COMPILER_BITNESS()

  string (TOLOWER "${PRODUCT_NAME}" lower_PRODUCT_NAME)

  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*${COMPILER}.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+.*${COMPILER}.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*")

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
    list (GET LOCAL_RESULT -1 DUMMY)
    set (${RESULT} ${DUMMY} PARENT_SCOPE)
  endif()
endfunction()

macro (OCCT_INSTALL_FILE_OR_DIR BEING_INSTALLED_OBJECT DESTINATION_PATH)
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/${BEING_INSTALLED_OBJECT}")
    if (IS_DIRECTORY "${BUILD_PATCH}/${BEING_INSTALLED_OBJECT}")
      # first of all, install original files
      install (DIRECTORY "${CMAKE_SOURCE_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")

      # secondly, rewrite original files with patched ones
      install (DIRECTORY "${BUILD_PATCH}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
    else()
      install (FILES     "${BUILD_PATCH}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
    endif()
  else()
    if (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${BEING_INSTALLED_OBJECT}")
      install (DIRECTORY "${CMAKE_SOURCE_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
    else()
      install (FILES     "${CMAKE_SOURCE_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
    endif()
  endif()
endmacro()

macro (OCCT_CONFIGURE_AND_INSTALL BEING_CONGIRUGED_FILE BUILD_NAME INSTALL_NAME DESTINATION_PATH)
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/${BEING_CONGIRUGED_FILE}")
    configure_file("${BUILD_PATCH}/${BEING_CONGIRUGED_FILE}" "${BUILD_NAME}" @ONLY)
  else()
    configure_file("${CMAKE_SOURCE_DIR}/${BEING_CONGIRUGED_FILE}" "${BUILD_NAME}" @ONLY)
  endif()

  install(FILES "${OCCT_BINARY_DIR}/${BUILD_NAME}" DESTINATION  "${DESTINATION_PATH}" RENAME ${INSTALL_NAME})
endmacro()

macro (COLLECT_AND_INSTALL_OCCT_HEADER_FILES ROOT_TARGET_OCCT_DIR OCCT_BUILD_TOOLKITS OCCT_COLLECT_SOURCE_DIR OCCT_INSTALL_DIR_PREFIX)
  set (OCCT_USED_PACKAGES)

  # consider patched header.in template
  set (TEMPLATE_HEADER_PATH "${CMAKE_SOURCE_DIR}/adm/templates/header.in")
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/adm/templates/header.in")
    set (TEMPLATE_HEADER_PATH "${BUILD_PATCH}/adm/templates/header.in")
  endif()

  set (ROOT_OCCT_DIR ${CMAKE_SOURCE_DIR})

  foreach (OCCT_USED_TOOLKIT ${OCCT_BUILD_TOOLKITS})
    # append all required package folders
    set (OCCT_TOOLKIT_PACKAGES)
    if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/src/${OCCT_USED_TOOLKIT}/PACKAGES")
      file (STRINGS "${BUILD_PATCH}/src/${OCCT_USED_TOOLKIT}/PACKAGES" OCCT_TOOLKIT_PACKAGES)
    elseif (EXISTS "${OCCT_COLLECT_SOURCE_DIR}/${OCCT_USED_TOOLKIT}/PACKAGES")
      file (STRINGS "${OCCT_COLLECT_SOURCE_DIR}/${OCCT_USED_TOOLKIT}/PACKAGES" OCCT_TOOLKIT_PACKAGES)
    endif()

    list (APPEND OCCT_USED_PACKAGES ${OCCT_TOOLKIT_PACKAGES})
  endforeach()

  list (REMOVE_DUPLICATES OCCT_USED_PACKAGES)

  set (OCCT_HEADER_FILES_COMPLETE)
  set (OCCT_HEADER_FILE_NAMES_NOT_IN_FILES)
  set (OCCT_HEADER_FILE_WITH_PROPER_NAMES)

  string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
  message (STATUS "Info: \(${CURRENT_TIME}\) Compare FILES with files in package directories...")

  foreach (OCCT_PACKAGE ${OCCT_USED_PACKAGES})
    if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/FILES")
      file (STRINGS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/FILES" OCCT_ALL_FILE_NAMES)
    elseif (EXISTS "${OCCT_COLLECT_SOURCE_DIR}/${OCCT_PACKAGE}/FILES")
      file (STRINGS "${OCCT_COLLECT_SOURCE_DIR}/${OCCT_PACKAGE}/FILES" OCCT_ALL_FILE_NAMES)
    else()
      message (WARNING "FILES has not been found in ${OCCT_COLLECT_SOURCE_DIR}/${OCCT_PACKAGE}")
      continue()
    endif()

    list (LENGTH OCCT_ALL_FILE_NAMES ALL_FILES_NB)
    math (EXPR ALL_FILES_NB "${ALL_FILES_NB} - 1" )

    # emit warnings if there is unprocessed headers
    file (GLOB OCCT_ALL_FILES_IN_DIR "${OCCT_COLLECT_SOURCE_DIR}/${OCCT_PACKAGE}/*.*")
    file (GLOB OCCT_ALL_FILES_IN_PATCH_DIR "${BUILD_PATCH}/src/${OCCT_PACKAGE}/*.*")

    # use patched header files
    foreach (OCCT_FILE_IN_PATCH_DIR ${OCCT_ALL_FILES_IN_PATCH_DIR})
      get_filename_component (OCCT_FILE_IN_PATCH_DIR_NAME ${OCCT_FILE_IN_PATCH_DIR} NAME)
      list (REMOVE_ITEM OCCT_ALL_FILES_IN_DIR "${OCCT_COLLECT_SOURCE_DIR}/${OCCT_PACKAGE}/${OCCT_FILE_IN_PATCH_DIR_NAME}")
      list (APPEND OCCT_ALL_FILES_IN_DIR "${OCCT_FILE_IN_PATCH_DIR}")
    endforeach()

    foreach (OCCT_FILE_IN_DIR ${OCCT_ALL_FILES_IN_DIR})
      get_filename_component (OCCT_FILE_IN_DIR_NAME ${OCCT_FILE_IN_DIR} NAME)

      set (OCCT_FILE_IN_DIR_STATUS OFF)

      if (${ALL_FILES_NB} LESS 0)
        break()
      endif()

      foreach (FILE_INDEX RANGE ${ALL_FILES_NB})
        list (GET OCCT_ALL_FILE_NAMES ${FILE_INDEX} OCCT_FILE_NAME)

        string (REGEX REPLACE "[^:]+:+" "" OCCT_FILE_NAME "${OCCT_FILE_NAME}")

        if ("${OCCT_FILE_IN_DIR_NAME}" STREQUAL "${OCCT_FILE_NAME}")
          set (OCCT_FILE_IN_DIR_STATUS ON)

          string (REGEX MATCH ".+\\.[hlg]xx|.+\\.h$" IS_HEADER_FOUND "${OCCT_FILE_NAME}")
          if (IS_HEADER_FOUND)
            list (APPEND OCCT_HEADER_FILES_COMPLETE ${OCCT_FILE_IN_DIR})

            # collect header files with name that does not contain its package one
            string (FIND "${OCCT_FILE_NAME}" "${OCCT_PACKAGE}_" FOUND_INDEX)
            if (NOT ${FOUND_INDEX} EQUAL 0)
              list (APPEND OCCT_HEADER_FILE_WITH_PROPER_NAMES "${OCCT_FILE_NAME}")
            endif()
          endif()

          # remove found element from list
          list (REMOVE_AT OCCT_ALL_FILE_NAMES ${FILE_INDEX})
          math (EXPR ALL_FILES_NB "${ALL_FILES_NB} - 1" ) # decrement number

          break()
        endif()
      endforeach()

      if (NOT OCCT_FILE_IN_DIR_STATUS)
        message (STATUS "Warning. File ${OCCT_FILE_IN_DIR} is not listed in ${OCCT_COLLECT_SOURCE_DIR}/${OCCT_PACKAGE}/FILES")

        string (REGEX MATCH ".+\\.[hlg]xx|.+\\.h$" IS_HEADER_FOUND "${OCCT_FILE_NAME}")
        if (IS_HEADER_FOUND)
          list (APPEND OCCT_HEADER_FILE_NAMES_NOT_IN_FILES ${OCCT_FILE_NAME})
        endif()
      endif()
    endforeach()
  endforeach()
  
  # create new file including found header
  string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
  message (STATUS "Info: \(${CURRENT_TIME}\) Create header-links in inc folder...")

  foreach (OCCT_HEADER_FILE ${OCCT_HEADER_FILES_COMPLETE})
    get_filename_component (HEADER_FILE_NAME ${OCCT_HEADER_FILE} NAME)
    set (OCCT_HEADER_FILE_CONTENT "#include \"${OCCT_HEADER_FILE}\"")
    configure_file ("${TEMPLATE_HEADER_PATH}" "${ROOT_TARGET_OCCT_DIR}/${OCCT_INSTALL_DIR_PREFIX}/${HEADER_FILE_NAME}" @ONLY)
  endforeach()
  
  install (FILES ${OCCT_HEADER_FILES_COMPLETE} DESTINATION "${INSTALL_DIR}/${OCCT_INSTALL_DIR_PREFIX}")
  
  string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
  message (STATUS "Info: \(${CURRENT_TIME}\) Checking headers in inc folder...")
    
  file (GLOB OCCT_HEADER_FILES_OLD "${ROOT_TARGET_OCCT_DIR}/${OCCT_INSTALL_DIR_PREFIX}/*")
  foreach (OCCT_HEADER_FILE_OLD ${OCCT_HEADER_FILES_OLD})
    get_filename_component (HEADER_FILE_NAME ${OCCT_HEADER_FILE_OLD} NAME)
    string (REGEX MATCH "^[a-zA-Z0-9]+" PACKAGE_NAME "${HEADER_FILE_NAME}")
    
    list (FIND OCCT_USED_PACKAGES ${PACKAGE_NAME} IS_HEADER_FOUND)
    if (NOT ${IS_HEADER_FOUND} EQUAL -1)
      if (NOT EXISTS "${OCCT_COLLECT_SOURCE_DIR}/${PACKAGE_NAME}/${HEADER_FILE_NAME}")
        message (STATUS "Warning. ${OCCT_HEADER_FILE_OLD} is not presented in the sources and will be removed from ${ROOT_TARGET_OCCT_DIR}/inc")
        file (REMOVE "${OCCT_HEADER_FILE_OLD}")
      else()
        list (FIND OCCT_HEADER_FILE_NAMES_NOT_IN_FILES ${PACKAGE_NAME} IS_HEADER_FOUND)
        if (NOT ${IS_HEADER_FOUND} EQUAL -1)
          message (STATUS "Warning. ${OCCT_HEADER_FILE_OLD} is presented in the sources but not involved in FILES and will be removed from ${ROOT_TARGET_OCCT_DIR}/inc")
          file (REMOVE "${OCCT_HEADER_FILE_OLD}")
        endif()
      endif()
    else()
      set (IS_HEADER_FOUND -1)
      if (NOT "${OCCT_HEADER_FILE_WITH_PROPER_NAMES}" STREQUAL "")
        list (FIND OCCT_HEADER_FILE_WITH_PROPER_NAMES ${HEADER_FILE_NAME} IS_HEADER_FOUND)
      endif()
      
      if (${IS_HEADER_FOUND} EQUAL -1)
        message (STATUS "Warning. \(${PACKAGE_NAME}\) ${OCCT_HEADER_FILE_OLD} is not used and will be removed from ${ROOT_TARGET_OCCT_DIR}/inc")
        file (REMOVE "${OCCT_HEADER_FILE_OLD}")
      endif()
    endif()
  endforeach()
endmacro()

macro (OCCT_COPY_FILE_OR_DIR BEING_COPIED_OBJECT DESTINATION_PATH)
  # first of all, copy original files
  if (EXISTS "${CMAKE_SOURCE_DIR}/${BEING_COPIED_OBJECT}")
    file (COPY "${CMAKE_SOURCE_DIR}/${BEING_COPIED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
  endif()

  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/${BEING_COPIED_OBJECT}")
    # secondly, rewrite original files with patched ones
    file (COPY "${BUILD_PATCH}/${BEING_COPIED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
  endif()
endmacro()

macro (OCCT_CONFIGURE BEING_CONGIRUGED_FILE FINAL_NAME)
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/${BEING_CONGIRUGED_FILE}")
    configure_file("${BUILD_PATCH}/${BEING_CONGIRUGED_FILE}" "${FINAL_NAME}" @ONLY)
  else()
    configure_file("${CMAKE_SOURCE_DIR}/${BEING_CONGIRUGED_FILE}" "${FINAL_NAME}" @ONLY)
  endif()
endmacro()

macro (OCCT_ADD_SUBDIRECTORY BEING_ADDED_DIRECTORY)
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/${BEING_ADDED_DIRECTORY}/CMakeLists.txt")
    add_subdirectory(${BUILD_PATCH}/${BEING_ADDED_DIRECTORY})
  elseif (EXISTS "${CMAKE_SOURCE_DIR}/${BEING_ADDED_DIRECTORY}/CMakeLists.txt")
    add_subdirectory (${CMAKE_SOURCE_DIR}/${BEING_ADDED_DIRECTORY})
  else()
    message (STATUS "${BEING_ADDED_DIRECTORY} directory is not included")
  endif()
endmacro()

function (OCCT_IS_PRODUCT_REQUIRED CSF_VAR_NAME USE_PRODUCT)
  set (${USE_PRODUCT} OFF PARENT_SCOPE)

  if (NOT BUILD_TOOLKITS)
    message(STATUS "Warning: the list of being used toolkits is empty")
  else()
    foreach (USED_TOOLKIT ${BUILD_TOOLKITS})
      if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/src/${USED_TOOLKIT}/EXTERNLIB")
        file (READ "${BUILD_PATCH}/src/${USED_TOOLKIT}/EXTERNLIB" FILE_CONTENT)
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

function (FILE_TO_LIST FILE_NAME FILE_CONTENT)
  set (LOCAL_FILE_CONTENT)
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/${FILE_NAME}")
    file (STRINGS "${BUILD_PATCH}/${FILE_NAME}" LOCAL_FILE_CONTENT)
  elseif (EXISTS "${CMAKE_SOURCE_DIR}/${FILE_NAME}")
    file (STRINGS "${CMAKE_SOURCE_DIR}/${FILE_NAME}" LOCAL_FILE_CONTENT)
  endif()

  set (${FILE_CONTENT} ${LOCAL_FILE_CONTENT} PARENT_SCOPE)
endfunction()

# Function to determine if TOOLKIT is OCCT toolkit
function (IS_OCCT_TOOLKIT TOOLKIT_NAME MODULES IS_TOOLKIT_FOUND)
  set (${IS_TOOLKIT_FOUND} OFF PARENT_SCOPE)
  foreach (MODULE ${${MODULES}})
    set (TOOLKITS ${${MODULE}_TOOLKITS})
    list (FIND TOOLKITS ${TOOLKIT_NAME} FOUND)

    if (NOT ${FOUND} EQUAL -1)
      set (${IS_TOOLKIT_FOUND} ON PARENT_SCOPE)
    endif()
  endforeach(MODULE)
endfunction()

# TOOLKIT_DEPS is defined with dependencies from file src/TOOLKIT_NAME/EXTERNLIB.
# CSF_ variables are ignored
function (OCCT_TOOLKIT_DEP TOOLKIT_NAME TOOLKIT_DEPS)
  FILE_TO_LIST ("src/${TOOLKIT_NAME}/EXTERNLIB" FILE_CONTENT)

  #list (APPEND LOCAL_TOOLKIT_DEPS ${TOOLKIT_NAME})
  set (LOCAL_TOOLKIT_DEPS)
  foreach (FILE_CONTENT_LINE ${FILE_CONTENT})
    string (REGEX MATCH "^TK" TK_FOUND ${FILE_CONTENT_LINE})
    if ("x${FILE_CONTENT_LINE}" STREQUAL "xDRAWEXE" OR NOT "${TK_FOUND}" STREQUAL "")
      list (APPEND LOCAL_TOOLKIT_DEPS ${FILE_CONTENT_LINE})
    endif()
  endforeach()

  set (${TOOLKIT_DEPS} ${LOCAL_TOOLKIT_DEPS} PARENT_SCOPE)
endfunction()

# TOOLKIT_FULL_DEPS is defined with complete dependencies (all levels)
function (OCCT_TOOLKIT_FULL_DEP TOOLKIT_NAME TOOLKIT_FULL_DEPS)
  # first level dependencies are stored in LOCAL_TOOLKIT_FULL_DEPS
  OCCT_TOOLKIT_DEP (${TOOLKIT_NAME} LOCAL_TOOLKIT_FULL_DEPS)

  list (LENGTH LOCAL_TOOLKIT_FULL_DEPS LIST_LENGTH)
  set (LIST_INDEX 0)
  while (LIST_INDEX LESS LIST_LENGTH)
    list (GET LOCAL_TOOLKIT_FULL_DEPS ${LIST_INDEX} CURRENT_TOOLKIT)
    OCCT_TOOLKIT_DEP (${CURRENT_TOOLKIT} CURRENT_TOOLKIT_DEPS)

    # append toolkits are not contained
    foreach (CURRENT_TOOLKIT_DEP ${CURRENT_TOOLKIT_DEPS})
      set (CURRENT_TOOLKIT_DEP_FOUND OFF)
      foreach (LOCAL_TOOLKIT_FULL_DEP ${LOCAL_TOOLKIT_FULL_DEPS})
        if ("${CURRENT_TOOLKIT_DEP}" STREQUAL "${LOCAL_TOOLKIT_FULL_DEP}")
          set (CURRENT_TOOLKIT_DEP_FOUND ON)
          break()
        endif()
      endforeach()
      if ("${CURRENT_TOOLKIT_DEP_FOUND}" STREQUAL "OFF")
        list (APPEND LOCAL_TOOLKIT_FULL_DEPS ${CURRENT_TOOLKIT_DEP})
      endif()
    endforeach()

    # increment the list index
    MATH(EXPR LIST_INDEX "${LIST_INDEX}+1")

    # calculate new length
    list (LENGTH LOCAL_TOOLKIT_FULL_DEPS LIST_LENGTH)
  endwhile()

  set (${TOOLKIT_FULL_DEPS} ${LOCAL_TOOLKIT_FULL_DEPS} PARENT_SCOPE)
endfunction()

# Function to get list of modules and toolkits from file adm/MODULES.
# Creates list <$MODULE_LIST> to store list of MODULES and
# <NAME_OF_MODULE>_TOOLKITS foreach module to store its toolkits.
function (OCCT_MODULES_AND_TOOLKITS MODULE_LIST)
  FILE_TO_LIST ("adm/MODULES" FILE_CONTENT)

  foreach (CONTENT_LINE ${FILE_CONTENT})
    string (REPLACE " " ";" CONTENT_LINE ${CONTENT_LINE})
    list (GET CONTENT_LINE 0 MODULE_NAME)
    list (REMOVE_AT CONTENT_LINE 0)
    list (APPEND ${MODULE_LIST} ${MODULE_NAME})
    # (!) REMOVE THE LINE BELOW (implicit variables)
    set (${MODULE_NAME}_TOOLKITS ${CONTENT_LINE} PARENT_SCOPE)
  endforeach()

  set (${MODULE_LIST} ${${MODULE_LIST}} PARENT_SCOPE)
endfunction()

# Function to get list of tools and toolkits from file adm/TOOLS.
# Creates list <$TOOL_LIST> to store list of TOOLS and
# <NAME_OF_TOOL>_TOOLKITS foreach tool to store its toolkits.
function (OCCT_TOOLS_AND_TOOLKITS TOOL_LIST)
  FILE_TO_LIST ("adm/TOOLS" FILE_CONTENT)

  foreach (CONTENT_LINE ${FILE_CONTENT})
    string (REPLACE " " ";" CONTENT_LINE ${CONTENT_LINE})
    list (GET CONTENT_LINE 0 TOOL_NAME)
    list (REMOVE_AT CONTENT_LINE 0)
    list (APPEND ${TOOL_LIST} ${TOOL_NAME})
    # (!) REMOVE THE LINE BELOW (implicit variables)
    set (${TOOL_NAME}_TOOL_TOOLKITS ${CONTENT_LINE} PARENT_SCOPE)
  endforeach()

  set (${TOOL_LIST} ${${TOOL_LIST}} PARENT_SCOPE)
endfunction()

# Returns OCC version string from file Standard_Version.hxx (if available)
function (OCC_VERSION OCC_VERSION_MAJOR OCC_VERSION_MINOR OCC_VERSION_MAINTENANCE OCC_VERSION_DEVELOPMENT OCC_VERSION_STRING_EXT)

  set (OCC_VERSION_MAJOR         7)
  set (OCC_VERSION_MINOR         0)
  set (OCC_VERSION_MAINTENANCE   0)
  set (OCC_VERSION_DEVELOPMENT   dev)
  set (OCC_VERSION_COMPLETE      "7.0.0")
 
  set (STANDARD_VERSION_FILE "${CMAKE_SOURCE_DIR}/src/Standard/Standard_Version.hxx")
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/src/Standard/Standard_Version.hxx")
    set (STANDARD_VERSION_FILE "${BUILD_PATCH}/src/Standard/Standard_Version.hxx")
  endif()

  if (EXISTS "${STANDARD_VERSION_FILE}")
    foreach (SOUGHT_VERSION OCC_VERSION_MAJOR OCC_VERSION_MINOR OCC_VERSION_MAINTENANCE)
      file (STRINGS "${STANDARD_VERSION_FILE}" ${SOUGHT_VERSION} REGEX "^#define ${SOUGHT_VERSION} .*")
      string (REGEX REPLACE ".*${SOUGHT_VERSION} .*([^ ]+).*" "\\1" ${SOUGHT_VERSION} "${${SOUGHT_VERSION}}" )
    endforeach()
    
    foreach (SOUGHT_VERSION OCC_VERSION_DEVELOPMENT OCC_VERSION_COMPLETE)
      file (STRINGS "${STANDARD_VERSION_FILE}" ${SOUGHT_VERSION} REGEX "^#define ${SOUGHT_VERSION} .*")
      string (REGEX REPLACE ".*${SOUGHT_VERSION} .*\"([^ ]+)\".*" "\\1" ${SOUGHT_VERSION} "${${SOUGHT_VERSION}}" )
    endforeach()
  endif()
 
  set (OCC_VERSION_MAJOR "${OCC_VERSION_MAJOR}" PARENT_SCOPE)
  set (OCC_VERSION_MINOR "${OCC_VERSION_MINOR}" PARENT_SCOPE)
  set (OCC_VERSION_MAINTENANCE "${OCC_VERSION_MAINTENANCE}" PARENT_SCOPE)
  set (OCC_VERSION_DEVELOPMENT "${OCC_VERSION_DEVELOPMENT}" PARENT_SCOPE)
  
  if (OCC_VERSION_DEVELOPMENT AND OCC_VERSION_COMPLETE)
    set (OCC_VERSION_STRING_EXT "${OCC_VERSION_COMPLETE}.${OCC_VERSION_DEVELOPMENT}" PARENT_SCOPE)
  else()
    set (OCC_VERSION_STRING_EXT "${OCC_VERSION_COMPLETE}" PARENT_SCOPE)
  endif()
endfunction()

macro (CHECK_PATH_FOR_CONSISTENCY THE_ROOT_PATH_NAME THE_BEING_CHECKED_PATH_NAME THE_VAR_TYPE THE_MESSAGE_OF_BEING_CHECKED_PATH)
  
  set (THE_ROOT_PATH "${${THE_ROOT_PATH_NAME}}")
  set (THE_BEING_CHECKED_PATH "${${THE_BEING_CHECKED_PATH_NAME}}")

  if (THE_BEING_CHECKED_PATH OR EXISTS "${THE_BEING_CHECKED_PATH}")
    get_filename_component (THE_ROOT_PATH_ABS "${THE_ROOT_PATH}" ABSOLUTE)
    get_filename_component (THE_BEING_CHECKED_PATH_ABS "${THE_BEING_CHECKED_PATH}" ABSOLUTE)

    string (REGEX MATCH "${THE_ROOT_PATH_ABS}" DOES_PATH_CONTAIN "${THE_BEING_CHECKED_PATH_ABS}")

    if (NOT DOES_PATH_CONTAIN) # if cmake found the being checked path at different place from THE_ROOT_PATH_ABS
      set (${THE_BEING_CHECKED_PATH_NAME} "" CACHE ${THE_VAR_TYPE} "${THE_MESSAGE_OF_BEING_CHECKED_PATH}" FORCE)
    endif()
  else()
    set (${THE_BEING_CHECKED_PATH_NAME} "" CACHE ${THE_VAR_TYPE} "${THE_MESSAGE_OF_BEING_CHECKED_PATH}" FORCE)
  endif()

endmacro()

# Adds OCCT_INSTALL_BIN_LETTER variable ("" for Release, "d" for Debug and 
# "i" for RelWithDebInfo) in OpenCASCADETargets-*.cmake files during 
# installation process.
# This and the following macros are used to overcome limitation of CMake
# prior to version 3.3 not supporting per-configuration install paths
# for install target files (see https://cmake.org/Bug/view.php?id=14317)
macro (OCCT_UPDATE_TARGET_FILE)
  if (NOT SINGLE_GENERATOR)
    OCCT_INSERT_CODE_FOR_TARGET ()
  endif()

  install (CODE
  "cmake_policy(PUSH)
  cmake_policy(SET CMP0007 NEW)
  string (TOLOWER \"\${CMAKE_INSTALL_CONFIG_NAME}\" CMAKE_INSTALL_CONFIG_NAME_LOWERCASE)
  file (GLOB ALL_OCCT_TARGET_FILES \"${INSTALL_DIR}/${INSTALL_DIR_CMAKE}/OpenCASCADE*Targets-\${CMAKE_INSTALL_CONFIG_NAME_LOWERCASE}.cmake\")
  foreach(TARGET_FILENAME \${ALL_OCCT_TARGET_FILES})
    file (STRINGS \"\${TARGET_FILENAME}\" TARGET_FILE_CONTENT)
    file (REMOVE \"\${TARGET_FILENAME}\")
    foreach (line IN LISTS TARGET_FILE_CONTENT)
      string (REGEX REPLACE \"[\\\\]?[\\\$]{OCCT_INSTALL_BIN_LETTER}\" \"\${OCCT_INSTALL_BIN_LETTER}\" line \"\${line}\")
      file (APPEND \"\${TARGET_FILENAME}\" \"\${line}\\n\")
    endforeach()
  endforeach()
  cmake_policy(POP)")
endmacro()

macro (OCCT_INSERT_CODE_FOR_TARGET)
  install(CODE "if (\"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$\")
    set (OCCT_INSTALL_BIN_LETTER \"\")
  elseif (\"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$\")
    set (OCCT_INSTALL_BIN_LETTER \"i\")
  elseif (\"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\")
    set (OCCT_INSTALL_BIN_LETTER \"d\")
  endif()")
endmacro()

macro (OCCT_UPDATE_DRAW_DEFAULT_FILE)
  install(CODE "cmake_policy(PUSH)
  cmake_policy(SET CMP0007 NEW)
  set (DRAW_DEFAULT_FILE_NAME \"${INSTALL_DIR}/${INSTALL_DIR_RESOURCE}/DrawResources/DrawPlugin\")
  file (STRINGS \"\${DRAW_DEFAULT_FILE_NAME}\" DRAW_DEFAULT_CONTENT)
  file (REMOVE \"\${DRAW_DEFAULT_FILE_NAME}\")
  foreach (line IN LISTS DRAW_DEFAULT_CONTENT)
    string (REGEX MATCH \": TK\([a-zA-Z]+\)$\" IS_TK_LINE \"\${line}\")
    string (REGEX REPLACE \": TK\([a-zA-Z]+\)$\" \": TK\${CMAKE_MATCH_1}${BUILD_SHARED_LIBRARY_NAME_POSTFIX}\" line \"\${line}\")
    file (APPEND \"\${DRAW_DEFAULT_FILE_NAME}\" \"\${line}\\n\")
  endforeach()
  cmake_policy(POP)")
endmacro()

macro (OCCT_CREATE_SYMLINK_TO_FILE LIBRARY_NAME LINK_NAME)
  if (NOT WIN32)
    install (CODE "if (EXISTS \"${LIBRARY_NAME}\")
        execute_process (COMMAND ln -s \"${LIBRARY_NAME}\" \"${LINK_NAME}\")
      endif()
    ")
  endif()
endmacro()
