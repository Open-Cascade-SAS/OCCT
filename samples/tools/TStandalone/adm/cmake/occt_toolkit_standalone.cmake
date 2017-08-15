# script for each OCCT toolkit

#Qt dependencies
#message ("Project is ${PROJECT_NAME}")
if (EXECUTABLE_PROJECT)
  set (PACKAGE_PREFIX_DIR "samples/tools")
  SET(SOURCE_DIR ${CMAKE_SOURCE_DIR}/../${PROJECT_NAME})
  include_directories("${SOURCE_DIR}")
  #message("Inc dir: ${CMAKE_SOURCE_DIR}/../${PROJECT_NAME}")
else()
  set (PACKAGE_PREFIX_DIR "tools")
  SET(SOURCE_DIR ${CASROOT_SOURCE_FILES}/${PACKAGE_PREFIX_DIR}/${PROJECT_NAME})
endif()

FIND_SOURCES_AND_HEADERS_FILES(${SOURCE_DIR} SOURCE_FILES HEADER_FILES)

#message ("Source dir is ${SOURCE_DIR}")
#message ("Sources are ${SOURCE_FILES}")

unset(RCC_FILES)
if (USE_QT)
  FIND_QT_PACKAGE(PROJECT_LIBRARIES_DEBUG PROJECT_LIBRARIES_RELEASE PROJECT_INCLUDES)

  FIND_AND_WRAP_MOC_FILES("${SOURCE_FILES}")
  FIND_AND_WRAP_RESOURCE_FILE("${SOURCE_DIR}/${PROJECT_NAME}.qrc" RCC_FILES)

endif()

if (TOOLKIT_NAME)
  set(TARGET_NAME TK${PROJECT_NAME})
else ()
  set(TARGET_NAME ${PROJECT_NAME})
endif()

if (EXECUTABLE_PROJECT)
  add_executable (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})
else()
  add_library(${TARGET_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
endif()


include_directories(
  ${PROJECT_INCLUDES}
  ${SOURCE_DIR}
  ${3RDPARTY_OCCT_INCLUDE_DIR}
 "${CMAKE_BINARY_DIR}/inc")

add_definitions(-D__WIN32__)
add_definitions(-DWNT)
add_definitions(-D${PROJECT_NAME}_EXPORTS)

# parce EXTERNLIB file
FILE_TO_LIST ("src/${PROJECT_NAME}/EXTERNLIB" USED_EXTERNLIB_AND_TOOLKITS)
foreach (USED_ITEM ${USED_EXTERNLIB_AND_TOOLKITS})
  string (REGEX MATCH "^ *#" COMMENT_FOUND ${USED_ITEM})
  if (NOT COMMENT_FOUND)
    set (LIB_ITEM ${USED_ITEM})
    set (LIB_PARSED_ITEM ${${USED_ITEM}})

    if (LIB_PARSED_ITEM)
      list (APPEND USED_TOOLKITS_BY_CURRENT_PROJECT ${${USED_ITEM}})
      #message("Append external lib: ${${USED_ITEM}}")
    else ()
      list (APPEND USED_TOOLKITS_BY_CURRENT_PROJECT ${USED_ITEM})
      #message("Append lib: ${USED_ITEM}")
    endif()

  endif()
endforeach()


target_link_libraries (${TARGET_NAME} ${USED_TOOLKITS_BY_CURRENT_PROJECT})

if (USE_QT)
  target_link_libraries (${TARGET_NAME} debug ${PROJECT_LIBRARIES_DEBUG})
  target_link_libraries (${TARGET_NAME} optimized ${PROJECT_LIBRARIES_RELEASE})

  if (NOT "${RCC_FILES}" STREQUAL "")
    #message("RCC files: ${RCC_FILES}")
    target_sources(${TARGET_NAME} PRIVATE ${RCC_FILES})
  endif()
endif()

if (EXECUTABLE_PROJECT)
  INSTALL(TARGETS ${TARGET_NAME} DESTINATION "${INSTALL_DIR_BIN}")
else()
  if (MSVC)
    install (DIRECTORY  ${PROJECT_BINARY_DIR}/Debug/
             DESTINATION "${INSTALL_DIR_BIN}"
             FILES_MATCHING PATTERN *.pdb)
    if (INSTALL_API_DIR)
      install (DIRECTORY  ${PROJECT_BINARY_DIR}/Debug/
               DESTINATION "${INSTALL_API_DIR}/bin"
               FILES_MATCHING PATTERN *.pdb)
    endif()
  endif()
  install (TARGETS ${TARGET_NAME}
           RUNTIME DESTINATION "${INSTALL_DIR_BIN}"
           ARCHIVE DESTINATION "${INSTALL_DIR_LIB}"
           LIBRARY DESTINATION "${INSTALL_DIR_LIB}")

  if (INSTALL_API_DIR)
    install (TARGETS ${TARGET_NAME}
             RUNTIME DESTINATION "${INSTALL_API_DIR}/bin")

    if (INSTALL_API)
      INSTALL(FILES ${HEADER_FILES} DESTINATION "${INSTALL_API_DIR}/${INSTALL_DIR_INCLUDE}")
    
      install (DIRECTORY  ${PROJECT_BINARY_DIR}/Debug/
               DESTINATION "${INSTALL_API_DIR}/lib"
               FILES_MATCHING PATTERN *.lib)
    endif()
  endif()

endif()
