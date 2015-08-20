# flex

# execute FindFLEX script by "find_package (Flex)" is required to define FLEX_TARGET macro

if (NOT DEFINED 3RDPARTY_FLEX_EXECUTABLE)
  set (3RDPARTY_FLEX_EXECUTABLE "" CACHE FILEPATH "The Path to the flex command")
endif()

# FLEX_EXECUTABLE is required by FLEX_TARGET macro and should be defined
set (FLEX_EXECUTABLE "${3RDPARTY_FLEX_EXECUTABLE}" CACHE FILEPATH "path to the flex executable" FORCE)

find_package (FLEX)

if (FLEX_FOUND)
  set (3RDPARTY_FLEX_EXECUTABLE "${FLEX_EXECUTABLE}" CACHE FILEPATH "The Path to the flex command" FORCE)
endif()

if (NOT 3RDPARTY_FLEX_EXECUTABLE OR NOT EXISTS "${3RDPARTY_FLEX_EXECUTABLE}")
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FLEX_EXECUTABLE)
endif()