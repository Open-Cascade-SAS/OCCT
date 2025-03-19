##

# Function to collect modules, toolkits, and packages information for documentation
function(OCCT_DOC_COLLECT_MODULES_INFO)
  # Set output variables in parent scope
  set(OCCT_MODULES ${OCCT_MODULES} PARENT_SCOPE)

  # For each module, collect its toolkits
  foreach(OCCT_MODULE ${OCCT_MODULES})
    if(NOT "${OCCT_MODULE}" STREQUAL "")
      set(MODULES_TOOLKITS ${${OCCT_MODULE}_TOOLKITS})
      set(TOOLKITS_IN_MODULE_${OCCT_MODULE} ${MODULES_TOOLKITS} PARENT_SCOPE)

      # For each toolkit, collect its packages
      foreach(TOOLKIT ${MODULES_TOOLKITS})
        set(TOOLKIT_PARENT_MODULE_${TOOLKIT} ${OCCT_MODULE} PARENT_SCOPE)

        # Get packages from toolkit
        set(TOOLKIT_PACKAGES "")
        EXTRACT_TOOLKIT_PACKAGES("src" ${TOOLKIT} TOOLKIT_PACKAGES)
        set(PACKAGES_IN_TOOLKIT_${TOOLKIT} ${TOOLKIT_PACKAGES} PARENT_SCOPE)
      endforeach()
    endif()
  endforeach()
endfunction()

# Function to generate module dependency graph
function(OCCT_DOC_CREATE_MODULE_DEPENDENCY_GRAPH OUTPUT_DIR FILENAME)
  set(DOT_FILE "${OUTPUT_DIR}/${FILENAME}.dot")

  # Create .dot file for module dependencies
  file(WRITE ${DOT_FILE} "digraph ${FILENAME}\n{\n")

  foreach(MODULE ${OCCT_MODULES})
    if(NOT "${MODULE}" STREQUAL "")
      file(APPEND ${DOT_FILE} "\t${MODULE} [ URL = \"module_${MODULE}.html\" ]\n")

      # Add dependencies between modules
      foreach(MODULE_TOOLKIT ${TOOLKITS_IN_MODULE_${MODULE}})
        foreach(DEPENDENT_TOOLKIT ${TOOLKIT_DEPENDENCY_${MODULE_TOOLKIT}})
          if(DEFINED TOOLKIT_PARENT_MODULE_${DEPENDENT_TOOLKIT} AND 
             NOT "${TOOLKIT_PARENT_MODULE_${DEPENDENT_TOOLKIT}}" STREQUAL "${MODULE}")
            file(APPEND ${DOT_FILE} "\t${TOOLKIT_PARENT_MODULE_${DEPENDENT_TOOLKIT}} -> ${MODULE} [ dir = \"back\", color = \"midnightblue\", style = \"solid\" ]\n")
          endif()
        endforeach()
      endforeach()
    endif()
  endforeach()

  file(APPEND ${DOT_FILE} "}\n")

  # Return the output file name
  set(DOT_OUTPUT_FILE "${FILENAME}" PARENT_SCOPE)
endfunction()

# Function to generate toolkit dependency graph for a module
function(OCCT_DOC_CREATE_TOOLKIT_DEPENDENCY_GRAPH OUTPUT_DIR FILENAME MODULE_NAME)
  set(DOT_FILE "${OUTPUT_DIR}/${FILENAME}.dot")

  # Create .dot file for toolkit dependencies within a module
  file(WRITE ${DOT_FILE} "digraph ${FILENAME}\n{\n")

  foreach(TOOLKIT ${TOOLKITS_IN_MODULE_${MODULE_NAME}})
    file(APPEND ${DOT_FILE} "\t${TOOLKIT} [ URL = \"toolkit_${TOOLKIT}.html\" ]\n")

    # Add dependencies between toolkits in the same module
    foreach(DEPENDENT_TOOLKIT ${TOOLKIT_DEPENDENCY_${TOOLKIT}})
      if(DEFINED TOOLKIT_PARENT_MODULE_${DEPENDENT_TOOLKIT} AND 
         "${TOOLKIT_PARENT_MODULE_${DEPENDENT_TOOLKIT}}" STREQUAL "${MODULE_NAME}")
        file(APPEND ${DOT_FILE} "\t${DEPENDENT_TOOLKIT} -> ${TOOLKIT} [ dir = \"back\", color = \"midnightblue\", style = \"solid\" ]\n")
      endif()
    endforeach()
  endforeach()

  file(APPEND ${DOT_FILE} "}\n")

  # Return the output file name
  set(DOT_OUTPUT_FILE "${FILENAME}" PARENT_SCOPE)
