# vtk

#if (NOT DEFINED INSTALL_VTK)
#  set (INSTALL_VTK OFF CACHE BOOL "${INSTALL_VTK_DESCR}")
#endif()

# vtk directory
if (NOT DEFINED 3RDPARTY_VTK_DIR)
  set (3RDPARTY_VTK_DIR "" CACHE PATH "The directory containing VTK")
endif()

# include occt macros. compiler_bitness, os_wiht_bit, compiler
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

# specify VTK folder in connectin with 3RDPARTY_DIR
if (3RDPARTY_DIR AND EXISTS "${3RDPARTY_DIR}")
  #CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_DIR 3RDPARTY_VTK_DIR PATH "The directory containing VTK")

  if (NOT 3RDPARTY_VTK_DIR OR NOT EXISTS "${3RDPARTY_VTK_DIR}")
    FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" VTK VTK_DIR_NAME)
    if (VTK_DIR_NAME)
      set (3RDPARTY_VTK_DIR "${3RDPARTY_DIR}/${VTK_DIR_NAME}" CACHE PATH "The directory containing VTK" FORCE)
    endif()
  endif()
else()
  #set (3RDPARTY_VTK_DIR "" CACHE PATH "The directory containing VTK" FORCE)
endif()

# vtk include directory
if (NOT DEFINED 3RDPARTY_VTK_INCLUDE_DIR)
  set (3RDPARTY_VTK_INCLUDE_DIR "" CACHE PATH "The directory containing headers of VTK")
endif()

# vtk library directory
if (NOT DEFINED 3RDPARTY_VTK_LIBRARY_DIR)
  set (3RDPARTY_VTK_LIBRARY_DIR "" CACHE PATH "The directory containing VTK libraries")
endif()

# vtk dll directory
if (WIN32 AND NOT DEFINED 3RDPARTY_VTK_DLL_DIR)
  set (3RDPARTY_VTK_DLL_DIR "" CACHE PATH "The directory containing VTK shared libraries")
endif()

# check 3RDPARTY_VTK_ paths for consistency with specified 3RDPARTY_VTK_DIR
if (3RDPARTY_VTK_DIR AND EXISTS "${3RDPARTY_VTK_DIR}")
  CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_VTK_DIR 3RDPARTY_VTK_INCLUDE_DIR PATH "The directory containing headers of VTK")
  CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_VTK_DIR 3RDPARTY_VTK_LIBRARY_DIR PATH "The directory containing VTK libraries")

  if (WIN32)
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_VTK_DIR 3RDPARTY_VTK_DLL_DIR PATH "The directory containing VTK shared library")
  endif()
endif()

if (3RDPARTY_VTK_DIR AND EXISTS "${3RDPARTY_VTK_DIR}")
  set (CACHED_VTK_DIR $ENV{VTK_DIR})
  set (ENV{VTK_DIR} "${3RDPARTY_VTK_DIR}")
endif()

find_package(VTK QUIET)

if (3RDPARTY_VTK_DIR AND EXISTS "${3RDPARTY_VTK_DIR}")
  set (ENV{VTK_DIR} ${CACHED_VTK_DIR})
endif()

