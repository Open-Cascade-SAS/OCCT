##

if(FLAGS_ALREADY_INCLUDED)
  return()
endif()
set(FLAGS_ALREADY_INCLUDED 1)

# force option /fp:precise for Visual Studio projects.
#
# Note that while this option is default for MSVC compiler, Visual Studio
# project can be switched later to use Intel Compiler (ICC).
# Enforcing -fp:precise ensures that in such case ICC will use correct
# option instead of its default -fp:fast which is harmful for OCCT.
if (MSVC)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:precise")
  set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /fp:precise")
endif()

# add SSE2 option for old MSVC compilers (VS 2005 - 2010, 32 bit only)
if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  if (MSVC AND ((MSVC_VERSION EQUAL 1400) OR (MSVC_VERSION EQUAL 1500) OR (MSVC_VERSION EQUAL 1600)))
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
    set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /arch:SSE2")
  endif()
endif()

if (MSVC)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
else()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fPIC")
  set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fexceptions -fPIC")
  add_definitions(-DOCC_CONVERT_SIGNALS)
endif()

# enable structured exceptions for MSVC
string (REGEX MATCH "EHsc" ISFLAG "${CMAKE_CXX_FLAGS}")
if (ISFLAG)
  string (REGEX REPLACE "EHsc" "EHa" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif (MSVC)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa")
endif()

# remove _WINDOWS flag if it exists
string (REGEX MATCH "/D_WINDOWS" IS_WINDOWSFLAG "${CMAKE_CXX_FLAGS}")
if (IS_WINDOWSFLAG)
  message (STATUS "Info: /D_WINDOWS has been removed from CMAKE_CXX_FLAGS")
  string (REGEX REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# remove WIN32 flag if it exists
string (REGEX MATCH "/DWIN32" IS_WIN32FLAG "${CMAKE_CXX_FLAGS}")
if (IS_WIN32FLAG)
  message (STATUS "Info: /DWIN32 has been removed from CMAKE_CXX_FLAGS")
  string (REGEX REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# remove _WINDOWS flag if it exists
string (REGEX MATCH "/D_WINDOWS" IS_WINDOWSFLAG "${CMAKE_C_FLAGS}")
if (IS_WINDOWSFLAG)
  message (STATUS "Info: /D_WINDOWS has been removed from CMAKE_C_FLAGS")
  string (REGEX REPLACE "/D_WINDOWS" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

# remove WIN32 flag if it exists
string (REGEX MATCH "/DWIN32" IS_WIN32FLAG "${CMAKE_C_FLAGS}")
if (IS_WIN32FLAG)
  message (STATUS "Info: /DWIN32 has been removed from CMAKE_C_FLAGS")
  string (REGEX REPLACE "/DWIN32" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

# remove DEBUG flag if it exists
string (REGEX MATCH "-DDEBUG" IS_DEBUG_CXX "${CMAKE_CXX_FLAGS_DEBUG}")
if (IS_DEBUG_CXX)
  message (STATUS "Info: -DDEBUG has been removed from CMAKE_CXX_FLAGS_DEBUG")
  string (REGEX REPLACE "-DDEBUG" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

string (REGEX MATCH "-DDEBUG" IS_DEBUG_C "${CMAKE_C_FLAGS_DEBUG}")
if (IS_DEBUG_C)
  message (STATUS "Info: -DDEBUG has been removed from CMAKE_C_FLAGS_DEBUG")
  string (REGEX REPLACE "-DDEBUG" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif()
# enable parallel compilation on MSVC 9 and above
if (MSVC AND (MSVC_VERSION GREATER 1400))
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# generate a single response file which enlist all of the object files
if (NOT DEFINED CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS)
  SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
endif()
if (NOT DEFINED CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS)
  SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)
endif()

# increase compiler warnings level (-W4 for MSVC, -Wextra for GCC)
if (MSVC)
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  if (BUILD_SHARED_LIBS)
    if (APPLE)
      set (CMAKE_SHARED_LINKER_FLAGS "-lm ${CMAKE_SHARED_LINKER_FLAGS}")
    elseif(NOT WIN32)
      set (CMAKE_SHARED_LINKER_FLAGS "-lm ${CMAKE_SHARED_LINKER_FLAGS}")
    endif()
  endif()
endif()

if ("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xClang")
  if (APPLE)
    # CLang can be used with both libstdc++ and libc++, however on OS X libstdc++ is outdated.
    set (CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
  elseif(NOT WIN32)
    # CLang for Windows (at least CLang 8.0 distributed with VS 2019)
    # does not support option "-std=c++0x"
    set (CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
  endif()
  # Optimize size of binaries
  set (CMAKE_SHARED_LINKER_FLAGS "-Wl,-s ${CMAKE_SHARED_LINKER_FLAGS}")
elseif(MINGW)
  add_definitions(-D_WIN32_WINNT=0x0501)
  # workaround bugs in mingw with vtable export
  set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")

  # Require C++11
  set (CMAKE_CXX_FLAGS "-std=gnu++0x ${CMAKE_CXX_FLAGS}")
  # Optimize size of binaries
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
elseif (DEFINED CMAKE_COMPILER_IS_GNUCXX)
  # Require C++11
  set (CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
  # Optimize size of binaries
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
endif()

if (BUILD_RELEASE_DISABLE_EXCEPTIONS)
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNo_Exception")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNo_Exception")
endif()
