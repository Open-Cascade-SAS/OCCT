# OCCT resource files generation

macro (OCCT_GENERATE_CONTENT_ONLY CurrentResource)
  set (RESOURCE_FILES)
  set (isResDirectory FALSE)
  if (IS_DIRECTORY "${OCCT_ROOT_DIR}/resources/${CurrentResource}")
    file (STRINGS "${OCCT_ROOT_DIR}/resources/${CurrentResource}/FILES" RESOURCE_FILES)
    set (CurrentResource_Directory "${CurrentResource}")
    set (isResDirectory TRUE)
  else()
    get_filename_component (CurrentResource_Name "${CurrentResource}" NAME)
    list (APPEND RESOURCE_FILES "res:::${CurrentResource_Name}")
    get_filename_component (CurrentResource_Directory "${CurrentResource}" DIRECTORY)
  endif()

  # Add current toolkit into RESOURCE_TOOLKITS array to copy it to the BUILD directory
  list (APPEND RESOURCE_TOOLKITS "${CurrentResource_Directory}")
  list (REMOVE_DUPLICATES RESOURCE_TOOLKITS)

  if (BUILD_RESOURCES)
    foreach (RESOURCE_FILE ${RESOURCE_FILES})
      string (REGEX MATCH "^[^:]+:::" IS_RESOURCE "${RESOURCE_FILE}")
      if (IS_RESOURCE)
        string (REGEX REPLACE "[^:]+:+" "" RESOURCE_FILE "${RESOURCE_FILE}")

        get_filename_component (CurrentResource_FileName "${RESOURCE_FILE}" NAME)
        string (REPLACE "." "_" CurrentResource_FileName "${CurrentResource_FileName}")
        set (HEADER_FILE_NAME "${CurrentResource_Directory}_${CurrentResource_FileName}.pxx")

        message(STATUS "Info. Generating header file from resource file: ${OCCT_ROOT_DIR}/resources/${CurrentResource_Directory}/${RESOURCE_FILE}")
        # generate content for header file
        set (OCCT_HEADER_FILE_CONTENT "// This file has been automatically generated from resource file resources/${CurrentResource_Directory}/${RESOURCE_FILE}\n\n")
        # read resource file
        file (STRINGS "${OCCT_ROOT_DIR}/resources/${CurrentResource_Directory}/${RESOURCE_FILE}" RESOURCE_FILE_LINES_LIST)
        set (OCCT_HEADER_FILE_CONTENT "${OCCT_HEADER_FILE_CONTENT}static const char ${CurrentResource_Directory}_${CurrentResource_FileName}[] =")
        foreach (line IN LISTS RESOURCE_FILE_LINES_LIST)
          string (REPLACE "\"" "\\\"" line "${line}")
          set (OCCT_HEADER_FILE_CONTENT "${OCCT_HEADER_FILE_CONTENT}\n  \"${line}\\n\"")
        endforeach()
        set (OCCT_HEADER_FILE_CONTENT "${OCCT_HEADER_FILE_CONTENT};\n\n")
        # Save generated content to header file
        set (HEADER_FILE "${OCCT_${CurrentResource_Directory}_FILES_LOCATION}/${HEADER_FILE_NAME}")
        if (EXISTS "${HEADER_FILE}")
          file (REMOVE "${HEADER_FILE}")
        endif()
        configure_file ("${OCCT_ROOT_DIR}/adm/templates/header.in" "${HEADER_FILE}" @ONLY NEWLINE_STYLE LF)
      endif()
    endforeach()
  endif()
endmacro()


FILE_TO_LIST ("adm/RESOURCES" RESOURCES)
foreach (CurrentResource ${RESOURCES})
  get_filename_component (CurrentResource_FileName "${CurrentResource}" NAME)
  if ("${CurrentResource_FileName}" STREQUAL TObj.msg OR
      "${CurrentResource_FileName}" STREQUAL BOPAlgo.msg OR
      "${CurrentResource_FileName}" STREQUAL Units.dat OR
      "${CurrentResource}" STREQUAL XSMessage OR
      "${CurrentResource}" STREQUAL SHMessage OR
      "${CurrentResource}" STREQUAL Shaders)
    OCCT_GENERATE_CONTENT_ONLY ("${CurrentResource}")
  endif()
endforeach()