endfunction()

# Function to generate dependency graph for a specific toolkit
function(OCCT_DOC_CREATE_SINGLE_TOOLKIT_DEPENDENCY_GRAPH OUTPUT_DIR FILENAME TOOLKIT_NAME)
  set(DOT_FILE "${OUTPUT_DIR}/${FILENAME}.dot")

  # Create .dot file for dependencies of a single toolkit
  file(WRITE ${DOT_FILE} "digraph ${FILENAME}\n{\n")

  file(APPEND ${DOT_FILE} "\t${TOOLKIT_NAME} [ URL = \"toolkit_${TOOLKIT_NAME}.html\", shape = box ]\n")

  # Add toolkit dependencies
  foreach(DEPENDENT_TOOLKIT ${TOOLKIT_DEPENDENCY_${TOOLKIT_NAME}})
    file(APPEND ${DOT_FILE} "\t${DEPENDENT_TOOLKIT} [ URL = \"toolkit_${DEPENDENT_TOOLKIT}.html\", shape = box ]\n")
    file(APPEND ${DOT_FILE} "\t${TOOLKIT_NAME} -> ${DEPENDENT_TOOLKIT} [ color = \"midnightblue\", style = \"solid\" ]\n")    
  endforeach()

  if(TOOLKIT_DEPENDENCY_${TOOLKIT_NAME})
    list(LENGTH TOOLKIT_DEPENDENCY_${TOOLKIT_NAME} DEPS_COUNT)
    if(DEPS_COUNT GREATER 1)
      file(APPEND ${DOT_FILE} "\taspect = 1\n")
    endif()
  endif()

  file(APPEND ${DOT_FILE} "}\n")

  # Return the output file name
  set(DOT_OUTPUT_FILE "${FILENAME}" PARENT_SCOPE)
endfunction()

