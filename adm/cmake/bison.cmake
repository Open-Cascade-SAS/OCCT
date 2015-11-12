# bison

# execute FindBISON script by "find_package (Bison)" is required to define BISON_TARGET macro

if (NOT DEFINED 3RDPARTY_BISON_EXECUTABLE)
  set (3RDPARTY_BISON_EXECUTABLE "" CACHE FILEPATH "The path to the bison command")
endif()

# BISON_EXECUTABLE is required by BISON_TARGET macro and should be defined
set (BISON_EXECUTABLE "${3RDPARTY_BISON_EXECUTABLE}" CACHE FILEPATH "path to the bison executable" FORCE)

find_package (BISON)

if (BISON_FOUND)
  set (3RDPARTY_BISON_EXECUTABLE "${BISON_EXECUTABLE}" CACHE FILEPATH "The Path to the bison command" FORCE)
endif()

if (NOT 3RDPARTY_BISON_EXECUTABLE OR NOT EXISTS "${3RDPARTY_BISON_EXECUTABLE}")
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_BISON_EXECUTABLE)
endif()