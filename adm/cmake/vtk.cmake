# vtk

if (NOT DEFINED INSTALL_VTK)
  set (INSTALL_VTK OFF CACHE BOOL "${INSTALL_VTK_DESCR}")
endif()

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

if (BUILD_SHARED_LIBS)
  # vtk library directory
  if (NOT DEFINED 3RDPARTY_VTK_LIBRARY_DIR)
    set (3RDPARTY_VTK_LIBRARY_DIR "" CACHE PATH "The directory containing VTK libraries")
  endif()

  # vtk dll directory
  if (WIN32 AND NOT DEFINED 3RDPARTY_VTK_DLL_DIR)
    set (3RDPARTY_VTK_DLL_DIR "" CACHE PATH "The directory containing VTK shared libraries")
  endif()
endif()

# check 3RDPARTY_VTK_ paths for consistency with specified 3RDPARTY_VTK_DIR
if (3RDPARTY_VTK_DIR AND EXISTS "${3RDPARTY_VTK_DIR}")
  CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_VTK_DIR 3RDPARTY_VTK_INCLUDE_DIR PATH "The directory containing headers of VTK")
  if (BUILD_SHARED_LIBS)
    CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_VTK_DIR 3RDPARTY_VTK_LIBRARY_DIR PATH "The directory containing VTK libraries")

    if (WIN32)
      CHECK_PATH_FOR_CONSISTENCY (3RDPARTY_VTK_DIR 3RDPARTY_VTK_DLL_DIR PATH "The directory containing VTK shared library")
    endif()
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

    if (BUILD_SHARED_LIBS)
      set (3RDPARTY_VTK_LIBRARY_DIRS)
      set (3RDPARTY_VTK_DLL_DIRS)
    endif()

    foreach (VTK_LIBRARY ${VTK_LIBRARIES})
      string (REGEX MATCH "^vtk" IS_VTK_LIBRARY ${VTK_LIBRARY})
      if (IS_VTK_LIBRARY AND TARGET ${VTK_LIBRARY})
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
          get_target_property (TARGET_VTK_IMPORT_CONFS ${VTK_LIBRARY} IMPORTED_CONFIGURATIONS)

          if (TARGET_VTK_IMPORT_CONFS)
            list (GET TARGET_VTK_IMPORT_CONFS 0 CHOSEN_IMPORT_CONF)

            # todo: choose configuration in connection with the build type
            #if (CMAKE_BUILD_TYPE)
            #  foreach (IMPORT_CONF ${TARGET_VTK_IMPORT_CONFS})
            #  endforeach()
            #endif()

            # Work-around against link failure in case if VTK contains dependency
            # on DirectX: its run-time is always present on Windows, but SDK can
            # be absent on current workstation, while not actually needed for 
            # OCCT linking.
            # VTK 6.1 for VC 10
            get_target_property (TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES ${VTK_LIBRARY} IMPORTED_LINK_INTERFACE_LIBRARIES_${CHOSEN_IMPORT_CONF})
            if(TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES)
              string (REGEX MATCH "[^;]*d3d[0-9]+[.]lib" HARDCODED_D3D9_LIB "${TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES}")
              if (HARDCODED_D3D9_LIB)
                message (STATUS "Warning: ${HARDCODED_D3D9_LIB} has been removed from imported dependencies of ${VTK_LIBRARY}")

                list (REMOVE_ITEM TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES ${HARDCODED_D3D9_LIB})
                set_target_properties (${VTK_LIBRARY} PROPERTIES IMPORTED_LINK_INTERFACE_LIBRARIES_${CHOSEN_IMPORT_CONF} "${TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES}")
              endif()
            endif()
            # VTK 6.1 for VC 12, 14
            get_target_property (TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES ${VTK_LIBRARY} INTERFACE_LINK_LIBRARIES)
            if(TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES)
              string (REGEX MATCH "[^;]*d3d[0-9]+[.]lib" HARDCODED_D3D9_LIB "${TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES}")
              if (HARDCODED_D3D9_LIB)
                message (STATUS "Warning: ${HARDCODED_D3D9_LIB} has been removed from imported dependencies of ${VTK_LIBRARY}")

                list (REMOVE_ITEM TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES ${HARDCODED_D3D9_LIB})
                set_target_properties (${VTK_LIBRARY} PROPERTIES INTERFACE_LINK_LIBRARIES "${TARGET_PROPERTY_IMP_LINK_INTERFACE_LIBRARIES}")
              endif()
            endif()

            get_target_property (TARGET_PROPERTY_IMP_PATH ${VTK_LIBRARY} IMPORTED_IMPLIB_${CHOSEN_IMPORT_CONF})
            if(TARGET_PROPERTY_IMP_PATH AND EXISTS "${TARGET_PROPERTY_IMP_PATH}")
              get_filename_component (TARGET_PROPERTY_IMP_DIR "${TARGET_PROPERTY_IMP_PATH}" PATH)
              list (APPEND 3RDPARTY_VTK_LIBRARY_DIRS "${TARGET_PROPERTY_IMP_DIR}")
            endif()

            get_target_property (TARGET_PROPERTY_LOCATION_PATH ${VTK_LIBRARY} IMPORTED_LOCATION_${CHOSEN_IMPORT_CONF})
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
      endif()
    endforeach()
  endif()

  if (3RDPARTY_VTK_INCLUDE_DIRS)
    list (REMOVE_DUPLICATES 3RDPARTY_VTK_INCLUDE_DIRS)
    list (APPEND 3RDPARTY_INCLUDE_DIRS ${3RDPARTY_VTK_INCLUDE_DIRS})

    list (GET 3RDPARTY_VTK_INCLUDE_DIRS 0 3RDPARTY_VTK_INCLUDE_DIR)
    set (3RDPARTY_VTK_INCLUDE_DIR "${3RDPARTY_VTK_INCLUDE_DIR}" CACHE PATH "The directory containing headers of VTK" FORCE)
  endif()

  if (BUILD_SHARED_LIBS)
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