# Function to generate main page for documentation
function(OCCT_DOC_GENERATE_MAIN_PAGE OUTPUT_DIR OUTPUT_FILE)
  set(MAIN_PAGE_FILE "${OUTPUT_DIR}/${OUTPUT_FILE}")
  file(WRITE ${MAIN_PAGE_FILE} "/**\n")

  # Check if we're generating documentation for a single module
  if(DOC_SINGLE_MODULE)
    file(APPEND ${MAIN_PAGE_FILE} "\\mainpage OCCT Module ${DOC_SINGLE_MODULE}\n")
  else()
    file(APPEND ${MAIN_PAGE_FILE} "\\mainpage Open CASCADE Technology Reference Manual\n\n")
    # List all modules
    foreach(MODULE ${OCCT_MODULES})
      if(NOT "${MODULE}" STREQUAL "")
        file(APPEND ${MAIN_PAGE_FILE} "\\li \\subpage module_${MODULE}\n")
      endif()
    endforeach()
    # Add modules relationship diagram
    OCCT_DOC_CREATE_MODULE_DEPENDENCY_GRAPH("${OUTPUT_DIR}/html" "schema_all_modules")
    file(APPEND ${MAIN_PAGE_FILE} "\\dotfile schema_all_modules.dot\n")
  endif()

  file(APPEND ${MAIN_PAGE_FILE} "**/\n\n")

  # Generate pages for modules and their toolkits
  foreach(MODULE ${OCCT_MODULES})
    if(NOT "${MODULE}" STREQUAL "")
      file(APPEND ${MAIN_PAGE_FILE} "/**\n")

      if(DOC_SINGLE_MODULE)
        file(APPEND ${MAIN_PAGE_FILE} "\\mainpage OCCT Module ${MODULE}\n")
      else()
        file(APPEND ${MAIN_PAGE_FILE} "\\page module_${MODULE} Module ${MODULE}\n")
      endif()

      # List toolkits in the module
      foreach(TOOLKIT ${TOOLKITS_IN_MODULE_${MODULE}})
        file(APPEND ${MAIN_PAGE_FILE} "\\li \\subpage toolkit_${TOOLKIT}\n")
      endforeach()

      # Add module diagram
      OCCT_DOC_CREATE_TOOLKIT_DEPENDENCY_GRAPH("${OUTPUT_DIR}/html" "schema_${MODULE}" ${MODULE})
      file(APPEND ${MAIN_PAGE_FILE} "\\dotfile schema_${MODULE}.dot\n")
      file(APPEND ${MAIN_PAGE_FILE} "**/\n\n")
    endif()
  endforeach()

  # Generate pages for toolkits and their packages
  foreach(MODULE ${OCCT_MODULES})
    if(NOT "${MODULE}" STREQUAL "")
      foreach(TOOLKIT ${TOOLKITS_IN_MODULE_${MODULE}})
        file(APPEND ${MAIN_PAGE_FILE} "/**\n")
        file(APPEND ${MAIN_PAGE_FILE} "\\page toolkit_${TOOLKIT} Toolkit ${TOOLKIT}\n")

        # List packages in toolkit
        foreach(PACKAGE ${PACKAGES_IN_TOOLKIT_${TOOLKIT}})
          set(PACKAGE_NAME ${PACKAGE})
          file(APPEND ${MAIN_PAGE_FILE} "\\li \\subpage package_${PACKAGE_NAME}\n")
        endforeach()

        # Add toolkit dependencies diagram
        OCCT_DOC_CREATE_SINGLE_TOOLKIT_DEPENDENCY_GRAPH("${OUTPUT_DIR}/html" "schema_${TOOLKIT}" ${TOOLKIT})
        file(APPEND ${MAIN_PAGE_FILE} "\\dotfile schema_${TOOLKIT}.dot\n")
        file(APPEND ${MAIN_PAGE_FILE} "**/\n\n")
      endforeach()
    endif()
  endforeach()

  # Generate pages for packages and their classes
  foreach(MODULE ${OCCT_MODULES})
    if(NOT "${MODULE}" STREQUAL "")
      foreach(TOOLKIT ${TOOLKITS_IN_MODULE_${MODULE}})
        foreach(PACKAGE ${PACKAGES_IN_TOOLKIT_${TOOLKIT}})
          file(APPEND ${MAIN_PAGE_FILE} "/**\n")
          file(APPEND ${MAIN_PAGE_FILE} "\\page package_${PACKAGE} Package ${PACKAGE}\n")

          # Find header files in the package
          file(GLOB PACKAGE_HEADERS "${CMAKE_SOURCE_DIR}/src/${PACKAGE}/*.hxx")
          foreach(HEADER ${PACKAGE_HEADERS})
            get_filename_component(HEADER_NAME ${HEADER} NAME_WE)
            if(NOT HEADER_NAME MATCHES "^Handle_" AND NOT HEADER_NAME MATCHES "^Standard$")
              file(APPEND ${MAIN_PAGE_FILE} "\\li \\subpage ${HEADER_NAME}\n")
              # Append header file to DOXYGEN_INPUT_FILES list
              list(APPEND DOXYGEN_INPUT_FILES "${HEADER}")
            endif()
          endforeach()

          file(APPEND ${MAIN_PAGE_FILE} "**/\n\n")
        endforeach()
      endforeach()
    endif()
  endforeach()

  # Create a variable containing all input files for Doxygen
  string(REPLACE ";" " " DOXYGEN_INPUT_FILES_STRING "${DOXYGEN_INPUT_FILES}")
  set(DOXYGEN_INPUT_FILES_STRING ${DOXYGEN_INPUT_FILES_STRING} PARENT_SCOPE)
endfunction()

# Function to extract dependency information for toolkits
function(OCCT_DOC_EXTRACT_TOOLKIT_DEPENDENCIES)
  foreach(MODULE ${OCCT_MODULES})
    if(NOT "${MODULE}" STREQUAL "")
      foreach(TOOLKIT ${TOOLKITS_IN_MODULE_${MODULE}})
        EXTRACT_TOOLKIT_EXTERNLIB("src" ${TOOLKIT} EXTERNLIB_LIST)

        set(DEPENDENT_TOOLKITS "")
        foreach(EXTERNLIB ${EXTERNLIB_LIST})
          if(EXTERNLIB MATCHES "^TK")
            list(APPEND DEPENDENT_TOOLKITS ${EXTERNLIB})
          endif()
        endforeach()

        set(TOOLKIT_DEPENDENCY_${TOOLKIT} ${DEPENDENT_TOOLKITS} PARENT_SCOPE)
      endforeach()
    endif()
  endforeach()
endfunction()

