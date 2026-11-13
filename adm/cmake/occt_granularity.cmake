# Library Granularity Support for OCCT
# This module provides functions to build OCCT with different library granularity levels:
# - Toolkit: One library per toolkit (default)
# - Module: One library per module
# - Monolithic: Single TKOpenCascade library
#
# Note: Draw module toolkits are ALWAYS built separately as they function as plugins.
# Note: Toolkits with PLUGIN.cmake file are marked as plugins and built separately.

if(OCCT_GRANULARITY_ALREADY_INCLUDED)
  return()
endif()
set(OCCT_GRANULARITY_ALREADY_INCLUDED 1)

#==================================================================================================

# Returns which module contains a given toolkit
# Arguments:
#   TOOLKIT - The toolkit name (e.g., TKernel)
#   MODULE_VAR - Output variable to store the module name
function(OCCT_GET_MODULE_FOR_TOOLKIT TOOLKIT MODULE_VAR)
  set(RESULT_MODULE "")
  foreach(OCCT_MODULE ${OCCT_MODULES})
    list(FIND OCCT_${OCCT_MODULE}_LIST_OF_TOOLKITS ${TOOLKIT} TOOLKIT_INDEX)
    if(NOT TOOLKIT_INDEX EQUAL -1)
      set(RESULT_MODULE ${OCCT_MODULE})
      break()
    endif()
  endforeach()
  set(${MODULE_VAR} ${RESULT_MODULE} PARENT_SCOPE)
endfunction()

#==================================================================================================

# Checks if a toolkit must be built separately (Draw plugins or plugin toolkits with symbol conflicts)
# Plugin toolkits are marked via PLUGIN.cmake files in their source directories.
# Arguments:
#   TOOLKIT - The toolkit name
#   RESULT_VAR - Output variable (TRUE/FALSE)
function(OCCT_IS_SEPARATE_TOOLKIT TOOLKIT RESULT_VAR)
  # Check if it's a Draw module toolkit
  list(FIND OCCT_Draw_LIST_OF_TOOLKITS ${TOOLKIT} DRAW_INDEX)
  if(NOT DRAW_INDEX EQUAL -1)
    set(${RESULT_VAR} TRUE PARENT_SCOPE)
    return()
  endif()

  # Check if toolkit is marked as plugin via PLUGIN.cmake
  # PLUGIN.cmake files set OCCT_<TOOLKIT>_IS_PLUGIN=TRUE
  if(OCCT_${TOOLKIT}_IS_PLUGIN)
    set(${RESULT_VAR} TRUE PARENT_SCOPE)
    return()
  endif()

  set(${RESULT_VAR} FALSE PARENT_SCOPE)
endfunction()