if (BUILD_SHARED_LIBS)
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
endif()

# Install vtk library using vtk targets
macro (OCCT_INSTALL_VTK VTK_LIBRARY_NAME)
  # Check that input library name contains "vtk" prefix
  string (REGEX MATCH "^vtk" IS_VTK_LIBRARY ${VTK_LIBRARY_NAME})
  # Check that input library was not already installed
  list (FIND OCCT_VTK_USED_TARGETS ${VTK_LIBRARY_NAME} VTK_LIBRARY_IS_USED)
  if (BUILD_SHARED_LIBS AND INSTALL_VTK AND TARGET ${VTK_LIBRARY_NAME} AND VTK_LIBRARY_IS_USED EQUAL -1 AND IS_VTK_LIBRARY)
    OCCT_MAKE_OS_WITH_BITNESS()

    # Get configuration of vtk
    get_target_property (TARGET_VTK_IMPORT_CONFS ${VTK_LIBRARY_NAME} IMPORTED_CONFIGURATIONS)
    list (GET TARGET_VTK_IMPORT_CONFS 0 CHOSEN_IMPORT_CONF)

    # Get dependencies for current input library
    get_property(VTK_LIBRARY_PATH       TARGET ${VTK_LIBRARY_NAME} PROPERTY LOCATION)
    get_property(VTK_DEPEND             TARGET ${VTK_LIBRARY_NAME} PROPERTY IMPORTED_LINK_DEPENDENT_LIBRARIES_${CHOSEN_IMPORT_CONF})
    get_property(VTK_IMPORTED_INTERFACE TARGET ${VTK_LIBRARY_NAME} PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_${CHOSEN_IMPORT_CONF})
    get_property(VTK_INTERFACE          TARGET ${VTK_LIBRARY_NAME} PROPERTY INTERFACE_LINK_LIBRARIES)
    list (APPEND VTK_DEPEND ${VTK_INTERFACE} ${VTK_IMPORTED_INTERFACE})

    # Install
    if (WIN32)
      if (SINGLE_GENERATOR)
        install (FILES "${VTK_LIBRARY_PATH}" DESTINATION "${INSTALL_DIR_BIN}")
      else()
        install (FILES "${VTK_LIBRARY_PATH}"
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_BIN}")
        install (FILES "${VTK_LIBRARY_PATH}"
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_BIN}i")
        install (FILES "${VTK_LIBRARY_PATH}"
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_BIN}d")
      endif()
    else()
      get_filename_component(3RDPARTY_VTK_LIBRARY_ABS ${VTK_LIBRARY_PATH} REALPATH)

      if (SINGLE_GENERATOR)
        install (FILES "${3RDPARTY_VTK_LIBRARY_ABS}"
                 DESTINATION "${INSTALL_DIR_LIB}")
      else()
        install (FILES "${3RDPARTY_VTK_LIBRARY_ABS}"
                 CONFIGURATIONS Release
                 DESTINATION "${INSTALL_DIR_LIB}")
        install (FILES "${3RDPARTY_VTK_LIBRARY_ABS}"
                 CONFIGURATIONS RelWithDebInfo
                 DESTINATION "${INSTALL_DIR_LIB}i")
        install (FILES "${3RDPARTY_VTK_LIBRARY_ABS}"
                 CONFIGURATIONS Debug
                 DESTINATION "${INSTALL_DIR_LIB}d")
      endif()
    endif()

    set (USED_3RDPARTY_VTK_DIR "")

    # Mark current library as already installed
    list (APPEND OCCT_VTK_USED_TARGETS ${VTK_LIBRARY_NAME})
    set (OCCT_VTK_USED_TARGETS "${OCCT_VTK_USED_TARGETS}" CACHE INTERNAL "" FORCE)

    # Recursively install all depended libraries
    foreach(VTK_TARGET ${VTK_DEPEND})
      OCCT_INSTALL_VTK(${VTK_TARGET})
    endforeach()
  endif()
endmacro()

if (NOT BUILD_SHARED_LIBS)
  OCCT_CHECK_AND_UNSET(3RDPARTY_VTK_DLL_DIR)
  OCCT_CHECK_AND_UNSET(3RDPARTY_VTK_LIBRARY_DIR)
  OCCT_CHECK_AND_UNSET(INSTALL_VTK)
endif()

# the library directory for using by the executable
if (NOT INSTALL_VTK)
  if (WIN32)
    set (USED_3RDPARTY_VTK_DIR ${3RDPARTY_VTK_DLL_DIR})
  else()
    set (USED_3RDPARTY_VTK_DIR ${3RDPARTY_VTK_LIBRARY_DIR})
  endif()
endif()

OCCT_CHECK_AND_UNSET (VTK_INCLUDE_DIRS)
OCCT_CHECK_AND_UNSET (VTK_LIBRARY_DIRS)
OCCT_CHECK_AND_UNSET (VTK_DIR)
