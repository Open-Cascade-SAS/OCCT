# Get all used packages from toolkit
foreach (MODULE ${TOOLKIT_MODULES})
  if (WIN32)
    list (APPEND PRECOMPILED_DEFS "-D__${MODULE}_DLL")
  endif()
  list (APPEND COMPILER_DIRECTORIES "${OCCT_SOURCE_DIR}/src/${MODULE}")
endforeach()
string (REGEX REPLACE ";" " " PRECOMPILED_DEFS "${PRECOMPILED_DEFS}")

# Get from toolkits EXTERNLIB all used libs
OCCT_TOOLKIT_DEP (${PROJECT_NAME} ${PROJECT_NAME}_DEPS)
foreach (DEP ${${PROJECT_NAME}_DEPS})
  IS_OCCT_TOOLKIT (${DEP} OCCT_MODULES FOUND_TOOLKIT)
  if ("${FOUND_TOOLKIT}" STREQUAL "ON")
    list ( APPEND USED_LIBS "${DEP}" )
  endif()
endforeach()

# Get all source files from used packages
foreach (CDIR ${COMPILER_DIRECTORIES})
  get_filename_component (CDirName ${CDIR} NAME)
  set (CDIR_files)
  file (STRINGS "${CDIR}/FILES" FLEX_Files REGEX ".+[.]lex")
  file (STRINGS "${CDIR}/FILES" BISON_Files REGEX ".+[.]yacc")
  file (STRINGS "${CDIR}/FILES" CFiles REGEX ".+[.]c")
  file (STRINGS "${CDIR}/FILES" MFiles REGEX ".+[.]mm")

  # Generate Flex and Bison files
  if (${BUILD_BISON_FLEX_FILES})
    list (LENGTH FLEX_Files FLength)
    list (LENGTH BISON_Files BLength)
    list (SORT FLEX_Files)
    list (SORT BISON_Files)
    if (${FLength} EQUAL ${BLength} AND NOT ${FLength} EQUAL 0)
      math (EXPR FLength "${FLength} - 1")
      foreach (FileIndex RANGE ${FLength})
        list (GET FLEX_Files ${FileIndex} FLEX_File)
        list (GET BISON_Files ${FileIndex} BISON_File)
        get_filename_component (FLEX_Name ${FLEX_File} NAME_WE)
        get_filename_component (BISON_Name ${BISON_File} NAME_WE)
        string (COMPARE EQUAL ${FLEX_Name} ${BISON_Name} isEQUAL)
        if (EXISTS "${FLEX_File}" AND EXISTS "${BISON_File}" AND ${isEQUAL})
          BISON_TARGET (Parser_${BISON_Name} ${BISON_File} ${CDIR}/${BISON_Name}.tab.c)
          FLEX_TARGET (Scanner_${FLEX_Name} ${FLEX_File} ${CDIR}/lex.${FLEX_Name}.c)
          ADD_FLEX_BISON_DEPENDENCY (Scanner_${FLEX_Name} Parser_${BISON_Name})
          list (APPEND CDIR_files ${BISON_Parser_${BISON_Name}_OUTPUTS} ${FLEX_Scanner_${FLEX_Name}_OUTPUTS})
        endif()
      endforeach()
    endif()
  endif()

  list (APPEND CDIR_files ${CFiles})
  if (APPLE)
    list ( APPEND CDIR_files ${MFiles})
  endif()

  foreach (CFile ${CDIR_files})
    list (APPEND USED_SRCFILES ${CDIR}/${CFile} )
    SOURCE_GROUP (${CDirName} FILES ${CDIR}/${CFile})
  endforeach()
endforeach()

list (APPEND COMPILER_DIRECTORIES "${OCCT_SOURCE_DIR}/src")
list (FIND USED_TOOLKITS ${PROJECT_NAME} isFOUND)

OCCT_MAKE_BUILD_POSTFIX()

# Create project for toolkit
if ("${USED_TOOLKITS}" STREQUAL "" OR NOT ${isFOUND} EQUAL -1)
  foreach (MODULE ${OCCT_MODULES})
    list (FIND ${MODULE}_TOOLKITS ${PROJECT_NAME} isFOUND)
    if (NOT ${isFOUND} EQUAL -1)
      set (CURRENT_MODULE ${MODULE})
    endif()
  endforeach()
  if (isEXE)
    add_executable (${PROJECT_NAME} ${USED_SRCFILES})
    install (TARGETS ${PROJECT_NAME} DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin${BUILD_POSTFIX}")
  else()
    add_library (${PROJECT_NAME} ${USED_SRCFILES})
    install (TARGETS ${PROJECT_NAME}
             RUNTIME DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin${BUILD_POSTFIX}"
             ARCHIVE DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib${BUILD_POSTFIX}"
             LIBRARY DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib${BUILD_POSTFIX}")
    if (MSVC)
      install (FILES  ${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin${BUILD_POSTFIX}/${PROJECT_NAME}.pdb
               CONFIGURATIONS Debug
               DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin${BUILD_POSTFIX}")
    endif()
  endif()

  set_property (TARGET ${PROJECT_NAME} PROPERTY FOLDER "Modules/${CURRENT_MODULE}")
  set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${PRECOMPILED_DEFS}")
  include_directories (${COMPILER_DIRECTORIES})
  target_link_libraries (${PROJECT_NAME} ${USED_LIBS})
endif()