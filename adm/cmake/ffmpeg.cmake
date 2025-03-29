# vcpkg processing
if (BUILD_USE_VCPKG)
  find_package(FFMPEG REQUIRED)
  set(CSF_FFmpeg ${FFMPEG_LIBRARIES})
  if (WIN32)
    set (USED_3RDPARTY_FREEIMAGE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
  else()
    set (USED_3RDPARTY_FREEIMAGE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib")
  endif()
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${FFMPEG_INCLUDE_DIRS}")
  return()
endif()

if (NOT DEFINED INSTALL_FFMPEG)
  set (INSTALL_FFMPEG OFF CACHE BOOL "${INSTALL_FFMPEG_DESCR}")
endif()

if (NOT DEFINED 3RDPARTY_FFMPEG_DIR)
  set (3RDPARTY_FFMPEG_DIR "" CACHE PATH "The directory containing FFmpeg")
endif()

# include occt macros. compiler_bitness, os_with_bit, compiler
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

# specify FFMPEG folder in connection with 3RDPARTY_DIR
if (3RDPARTY_DIR AND EXISTS "${3RDPARTY_DIR}")
  if (NOT 3RDPARTY_FFMPEG_DIR OR NOT EXISTS "${3RDPARTY_FFMPEG_DIR}")
    FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" FFMPEG FFMPEG_DIR_NAME)
    if (FFMPEG_DIR_NAME)
      set (3RDPARTY_FFMPEG_DIR "${3RDPARTY_DIR}/${FFMPEG_DIR_NAME}" CACHE PATH "The directory containing FFmpeg" FORCE)
    endif()
  endif()
else()
endif()

# define required FFMPEG variables
if (NOT DEFINED 3RDPARTY_FFMPEG_INCLUDE_DIR)
  set (3RDPARTY_FFMPEG_INCLUDE_DIR  "" CACHE PATH "the path of headers directory")
endif()

separate_arguments (CSF_FFmpeg)
foreach (LIBRARY_NAME ${CSF_FFmpeg})
  if (NOT DEFINED 3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} OR NOT 3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME} OR NOT EXISTS "${3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME}}")
    set (3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME}               "" CACHE FILEPATH "FFmpeg framework (${LIBRARY_NAME})" FORCE)
  endif()

  if (NOT DEFINED 3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME})
    set (3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME}           "" CACHE PATH "The directory containing FFmpeg framework (${LIBRARY_NAME})")
  endif()

  if (NOT DEFINED 3RDPARTY_FFMPEG_LIBRARY_DIR)
    set (3RDPARTY_FFMPEG_LIBRARY_DIR                           "" CACHE PATH "The directory containing FFmpeg libraries")
  endif()

  if (WIN32)
    if (NOT DEFINED 3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME} OR NOT 3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME} OR NOT EXISTS "${3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME}}")
      set (3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}                 "" CACHE FILEPATH "FFmpeg shared libraries (${LIBRARY_NAME})" FORCE)
    endif()
  endif()

  if (WIN32)
    if (NOT DEFINED 3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME})
      set (3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME}               "" CACHE PATH "The directory containing FFmpeg shared libraries (${LIBRARY_NAME})")
    endif()
    if (NOT DEFINED 3RDPARTY_FFMPEG_DLL_DIR)
      set (3RDPARTY_FFMPEG_DLL_DIR                               "" CACHE PATH "The directory containing FFmpeg shared libraries")
    endif()
  endif()

  # check 3RDPARTY_${PRODUCT_NAME}_ paths for consistency with specified 3RDPARTY_${PRODUCT_NAME}_DIR
  if (3RDPARTY_FFMPEG_DIR AND EXISTS "${3RDPARTY_FFMPEG_DIR}")
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_INCLUDE_DIR PATH "the path to FFmpeg")
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} FILEPATH "the path to FFmpeg framework (${LIBRARY_NAME})")

    if (3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} AND EXISTS "${3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME}}")
      get_filename_component (3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME}}" PATH)
      set (3RDPARTY_FFMPEG_LIBRARY_DIR "${3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME}}" CACHE PATH "The directory containing FFmpeg libraries" FORCE)
      set (3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME}}" CACHE PATH "The directory containing FFmpeg library (${LIBRARY_NAME})" FORCE)
    else()
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME} PATH "The directory containing FFmpeg library (${LIBRARY_NAME})")
    endif()

    if (WIN32)
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME} FILEPATH "the path to FFmpeg shared library (${LIBRARY_NAME})")

      if (3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME} AND EXISTS "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}")
        get_filename_component (3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}" PATH)
        set (3RDPARTY_FFMPEG_DLL_DIR "${3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME}}" CACHE PATH "The directory containing FFmpeg shared libraries" FORCE)
        set (3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME}}" CACHE PATH "The directory containing FFmpeg shared library (${LIBRARY_NAME})" FORCE)
      else()
        CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_FFMPEG_DIR 3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME} PATH "The directory containing FFmpeg shared library (${LIBRARY_NAME})")
      endif()
    endif()
  endif()
