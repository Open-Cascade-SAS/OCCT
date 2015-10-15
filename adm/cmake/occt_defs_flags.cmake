
if (MSVC)
  add_definitions(/fp:precise)
endif()

# set compiler short name and choose SSE2 option for appropriate MSVC compilers
# ONLY for 32-bit
if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  if (MSVC80 OR MSVC90 OR MSVC10)
    add_definitions(/arch:SSE2)
  endif()
endif()

if (WIN32)
  add_definitions (-wd4996)
elseif (APPLE)
  add_definitions (-fexceptions -fPIC -DOCC_CONVERT_SIGNALS)
else()
  add_definitions (-fexceptions -fPIC -DOCC_CONVERT_SIGNALS)
endif()

# enable structured exceptions for MSVC
string (REGEX MATCH "EHsc" ISFLAG "${CMAKE_CXX_FLAGS}")
if (ISFLAG)
  string (REGEX REPLACE "EHsc" "EHa" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif (WIN32)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -EHa")
endif()

# remove _WINDOWS flag if it exists
string (REGEX MATCH "/D_WINDOWS" IS_WINDOWSFLAG "${CMAKE_CXX_FLAGS}")
if (IS_WINDOWSFLAG)
  message (STATUS "/D_WINDOWS has been removed from CMAKE_CXX_FLAGS")
  string (REGEX REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# remove WIN32 flag if it exists
string (REGEX MATCH "/DWIN32" IS_WIN32FLAG "${CMAKE_CXX_FLAGS}")
if (IS_WIN32FLAG)
  message (STATUS "/DWIN32 has been removed from CMAKE_CXX_FLAGS")
  string (REGEX REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# remove _WINDOWS flag if it exists
string (REGEX MATCH "/D_WINDOWS" IS_WINDOWSFLAG "${CMAKE_C_FLAGS}")
if (IS_WINDOWSFLAG)
  message (STATUS "/D_WINDOWS has been removed from CMAKE_C_FLAGS")
  string (REGEX REPLACE "/D_WINDOWS" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

# remove WIN32 flag if it exists
string (REGEX MATCH "/DWIN32" IS_WIN32FLAG "${CMAKE_C_FLAGS}")
if (IS_WIN32FLAG)
  message (STATUS "/DWIN32 has been removed from CMAKE_C_FLAGS")
  string (REGEX REPLACE "/DWIN32" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

# remove DEBUG flag if it exists
string (REGEX MATCH "-DDEBUG" IS_DEBUG_CXX "${CMAKE_CXX_FLAGS_DEBUG}")
if (IS_DEBUG_CXX)
  message (STATUS "-DDEBUG has been removed from CMAKE_CXX_FLAGS_DEBUG")
  string (REGEX REPLACE "-DDEBUG" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

string (REGEX MATCH "-DDEBUG" IS_DEBUG_C "${CMAKE_C_FLAGS_DEBUG}")
if (IS_DEBUG_C)
  message (STATUS "-DDEBUG has been removed from CMAKE_C_FLAGS_DEBUG")
  string (REGEX REPLACE "-DDEBUG" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif()
# enable parallel compilation on MSVC 9 and above
if (MSVC AND NOT MSVC70 AND NOT MSVC80)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP")
endif()

# generate a single response file which enlist all of the object files
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)
# increase compiler warnings level (-W4 for MSVC, -Wall for GCC)
if (MSVC)
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

if (DEFINED CMAKE_COMPILER_IS_GNUCXX OR "x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xClang")
  set (CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNo_Exception")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNo_Exception")