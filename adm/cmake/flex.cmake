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
# CMAKE_INCLUDE_PATH variables to make these paths searhed by find_package
if (3RDPARTY_DIR)
  file (GLOB FLEX_PATHS LIST_DIRECTORIES true "${3RDPARTY_DIR}/*flex*")
  foreach (candidate_path ${FLEX_PATHS})
    if (IS_DIRECTORY ${candidate_path})
      list (APPEND CMAKE_PROGRAM_PATH ${candidate_path})
      list (APPEND CMAKE_INCLUDE_PATH ${candidate_path})
    endif()
  endforeach()
endif()
 
# flex 2.5.37 is required because closest known lower version, 2.5.3 from WOK 6.8.0,
# generates code which is unusable on Windows (includes unistd.h without any way to avoid this)
find_package (FLEX 2.5.37)

if (NOT FLEX_FOUND OR NOT FLEX_INCLUDE_DIR OR NOT EXISTS "${FLEX_INCLUDE_DIR}/FlexLexer.h")
  list (APPEND 3RDPARTY_NOT_INCLUDED FLEX_INCLUDE_DIR)
endif()