endforeach()

# header
if (NOT 3RDPARTY_FFMPEG_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_FFMPEG_INCLUDE_DIR}")
  set (HEADER_NAMES avutil.h libavutil/avutil.h)

  # set 3RDPARTY_FFMPEG_INCLUDE_DIR as notfound, otherwise find_library can't assign a new value to 3RDPARTY_FFMPEG_INCLUDE_DIR
  set (3RDPARTY_FFMPEG_INCLUDE_DIR "3RDPARTY_FFMPEG_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "the path to header directory" FORCE)

  if (3RDPARTY_FFMPEG_DIR AND EXISTS "${3RDPARTY_FFMPEG_DIR}")
    find_path (3RDPARTY_FFMPEG_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                                 PATHS ${3RDPARTY_FFMPEG_DIR}
                                                 PATH_SUFFIXES include
                                                 CMAKE_FIND_ROOT_PATH_BOTH
                                                 NO_DEFAULT_PATH)
  else()
    find_path (3RDPARTY_FFMPEG_INCLUDE_DIR NAMES ${HEADER_NAMES}
                                                 PATH_SUFFIXES include
                                                 CMAKE_FIND_ROOT_PATH_BOTH)
  endif()
endif()

if (3RDPARTY_FFMPEG_INCLUDE_DIR AND EXISTS "${3RDPARTY_FFMPEG_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_FFMPEG_INCLUDE_DIR}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_FFMPEG_INCLUDE_DIR)

  set (3RDPARTY_FFMPEG_INCLUDE_DIR "" CACHE FILEPATH "the path to avutil.h" FORCE)
endif()

# libraries
foreach (LIBRARY_NAME ${CSF_FFmpeg})
  if (NOT 3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} OR NOT EXISTS "${3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME}}")
    set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .so .dylib .a)

    set (FFMPEG_PATH_SUFFIXES lib)
    if (ANDROID)
      set (FFMPEG_PATH_SUFFIXES ${FFMPEG_PATH_SUFFIXES} libs/${ANDROID_ABI})
    elseif(APPLE)
      set (FFMPEG_PATH_SUFFIXES ${FFMPEG_PATH_SUFFIXES} Frameworks)
    endif()

    # set 3RDPARTY_FFMPEG_LIBRARY as notfound, otherwise find_library can't assign a new value to 3RDPARTY_FFMPEG_LIBRARY
    set (3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} "3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME}-NOTFOUND" CACHE FILEPATH "The path to FFmpeg library (${LIBRARY_NAME})" FORCE)

    if (3RDPARTY_FFMPEG_DIR AND EXISTS "${3RDPARTY_FFMPEG_DIR}")
      find_library (3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} NAMES ${LIBRARY_NAME}
                    PATHS "${3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME}}" "${3RDPARTY_FFMPEG_DIR}"
                    PATH_SUFFIXES ${FFMPEG_PATH_SUFFIXES}
                    CMAKE_FIND_ROOT_PATH_BOTH
                    NO_DEFAULT_PATH)
    else()
      find_library (3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} NAMES ${LIBRARY_NAME}
                    PATH_SUFFIXES ${FFMPEG_PATH_SUFFIXES}
                    CMAKE_FIND_ROOT_PATH_BOTH)
    endif()

    if (3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} AND EXISTS "${3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME}}")
      get_filename_component (3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME}}" PATH)
      set (3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME}}" CACHE PATH "The directory containing FFmpeg library (${LIBRARY_NAME})" FORCE)
    else()
      set (3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME} "" CACHE PATH "The directory containing FFmpeg library (${LIBRARY_NAME})" FORCE)
    endif()
  endif()

  if (3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME} AND EXISTS "${3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME}}")
    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME}}")
  else()
    list (APPEND 3RDPARTY_NO_LIBS 3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME})

    set (3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} "" CACHE FILEPATH "The path to FFmpeg library (${LIBRARY_NAME})" FORCE)
  endif()

  # shared library
  if (WIN32)
    if (NOT 3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME} OR NOT EXISTS "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}")

      set (CMAKE_FIND_LIBRARY_SUFFIXES .dll)
      set (3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME} "3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}-NOTFOUND" CACHE FILEPATH "The path to FFmpeg shared library (${LIBRARY_NAME})" FORCE)

      # find FFmpeg shared library
      file (GLOB 3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_DIR}/bin/${LIBRARY_NAME}[-][0-9]*")

      if (3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME} AND EXISTS "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}")
        set (3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}" CACHE FILEPATH "FFmpeg shared library (${LIBRARY_NAME})" FORCE)
        get_filename_component (3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}" PATH)
        set (3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME} "${3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME}}" CACHE PATH "The directory containing FFmpeg library (${LIBRARY_NAME})" FORCE)
      else()
        set (3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME} "" CACHE PATH "The directory containing FFmpeg shared library (${LIBRARY_NAME})" FORCE)
      endif()
    endif()

    if (3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME} OR EXISTS "${3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME}}")
      list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME}}")
    else()
      list (APPEND 3RDPARTY_NO_DLLS 3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME})
    endif()
  endif()

  # install instructions
  if (INSTALL_FFMPEG)
    OCCT_MAKE_OS_WITH_BITNESS()
    OCCT_MAKE_COMPILER_SHORT_NAME()

    if (WIN32)
      if (DEFINED INSTALL_BIN_DIR)
        install (FILES "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}" DESTINATION "${INSTALL_BIN_DIR}")
      else()
        install (FILES "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}"
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/${INSTALL_DIR_BIN}")
        install (FILES "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}"
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/${INSTALL_DIR_BIN}i")
        install (FILES "${3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME}}"
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/${INSTALL_DIR_BIN}d")
      endif()
    else()
      get_filename_component(3RDPARTY_FFMPEG_LIBRARY_ABS ${3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME}} REALPATH)
      get_filename_component(3RDPARTY_FFMPEG_LIBRARY_NAME ${3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME}} NAME)

      if (DEFINED INSTALL_LIB_DIR)
        install (FILES "${3RDPARTY_FFMPEG_LIBRARY_ABS}"
                 DESTINATION "${INSTALL_LIB_DIR}"
                 RENAME ${3RDPARTY_FFMPEG_LIBRARY_NAME}.6)
      else()
        install (FILES "${3RDPARTY_FFMPEG_LIBRARY_ABS}"
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib"
                 RENAME ${3RDPARTY_FFMPEG_LIBRARY_NAME}.6)
        install (FILES "${3RDPARTY_FFMPEG_LIBRARY_ABS}"
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libi"
                 RENAME ${3RDPARTY_FFMPEG_LIBRARY_NAME}.6)
        install (FILES "${3RDPARTY_FFMPEG_LIBRARY_ABS}"
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libd"
                 RENAME ${3RDPARTY_FFMPEG_LIBRARY_NAME}.6)
      endif()
    endif()

    set (USED_3RDPARTY_FFMPEG_DIR "")
  else()
    # the library directory for using by the executable
    if (WIN32)
      set (USED_3RDPARTY_FFMPEG_DIR ${3RDPARTY_FFMPEG_DLL_DIR_${LIBRARY_NAME}})
    else()
      set (USED_3RDPARTY_FFMPEG_DIR ${3RDPARTY_FFMPEG_LIBRARY_DIR_${LIBRARY_NAME}})
    endif()
  endif()

  mark_as_advanced (3RDPARTY_FFMPEG_LIBRARY_${LIBRARY_NAME} 3RDPARTY_FFMPEG_DLL_${LIBRARY_NAME})
endforeach()

# hide all redundant variables
mark_as_advanced (FFMPEG_INCLUDE_DIRS)
mark_as_advanced (FFMPEG_LIBRARY_DIRS)
mark_as_advanced (FFMPEG_DIR)
