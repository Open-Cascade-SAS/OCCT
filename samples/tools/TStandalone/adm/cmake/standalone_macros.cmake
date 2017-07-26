#freeimage

macro (FIND_SOURCES_AND_HEADERS_FILES CURRENT_SOURCES_DIR SOURCE_FILES HEADER_FILES)
  if (EXISTS "${CURRENT_SOURCES_DIR}/FILES")
    file (STRINGS "${CURRENT_SOURCES_DIR}/FILES" HEADER_FILES_H REGEX ".+[.]h")
    file (STRINGS "${CURRENT_SOURCES_DIR}/FILES" SOURCE_FILES_C REGEX ".+[.]c")

    if (NOT "${HEADER_FILES_H}" STREQUAL "")
      foreach (ORIGIN_FILE ${HEADER_FILES_H})
        list (APPEND HEADER_FILES_ ${CURRENT_SOURCES_DIR}/${ORIGIN_FILE})
      endforeach()

      #message("${HEADER_FILES_}")
      set(${HEADER_FILES} ${HEADER_FILES_})
    endif()

    if(NOT "${SOURCE_FILES_C}" STREQUAL "")
      foreach (ORIGIN_FILE ${SOURCE_FILES_C})
        list (APPEND SOURCE_FILES_ ${CURRENT_SOURCES_DIR}/${ORIGIN_FILE})
      endforeach()

      #message("${SOURCE_FILES_}")
      set(${SOURCE_FILES} ${SOURCE_FILES_})
    endif()

    #set(${HEADER_FILES} ${HEADER_FILES_H})
    #set(${SOURCE_FILES} ${SOURCE_FILES_C})

  else()
    message(WARNING "file FILES is absent in $TARGET")
  endif()

endmacro()