# Function to load file lists for documentation
function(OCCT_DOC_LOAD_FILE_LISTS)
  # Load list of HTML documentation files
  set(FILES_HTML_PATH "${CMAKE_SOURCE_DIR}/dox/FILES_HTML.txt")
  if(EXISTS ${FILES_HTML_PATH})
    file(STRINGS ${FILES_HTML_PATH} HTML_FILES REGEX "^[^#]+")
    set(OCCT_DOC_HTML_FILES ${HTML_FILES} PARENT_SCOPE)
  else()
    set(OCCT_DOC_HTML_FILES "" PARENT_SCOPE)
  endif()

  # Load list of PDF documentation files
  set(FILES_PDF_PATH "${CMAKE_SOURCE_DIR}/dox/FILES_PDF.txt")
  if(EXISTS ${FILES_PDF_PATH})
    file(STRINGS ${FILES_PDF_PATH} PDF_FILES REGEX "^[^#]+")
    set(OCCT_DOC_PDF_FILES ${PDF_FILES} PARENT_SCOPE)
  else()
    set(OCCT_DOC_PDF_FILES "" PARENT_SCOPE)
  endif()
endfunction()

# Function to collect image directories from input files
function(OCCT_DOC_COLLECT_IMAGE_DIRS INPUT_DIRS RESULT_IMAGE_DIRS)
  set(IMAGE_DIRS "")

  foreach(INPUT_DIR ${INPUT_DIRS})
    # Check if directory exists
    if(EXISTS "${INPUT_DIR}")
      # Add the standard "images" subdirectory if it exists
      if(EXISTS "${INPUT_DIR}/images")
        list(APPEND IMAGE_DIRS "${INPUT_DIR}/images")
      endif()

      # Find all subdirectories containing images
      file(GLOB_RECURSE IMAGE_FILES 
           "${INPUT_DIR}/*.png" 
           "${INPUT_DIR}/*.jpg" 
           "${INPUT_DIR}/*.jpeg" 
           "${INPUT_DIR}/*.gif" 
           "${INPUT_DIR}/*.svg")

      foreach(IMAGE_FILE ${IMAGE_FILES})
        get_filename_component(IMAGE_DIR ${IMAGE_FILE} DIRECTORY)
        list(APPEND IMAGE_DIRS "${IMAGE_DIR}")
      endforeach()
    endif()
  endforeach()

  if(IMAGE_DIRS)
    list(REMOVE_DUPLICATES IMAGE_DIRS)
  endif()

  set(${RESULT_IMAGE_DIRS} ${IMAGE_DIRS} PARENT_SCOPE)
endfunction()

