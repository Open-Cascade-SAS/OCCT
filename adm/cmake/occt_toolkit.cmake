# script for each OCCT toolkit

# parce PACKAGES file
if ("${PROJECT_NAME}" STREQUAL DRAWEXE)
  set (USED_PACKAGES DRAWEXE)
else()
  FILE_TO_LIST ("src/${PROJECT_NAME}/PACKAGES" USED_PACKAGES)
endif()

set (PRECOMPILED_DEFS)

# Get all used packages from toolkit
foreach (OCCT_PACKAGE ${USED_PACKAGES})
  
  # TKService contains platform-dependent packages: Xw and WNT
  if ((WIN32 AND "${OCCT_PACKAGE}" STREQUAL "Xw") OR (NOT WIN32 AND "${OCCT_PACKAGE}" STREQUAL "WNT"))
    # do nothing
  else()

    if (WIN32)
      list (APPEND PRECOMPILED_DEFS "-D__${OCCT_PACKAGE}_DLL")
    endif()

    set (SOURCE_FILES)
    set (HEADER_FILES)

    # Generate Flex and Bison files
    if (${BUILD_YACCLEX})

      # flex files
      OCCT_ORIGIN_AND_PATCHED_FILES ("src/${OCCT_PACKAGE}" "*[.]lex" SOURCE_FILES_FLEX)
      list (LENGTH SOURCE_FILES_FLEX SOURCE_FILES_FLEX_LEN)

      # bison files
      OCCT_ORIGIN_AND_PATCHED_FILES ("src/${OCCT_PACKAGE}" "*[.]yacc" SOURCE_FILES_BISON)
      list (LENGTH SOURCE_FILES_BISON SOURCE_FILES_BISON_LEN)

      if (${SOURCE_FILES_FLEX_LEN} EQUAL ${SOURCE_FILES_BISON_LEN} AND NOT ${SOURCE_FILES_FLEX_LEN} EQUAL 0)
      
        list (SORT SOURCE_FILES_FLEX)
        list (SORT SOURCE_FILES_BISON)

        math (EXPR SOURCE_FILES_FLEX_LEN "${SOURCE_FILES_FLEX_LEN} - 1")
        foreach (FLEX_FILE_INDEX RANGE ${SOURCE_FILES_FLEX_LEN})

          list (GET SOURCE_FILES_FLEX ${FLEX_FILE_INDEX} CURRENT_FLEX_FILE)
          get_filename_component (CURRENT_FLEX_FILE_NAME ${CURRENT_FLEX_FILE} NAME_WE)

          list (GET SOURCE_FILES_BISON ${FLEX_FILE_INDEX} CURRENT_BISON_FILE)
          get_filename_component (CURRENT_BISON_FILE_NAME ${CURRENT_BISON_FILE} NAME_WE)
          
          string (COMPARE EQUAL ${CURRENT_FLEX_FILE_NAME} ${CURRENT_BISON_FILE_NAME} ARE_FILES_EQUAL)

          if (EXISTS "${CURRENT_FLEX_FILE}" AND EXISTS "${CURRENT_BISON_FILE}" AND ${ARE_FILES_EQUAL})
            set (BISON_OUTPUT_FILE ${CURRENT_BISON_FILE_NAME}.tab.c)
            set (FLEX_OUTPUT_FILE lex.${CURRENT_FLEX_FILE_NAME}.c)
            BISON_TARGET (Parser_${CURRENT_BISON_FILE_NAME} ${CURRENT_BISON_FILE} ${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/${BISON_OUTPUT_FILE} COMPILE_FLAGS "-p ${CURRENT_BISON_FILE_NAME}")
            FLEX_TARGET  (Scanner_${CURRENT_FLEX_FILE_NAME} ${CURRENT_FLEX_FILE} ${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/${FLEX_OUTPUT_FILE} COMPILE_FLAGS "-P${CURRENT_FLEX_FILE_NAME}")
            ADD_FLEX_BISON_DEPENDENCY (Scanner_${CURRENT_FLEX_FILE_NAME} Parser_${CURRENT_BISON_FILE_NAME})
           
            list (APPEND SOURCE_FILES ${BISON_OUTPUT_FILE} ${FLEX_OUTPUT_FILE})
          endif()
        endforeach()
      endif()
    endif()

    # header files
    if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/FILES")
      file (STRINGS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/FILES" HEADER_FILES_M   REGEX ".+[.]h")
      file (STRINGS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/FILES" HEADER_FILES_LXX REGEX ".+[.]lxx")
      file (STRINGS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/FILES" HEADER_FILES_GXX REGEX ".+[.]gxx")

      file (STRINGS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/FILES" SOURCE_FILES_C REGEX ".+[.]c")
      if(APPLE)
        file (STRINGS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/FILES" SOURCE_FILES_M REGEX ".+[.]mm")
      endif()
    else()
      file (STRINGS "${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/FILES"     HEADER_FILES_M   REGEX ".+[.]h")
      file (STRINGS "${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/FILES"     HEADER_FILES_LXX REGEX ".+[.]lxx")
      file (STRINGS "${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/FILES"     HEADER_FILES_GXX REGEX ".+[.]gxx")

      file (STRINGS "${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/FILES"     SOURCE_FILES_C REGEX ".+[.]c")
      if(APPLE)
        file (STRINGS "${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/FILES"   SOURCE_FILES_M REGEX ".+[.]mm")
      endif()
    endif()
    
    list (APPEND HEADER_FILES ${HEADER_FILES_M} ${HEADER_FILES_LXX} ${SOURCE_FILES_GXX})
    list (APPEND SOURCE_FILES ${SOURCE_FILES_C})
    if(APPLE)
      list (APPEND SOURCE_FILES ${SOURCE_FILES_M})
    endif()

    foreach(HEADER_FILE ${HEADER_FILES})
      if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/${HEADER_FILE}")
        message (STATUS "Info: consider patched file: ${BUILD_PATCH}/src/${OCCT_PACKAGE}/${HEADER_FILE}")
        list (APPEND USED_INCFILES "${BUILD_PATCH}/src/${OCCT_PACKAGE}/${HEADER_FILE}")
        SOURCE_GROUP ("Header Files\\${OCCT_PACKAGE}" FILES "${BUILD_PATCH}/src/${OCCT_PACKAGE}/${HEADER_FILE}")
      else()
        list (APPEND USED_INCFILES "${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/${HEADER_FILE}")
        SOURCE_GROUP ("Header Files\\${OCCT_PACKAGE}" FILES "${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/${HEADER_FILE}")
      endif()
    endforeach()

    foreach(SOURCE_FILE ${SOURCE_FILES})
      if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/src/${OCCT_PACKAGE}/${SOURCE_FILE}")
        message (STATUS "Info: consider patched file: ${BUILD_PATCH}/src/${OCCT_PACKAGE}/${SOURCE_FILE}")
        list (APPEND USED_SRCFILES "${BUILD_PATCH}/src/${OCCT_PACKAGE}/${SOURCE_FILE}")
        SOURCE_GROUP ("Source Files\\${OCCT_PACKAGE}" FILES "${BUILD_PATCH}/src/${OCCT_PACKAGE}/${SOURCE_FILE}")
      else()
        list (APPEND USED_SRCFILES "${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/${SOURCE_FILE}")
        SOURCE_GROUP ("Source Files\\${OCCT_PACKAGE}" FILES "${CMAKE_SOURCE_DIR}/src/${OCCT_PACKAGE}/${SOURCE_FILE}")
      endif()
    endforeach()
  endif()
endforeach()
string (REGEX REPLACE ";" " " PRECOMPILED_DEFS "${PRECOMPILED_DEFS}")

set (USED_RCFILE "")
if (MSVC)
  set (USED_RCFILE "${CMAKE_BINARY_DIR}/resources/${PROJECT_NAME}.rc")

  if (APPLY_OCCT_PATCH_DIR AND EXISTS "${APPLY_OCCT_PATCH_DIR}/adm/templates/occt_toolkit.rc.in")
    configure_file("${APPLY_OCCT_PATCH_DIR}/adm/templates/occt_toolkit.rc.in" "${USED_RCFILE}" @ONLY)
  else()
    configure_file("${CMAKE_SOURCE_DIR}/adm/templates/occt_toolkit.rc.in" "${USED_RCFILE}" @ONLY)
  endif()
endif()

set (CURRENT_MODULE)
foreach (OCCT_MODULE ${OCCT_MODULES})
  list (FIND ${OCCT_MODULE}_TOOLKITS ${PROJECT_NAME} CURRENT_PROJECT_IS_BUILT)
  if (NOT ${CURRENT_PROJECT_IS_BUILT} EQUAL -1)
    set (CURRENT_MODULE ${OCCT_MODULE})
  endif()
endforeach()

if (NOT SINGLE_GENERATOR)
  OCCT_INSERT_CODE_FOR_TARGET ()
endif()

if ("${PROJECT_NAME}" STREQUAL "DRAWEXE")
  add_executable (${PROJECT_NAME} ${USED_SRCFILES} ${USED_INCFILES} ${USED_RCFILE})

  install (TARGETS ${PROJECT_NAME}
           DESTINATION "${INSTALL_DIR}/${INSTALL_DIR_BIN}\${OCCT_INSTALL_BIN_LETTER}")
else()
  add_library (${PROJECT_NAME} ${USED_SRCFILES} ${USED_INCFILES} ${USED_RCFILE})

  if (MSVC)
    install (FILES  ${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bind/${PROJECT_NAME}.pdb
             CONFIGURATIONS Debug
             DESTINATION "${INSTALL_DIR}/${INSTALL_DIR_BIN}\${OCCT_INSTALL_BIN_LETTER}")
  endif()
  install (TARGETS ${PROJECT_NAME}
           EXPORT OpenCASCADE${CURRENT_MODULE}Targets
           RUNTIME DESTINATION "${INSTALL_DIR_BIN}\${OCCT_INSTALL_BIN_LETTER}"
           ARCHIVE DESTINATION "${INSTALL_DIR_LIB}\${OCCT_INSTALL_BIN_LETTER}"
           LIBRARY DESTINATION "${INSTALL_DIR_LIB}\${OCCT_INSTALL_BIN_LETTER}")
endif()

if (CURRENT_MODULE)
  set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "Modules/${CURRENT_MODULE}")
  set_target_properties (${PROJECT_NAME} PROPERTIES MODULE "${CURRENT_MODULE}")
endif()

get_property (OCC_VERSION_MAJOR GLOBAL PROPERTY OCC_VERSION_MAJOR)
get_property (OCC_VERSION_MINOR GLOBAL PROPERTY OCC_VERSION_MINOR)
get_property (OCC_VERSION_MAINTENANCE GLOBAL PROPERTY OCC_VERSION_MAINTENANCE)

if (ANDROID)
  # do not append version to the filename
  set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${PRECOMPILED_DEFS}")
else()
  set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${PRECOMPILED_DEFS}"
                                                    SOVERSION     "${OCC_VERSION_MAJOR}"
                                                    VERSION       "${OCC_VERSION_MAJOR}.${OCC_VERSION_MINOR}.${OCC_VERSION_MAINTENANCE}")
endif()

set (USED_TOOLKITS_BY_CURRENT_PROJECT)
set (USED_EXTERNAL_LIBS_BY_CURRENT_PROJECT)

# parce EXTERNLIB file
FILE_TO_LIST ("src/${PROJECT_NAME}/EXTERNLIB" USED_EXTERNLIB_AND_TOOLKITS)
foreach (USED_ITEM ${USED_EXTERNLIB_AND_TOOLKITS})
  string (REGEX MATCH "^ *#" COMMENT_FOUND ${USED_ITEM})
  if (NOT COMMENT_FOUND)
    string (REGEX MATCH "^TK" TK_FOUND ${USED_ITEM})
    string (REGEX MATCH "^vtk" VTK_FOUND ${USED_ITEM})
    
    if (NOT "${TK_FOUND}" STREQUAL "" OR NOT "${VTK_FOUND}" STREQUAL "")
      list (APPEND USED_TOOLKITS_BY_CURRENT_PROJECT ${USED_ITEM})
    else()
      string (REGEX MATCH "^CSF_" CSF_FOUND ${USED_ITEM})
      if ("${CSF_FOUND}" STREQUAL "")
        message (STATUS "Info: ${USED_ITEM} from ${PROJECT_NAME} skipped due to it is empty")
      else() # get CSF_ value
        set (CURRENT_CSF ${${USED_ITEM}})
        if (NOT "${CURRENT_CSF}" STREQUAL "")
          # prepare a list from a string with whitespaces
          separate_arguments (CURRENT_CSF)
          list (APPEND USED_EXTERNAL_LIBS_BY_CURRENT_PROJECT ${CURRENT_CSF})
        endif()
      endif()
    endif()
  endif()
endforeach()

if (APPLE)
  list (FIND USED_EXTERNAL_LIBS_BY_CURRENT_PROJECT X11 IS_X11_FOUND)
  if (NOT ${IS_X11_FOUND} EQUAL -1)
    find_package (X11 COMPONENTS X11 Xext Xmu Xi)
    if (NOT X11_FOUND)
      message (STATUS "Warning: X11 is not found. It's required to install The XQuartz project: http://www.xquartz.org")
    endif()
  endif()
endif()

target_link_libraries (${PROJECT_NAME} ${USED_TOOLKITS_BY_CURRENT_PROJECT} ${USED_EXTERNAL_LIBS_BY_CURRENT_PROJECT})
