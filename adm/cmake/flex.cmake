# flex

# execute FindFLEX script by "find_package (Flex)" is required to define FLEX_TARGET macro

# delete obsolete 3RDPARTY_FLEX_EXECUTABLE cache variable (not used anymore)
unset (3RDPARTY_FLEX_EXECUTABLE CACHE)

# delete FLEX_EXECUTABLE cache variable if it is empty, otherwise find_package will fail
# without reasonable diagnostic
if (NOT FLEX_EXECUTABLE)
  unset (FLEX_EXECUTABLE CACHE)
endif()

# Add paths to 3rdparty subfolders containing name "flex" to CMAKE_PROGRAM_PATH variable to make
# these paths searhed by find_package
if (3RDPARTY_DIR)
  file (GLOB FLEX_PATHS LIST_DIRECTORIES true "${3RDPARTY_DIR}/*flex*")
  foreach (candidate_path ${FLEX_PATHS})
    if (IS_DIRECTORY ${candidate_path})
      list (APPEND CMAKE_PROGRAM_PATH ${candidate_path})
    endif()
  endforeach()
endif()
 
find_package (FLEX 2.5.3)

if (NOT FLEX_FOUND OR NOT FLEX_EXECUTABLE OR NOT EXISTS "${FLEX_EXECUTABLE}")
  list (APPEND 3RDPARTY_NOT_INCLUDED FLEX_EXECUTABLE)
endif()