# Legacy alias for backward compatibility
function(OCCT_IS_DRAW_TOOLKIT TOOLKIT RESULT_VAR)
  OCCT_IS_SEPARATE_TOOLKIT(${TOOLKIT} RESULT)
  set(${RESULT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()

#==================================================================================================

# Returns the actual library name for a toolkit based on granularity settings
# Arguments:
#   TOOLKIT - The toolkit name
#   MODULE - The module name (can be empty, will be determined if needed)
#   RESULT_VAR - Output variable to store the library name
function(OCCT_GET_MERGED_LIBRARY_NAME TOOLKIT MODULE RESULT_VAR)
  # Separate toolkits (Draw and plugins) always keep their original name
  OCCT_IS_SEPARATE_TOOLKIT(${TOOLKIT} IS_SEPARATE)
  if(IS_SEPARATE)
    set(${RESULT_VAR} ${TOOLKIT} PARENT_SCOPE)
    return()
  endif()

  if("${BUILD_LIBRARY_GRANULARITY}" STREQUAL "Toolkit")
    set(${RESULT_VAR} ${TOOLKIT} PARENT_SCOPE)
  elseif("${BUILD_LIBRARY_GRANULARITY}" STREQUAL "Module")
    if("${MODULE}" STREQUAL "")
      OCCT_GET_MODULE_FOR_TOOLKIT(${TOOLKIT} MODULE)
    endif()
    set(${RESULT_VAR} "TK${MODULE}" PARENT_SCOPE)
  elseif("${BUILD_LIBRARY_GRANULARITY}" STREQUAL "Monolithic")
    set(${RESULT_VAR} "TKOpenCascade" PARENT_SCOPE)
  else()
    set(${RESULT_VAR} ${TOOLKIT} PARENT_SCOPE)
  endif()
endfunction()

#==================================================================================================

# Resolves a toolkit name to its actual library target name
# Arguments:
#   TOOLKIT - The toolkit name
#   RESULT_VAR - Output variable to store the resolved library name
function(OCCT_RESOLVE_TOOLKIT_LIBRARY TOOLKIT RESULT_VAR)
  OCCT_GET_MODULE_FOR_TOOLKIT(${TOOLKIT} MODULE)
  OCCT_GET_MERGED_LIBRARY_NAME(${TOOLKIT} "${MODULE}" LIBRARY_NAME)
  set(${RESULT_VAR} ${LIBRARY_NAME} PARENT_SCOPE)
endfunction()

#==================================================================================================

# Collects all source files, headers, and include directories from a list of toolkits
# Arguments:
#   TOOLKITS - List of toolkit names
#   SOURCES_VAR - Output variable for source files
#   HEADERS_VAR - Output variable for header files
#   INCLUDES_VAR - Output variable for include directories
function(OCCT_COLLECT_MERGED_SOURCES TOOLKITS SOURCES_VAR HEADERS_VAR INCLUDES_VAR)
  set(ALL_SOURCES)
  set(ALL_HEADERS)
  set(ALL_INCLUDES)

  foreach(TK ${TOOLKITS})
    # Get packages for this toolkit
    set(TK_PACKAGES ${OCCT_${TK}_LIST_OF_PACKAGES})

    foreach(OCCT_PACKAGE ${TK_PACKAGES})
      # Get package files
      EXTRACT_PACKAGE_FILES("src" ${OCCT_PACKAGE} PACKAGE_FILES PACKAGE_INCLUDE_DIR)

      # Filter source and header files
      set(PKG_HEADERS ${PACKAGE_FILES})
      set(PKG_SOURCES ${PACKAGE_FILES})

      list(FILTER PKG_HEADERS INCLUDE REGEX ".+[.](h|p|g|lxx|hxx|pxx|hpp|gxx)$")
      if(APPLE)
        list(FILTER PKG_SOURCES INCLUDE REGEX ".+[.](c|cxx|cpp|mm)$")
      else()
        list(FILTER PKG_SOURCES INCLUDE REGEX ".+[.](c|cpp|cxx)$")
      endif()

      list(APPEND ALL_SOURCES ${PKG_SOURCES})
      list(APPEND ALL_HEADERS ${PKG_HEADERS})
      list(APPEND ALL_INCLUDES ${PACKAGE_INCLUDE_DIR})
    endforeach()

    # Also check for files directly in toolkit directory
    EXTRACT_PACKAGE_FILES("src" ${TK} TK_FILES TK_INCLUDE_DIR)
    if(TK_FILES)
      set(TK_HEADERS ${TK_FILES})
      set(TK_SOURCES ${TK_FILES})

      list(FILTER TK_HEADERS INCLUDE REGEX ".+[.](h|p|g|lxx|hxx|pxx|hpp|gxx)$")
      if(APPLE)
        list(FILTER TK_SOURCES INCLUDE REGEX ".+[.](c|cxx|cpp|mm)$")
      else()
        list(FILTER TK_SOURCES INCLUDE REGEX ".+[.](c|cpp|cxx)$")
      endif()

      list(APPEND ALL_SOURCES ${TK_SOURCES})
      list(APPEND ALL_HEADERS ${TK_HEADERS})
      list(APPEND ALL_INCLUDES ${TK_INCLUDE_DIR})
    endif()
  endforeach()

  if(ALL_INCLUDES)
    list(REMOVE_DUPLICATES ALL_INCLUDES)
  endif()

  set(${SOURCES_VAR} ${ALL_SOURCES} PARENT_SCOPE)
  set(${HEADERS_VAR} ${ALL_HEADERS} PARENT_SCOPE)
  set(${INCLUDES_VAR} ${ALL_INCLUDES} PARENT_SCOPE)
endfunction()

#==================================================================================================

# Computes external dependencies for a merged library, filtering out internal dependencies
# Arguments:
#   TOOLKITS - List of toolkits being merged
#   LIB_NAME - Name of the merged library
#   TK_DEPS_VAR - Output variable for toolkit dependencies (external to this library)
#   CSF_DEPS_VAR - Output variable for CSF dependencies
function(OCCT_COMPUTE_MERGED_DEPENDENCIES TOOLKITS LIB_NAME TK_DEPS_VAR CSF_DEPS_VAR)
  set(ALL_TK_DEPS)
  set(ALL_CSF_DEPS)

  foreach(TK ${TOOLKITS})
    # Get external libs for this toolkit
    set(EXTERNLIB ${OCCT_${TK}_EXTERNAL_LIBS})

    foreach(DEP ${EXTERNLIB})
      string(REGEX MATCH "^CSF_" IS_CSF "${DEP}")
      string(REGEX MATCH "^TK" IS_TK "${DEP}")
      string(REGEX MATCH "^vtk" IS_VTK "${DEP}")

      if(IS_CSF)
        list(APPEND ALL_CSF_DEPS ${DEP})
      elseif(IS_TK)
        # Check if this TK dependency is internal (part of the same merged library)
        list(FIND TOOLKITS ${DEP} IS_INTERNAL)
        if(IS_INTERNAL EQUAL -1)
          # External dependency - map to its merged library name
          OCCT_RESOLVE_TOOLKIT_LIBRARY(${DEP} MAPPED_DEP)
          list(APPEND ALL_TK_DEPS ${MAPPED_DEP})
        endif()
      elseif(IS_VTK)
        list(APPEND ALL_TK_DEPS ${DEP})
      endif()
    endforeach()
  endforeach()

  if(ALL_TK_DEPS)
    list(REMOVE_DUPLICATES ALL_TK_DEPS)
    # Remove self-reference if present
    list(REMOVE_ITEM ALL_TK_DEPS ${LIB_NAME})
  endif()

  if(ALL_CSF_DEPS)
    list(REMOVE_DUPLICATES ALL_CSF_DEPS)
  endif()

  set(${TK_DEPS_VAR} ${ALL_TK_DEPS} PARENT_SCOPE)
  set(${CSF_DEPS_VAR} ${ALL_CSF_DEPS} PARENT_SCOPE)
endfunction()

#==================================================================================================

# Builds a merged library from multiple toolkits
# Arguments:
#   LIB_NAME - Name of the library to create
#   TOOLKITS - List of toolkits to merge
#   MODULE - Module name for folder organization
function(OCCT_BUILD_MERGED_LIBRARY LIB_NAME TOOLKITS MODULE)
  message(STATUS "Building merged library: ${LIB_NAME} from toolkits: ${TOOLKITS}")

  # Collect all sources
  OCCT_COLLECT_MERGED_SOURCES("${TOOLKITS}" MERGED_SOURCES MERGED_HEADERS MERGED_INCLUDES)

  if(NOT MERGED_SOURCES)
    message(WARNING "No source files found for merged library ${LIB_NAME}")
    return()
  endif()

  # Create the library
  add_library(${LIB_NAME} ${MERGED_SOURCES} ${MERGED_HEADERS})

  # Set include directories
  target_include_directories(${LIB_NAME} PRIVATE ${MERGED_INCLUDES})
  target_include_directories(${LIB_NAME} PRIVATE "${CMAKE_BINARY_DIR}/${INSTALL_DIR_INCLUDE}")
  if(3RDPARTY_INCLUDE_DIRS)
    target_include_directories(${LIB_NAME} PRIVATE ${3RDPARTY_INCLUDE_DIRS})
  endif()

  # Add export definitions for all contained toolkits
  set(PRECOMPILED_DEFS)
  foreach(TK ${TOOLKITS})
    string(TOUPPER ${TK} TK_UPPER)
    list(APPEND PRECOMPILED_DEFS "${TK_UPPER}_EXPORTS")
  endforeach()

  if(NOT BUILD_SHARED_LIBS)
    list(APPEND PRECOMPILED_DEFS "OCCT_NO_PLUGINS")
    if(WIN32)
      list(APPEND PRECOMPILED_DEFS "OCCT_STATIC_BUILD")
    endif()
  endif()

  target_compile_definitions(${LIB_NAME} PRIVATE ${PRECOMPILED_DEFS})

  # Compute and link dependencies
  OCCT_COMPUTE_MERGED_DEPENDENCIES("${TOOLKITS}" ${LIB_NAME} TK_DEPENDENCIES CSF_DEPENDENCIES)

  if(TK_DEPENDENCIES)
    target_link_libraries(${LIB_NAME} PUBLIC ${TK_DEPENDENCIES})
  endif()

  # Process CSF dependencies
  set(EXTERNAL_LIBS)
  foreach(CSF_DEP ${CSF_DEPENDENCIES})
    set(CURRENT_CSF ${${CSF_DEP}})
    if(NOT "${CURRENT_CSF}" STREQUAL "")
      PROCESS_CSF_LIBRARIES("${CURRENT_CSF}" EXTERNAL_LIBS "${LIB_NAME}")
    endif()
  endforeach()

  if(EXTERNAL_LIBS)
    list(REMOVE_DUPLICATES EXTERNAL_LIBS)
    target_link_libraries(${LIB_NAME} PRIVATE ${EXTERNAL_LIBS})
  endif()

  # Set properties
  set_target_properties(${LIB_NAME} PROPERTIES FOLDER "Modules/${MODULE}")
  set_target_properties(${LIB_NAME} PROPERTIES MODULE "${MODULE}")

  # Set version properties
  get_property(OCC_VERSION_MAJOR GLOBAL PROPERTY OCC_VERSION_MAJOR)
  get_property(OCC_VERSION_MINOR GLOBAL PROPERTY OCC_VERSION_MINOR)
  get_property(OCC_VERSION_MAINTENANCE GLOBAL PROPERTY OCC_VERSION_MAINTENANCE)

  set(OCC_SOVERSION "")
  if(BUILD_SOVERSION_NUMBERS GREATER 2)
    set(OCC_SOVERSION "${OCC_VERSION_MAJOR}.${OCC_VERSION_MINOR}.${OCC_VERSION_MAINTENANCE}")
  elseif(BUILD_SOVERSION_NUMBERS GREATER 1)
    set(OCC_SOVERSION "${OCC_VERSION_MAJOR}.${OCC_VERSION_MINOR}")
  elseif(BUILD_SOVERSION_NUMBERS GREATER 0)
    set(OCC_SOVERSION "${OCC_VERSION_MAJOR}")
  endif()

  set_target_properties(${LIB_NAME} PROPERTIES
    SOVERSION "${OCC_SOVERSION}"
    VERSION "${OCC_VERSION_MAJOR}.${OCC_VERSION_MINOR}.${OCC_VERSION_MAINTENANCE}")

  if(APPLE AND NOT "${INSTALL_NAME_DIR}" STREQUAL "")
    set_target_properties(${LIB_NAME} PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "${INSTALL_NAME_DIR}")
  endif()

  # Handle shared library name postfix
  if(BUILD_SHARED_LIBS AND NOT "${BUILD_SHARED_LIBRARY_NAME_POSTFIX}" STREQUAL "")
    set(CMAKE_SHARED_LIBRARY_SUFFIX_DEFAULT ${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(CMAKE_SHARED_LIBRARY_SUFFIX "${BUILD_SHARED_LIBRARY_NAME_POSTFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  endif()

  # Install the library
  if(LAYOUT_IS_VCPKG)
    install(TARGETS ${LIB_NAME}
      EXPORT OpenCASCADE${MODULE}Targets
      RUNTIME DESTINATION "$<IF:$<CONFIG:Debug>,debug/${INSTALL_DIR_BIN},${INSTALL_DIR_BIN}>"
      ARCHIVE DESTINATION "$<IF:$<CONFIG:Debug>,debug/${INSTALL_DIR_LIB},${INSTALL_DIR_LIB}>"
      LIBRARY DESTINATION "$<IF:$<CONFIG:Debug>,debug/${INSTALL_DIR_LIB},${INSTALL_DIR_LIB}>"
      INCLUDES DESTINATION ${INSTALL_DIR_INCLUDE})
  else()
    install(TARGETS ${LIB_NAME}
      EXPORT OpenCASCADE${MODULE}Targets
      RUNTIME DESTINATION "${INSTALL_DIR_BIN}\${OCCT_INSTALL_BIN_LETTER}"
      ARCHIVE DESTINATION "${INSTALL_DIR_LIB}\${OCCT_INSTALL_BIN_LETTER}"
      LIBRARY DESTINATION "${INSTALL_DIR_LIB}\${OCCT_INSTALL_BIN_LETTER}"
      INCLUDES DESTINATION ${INSTALL_DIR_INCLUDE})
  endif()

  # Create symlink for shared library name postfix on non-Windows
  if(NOT WIN32 AND BUILD_SHARED_LIBS AND NOT "${BUILD_SHARED_LIBRARY_NAME_POSTFIX}" STREQUAL "")
    set(LINK_NAME "${INSTALL_DIR}/${INSTALL_DIR_LIB}\${OCCT_INSTALL_BIN_LETTER}/lib${LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX_DEFAULT}")
    set(LIBRARY_NAME "${INSTALL_DIR}/${INSTALL_DIR_LIB}\${OCCT_INSTALL_BIN_LETTER}/lib${LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    OCCT_CREATE_SYMLINK_TO_FILE(${LIBRARY_NAME} ${LINK_NAME})
  endif()

  # Store list of merged libraries for export
  get_property(MERGED_LIBS GLOBAL PROPERTY OCCT_MERGED_LIBRARIES)
  list(APPEND MERGED_LIBS ${LIB_NAME})
  set_property(GLOBAL PROPERTY OCCT_MERGED_LIBRARIES ${MERGED_LIBS})
endfunction()

#==================================================================================================

# Gets the list of all merged library names
# Arguments:
#   RESULT_VAR - Output variable for the list
function(OCCT_GET_MERGED_LIBRARIES RESULT_VAR)
  get_property(MERGED_LIBS GLOBAL PROPERTY OCCT_MERGED_LIBRARIES)
  set(${RESULT_VAR} ${MERGED_LIBS} PARENT_SCOPE)
endfunction()
