project (Overview)

# directory that contains all raw OCCT overview articles (markdown format)
set (OCCT_OVERVIEW_DIR "${CMAKE_SOURCE_DIR}/dox")

# directory that contains resources for the generation of OCCT documentation
set (OCCT_OVERVIEW_RESOURCE_DIR "${OCCT_OVERVIEW_DIR}/resources")

# directory that contains all generated OCCT articles
set (OCCT_GENERATED_DOC_DIR "${CMAKE_BINARY_DIR}/doc")
if (NOT EXISTS "${OCCT_GENERATED_DOC_DIR}")
  file (MAKE_DIRECTORY "${OCCT_GENERATED_DOC_DIR}")
endif()

# directory that contains generated OCCT overview articles
set (OCCT_GENERATED_OVERVIEW_DIR "${OCCT_GENERATED_DOC_DIR}/overview")
if (NOT EXISTS "${OCCT_GENERATED_OVERVIEW_DIR}")
  file (MAKE_DIRECTORY "${OCCT_GENERATED_OVERVIEW_DIR}")
endif()

# directory that contains generated OCCT overview articles in html format
set (OCCT_GENERATED_HTML_OVERVIEW_DIR "${OCCT_GENERATED_OVERVIEW_DIR}/html")
if (NOT EXISTS "${OCCT_GENERATED_HTML_OVERVIEW_DIR}")
  file (MAKE_DIRECTORY "${OCCT_GENERATED_HTML_OVERVIEW_DIR}")
endif()

# read OCCT articles that should be generated
set (OCCT_ARTICLES "")
if (EXISTS "${OCCT_OVERVIEW_DIR}/FILES_HTML.txt" )
  file (STRINGS "${OCCT_OVERVIEW_DIR}/FILES_HTML.txt" OCCT_ARTICLES REGEX "^[^#]+")
else()
  message (FATAL_ERROR "FILES_HTML.txt is not found in ${OCCT_OVERVIEW_DIR}")
endif()

# configuration file for doxygen
set (OCCT_CONFIG_FOR_DOXYGEN "${OCCT_GENERATED_DOC_DIR}/occt.cfg")

set (3RDPARTY_MATHJAX_RELATIVE_PATH "https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.1")

configure_file ("${OCCT_OVERVIEW_RESOURCE_DIR}/occt_ug_html.doxyfile" "${OCCT_CONFIG_FOR_DOXYGEN}" COPYONLY)

# Add common options for generation of Overview and User Guides
file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\nPROJECT_NUMBER         = ${OCC_VERSION_STRING_EXT}")
file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\nOUTPUT_DIRECTORY       = ${OCCT_GENERATED_OVERVIEW_DIR}/.")
file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\nPROJECT_LOGO           = ${OCCT_OVERVIEW_DIR}/resources/occ_logo.png")

set (OCCT_ARTICLE_PARAM_INPUT     "INPUT                  =")
set (OCCT_ARTICLE_PARAM_IMAGEPATH "IMAGE_PATH             = ${OCCT_OVERVIEW_DIR}/resources/ ")

set (OCCT_ARTICLES_ABSPATH "")
foreach (OCCT_ARTICLE ${OCCT_ARTICLES})
  get_filename_component (OCCT_ARTICLE_ABSOLUTE_PATH "${OCCT_OVERVIEW_DIR}/${OCCT_ARTICLE}" ABSOLUTE)
  get_filename_component (OCCT_ARTICLE_PARENT_DIR "${OCCT_ARTICLE_ABSOLUTE_PATH}" PATH)

  if (EXISTS "${OCCT_ARTICLE_PARENT_DIR}/images")
    set (OCCT_ARTICLE_PARAM_IMAGEPATH "${OCCT_ARTICLE_PARAM_IMAGEPATH} ${OCCT_ARTICLE_PARENT_DIR}/images")
  endif()

  set (OCCT_ARTICLE_PARAM_INPUT "${OCCT_ARTICLE_PARAM_INPUT} ${OCCT_ARTICLE_ABSOLUTE_PATH}")

  set (SOURCE_GROUP_NAME "")

  # add the file to appropriate source group
  get_filename_component (CURRENT_TAIL_PATH "${OCCT_ARTICLE}" PATH)
  while (CURRENT_TAIL_PATH AND NOT "${CURRENT_TAIL_PATH}" STREQUAL "..")
    get_filename_component (CURRENT_DIR_NAME "${CURRENT_TAIL_PATH}" NAME)
    set (SOURCE_GROUP_NAME "${CURRENT_DIR_NAME}\\${SOURCE_GROUP_NAME}")

    # one step out (get parent directory)
    get_filename_component (CURRENT_TAIL_PATH "${CURRENT_TAIL_PATH}" PATH)
  endwhile()
  SOURCE_GROUP ("${SOURCE_GROUP_NAME}" FILES ${OCCT_ARTICLE_ABSOLUTE_PATH})

  list (APPEND OCCT_ARTICLES_ABSPATH ${OCCT_ARTICLE_ABSOLUTE_PATH})
endforeach()

file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\n${OCCT_ARTICLE_PARAM_INPUT}")
file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\n${OCCT_ARTICLE_PARAM_IMAGEPATH}")

# Add document type-specific options
# generate tree view
file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\nGENERATE_TREEVIEW      = YES")

# HTML Search engine options
file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\nSEARCHENGINE           = YES")
file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\nSERVER_BASED_SEARCH    = YES")
file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\nEXTERNAL_SEARCH        = NO")

# Formula options
file (APPEND ${OCCT_CONFIG_FOR_DOXYGEN} "\nMATHJAX_RELPATH        = ${3RDPARTY_MATHJAX_RELATIVE_PATH}")

# Copy index file to provide fast access to HTML documentation
file(COPY "${OCCT_OVERVIEW_RESOURCE_DIR}/index.html" DESTINATION "${OCCT_GENERATED_OVERVIEW_DIR}")

# get relative paths to documentation source files
file (GLOB_RECURSE OCCT_OVERVIEW_FILES "${OCCT_OVERVIEW_DIR}/*")

add_custom_command(OUTPUT "${OCCT_GENERATED_DOC_DIR}/overview/html/occt_logo.png"
  COMMAND ${3RDPARTY_DOXYGEN_EXECUTABLE} ${OCCT_CONFIG_FOR_DOXYGEN}
  DEPENDS ${OCCT_OVERVIEW_FILES}
  WORKING_DIRECTORY ${OCCT_GENERATED_DOC_DIR}
  COMMENT "Generating OCCT overview documentation with Doxygen"
  VERBATIM
)

add_custom_target(${PROJECT_NAME} ALL
  DEPENDS "${OCCT_GENERATED_DOC_DIR}/overview/html/occt_logo.png" ${OCCT_OVERVIEW_FILES}
  SOURCES ${OCCT_ARTICLES_ABSPATH}
  VERBATIM
)

# Create Documentation folder in solution view
set_property (TARGET ${PROJECT_NAME} PROPERTY FOLDER "Documentation")