# Function to configure and run Doxygen for documentation generation
function(OCCT_DOC_CONFIGURE_DOXYGEN OUTPUT_DIR CONFIG_FILE DOC_TYPE)
  # Create output directory if it doesn't exist
  file(MAKE_DIRECTORY ${OUTPUT_DIR})

  # Use existing Doxygen template file as base
  if(DOC_TYPE STREQUAL "OVERVIEW")
    set(TEMPLATE_DOXYFILE "${CMAKE_SOURCE_DIR}/dox/resources/occt_ug_html.doxyfile")
  else()
    set(TEMPLATE_DOXYFILE "${CMAKE_SOURCE_DIR}/dox/resources/occt_rm.doxyfile")
  endif()

  # Define Doxygen parameters that need to be overridden from the template
  set(DOXYGEN_CONFIG_FILE "${OUTPUT_DIR}/${CONFIG_FILE}")

  # Check if template file exists
  if(NOT EXISTS ${TEMPLATE_DOXYFILE})
    message(FATAL_ERROR "ERROR: Doxygen template file not found: ${TEMPLATE_DOXYFILE}")
    return()
  endif()

  # Read template content
  file(READ ${TEMPLATE_DOXYFILE} DOXYGEN_TEMPLATE_CONTENT)

  # Create the output Doxyfile
  file(WRITE ${DOXYGEN_CONFIG_FILE} "# Doxyfile generated by OCCT_DOC_CONFIGURE_DOXYGEN\n")
  file(APPEND ${DOXYGEN_CONFIG_FILE} "# Base template: ${TEMPLATE_DOXYFILE}\n\n")
  file(APPEND ${DOXYGEN_CONFIG_FILE} "${DOXYGEN_TEMPLATE_CONTENT}\n\n")

  file(APPEND ${DOXYGEN_CONFIG_FILE} "# Custom overrides set by CMake:\n")

  # Project information
  file(APPEND ${DOXYGEN_CONFIG_FILE} "PROJECT_NUMBER = ${OCC_VERSION_STRING_EXT}\n")
  file(APPEND ${DOXYGEN_CONFIG_FILE} "OUTPUT_DIRECTORY = ${OUTPUT_DIR}\n")

  # Ensure client-side search is configured correctly
  file(APPEND ${DOXYGEN_CONFIG_FILE} "\n# Search engine settings\n")
  file(APPEND ${DOXYGEN_CONFIG_FILE} "SEARCHENGINE = YES\n")
  file(APPEND ${DOXYGEN_CONFIG_FILE} "SERVER_BASED_SEARCH = NO\n")
  file(APPEND ${DOXYGEN_CONFIG_FILE} "EXTERNAL_SEARCH = NO\n")
  file(APPEND ${DOXYGEN_CONFIG_FILE} "SEARCHDATA_FILE = searchdata.xml\n")

  # Additional parameters based on the document type
  if(DOC_TYPE STREQUAL "OVERVIEW")
    # Settings for Overview documentation
    file(APPEND ${DOXYGEN_CONFIG_FILE} "PROJECT_LOGO = ${CMAKE_SOURCE_DIR}/dox/resources/occ_logo.png\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "EXTRACT_ALL = NO\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "EXTRACT_PRIVATE = NO\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "EXTRACT_STATIC = NO\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "EXCLUDE_PATTERNS = */src/* */inc/* */drv/* */Properties/*\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "ENABLED_SECTIONS = OVERVIEW_SECTION\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "GENERATE_TAGFILE = ${OUTPUT_DIR}/occt.tag\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "GENERATE_TREEVIEW = YES\n")
    
    # Setup tag file for cross-referencing with Reference Manual
    if(BUILD_DOC_RefMan)
      file(APPEND ${DOXYGEN_CONFIG_FILE} "\n# Cross-referencing with Reference Manual\n")
      file(APPEND ${DOXYGEN_CONFIG_FILE} "TAGFILES = \"${CMAKE_BINARY_DIR}/doc/refman/occt_refman.tag=../../refman/html\"\n")
    endif()

    # Input files for overview
    if(DEFINED OCCT_OVERVIEW_FILES)
      file(APPEND ${DOXYGEN_CONFIG_FILE} "INPUT = ${OCCT_OVERVIEW_FILES}\n")
    else()
      file(APPEND ${DOXYGEN_CONFIG_FILE} "INPUT = ${CMAKE_SOURCE_DIR}/dox\n")
    endif()

    # Collect image directories for overview
    set(OVERVIEW_INPUT_DIRS ${CMAKE_SOURCE_DIR}/dox)
    OCCT_DOC_COLLECT_IMAGE_DIRS("${OVERVIEW_INPUT_DIRS}" OVERVIEW_IMAGE_DIRS)

    # Image path for overview
    if(OVERVIEW_IMAGE_DIRS)
      string(REPLACE ";" " " OVERVIEW_IMAGE_DIRS_STR "${OVERVIEW_IMAGE_DIRS}")
      file(APPEND ${DOXYGEN_CONFIG_FILE} "IMAGE_PATH = ${OVERVIEW_IMAGE_DIRS_STR} ${CMAKE_SOURCE_DIR}/dox/resources\n")
    else()
      file(APPEND ${DOXYGEN_CONFIG_FILE} "IMAGE_PATH = ${CMAKE_SOURCE_DIR}/dox/resources\n")
    endif()

    # Example paths
    file(APPEND ${DOXYGEN_CONFIG_FILE} "EXAMPLE_PATH = ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/samples\n")
  else()
    # Settings for Reference Manual
    file(APPEND ${DOXYGEN_CONFIG_FILE} "PROJECT_NAME = \"Open CASCADE Technology Reference Manual\"\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "PROJECT_LOGO = ${CMAKE_SOURCE_DIR}/dox/resources/occ_logo.png\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "BUILTIN_STL_SUPPORT = YES\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "EXTRACT_PRIVATE = NO\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "EXTRACT_PACKAGE = YES\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "EXCLUDE_PATTERNS = */Properties/*\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "FILE_PATTERNS = *.h *.hxx *.lxx *.gxx *.pxx *.cxx *.cpp *.c *.md\n")

    # Generate a tag file for cross-referencing from Overview
    file(APPEND ${DOXYGEN_CONFIG_FILE} "\n# Generate tag file for cross-referencing\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "GENERATE_TAGFILE = ${OUTPUT_DIR}/occt_refman.tag\n")

    # Input files for reference manual - CRITICAL FOR PROPER GENERATION
    file(APPEND ${DOXYGEN_CONFIG_FILE} "\n# Input files for reference manual\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "INPUT = ${CMAKE_SOURCE_DIR}/src\n")

    # If generating documentation for specific modules
    if(DEFINED OCCT_DOC_MODULES)
      set(MODULE_PATHS "")
      foreach(MODULE ${OCCT_DOC_MODULES})
        foreach(TOOLKIT ${${MODULE}_TOOLKITS})
          list(APPEND MODULE_PATHS "${CMAKE_SOURCE_DIR}/src/${TOOLKIT}")
        endforeach()
      endforeach()
      string(REPLACE ";" " " MODULE_PATHS_STR "${MODULE_PATHS}")
      file(APPEND ${DOXYGEN_CONFIG_FILE} "INPUT += ${MODULE_PATHS_STR}\n") # Use += to append to existing INPUT
    endif()

    # Configure image path for reference manual
    set(REFMAN_INPUT_DIRS ${CMAKE_SOURCE_DIR}/src)
    OCCT_DOC_COLLECT_IMAGE_DIRS("${REFMAN_INPUT_DIRS}" REFMAN_IMAGE_DIRS)

    if(REFMAN_IMAGE_DIRS)
      string(REPLACE ";" " " REFMAN_IMAGE_DIRS_STR "${REFMAN_IMAGE_DIRS}")
      file(APPEND ${DOXYGEN_CONFIG_FILE} "IMAGE_PATH = ${REFMAN_IMAGE_DIRS_STR} ${CMAKE_SOURCE_DIR}/dox/resources\n")
    else()
      file(APPEND ${DOXYGEN_CONFIG_FILE} "IMAGE_PATH = ${CMAKE_SOURCE_DIR}/dox/resources\n")
    endif()

    # Add main page file if generated
    if(EXISTS "${OUTPUT_DIR}/main_page.dox")
      file(APPEND ${DOXYGEN_CONFIG_FILE} "INPUT += ${OUTPUT_DIR}/main_page.dox\n") # Use += to append to existing INPUT
    endif()

    # Add header files to Doxygen input
    if(DEFINED DOXYGEN_INPUT_FILES_STRING)
      file(APPEND ${DOXYGEN_CONFIG_FILE} "INPUT += ${DOXYGEN_INPUT_FILES_STRING}\n") # Use += to append to existing INPUT
    endif()
  endif()

  # Custom CSS
  if(EXISTS "${CMAKE_SOURCE_DIR}/dox/resources/custom.css")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "HTML_EXTRA_STYLESHEET = ${CMAKE_SOURCE_DIR}/dox/resources/custom.css\n")
  endif()

  # Set paths for dot tool
  if(GRAPHVIZ_DOT_EXECUTABLE)
    file(APPEND ${DOXYGEN_CONFIG_FILE} "DOT_PATH = ${GRAPHVIZ_DOT_EXECUTABLE}\n")
    get_filename_component(DOT_DIRECTORY ${GRAPHVIZ_DOT_EXECUTABLE} DIRECTORY)
    file(APPEND ${DOXYGEN_CONFIG_FILE} "DOTFONTPATH = ${DOT_DIRECTORY}\n")
    file(APPEND ${DOXYGEN_CONFIG_FILE} "DOTFILE_DIRS = ${OUTPUT_DIR}/html\n")
  endif()

  # Confirm file creation
  if(EXISTS ${DOXYGEN_CONFIG_FILE})
    message(STATUS "Successfully created Doxygen configuration file at: ${DOXYGEN_CONFIG_FILE}")
  else()
    message(FATAL_ERROR "Failed to create Doxygen configuration file at: ${DOXYGEN_CONFIG_FILE}")
  endif()
endfunction()

# Function to check if required tools are available
function(OCCT_DOC_CHECK_TOOLS)
  # Find Doxygen
  find_package(Doxygen QUIET)
  if(NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen not found. Documentation will not be generated.")
    set(OCCT_DOC_TOOLS_AVAILABLE FALSE PARENT_SCOPE)
    return()
  endif()

  # Find Graphviz
  find_program(GRAPHVIZ_DOT_EXECUTABLE NAMES dot)

  # Check for MathJax for LaTeX formulas
  if(NOT MATHJAX_PATH)
    set(MATHJAX_PATH "https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5")
  endif()

  # Find tools for PDF generation if needed
  if(BUILD_DOC_PDF)
    # Find pdflatex
    find_program(PDFLATEX_EXECUTABLE NAMES pdflatex)
    if(NOT PDFLATEX_EXECUTABLE)
      message(WARNING "pdflatex not found. PDF documentation will not be generated.")
      set(BUILD_DOC_PDF FALSE PARENT_SCOPE)
    endif()

    # Find Inkscape (for SVG to PNG conversion for PDFs)
    find_program(INKSCAPE_EXECUTABLE NAMES inkscape)
    if(NOT INKSCAPE_EXECUTABLE)
      message(WARNING "Inkscape not found. SVG images will not be properly converted in PDF documentation.")
    endif()

    set(PDFLATEX_EXECUTABLE ${PDFLATEX_EXECUTABLE} PARENT_SCOPE)
    set(INKSCAPE_EXECUTABLE ${INKSCAPE_EXECUTABLE} PARENT_SCOPE)
  endif()

  # Find tools for CHM generation if needed
  if(BUILD_DOC_CHM AND WIN32)
    # Find HTML Help Compiler
    find_program(HHC_EXECUTABLE NAMES hhc HHC)
    if(NOT HHC_EXECUTABLE)
      message(WARNING "HTML Help Compiler not found. CHM documentation will not be generated.")
      set(BUILD_DOC_CHM FALSE PARENT_SCOPE)
    endif()

    set(HHC_EXECUTABLE ${HHC_EXECUTABLE} PARENT_SCOPE)
  endif()

  set(GRAPHVIZ_DOT_EXECUTABLE ${GRAPHVIZ_DOT_EXECUTABLE} PARENT_SCOPE)
  set(MATHJAX_PATH ${MATHJAX_PATH} PARENT_SCOPE)
  set(OCCT_DOC_TOOLS_AVAILABLE TRUE PARENT_SCOPE)
endfunction()

# Function to process LaTeX files for PDF generation
function(OCCT_DOC_PROCESS_LATEX OUTPUT_DIR)
  # Skip if PDF generation is not enabled or pdflatex not found
  if(NOT BUILD_DOC_PDF OR NOT PDFLATEX_EXECUTABLE)
    return()
  endif()

  message(STATUS "Processing LaTeX files for PDF generation...")

  # Process SVG images if Inkscape is available
  if(INKSCAPE_EXECUTABLE)
    file(GLOB SVG_FILES "${OUTPUT_DIR}/latex/*.svg")
    foreach(SVG_FILE ${SVG_FILES})
      get_filename_component(FILE_NAME ${SVG_FILE} NAME_WE)
      set(PNG_FILE "${OUTPUT_DIR}/latex/${FILE_NAME}.png")

      execute_process(
        COMMAND ${INKSCAPE_EXECUTABLE} -z -e ${PNG_FILE} ${SVG_FILE}
        RESULT_VARIABLE INKSCAPE_RESULT
      )

      if(NOT INKSCAPE_RESULT EQUAL 0)
        message(WARNING "Failed to convert ${SVG_FILE} to PNG")
      endif()
    endforeach()
  endif()

  # Generate PDF from LaTeX
  execute_process(
    COMMAND ${PDFLATEX_EXECUTABLE} -interaction=nonstopmode refman.tex
    WORKING_DIRECTORY "${OUTPUT_DIR}/latex"
    RESULT_VARIABLE LATEX_RESULT
    OUTPUT_VARIABLE LATEX_OUTPUT
    ERROR_VARIABLE LATEX_ERROR
  )

  if(NOT LATEX_RESULT EQUAL 0)
    message(WARNING "Error generating PDF: ${LATEX_ERROR}")
  else()
    # Run pdflatex again for references
    execute_process(
      COMMAND ${PDFLATEX_EXECUTABLE} -interaction=nonstopmode refman.tex
      WORKING_DIRECTORY "${OUTPUT_DIR}/latex"
    )

    message(STATUS "PDF documentation generated at ${OUTPUT_DIR}/latex/refman.pdf")

    # Copy the PDF to a more accessible location
    file(COPY "${OUTPUT_DIR}/latex/refman.pdf" DESTINATION "${OUTPUT_DIR}")
    file(RENAME "${OUTPUT_DIR}/refman.pdf" "${OUTPUT_DIR}/${DOC_OUTPUT_NAME}.pdf")
  endif()
endfunction()

# Main function to set up documentation targets
function(OCCT_SETUP_DOC_TARGETS)
  # Check if required tools are available
  OCCT_DOC_CHECK_TOOLS()
  if(NOT OCCT_DOC_TOOLS_AVAILABLE)
    return()
  endif()

  # Load lists of documentation files
  OCCT_DOC_LOAD_FILE_LISTS()

  # Collect module information
  OCCT_DOC_COLLECT_MODULES_INFO()

  # Extract toolkit dependencies
  OCCT_DOC_EXTRACT_TOOLKIT_DEPENDENCIES()

  # Create documentation output directories
  set(DOC_ROOT_DIR "${CMAKE_BINARY_DIR}/doc")
  file(MAKE_DIRECTORY ${DOC_ROOT_DIR})

  # Setup Reference Manual target
  if(BUILD_DOC_RefMan)
    # Create output directories
    set(REFMAN_OUTPUT_DIR "${DOC_ROOT_DIR}/refman")
    file(MAKE_DIRECTORY ${REFMAN_OUTPUT_DIR})
    file(MAKE_DIRECTORY "${REFMAN_OUTPUT_DIR}/html")

    # Copy index file to provide fast access to HTML documentation
    file(COPY "${CMAKE_SOURCE_DIR}/dox/resources/index.html" DESTINATION "${REFMAN_OUTPUT_DIR}")

    # Generate main page for reference manual
    OCCT_DOC_GENERATE_MAIN_PAGE(${REFMAN_OUTPUT_DIR} "main_page.dox")

    # Configure Doxygen
    set(DOC_TYPE "REFMAN")
    OCCT_DOC_CONFIGURE_DOXYGEN(${REFMAN_OUTPUT_DIR} "Doxyfile" ${DOC_TYPE})

    # Add custom target for reference manual
    add_custom_target(RefMan
      COMMAND ${DOXYGEN_EXECUTABLE} ${REFMAN_OUTPUT_DIR}/Doxyfile
      COMMENT "Generating Reference Manual with Doxygen"
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      VERBATIM
    )

    # Add custom command to install generated documentation if required
    if(INSTALL_DOC_RefMan)
      install(DIRECTORY "${REFMAN_OUTPUT_DIR}/html/"
              DESTINATION "${INSTALL_DIR_DOC}/refman/html"
              OPTIONAL)
    endif()
  endif()

  # Setup Overview documentation target (after RefMan so it can use the RefMan tag file)
  if(BUILD_DOC_Overview)
    # Create output directories
    set(OVERVIEW_OUTPUT_DIR "${DOC_ROOT_DIR}/overview")
    file(MAKE_DIRECTORY ${OVERVIEW_OUTPUT_DIR})
    file(MAKE_DIRECTORY "${OVERVIEW_OUTPUT_DIR}/html")

    # Configure Doxygen for Overview
    set(DOC_TYPE "OVERVIEW")
    OCCT_DOC_CONFIGURE_DOXYGEN(${OVERVIEW_OUTPUT_DIR} "Doxyfile" ${DOC_TYPE})

    # Add custom target for overview documentation
    add_custom_target(Overview
      COMMAND ${DOXYGEN_EXECUTABLE} ${OVERVIEW_OUTPUT_DIR}/Doxyfile
      COMMENT "Generating Overview documentation with Doxygen"
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      VERBATIM
    )

    # Copy index file to provide fast access to HTML documentation
    file(COPY "${CMAKE_SOURCE_DIR}/dox/resources/index.html" DESTINATION "${OVERVIEW_OUTPUT_DIR}")

    # Add custom command to copy generated documentation to install location if required
    if(INSTALL_DOC_Overview)
      install(DIRECTORY "${OVERVIEW_OUTPUT_DIR}/html/"
              DESTINATION "${INSTALL_DIR_DOC}/overview/html"
              OPTIONAL)

      # Create overview.html only for windows
      if(WIN32)
        install(FILES "${CMAKE_SOURCE_DIR}/dox/resources/overview.html"
                DESTINATION "${INSTALL_DIR_DOC}")
      endif()
    endif()
  endif()

  # Combined documentation target
  if(BUILD_DOC_Overview AND BUILD_DOC_RefMan)
    add_custom_target(doc ALL
      DEPENDS RefMan Overview
      COMMENT "Generating all documentation"
    )
    set_property (TARGET doc PROPERTY FOLDER "Documentation")
    set_property (TARGET Overview PROPERTY FOLDER "Documentation")
    set_property (TARGET RefMan PROPERTY FOLDER "Documentation")
    add_dependencies(Overview RefMan) # Ensure Overview uses RefMan tag file
  elseif(BUILD_DOC_Overview)
    add_custom_target(doc ALL
      DEPENDS Overview
      COMMENT "Generating Overview documentation"
    )
    set_property (TARGET Overview PROPERTY FOLDER "Documentation")
  elseif(BUILD_DOC_RefMan)
    add_custom_target(doc ALL
      DEPENDS RefMan
      COMMENT "Generating Reference Manual"
    )
    set_property (TARGET RefMan PROPERTY FOLDER "Documentation")
  endif()
endfunction()
