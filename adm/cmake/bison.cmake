# bison

# execute FindBISON script by "find_package (Bison)" is required to define BISON_TARGET macro

# delete obsolete 3RDPARTY_BISON_EXECUTABLE cache variable (not used anymore)
unset (3RDPARTY_BISON_EXECUTABLE CACHE)

# delete BISON_EXECUTABLE cache variable if it is empty, otherwise find_package will fail
# without reasonable diagnostic
if (NOT BISON_EXECUTABLE OR NOT EXISTS "${BISON_EXECUTABLE}")
  unset (BISON_EXECUTABLE CACHE)
endif()

# Add paths to 3rdparty subfolders containing name "bison" to CMAKE_PROGRAM_PATH variable to make
# these paths searhed by find_package
if (3RDPARTY_DIR)
  file (GLOB BISON_PATHS LIST_DIRECTORIES true "${3RDPARTY_DIR}/*bison*/")
  foreach (candidate_path ${BISON_PATHS})
    if (IS_DIRECTORY ${candidate_path})
      list (APPEND CMAKE_PROGRAM_PATH ${candidate_path})
    endif()
  endforeach()
endif()
 
# bison 3.2 is required because it provides options to avoid generation of redundant header
# files and embedding of local paths in the generated code
find_package (BISON 3.2)
