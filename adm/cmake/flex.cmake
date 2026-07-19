# flex

# execute FindFLEX script by "find_package (Flex)" is required to define FLEX_TARGET macro

# delete obsolete 3RDPARTY_FLEX_EXECUTABLE cache variable (not used anymore)
unset (3RDPARTY_FLEX_EXECUTABLE CACHE)

# delete FLEX_EXECUTABLE cache variable if it is empty, otherwise find_package will fail
# without reasonable diagnostic
if (NOT FLEX_EXECUTABLE OR NOT EXISTS "${FLEX_EXECUTABLE}")
  unset (FLEX_EXECUTABLE CACHE)
endif()
if (NOT FLEX_INCLUDE_DIR OR NOT EXISTS "${FLEX_INCLUDE_DIR}")
  unset (FLEX_INCLUDE_DIR CACHE)
endif()

# Add paths to 3rdparty subfolders containing name "flex" to CMAKE_PROGRAM_PATH and 
# CMAKE_INCLUDE_PATH variables to make these paths searched by find_package
if (3RDPARTY_DIR)
  file (GLOB FLEX_PATHS LIST_DIRECTORIES true "${3RDPARTY_DIR}/*flex*")
  foreach (candidate_path ${FLEX_PATHS})
    if (IS_DIRECTORY ${candidate_path})
      list (APPEND CMAKE_PROGRAM_PATH ${candidate_path})
      list (APPEND CMAKE_INCLUDE_PATH ${candidate_path})
    endif()
  endforeach()
endif()
 
find_package (FLEX 2.6.4)

if (NOT FLEX_FOUND OR NOT FLEX_INCLUDE_DIR OR NOT EXISTS "${FLEX_INCLUDE_DIR}/FlexLexer.h")
  list (APPEND 3RDPARTY_NOT_INCLUDED FLEX_INCLUDE_DIR)
endif()

# remove old general version of FlexLexer
if (EXISTS ${OCCT_ROOT_DIR}/${RELATIVE_SOURCES_DIR}/FlexLexer/FlexLexer.h)
  message (STATUS "Info: remove old FLEX header file: ${OCCT_ROOT_DIR}/src/FlexLexer/FlexLexer.h")
  file(REMOVE ${OCCT_ROOT_DIR}/src/FlexLexer/FlexLexer.h)
endif()
# install copy of FlexLexer.h locally to allow further building without flex
if (FLEX_INCLUDE_DIR AND EXISTS "${FLEX_INCLUDE_DIR}/FlexLexer.h")
  configure_file("${FLEX_INCLUDE_DIR}/FlexLexer.h" "${OCCT_ROOT_DIR}/src/FlexLexer/FlexLexer.h" @ONLY NEWLINE_STYLE LF)
endif()
