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

macro (OCCT_CHECK_AND_UNSET_GROUP GROUPNAME)
  get_cmake_property(VARS VARIABLES)
  string (REGEX MATCHALL "(^|;)${GROUPNAME}[A-Za-z0-9_]*" GROUPNAME_VARS "${VARS}")
  foreach(GROUPNAME_VAR ${GROUPNAME_VARS})
    OCCT_CHECK_AND_UNSET(${GROUPNAME_VAR})
  endforeach()
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

function (FILE_TO_LIST FILE_NAME FILE_CONTENT)
  set (LOCAL_FILE_CONTENT)
  if (EXISTS "${OCCT_ROOT_DIR}/${FILE_NAME}")
    file (STRINGS "${OCCT_ROOT_DIR}/${FILE_NAME}" LOCAL_FILE_CONTENT)
  endif()

  set (${FILE_CONTENT} ${LOCAL_FILE_CONTENT} PARENT_SCOPE)
endfunction()

function(FIND_FOLDER_OR_FILE FILE_OR_FOLDER_NAME RESULT_PATH)
  if (EXISTS "${OCCT_ROOT_DIR}/${FILE_OR_FOLDER_NAME}")
    set (${RESULT_PATH} "${OCCT_ROOT_DIR}/${FILE_OR_FOLDER_NAME}" PARENT_SCOPE)
  else()
    set (${RESULT_PATH} "" PARENT_SCOPE)
  endif()
endfunction()

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
    if (MSVC_VERSION LESS 1914)
      message (FATAL_ERROR "Microsoft Visual C++ 19.14 (VS 2017 15.7) or newer is required for C++17 support")
    endif()
    if ((MSVC_VERSION GREATER 1900) AND (MSVC_VERSION LESS 2000))
      # Since Visual Studio 15 (2017), its version diverged from version of
      # compiler which is 14.1; as that compiler uses the same run-time as 14.0,
      # we keep its id as "vc14" to be compatible
      set (COMPILER vc14)
    else()
      message (FATAL_ERROR "Unrecognized MSVC_VERSION")
    endif()
  elseif (DEFINED CMAKE_COMPILER_IS_GNUCC)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
      message (FATAL_ERROR "GCC version 8.0 or newer is required for C++17 support")
    endif()
    set (COMPILER gcc)
  elseif (DEFINED CMAKE_COMPILER_IS_GNUCXX)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
      message (FATAL_ERROR "GCC version 8.0 or newer is required for C++17 support")
    endif()
    set (COMPILER gxx)
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "[Cc][Ll][Aa][Nn][Gg]")
    if(APPLE)
      if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0.0)
        message (FATAL_ERROR "Apple Clang version 11.0.0 or newer is required for C++17 support")
      endif()
    else()
      if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        message (FATAL_ERROR "Clang version 7.0 or newer is required for C++17 support")
      endif()
    endif()
    set (COMPILER clang)
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "[Ii][Nn][Tt][Ee][Ll]")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17.1.1)
      message (FATAL_ERROR "Intel C++ Compiler version 17.1.1 or newer is required for C++17 support")
    endif()
    set (COMPILER icc)
  else()
    message (AUTHOR_WARNING "Unknown compiler - please verify C++17 support")
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
  file (GLOB ORIGIN_FILES "${OCCT_ROOT_DIR}/${RELATIVE_PATH}/${SEARCH_TEMPLATE}")
  set (${RESULT} ${ORIGIN_FILES} PARENT_SCOPE)
endfunction()

function (FILLUP_PRODUCT_SEARCH_TEMPLATE PRODUCT_NAME COMPILER COMPILER_BITNESS SEARCH_TEMPLATES)
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*${COMPILER}.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+.*${COMPILER}.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "^[a-zA-Z]*[0-9]*-${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+.*${COMPILER}.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*.*${COMPILER_BITNESS}")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*[0-9.]+")
  list (APPEND SEARCH_TEMPLATES "^[^a-zA-Z]*${lower_PRODUCT_NAME}[^a-zA-Z]*")
  set (SEARCH_TEMPLATES ${SEARCH_TEMPLATES} PARENT_SCOPE)
endfunction()