if (VTK_FOUND)

  # add compiler flags, preprocessor definitions, include and link dirs
  include (${VTK_USE_FILE})

  if (VTK_LIBRARIES)

    set (3RDPARTY_VTK_INCLUDE_DIRS)
    set (3RDPARTY_VTK_LIBRARY_DIRS)
    set (3RDPARTY_VTK_DLL_DIRS)

    foreach (VTK_LIBRARY ${VTK_LIBRARIES})
      string (REGEX MATCH "^vtk" IS_VTK_LIBRARY ${VTK_LIBRARY})
      if (IS_VTK_LIBRARY)
        # get paths from corresponding variables
        if (${VTK_LIBRARY}_INCLUDE_DIRS AND EXISTS "${${VTK_LIBRARY}_INCLUDE_DIRS}")
          list (APPEND 3RDPARTY_VTK_INCLUDE_DIRS "${${VTK_LIBRARY}_INCLUDE_DIRS}")
        endif()

        if (${VTK_LIBRARY}_LIBRARY_DIRS AND EXISTS "${${VTK_LIBRARY}_LIBRARY_DIRS}")
          list (APPEND 3RDPARTY_VTK_LIBRARY_DIRS "${${VTK_LIBRARY}_LIBRARY_DIRS}")
        endif()

        if (${VTK_LIBRARY}_RUNTIME_LIBRARY_DIRS AND EXISTS "${${VTK_LIBRARY}_RUNTIME_LIBRARY_DIRS}")
          list (APPEND 3RDPARTY_VTK_DLL_DIRS "${${VTK_LIBRARY}_RUNTIME_LIBRARY_DIRS}")
          if (NOT WIN32)
            list (APPEND 3RDPARTY_VTK_LIBRARY_DIRS "${${VTK_LIBRARY}_RUNTIME_LIBRARY_DIRS}")
          endif()
        endif()

        # get paths from corresponding properties
        get_property (TARGET_VTK_IMPORT_CONFS TARGET ${VTK_LIBRARY} PROPERTY IMPORTED_CONFIGURATIONS)

        if (TARGET_VTK_IMPORT_CONFS)
          list (GET TARGET_VTK_IMPORT_CONFS 0 CHOSEN_IMPORT_CONF)
          
          # todo: choose configuration in connection with the build type
          #if (CMAKE_BUILD_TYPE)
          #  foreach (IMPORT_CONF ${TARGET_VTK_IMPORT_CONFS})
          #  endforeach()
          #endif()

          get_property (TARGET_PROPERTY_IMP_PATH TARGET ${VTK_LIBRARY} PROPERTY IMPORTED_IMPLIB_${CHOSEN_IMPORT_CONF})
          if(TARGET_PROPERTY_IMP_PATH AND EXISTS "${TARGET_PROPERTY_IMP_PATH}")
            get_filename_component (TARGET_PROPERTY_IMP_DIR "${TARGET_PROPERTY_IMP_PATH}" PATH)
            list (APPEND 3RDPARTY_VTK_LIBRARY_DIRS "${TARGET_PROPERTY_IMP_DIR}")
          endif()

          get_property (TARGET_PROPERTY_LOCATION_PATH TARGET ${VTK_LIBRARY} PROPERTY IMPORTED_LOCATION_${CHOSEN_IMPORT_CONF})
          if(TARGET_PROPERTY_LOCATION_PATH AND EXISTS "${TARGET_PROPERTY_LOCATION_PATH}")
            get_filename_component (TARGET_PROPERTY_LOCATION_DIR "${TARGET_PROPERTY_LOCATION_PATH}" PATH)

            if (WIN32)
              list (APPEND 3RDPARTY_VTK_DLL_DIRS "${TARGET_PROPERTY_LOCATION_DIR}")
            else()
              list (APPEND 3RDPARTY_VTK_LIBRARY_DIRS "${TARGET_PROPERTY_LOCATION_DIR}")
            endif()
          endif()
        endif()
      endif()
    endforeach()
  endif()

  if (3RDPARTY_VTK_INCLUDE_DIRS)
    list (REMOVE_DUPLICATES 3RDPARTY_VTK_INCLUDE_DIRS)
    list (APPEND 3RDPARTY_INCLUDE_DIRS ${3RDPARTY_VTK_INCLUDE_DIRS})

    list (GET 3RDPARTY_VTK_INCLUDE_DIRS 0 3RDPARTY_VTK_INCLUDE_DIR)
    set (3RDPARTY_VTK_INCLUDE_DIR "${3RDPARTY_VTK_INCLUDE_DIR}" CACHE PATH "The directory containing headers of VTK" FORCE)
  endif()

  if (3RDPARTY_VTK_LIBRARY_DIRS)
    list (REMOVE_DUPLICATES 3RDPARTY_VTK_LIBRARY_DIRS)
    list (APPEND 3RDPARTY_LIBRARY_DIRS ${3RDPARTY_VTK_LIBRARY_DIRS})

    list (GET 3RDPARTY_VTK_LIBRARY_DIRS 0 3RDPARTY_VTK_LIBRARY_DIR)
    set (3RDPARTY_VTK_LIBRARY_DIR "${3RDPARTY_VTK_LIBRARY_DIR}" CACHE PATH "The directory containing VTK libraries" FORCE)
  endif()

  if (WIN32)
    if (3RDPARTY_VTK_DLL_DIRS)
      list (REMOVE_DUPLICATES 3RDPARTY_VTK_DLL_DIRS)
      list (APPEND 3RDPARTY_DLL_DIRS ${3RDPARTY_VTK_DLL_DIRS})

      list (GET 3RDPARTY_VTK_DLL_DIRS 0 3RDPARTY_VTK_DLL_DIR)
      set (3RDPARTY_VTK_DLL_DIR "${3RDPARTY_VTK_DLL_DIR}" CACHE PATH "The directory containing VTK shared libraries" FORCE)
    endif()
  endif()
endif()

if (3RDPARTY_VTK_INCLUDE_DIR AND EXISTS "${3RDPARTY_VTK_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS ${3RDPARTY_VTK_INCLUDE_DIR})
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_VTK_INCLUDE_DIR)
endif()

if (3RDPARTY_VTK_LIBRARY_DIR AND EXISTS "${3RDPARTY_VTK_LIBRARY_DIR}")
  list (APPEND 3RDPARTY_LIBRARY_DIRS ${3RDPARTY_VTK_LIBRARY_DIR})
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_VTK_LIBRARY_DIR)
endif()

if (WIN32)
  if (3RDPARTY_VTK_DLL_DIR OR EXISTS "${3RDPARTY_VTK_DLL_DIR}")
    list (APPEND 3RDPARTY_DLL_DIRS ${3RDPARTY_VTK_DLL_DIR})
  else()
    list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_VTK_DLL_DIR)
  endif()
endif()

# the library directory for using by the executable
if (WIN32)
  set (USED_3RDPARTY_VTK_DIR ${3RDPARTY_VTK_DLL_DIR})
else()
  set (USED_3RDPARTY_VTK_DIR ${3RDPARTY_VTK_LIBRARY_DIR})
endif()

OCCT_CHECK_AND_UNSET (VTK_INCLUDE_DIRS)
OCCT_CHECK_AND_UNSET (VTK_LIBRARY_DIRS)
OCCT_CHECK_AND_UNSET (VTK_DIR)