function (FIND_PRODUCT_DIR ROOT_DIR PRODUCT_NAME RESULT)
  OCCT_MAKE_COMPILER_SHORT_NAME()
  OCCT_MAKE_COMPILER_BITNESS()

  string (TOLOWER "${PRODUCT_NAME}" lower_PRODUCT_NAME)
  if ("${lower_PRODUCT_NAME}" STREQUAL "egl")
    string (SUBSTRING "${lower_PRODUCT_NAME}" 1 -1 lower_PRODUCT_NAME)
    list (APPEND SEARCH_TEMPLATES "[^gl]+${lower_PRODUCT_NAME}.*")
  elseif ("${lower_PRODUCT_NAME}" STREQUAL "tbb")
    list (APPEND SEARCH_TEMPLATES "^.*${lower_PRODUCT_NAME}.*")
  else()
    FILLUP_PRODUCT_SEARCH_TEMPLATE(${lower_PRODUCT_NAME} ${COMPILER} ${COMPILER_BITNESS} SEARCH_TEMPLATES)
    if (WIN32 AND "${COMPILER}" STREQUAL "clang")
      # for clang on Windows, search for "vc" as well
      FILLUP_PRODUCT_SEARCH_TEMPLATE(${lower_PRODUCT_NAME} "vc" ${COMPILER_BITNESS} SEARCH_TEMPLATES)
    endif()
  endif()

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
  if (IS_DIRECTORY "${OCCT_ROOT_DIR}/${BEING_INSTALLED_OBJECT}")
    install (DIRECTORY "${OCCT_ROOT_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
  else()
    install (FILES     "${OCCT_ROOT_DIR}/${BEING_INSTALLED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
  endif()
endmacro()

macro (OCCT_CONFIGURE_AND_INSTALL BEING_CONGIRUGED_FILE BUILD_NAME INSTALL_NAME DESTINATION_PATH)
  configure_file("${OCCT_ROOT_DIR}/${BEING_CONGIRUGED_FILE}" "${BUILD_NAME}" @ONLY)
  install(FILES "${OCCT_BINARY_DIR}/${BUILD_NAME}" DESTINATION  "${DESTINATION_PATH}" RENAME ${INSTALL_NAME})
endmacro()

function (EXTRACT_TOOLKIT_PACKAGES RELATIVE_PATH OCCT_TOOLKIT RESULT_PACKAGES)
  set (${RESULT_PACKAGES} ${OCCT_${OCCT_TOOLKIT}_LIST_OF_PACKAGES} PARENT_SCOPE)
endfunction()

function(EXTRACT_TOOLKIT_EXTERNLIB RELATIVE_PATH OCCT_TOOLKIT RESULT_LIBS)
  set (${RESULT_LIBS} ${OCCT_${OCCT_TOOLKIT}_EXTERNAL_LIBS} PARENT_SCOPE)
endfunction()

function (EXTRACT_PACKAGE_FILES RELATIVE_PATH OCCT_PACKAGE RESULT_FILES RESULT_INCLUDE_FOLDER)

  # Package name can be relative path, need to get only the name of the final element
  get_filename_component (OCCT_PACKAGE ${OCCT_PACKAGE} NAME)

  # package name is not unique, it can be reuse in tools and src,
  # use extra parameter as relative path to distinguish between them
  set (OCCT_PACKAGE_FILES "")
  get_property(OCCT_PACKAGE_FILES GLOBAL PROPERTY OCCT_PACKAGE_${RELATIVE_PATH}_${OCCT_PACKAGE}_FILES)
  get_property(OCCT_PACKAGE_INCLUDE_DIR GLOBAL PROPERTY OCCT_PACKAGE_${RELATIVE_PATH}_${OCCT_PACKAGE}_INCLUDE_DIR)
  if (OCCT_PACKAGE_FILES)
    set (${RESULT_FILES} ${OCCT_PACKAGE_FILES} PARENT_SCOPE)
    set (${RESULT_INCLUDE_FOLDER} ${OCCT_PACKAGE_INCLUDE_DIR} PARENT_SCOPE)
    return()
  endif()

  set (OCCT_PACKAGE_INCLUDE_DIR "${OCCT_${OCCT_PACKAGE}_FILES_LOCATION}")
  set (OCCT_PACKAGE_FILES "${OCCT_${OCCT_PACKAGE}_FILES}")

  # collect and search for the files in the package directory or patched one
  # FILE only contains filename that must to be inside package or patched directory
  set (FILE_PATH_LIST)

  foreach (OCCT_FILE ${OCCT_PACKAGE_FILES})
    string (REGEX REPLACE "[^:]+:+" "" OCCT_FILE "${OCCT_FILE}")
    list (APPEND FILE_PATH_LIST "${OCCT_PACKAGE_INCLUDE_DIR}/${OCCT_FILE}")
  endforeach()

  if (NOT FILE_PATH_LIST)
    message (WARNING "FILES has not been found in ${OCCT_PACKAGE_INCLUDE_DIR} for package ${OCCT_PACKAGE}")
  endif()

  set (${RESULT_FILES} ${FILE_PATH_LIST} PARENT_SCOPE)
  set (${RESULT_INCLUDE_FOLDER} ${OCCT_PACKAGE_INCLUDE_DIR} PARENT_SCOPE)
  set_property(GLOBAL PROPERTY OCCT_PACKAGE_${RELATIVE_PATH}_${OCCT_PACKAGE}_FILES "${FILE_PATH_LIST}")
  set_property(GLOBAL PROPERTY OCCT_PACKAGE_${RELATIVE_PATH}_${OCCT_PACKAGE}_INCLUDE_DIR "${OCCT_PACKAGE_INCLUDE_DIR}")
endfunction()

# SOLUTION_TYPE: MODULES, TOOLS, SAMPLES
# OCCT_TOOLKIT: TK*
# RESULT_TKS_AS_DEPS: TK* dependencies
# RESULT_INCLUDE_FOLDERS: include folders
function(EXCTRACT_TOOLKIT_DEPS SOLUTION_TYPE OCCT_TOOLKIT RESULT_TKS_AS_DEPS RESULT_INCLUDE_FOLDERS)
  set (OCCT_TOOLKIT_DEPS "")
  set (OCCT_TOOLKIT_INCLUDE_FOLDERS "")
  get_property(OCCT_TOOLKIT_DEPS GLOBAL PROPERTY OCCT_TOOLKIT_${OCCT_TOOLKIT}_DEPS)
  get_property(OCCT_TOOLKIT_INCLUDE_FOLDERS GLOBAL PROPERTY OCCT_TOOLKIT_${OCCT_TOOLKIT}_INCLUDE_FOLDERS)
  if (OCCT_TOOLKIT_DEPS)
    set (${RESULT_TKS_AS_DEPS} ${OCCT_TOOLKIT_DEPS} PARENT_SCOPE)
    set (${RESULT_INCLUDE_FOLDERS} ${OCCT_TOOLKIT_INCLUDE_FOLDERS} PARENT_SCOPE)
    return()
  endif()
  set (EXTERNAL_LIBS)
  EXTRACT_TOOLKIT_EXTERNLIB (${SOLUTION_TYPE} ${OCCT_TOOLKIT} EXTERNAL_LIBS)
  foreach (EXTERNAL_LIB ${EXTERNAL_LIBS})
    string (REGEX MATCH "^TK" TK_FOUND ${EXTERNAL_LIB})
    if (TK_FOUND)
      list (APPEND OCCT_TOOLKIT_DEPS ${EXTERNAL_LIB})
    endif()
  endforeach()

  set (OCCT_TOOLKIT_PACKAGES)
  EXTRACT_TOOLKIT_PACKAGES (${SOLUTION_TYPE} ${OCCT_TOOLKIT} OCCT_TOOLKIT_PACKAGES)
  foreach(OCCT_PACKAGE ${OCCT_TOOLKIT_PACKAGES})
    EXTRACT_PACKAGE_FILES (${SOLUTION_TYPE} ${OCCT_PACKAGE} OCCT_PACKAGE_FILES OCCT_PACKAGE_INCLUDE_DIR)
    list (APPEND OCCT_TOOLKIT_INCLUDE_FOLDERS ${OCCT_PACKAGE_INCLUDE_DIR})
  endforeach()

  set (${RESULT_TKS_AS_DEPS} ${OCCT_TOOLKIT_DEPS} PARENT_SCOPE)
  set (${RESULT_INCLUDE_FOLDERS} ${OCCT_TOOLKIT_INCLUDE_FOLDERS} PARENT_SCOPE)
  set_property(GLOBAL PROPERTY OCCT_TOOLKIT_${OCCT_TOOLKIT}_DEPS "${OCCT_TOOLKIT_DEPS}")
  set_property(GLOBAL PROPERTY OCCT_TOOLKIT_${OCCT_TOOLKIT}_INCLUDE_FOLDERS "${OCCT_TOOLKIT_INCLUDE_FOLDERS}")
endfunction()

# SOLUTION_TYPE: MODULES, TOOLS, SAMPLES
# OCCT_TOOLKIT: TK*
# RESULT_TKS_AS_DEPS: list of TK* dependencies
# RESULT_INCLUDE_FOLDERS: list of include folders
function(EXCTRACT_TOOLKIT_FULL_DEPS SOLUTION_TYPE OCCT_TOOLKIT RESULT_TKS_AS_DEPS RESULT_INCLUDE_FOLDERS)
  set (OCCT_TOOLKIT_DEPS "")
  set (OCCT_TOOLKIT_INCLUDE_FOLDERS "")
  get_property(OCCT_TOOLKIT_DEPS GLOBAL PROPERTY OCCT_TOOLKIT_${OCCT_TOOLKIT}_FULL_DEPS)
  get_property(OCCT_TOOLKIT_INCLUDE_FOLDERS GLOBAL PROPERTY OCCT_TOOLKIT_${OCCT_TOOLKIT}_FULL_INCLUDE_FOLDERS)
  if (OCCT_TOOLKIT_DEPS)
    set (${RESULT_TKS_AS_DEPS} ${OCCT_TOOLKIT_DEPS} PARENT_SCOPE)
    set (${RESULT_INCLUDE_FOLDERS} ${OCCT_TOOLKIT_INCLUDE_FOLDERS} PARENT_SCOPE)
    return()
  endif()

  EXCTRACT_TOOLKIT_DEPS(${SOLUTION_TYPE} ${OCCT_TOOLKIT} OCCT_TOOLKIT_DEPS OCCT_TOOLKIT_INCLUDE_DIR)
  list(APPEND OCCT_TOOLKIT_FULL_DEPS ${OCCT_TOOLKIT_DEPS})
  list(APPEND OCCT_TOOLKIT_INCLUDE_FOLDERS ${OCCT_TOOLKIT_INCLUDE_DIR})

  foreach(DEP ${OCCT_TOOLKIT_DEPS})
    EXCTRACT_TOOLKIT_FULL_DEPS(${SOLUTION_TYPE} ${DEP} DEP_TOOLKIT_DEPS DEP_INCLUDE_DIRS)
    list(APPEND OCCT_TOOLKIT_FULL_DEPS ${DEP_TOOLKIT_DEPS})
    list(APPEND OCCT_TOOLKIT_INCLUDE_FOLDERS ${DEP_INCLUDE_DIRS})
  endforeach()

  if (OCCT_TOOLKIT_FULL_DEPS)
    list(REMOVE_DUPLICATES OCCT_TOOLKIT_FULL_DEPS)
  endif()

  if (OCCT_TOOLKIT_INCLUDE_FOLDERS)
    list(REMOVE_DUPLICATES OCCT_TOOLKIT_INCLUDE_FOLDERS)
  endif()

  set (${RESULT_TKS_AS_DEPS} ${OCCT_TOOLKIT_FULL_DEPS} PARENT_SCOPE)
  set (${RESULT_INCLUDE_FOLDERS} ${OCCT_TOOLKIT_INCLUDE_FOLDERS} PARENT_SCOPE)
  set_property(GLOBAL PROPERTY OCCT_TOOLKIT_${OCCT_TOOLKIT}_FULL_DEPS "${OCCT_TOOLKIT_FULL_DEPS}")
  set_property(GLOBAL PROPERTY OCCT_TOOLKIT_${OCCT_TOOLKIT}_FULL_INCLUDE_FOLDERS "${OCCT_TOOLKIT_INCLUDE_FOLDERS}")
endfunction()

function (FILE_TO_LIST FILE_NAME FILE_CONTENT)
  set (LOCAL_FILE_CONTENT)
  if (EXISTS "${OCCT_ROOT_DIR}/${FILE_NAME}")
    file (STRINGS "${OCCT_ROOT_DIR}/${FILE_NAME}" LOCAL_FILE_CONTENT)
  endif()

  set (${FILE_CONTENT} ${LOCAL_FILE_CONTENT} PARENT_SCOPE)
endfunction()

function (COLLECT_AND_INSTALL_OCCT_HEADER_FILES THE_ROOT_TARGET_OCCT_DIR THE_OCCT_BUILD_TOOLKITS THE_RELATIVE_PATH THE_OCCT_INSTALL_DIR_PREFIX)
  set (OCCT_USED_PACKAGES)

  # consider patched header.in template
  set (TEMPLATE_HEADER_PATH "${OCCT_ROOT_DIR}/adm/templates/header.in")

  set (OCCT_HEADER_FILES_COMPLETE)
  foreach(OCCT_TOOLKIT ${THE_OCCT_BUILD_TOOLKITS})
    # parse PACKAGES file
    EXTRACT_TOOLKIT_PACKAGES (${THE_RELATIVE_PATH} ${OCCT_TOOLKIT} USED_PACKAGES)
    foreach(OCCT_PACKAGE ${USED_PACKAGES})
      EXTRACT_PACKAGE_FILES (${THE_RELATIVE_PATH} ${OCCT_PACKAGE} ALL_FILES _)
      set (HEADER_FILES_FILTERING ${ALL_FILES})
      list (FILTER HEADER_FILES_FILTERING INCLUDE REGEX ".+[.](h|g|p|lxx|hxx|pxx|hpp|gxx)$")
      list (APPEND OCCT_HEADER_FILES_COMPLETE ${HEADER_FILES_FILTERING})
    endforeach()
    # parse root of the toolkit file
    EXTRACT_PACKAGE_FILES (${THE_RELATIVE_PATH} ${OCCT_TOOLKIT} ALL_FILES _)
    set (HEADER_FILES_FILTERING ${ALL_FILES})
    list (FILTER HEADER_FILES_FILTERING INCLUDE REGEX ".+[.](h|g|p|lxx|hxx|pxx|hpp|gxx)$")
    list (APPEND OCCT_HEADER_FILES_COMPLETE ${HEADER_FILES_FILTERING})
  endforeach()

  # Check that copying is done and match the include installation type.
  # Check by first file in list.
  list(GET OCCT_HEADER_FILES_COMPLETE 0 FIRST_OCCT_HEADER_FILE)
  get_filename_component (FIRST_OCCT_HEADER_FILE ${FIRST_OCCT_HEADER_FILE} NAME)
  set (TO_FORCE_COPY FALSE)
  if (NOT EXISTS "${THE_ROOT_TARGET_OCCT_DIR}/${THE_OCCT_INSTALL_DIR_PREFIX}/${FIRST_OCCT_HEADER_FILE}")
    set (TO_FORCE_COPY TRUE)
  else()
    # get content and check the number of lines inside file.
    # If more then 1 then it is a symlink.
    file (STRINGS "${THE_ROOT_TARGET_OCCT_DIR}/${THE_OCCT_INSTALL_DIR_PREFIX}/${FIRST_OCCT_HEADER_FILE}" FIRST_OCCT_HEADER_FILE_CONTENT)
    list (LENGTH FIRST_OCCT_HEADER_FILE_CONTENT FIRST_OCCT_HEADER_FILE_CONTENT_LEN)
    if (${FIRST_OCCT_HEADER_FILE_CONTENT_LEN} EQUAL 1 AND BUILD_INCLUDE_SYMLINK)
      set (TO_FORCE_COPY TRUE)
    elseif(${FIRST_OCCT_HEADER_FILE_CONTENT_LEN} GREATER 1 AND NOT BUILD_INCLUDE_SYMLINK)
      set (TO_FORCE_COPY TRUE)
    endif()
  endif()
  
  foreach (OCCT_HEADER_FILE ${OCCT_HEADER_FILES_COMPLETE})
    get_filename_component (HEADER_FILE_NAME ${OCCT_HEADER_FILE} NAME)
    set(TARGET_FILE "${THE_ROOT_TARGET_OCCT_DIR}/${THE_OCCT_INSTALL_DIR_PREFIX}/${HEADER_FILE_NAME}")

    # Check if the file already exists in the target directory
    if (TO_FORCE_COPY OR NOT EXISTS "${TARGET_FILE}")
      if (BUILD_INCLUDE_SYMLINK)
        file (CREATE_LINK "${OCCT_HEADER_FILE}" "${TARGET_FILE}" SYMBOLIC)
      else()
        set (OCCT_HEADER_FILE_CONTENT "#include \"${OCCT_HEADER_FILE}\"")
        configure_file ("${TEMPLATE_HEADER_PATH}" "${TARGET_FILE}" @ONLY)
      endif()
    endif()
  endforeach()

  set (OCCT_HEADER_FILES_INSTALLATION ${OCCT_HEADER_FILES_COMPLETE})
  list (FILTER OCCT_HEADER_FILES_INSTALLATION INCLUDE REGEX ".*[.](h|hxx|lxx)$")
  install (FILES ${OCCT_HEADER_FILES_INSTALLATION} DESTINATION "${INSTALL_DIR}/${THE_OCCT_INSTALL_DIR_PREFIX}")
endfunction()

# Macro to configure and install Standard_Version.hxx file
macro (CONFIGURE_AND_INSTALL_VERSION_HEADER)
  if (DEFINED BUILD_OCCT_VERSION_EXT AND "${BUILD_OCCT_VERSION_EXT}" STREQUAL "${OCC_VERSION_STRING_EXT}" AND EXISTS "${CMAKE_BINARY_DIR}/${INSTALL_DIR_INCLUDE}/Standard_Version.hxx")
    install(FILES "${OCCT_BINARY_DIR}/${INSTALL_DIR_INCLUDE}/Standard_Version.hxx" DESTINATION  "${INSTALL_DIR}/${INSTALL_DIR_INCLUDE}")
  else()
    set(BUILD_OCCT_VERSION_EXT "${OCC_VERSION_STRING_EXT}" CACHE STRING "OCCT Version string. Used only for caching, can't impact on build. For modification of version, please check adm/cmake/version.cmake" FORCE)
    mark_as_advanced(BUILD_OCCT_VERSION_EXT)
    string(TIMESTAMP OCCT_VERSION_DATE "%Y-%m-%d" UTC)
    OCCT_CONFIGURE_AND_INSTALL ("adm/templates/Standard_Version.hxx.in" "${INSTALL_DIR_INCLUDE}/Standard_Version.hxx" "Standard_Version.hxx" "${INSTALL_DIR}/${INSTALL_DIR_INCLUDE}")
  endif()
endmacro()

function(ADD_PRECOMPILED_HEADER INPUT_TARGET PRECOMPILED_HEADER THE_IS_PRIVATE)
  if (NOT BUILD_USE_PCH)
    return()
  endif()

  # Angular bracket syntax is achieved using $<ANGLE-R> for closing bracket
  if (${THE_IS_PRIVATE})
    target_precompile_headers(${INPUT_TARGET} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:<${PRECOMPILED_HEADER}$<ANGLE-R>>")
  else()
    target_precompile_headers(${INPUT_TARGET} PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:<${PRECOMPILED_HEADER}$<ANGLE-R>>")
  endif()
endfunction()

macro (OCCT_COPY_FILE_OR_DIR BEING_COPIED_OBJECT DESTINATION_PATH)
  # first of all, copy original files
  if (EXISTS "${OCCT_ROOT_DIR}/${BEING_COPIED_OBJECT}")
    file (COPY "${OCCT_ROOT_DIR}/${BEING_COPIED_OBJECT}" DESTINATION  "${DESTINATION_PATH}")
  endif()
endmacro()

macro (OCCT_CONFIGURE BEING_CONGIRUGED_FILE FINAL_NAME)
  configure_file("${OCCT_ROOT_DIR}/${BEING_CONGIRUGED_FILE}" "${FINAL_NAME}" @ONLY)
endmacro()

macro (OCCT_ADD_SUBDIRECTORY BEING_ADDED_DIRECTORY)
  if (EXISTS "${OCCT_ROOT_DIR}/${BEING_ADDED_DIRECTORY}/CMakeLists.txt")
    add_subdirectory (${OCCT_ROOT_DIR}/${BEING_ADDED_DIRECTORY})
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
      set (FILE_CONTENT)
      EXTRACT_TOOLKIT_EXTERNLIB ("src" ${USED_TOOLKIT} FILE_CONTENT)

      string (REGEX MATCH "${CSF_VAR_NAME}" DOES_FILE_CONTAIN "${FILE_CONTENT}")

      if (DOES_FILE_CONTAIN)
        set (${USE_PRODUCT} ON PARENT_SCOPE)
        break()
      endif()
    endforeach()
  endif()
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

# Function to get list of modules/toolkits/samples from file adm/${FILE_NAME}.
# Creates list <$MODULE_LIST> to store list of MODULES and
# <NAME_OF_MODULE>_TOOLKITS foreach module to store its toolkits, where "TOOLKITS" is defined by TOOLKITS_NAME_SUFFIX.
function (OCCT_MODULES_AND_TOOLKITS FILE_NAME TOOLKITS_NAME_SUFFIX MODULE_LIST)
  FILE_TO_LIST ("adm/${FILE_NAME}" FILE_CONTENT)

  foreach (CONTENT_LINE ${FILE_CONTENT})
    string (REPLACE " " ";" CONTENT_LINE ${CONTENT_LINE})
    list (GET CONTENT_LINE 0 MODULE_NAME)
    list (REMOVE_AT CONTENT_LINE 0)
    list (APPEND ${MODULE_LIST} ${MODULE_NAME})
    # (!) REMOVE THE LINE BELOW (implicit variables)
    set (${MODULE_NAME}_${TOOLKITS_NAME_SUFFIX} ${CONTENT_LINE} PARENT_SCOPE)
  endforeach()

  set (${MODULE_LIST} ${${MODULE_LIST}} PARENT_SCOPE)
endfunction()


# Macro to extract git hash from the source directory
# and store it in the variable GIT_HASH
# in case if git is not found or error occurs, GIT_HASH is set to empty string
macro(OCCT_GET_GIT_HASH)
  set(GIT_HASH "")
  
  find_package(Git QUIET)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${OCCT_ROOT_DIR}
      OUTPUT_VARIABLE GIT_HASH
      ERROR_VARIABLE GIT_ERROR 
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT GIT_ERROR)
      # Check if working directory is clean
      execute_process(
        COMMAND ${GIT_EXECUTABLE} status --porcelain
        WORKING_DIRECTORY ${OCCT_ROOT_DIR}
        OUTPUT_VARIABLE GIT_STATUS
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      if(NOT "${GIT_STATUS}" STREQUAL "")
        message(DEBUG "Git working directory is not clean. Git hash may be incorrect.")
      endif()
    else()
      set(GIT_HASH "")
    endif()
  endif()
endmacro()

# Returns OCC version string
function (OCC_VERSION OCC_VERSION_MAJOR OCC_VERSION_MINOR OCC_VERSION_MAINTENANCE OCC_VERSION_DEVELOPMENT OCC_VERSION_STRING_EXT)

  include (version)
  set (OCC_VERSION_COMPLETE "${OCC_VERSION_MAJOR}.${OCC_VERSION_MINOR}.${OCC_VERSION_MAINTENANCE}")
  set (OCC_VERSION_STRING_EXT "${OCC_VERSION_COMPLETE}")

  set (OCC_VERSION_MAJOR "${OCC_VERSION_MAJOR}" PARENT_SCOPE)
  set (OCC_VERSION_MINOR "${OCC_VERSION_MINOR}" PARENT_SCOPE)
  set (OCC_VERSION_MAINTENANCE "${OCC_VERSION_MAINTENANCE}" PARENT_SCOPE)
  set (OCCT_ON_DEVELOPMENT OFF)
  if (NOT "${OCC_VERSION_DEVELOPMENT}" STREQUAL "" AND NOT "${OCC_VERSION_DEVELOPMENT}" STREQUAL "OCC_VERSION_DEVELOPMENT")
    set (OCCT_ON_DEVELOPMENT ON)
  endif()
  if (${OCCT_ON_DEVELOPMENT})
    set (OCC_VERSION_DEVELOPMENT "${OCC_VERSION_DEVELOPMENT}" PARENT_SCOPE)
  endif()
  
  set (SET_OCC_VERSION_DEVELOPMENT "")
  if (${OCCT_ON_DEVELOPMENT})
    # Use special flag from cache to turn on or off git hash extraction
    if (NOT DEFINED USE_GIT_HASH)
      set (USE_GIT_HASH ON CACHE BOOL "Use git hash in version string")
    endif()
    if (${USE_GIT_HASH})
      OCCT_GET_GIT_HASH()
    endif()
    if (NOT "${GIT_HASH}" STREQUAL "")
      set (OCC_VERSION_DEVELOPMENT "${OCC_VERSION_DEVELOPMENT}-${GIT_HASH}")
      set (OCC_VERSION_DEVELOPMENT "${OCC_VERSION_DEVELOPMENT}" PARENT_SCOPE)
    else()
      set (OCC_VERSION_DEVELOPMENT "${OCC_VERSION_DEVELOPMENT}")
    endif()
    set (OCC_VERSION_STRING_EXT "${OCC_VERSION_COMPLETE}.${OCC_VERSION_DEVELOPMENT}")
    set (OCC_VERSION_STRING_EXT "${OCC_VERSION_STRING_EXT}" PARENT_SCOPE)
    set (SET_OCC_VERSION_DEVELOPMENT "#define OCC_VERSION_DEVELOPMENT \"${OCC_VERSION_DEVELOPMENT}\"")
    set (SET_OCC_VERSION_DEVELOPMENT "${SET_OCC_VERSION_DEVELOPMENT}" PARENT_SCOPE)
  else()
    OCCT_CHECK_AND_UNSET(USE_GIT_HASH)
  endif()
  set (OCC_VERSION_STRING_EXT "${OCC_VERSION_STRING_EXT}" PARENT_SCOPE)
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

macro (FLEX_AND_BISON_TARGET_APPLY THE_PACKAGE_NAME RELATIVE_SOURCES_DIR)
  # Generate Flex and Bison files
  if (NOT ${BUILD_YACCLEX})
    return()
  endif()
  # flex files
  OCCT_ORIGIN_AND_PATCHED_FILES ("${RELATIVE_SOURCES_DIR}/${THE_PACKAGE_NAME}" "*[.]lex" SOURCE_FILES_FLEX)
  list (LENGTH SOURCE_FILES_FLEX SOURCE_FILES_FLEX_LEN)
  # bison files
  OCCT_ORIGIN_AND_PATCHED_FILES ("${RELATIVE_SOURCES_DIR}/${THE_PACKAGE_NAME}" "*[.]yacc" SOURCE_FILES_BISON)
  list (LENGTH SOURCE_FILES_BISON SOURCE_FILES_BISON_LEN)
  if (NOT (${SOURCE_FILES_FLEX_LEN} EQUAL ${SOURCE_FILES_BISON_LEN} AND NOT ${SOURCE_FILES_FLEX_LEN} EQUAL 0))
    message(FATAL_ERROR "Error: number of FLEX and BISON files is not equal for ${THE_PACKAGE_NAME}")
  endif()
  list (SORT SOURCE_FILES_FLEX)
  list (SORT SOURCE_FILES_BISON)
  math (EXPR SOURCE_FILES_FLEX_LEN "${SOURCE_FILES_FLEX_LEN} - 1")
  foreach (FLEX_FILE_INDEX RANGE ${SOURCE_FILES_FLEX_LEN})
    list (GET SOURCE_FILES_FLEX ${FLEX_FILE_INDEX} CURRENT_FLEX_FILE)
    get_filename_component (CURRENT_FLEX_FILE_NAME ${CURRENT_FLEX_FILE} NAME_WE)
    list (GET SOURCE_FILES_BISON ${FLEX_FILE_INDEX} CURRENT_BISON_FILE)
    get_filename_component (CURRENT_BISON_FILE_NAME ${CURRENT_BISON_FILE} NAME_WE)
    string (COMPARE EQUAL ${CURRENT_FLEX_FILE_NAME} ${CURRENT_BISON_FILE_NAME} ARE_FILES_EQUAL)
    if (NOT (EXISTS "${CURRENT_FLEX_FILE}" AND EXISTS "${CURRENT_BISON_FILE}" AND ${ARE_FILES_EQUAL}))
      continue()
    endif()
    # Note: files are generated in original source directory (not in patch!)
    set (FLEX_BISON_TARGET_DIR "${OCCT_ROOT_DIR}/${RELATIVE_SOURCES_DIR}/${THE_PACKAGE_NAME}")
    # choose appropriate extension for generated files: "cxx" if source file contains
    # instruction to generate C++ code, "c" otherwise
    set (BISON_OUTPUT_FILE_EXT "c")
    set (FLEX_OUTPUT_FILE_EXT "c")
    file (STRINGS "${CURRENT_BISON_FILE}" FILE_BISON_CONTENT)
    foreach (FILE_BISON_CONTENT_LINE ${FILE_BISON_CONTENT})
      string (REGEX MATCH "%language \"C\\+\\+\"" CXX_BISON_LANGUAGE_FOUND ${FILE_BISON_CONTENT_LINE})
      if (CXX_BISON_LANGUAGE_FOUND)
        set (BISON_OUTPUT_FILE_EXT "cxx")
      endif()
    endforeach()
    file (STRINGS "${CURRENT_FLEX_FILE}" FILE_FLEX_CONTENT)
    foreach (FILE_FLEX_CONTENT_LINE ${FILE_FLEX_CONTENT})
      string (REGEX MATCH "%option c\\+\\+" CXX_FLEX_LANGUAGE_FOUND ${FILE_FLEX_CONTENT_LINE})
      if (CXX_FLEX_LANGUAGE_FOUND)
        set (FLEX_OUTPUT_FILE_EXT "cxx")
      endif()
    endforeach()
    set (BISON_OUTPUT_FILE ${CURRENT_BISON_FILE_NAME}.tab.${BISON_OUTPUT_FILE_EXT})
    set (FLEX_OUTPUT_FILE lex.${CURRENT_FLEX_FILE_NAME}.${FLEX_OUTPUT_FILE_EXT})
    if (EXISTS ${FLEX_BISON_TARGET_DIR}/${CURRENT_BISON_FILE_NAME}.tab.${BISON_OUTPUT_FILE_EXT})
      message (STATUS "Info: remove old output BISON file: ${FLEX_BISON_TARGET_DIR}/${CURRENT_BISON_FILE_NAME}.tab.${BISON_OUTPUT_FILE_EXT}")
      file(REMOVE ${FLEX_BISON_TARGET_DIR}/${CURRENT_BISON_FILE_NAME}.tab.${BISON_OUTPUT_FILE_EXT})
    endif()
    if (EXISTS ${FLEX_BISON_TARGET_DIR}/${CURRENT_BISON_FILE_NAME}.tab.hxx)
      message (STATUS "Info: remove old output BISON file: ${FLEX_BISON_TARGET_DIR}/${CURRENT_BISON_FILE_NAME}.tab.hxx")
      file(REMOVE ${FLEX_BISON_TARGET_DIR}/${CURRENT_BISON_FILE_NAME}.tab.hxx)
    endif()
    if (EXISTS ${FLEX_BISON_TARGET_DIR}/${FLEX_OUTPUT_FILE})
      message (STATUS "Info: remove old output FLEX file: ${FLEX_BISON_TARGET_DIR}/${FLEX_OUTPUT_FILE}")
      file(REMOVE ${FLEX_BISON_TARGET_DIR}/${FLEX_OUTPUT_FILE})
    endif()
    BISON_TARGET (Parser_${CURRENT_BISON_FILE_NAME} ${CURRENT_BISON_FILE} "${FLEX_BISON_TARGET_DIR}/${BISON_OUTPUT_FILE}"
                  COMPILE_FLAGS "-p ${CURRENT_BISON_FILE_NAME} -l -M ${OCCT_ROOT_DIR}/${RELATIVE_SOURCES_DIR}/=")
    FLEX_TARGET  (Scanner_${CURRENT_FLEX_FILE_NAME} ${CURRENT_FLEX_FILE} "${FLEX_BISON_TARGET_DIR}/${FLEX_OUTPUT_FILE}"
                  COMPILE_FLAGS "-P${CURRENT_FLEX_FILE_NAME} -L")
    ADD_FLEX_BISON_DEPENDENCY (Scanner_${CURRENT_FLEX_FILE_NAME} Parser_${CURRENT_BISON_FILE_NAME})
  endforeach()
endmacro()

# Adds OCCT_INSTALL_BIN_LETTER variable ("" for Release, "d" for Debug and 
# "i" for RelWithDebInfo) in OpenCASCADETargets-*.cmake files during 
# installation process.
# This and the following macros are used to overcome limitation of CMake
# prior to version 3.3 not supporting per-configuration install paths
# for install target files (see https://cmake.org/Bug/view.php?id=14317)
macro (OCCT_UPDATE_TARGET_FILE)
  if (MSVC)
    OCCT_INSERT_CODE_FOR_TARGET ()
  endif()

  install (CODE
  "string (TOLOWER \"\${CMAKE_INSTALL_CONFIG_NAME}\" CMAKE_INSTALL_CONFIG_NAME_LOWERCASE)
  file (GLOB ALL_OCCT_TARGET_FILES \"\$ENV{DESTDIR}${INSTALL_DIR}/${INSTALL_DIR_CMAKE}/OpenCASCADE*Targets-\${CMAKE_INSTALL_CONFIG_NAME_LOWERCASE}.cmake\")
  foreach(TARGET_FILENAME \${ALL_OCCT_TARGET_FILES})
    file (STRINGS \"\${TARGET_FILENAME}\" TARGET_FILE_CONTENT)
    file (REMOVE \"\${TARGET_FILENAME}\")
    foreach (line IN LISTS TARGET_FILE_CONTENT)
      string (REGEX REPLACE \"[\\\\]?[\\\$]{OCCT_INSTALL_BIN_LETTER}\" \"\${OCCT_INSTALL_BIN_LETTER}\" line \"\${line}\")
      file (APPEND \"\${TARGET_FILENAME}\" \"\${line}\\n\")
    endforeach()
  endforeach()")
endmacro()

macro (OCCT_INSERT_CODE_FOR_TARGET)
  if (LAYOUT_IS_VCPKG)
    install(CODE "set (OCCT_INSTALL_BIN_LETTER \"\")")
  else()
    install(CODE "if (\"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$\")
      set (OCCT_INSTALL_BIN_LETTER \"\")
    elseif (\"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$\")
      set (OCCT_INSTALL_BIN_LETTER \"i\")
    elseif (\"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"^([Dd][Ee][Bb][Uu][Gg])$\")
      set (OCCT_INSTALL_BIN_LETTER \"d\")
    endif()")
  endif()
endmacro()

macro (OCCT_UPDATE_DRAW_DEFAULT_FILE)
  install(CODE "set (DRAW_DEFAULT_FILE_NAME \"${INSTALL_DIR}/${INSTALL_DIR_RESOURCE}/DrawResources/DrawPlugin\")
  file (STRINGS \"\${DRAW_DEFAULT_FILE_NAME}\" DRAW_DEFAULT_CONTENT)
  file (REMOVE \"\${DRAW_DEFAULT_FILE_NAME}\")
  foreach (line IN LISTS DRAW_DEFAULT_CONTENT)
    string (REGEX MATCH \": TK\([a-zA-Z]+\)$\" IS_TK_LINE \"\${line}\")
    string (REGEX REPLACE \": TK\([a-zA-Z]+\)$\" \": TK\${CMAKE_MATCH_1}${BUILD_SHARED_LIBRARY_NAME_POSTFIX}\" line \"\${line}\")
    file (APPEND \"\${DRAW_DEFAULT_FILE_NAME}\" \"\${line}\\n\")
  endforeach()")
endmacro()

macro (OCCT_CREATE_SYMLINK_TO_FILE LIBRARY_NAME LINK_NAME)
  if (NOT WIN32)
    install (CODE "if (EXISTS \"${LIBRARY_NAME}\")
        execute_process (COMMAND ln -s \"${LIBRARY_NAME}\" \"${LINK_NAME}\")
      endif()
    ")
  endif()
endmacro()

# Function to process CSF libraries and append their file names to a specified list.
# Additionally, handle library directories for different build configurations.
# Arguments:
#   CURRENT_CSF - The current CSF libraries to process.
#   LIST_NAME - The name of the list to append the processed library file names to.
#   TARGET_NAME - The target to which the library directories will be added.
function (PROCESS_CSF_LIBRARIES CURRENT_CSF LIST_NAME TARGET_NAME)
  separate_arguments (CURRENT_CSF)

  # Local variables to collect found libraries and directories
  set(FOUND_LIBS "")
  set(FOUND_DEBUG_DIRS "")
  set(FOUND_RELEASE_DIRS "")

  # Check if the result is already cached
  string(REPLACE ";" "_" CACHE_KEY "${CURRENT_CSF}")
  get_property(CACHED_LIBS GLOBAL PROPERTY "CACHED_LIBS_${CACHE_KEY}" SET)
  get_property(CACHED_DEBUG_DIRS GLOBAL PROPERTY "CACHED_DEBUG_DIRS_${CACHE_KEY}" SET)
  get_property(CACHED_RELEASE_DIRS GLOBAL PROPERTY "CACHED_RELEASE_DIRS_${CACHE_KEY}" SET)
  if (CACHED_LIBS AND NOT "${CACHED_LIBS}" STREQUAL "1")
    list (APPEND FOUND_LIBS ${CACHED_LIBS})
    if (CACHED_DEBUG_DIRS)
      list (APPEND FOUND_DEBUG_DIRS ${CACHED_DEBUG_DIRS})
    endif()
    if (CACHED_RELEASE_DIRS)
      list (APPEND FOUND_RELEASE_DIRS ${CACHED_RELEASE_DIRS})
    endif()
  else()
    foreach (CSF_LIBRARY ${CURRENT_CSF})
      set (LIBRARY_FROM_CACHE 0)
      set (CSF_LIBRARY_ORIGINAL ${CSF_LIBRARY})
      string (TOLOWER "${CSF_LIBRARY}" CSF_LIBRARY)
      string (REPLACE "+" "[+]" CSF_LIBRARY "${CSF_LIBRARY}")
      string (REPLACE "." "" CSF_LIBRARY "${CSF_LIBRARY}")
      get_cmake_property(ALL_CACHE_VARIABLES CACHE_VARIABLES)
      string (REGEX MATCHALL "(^|;)3RDPARTY_[^;]+_LIBRARY[^;]*" ALL_CACHE_VARIABLES "${ALL_CACHE_VARIABLES}")
      set (DEBUG_DIR "")
      set (RELEASE_DIR "")
      foreach (CACHE_VARIABLE ${ALL_CACHE_VARIABLES})
        set (CURRENT_CACHE_LIBRARY ${${CACHE_VARIABLE}})
        string (TOLOWER "${CACHE_VARIABLE}" CACHE_VARIABLE)
        if (NOT EXISTS "${CURRENT_CACHE_LIBRARY}" OR IS_DIRECTORY "${CURRENT_CACHE_LIBRARY}")
          continue()
        endif()
        string (REGEX MATCH "_${CSF_LIBRARY}$" IS_ENDING "${CACHE_VARIABLE}")
        string (REGEX MATCH "^([a-z]+)" CSF_WO_VERSION "${CSF_LIBRARY}")
        string (REGEX MATCH "_${CSF_WO_VERSION}$" IS_ENDING_WO_VERSION "${CACHE_VARIABLE}")

        if ("3rdparty_${CSF_LIBRARY}_library" STREQUAL "${CACHE_VARIABLE}" OR
            "3rdparty_${CSF_WO_VERSION}_library" STREQUAL "${CACHE_VARIABLE}" OR
            NOT "x${IS_ENDING}" STREQUAL "x" OR
            NOT "x${IS_ENDING_WO_VERSION}" STREQUAL "x")
          get_filename_component(LIBRARY_NAME "${CURRENT_CACHE_LIBRARY}" NAME)
          list (APPEND FOUND_LIBS "${LIBRARY_NAME}")
          get_filename_component(LIBRARY_DIR "${CURRENT_CACHE_LIBRARY}" DIRECTORY)
          set (RELEASE_DIR "${LIBRARY_DIR}")
          set (LIBRARY_FROM_CACHE 1)
        elseif ("3rdparty_${CSF_LIBRARY}_library_debug" STREQUAL "${CACHE_VARIABLE}" OR
                "3rdparty_${CSF_LIBRARY}_debug_library" STREQUAL "${CACHE_VARIABLE}")
          get_filename_component(LIBRARY_NAME "${CURRENT_CACHE_LIBRARY}" NAME)
          list (APPEND FOUND_LIBS "${LIBRARY_NAME}")
          get_filename_component(LIBRARY_DIR "${CURRENT_CACHE_LIBRARY}" DIRECTORY)
          set (DEBUG_DIR "${LIBRARY_DIR}")
          set (LIBRARY_FROM_CACHE 1)
        endif()

        if (DEBUG_DIR AND RELEASE_DIR)
          break()
        endif()
      endforeach()
      if (NOT ${LIBRARY_FROM_CACHE} AND NOT "${CSF_LIBRARY}" STREQUAL "")
        list (APPEND FOUND_LIBS "${CSF_LIBRARY_ORIGINAL}")
        continue()
      endif()
      if (DEBUG_DIR AND RELEASE_DIR)
        list (APPEND FOUND_DEBUG_DIRS "${DEBUG_DIR}")
        list (APPEND FOUND_RELEASE_DIRS "${RELEASE_DIR}")
      elseif (DEBUG_DIR)
        list (APPEND FOUND_DEBUG_DIRS "${DEBUG_DIR}")
        list (APPEND FOUND_RELEASE_DIRS "${DEBUG_DIR}")
        message (WARNING "Debug directory found but no release directory found. Using debug directory for both configurations.")
      elseif (RELEASE_DIR)
        list (APPEND FOUND_DEBUG_DIRS "${RELEASE_DIR}")
        list (APPEND FOUND_RELEASE_DIRS "${RELEASE_DIR}")
      endif()
    endforeach()

    # Cache the result
    set_property(GLOBAL PROPERTY "CACHED_LIBS_${CACHE_KEY}" "${FOUND_LIBS}")
    set_property(GLOBAL PROPERTY "CACHED_DEBUG_DIRS_${CACHE_KEY}" "${FOUND_DEBUG_DIRS}")
    set_property(GLOBAL PROPERTY "CACHED_RELEASE_DIRS_${CACHE_KEY}" "${FOUND_RELEASE_DIRS}")
  endif()

  # Append found values to the external variable
  list(APPEND ${LIST_NAME} ${FOUND_LIBS})
  set(${LIST_NAME} "${${LIST_NAME}}" PARENT_SCOPE)

  # Handle library directories for different build configurations
  list (REMOVE_DUPLICATES FOUND_RELEASE_DIRS)
  list (REMOVE_DUPLICATES FOUND_DEBUG_DIRS)

  foreach (RELEASE_DIR ${FOUND_RELEASE_DIRS})
    get_filename_component(RELEASE_DIR_ABS "${RELEASE_DIR}" ABSOLUTE)
    target_link_directories(${TARGET_NAME} PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_DIR_ABS}>;$<$<CONFIG:RELWITHDEBINFO>:${RELEASE_DIR_ABS}>")
  endforeach()

  foreach (DEBUG_DIR ${FOUND_DEBUG_DIRS})
    get_filename_component(DEBUG_DIR_ABS "${DEBUG_DIR}" ABSOLUTE)
    target_link_directories(${TARGET_NAME} PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_DIR_ABS}>")
  endforeach()
endfunction()
macro(OCCT_ADD_VCPKG_FEATURE THE_FEATURE)
  if (BUILD_USE_VCPKG)
    list(APPEND VCPKG_MANIFEST_FEATURES "${THE_FEATURE}" PARENT_SCOPE)
  endif()
endmacro()

macro (OCCT_UNSET_VCPKG_FEATURE THE_FEATURE)
  if (BUILD_USE_VCPKG)
    list (REMOVE_ITEM VCPKG_MANIFEST_FEATURES "${THE_FEATURE}" PARENT_SCOPE)
  endif()
endmacro()
