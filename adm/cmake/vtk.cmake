# vtk

if (NOT DEFINED INSTALL_VTK)
  set (INSTALL_VTK OFF CACHE BOOL "${INSTALL_VTK_DESCR}")
endif()

# vtk directory
if (NOT DEFINED 3RDPARTY_VTK_DIR)
  set (3RDPARTY_VTK_DIR "" CACHE PATH "The directory containing vtk")
endif()

# vtk include directory
if (NOT DEFINED 3RDPARTY_VTK_INCLUDE_DIR)
  set (3RDPARTY_VTK_INCLUDE_DIR "" CACHE FILEPATH "The directory containing headers of vtk")
endif()

# vtk library directory
if (NOT DEFINED 3RDPARTY_VTK_LIBRARY_DIR)
  set (3RDPARTY_VTK_LIBRARY_DIR "" CACHE FILEPATH "The directory containing vtk library")
endif()

# vtk dll directory
if (WIN32 AND NOT DEFINED 3RDPARTY_VTK_DLL_DIR)
  set (3RDPARTY_VTK_DLL_DIR "" CACHE FILEPATH "The directory containing VTK dll")
endif()

# search for vtk in user defined directory
if (NOT 3RDPARTY_VTK_DIR AND 3RDPARTY_DIR)
  FIND_PRODUCT_DIR("${3RDPARTY_DIR}" vtk VTK_DIR_NAME)
  if (VTK_DIR_NAME)
    set (3RDPARTY_VTK_DIR "${3RDPARTY_DIR}/${VTK_DIR_NAME}" CACHE PATH "The directory containing vtk product" FORCE)
  endif()
endif()

# find installed vtk
find_package(VTK QUIET)

# find native vtk
if (NOT VTK_FOUND)
  find_package(VTK QUIET PATHS "${3RDPARTY_VTK_DIR}")
endif()

if (NOT VTK_FOUND AND NOT 3RDPARTY_VTK_DIR OR NOT EXISTS "${3RDPARTY_VTK_DIR}")
  message(SEND_ERROR "VTK not found. Set the 3RDPARTY_VTK_DIR cmake cache entry to the directory containing VTK.")
  set (3RDPARTY_VTK_DIR "3RDPARTY_VTK_DIR-NOTFOUND" CACHE PATH "The directory containing vtk product" FORCE)
endif()

set(VTK_VERSION "")
if (3RDPARTY_VTK_DIR AND EXISTS "${3RDPARTY_VTK_DIR}")
  get_filename_component(3RDPARTY_VTK_DIR_NAME "${3RDPARTY_VTK_DIR}" NAME)
  string(REGEX MATCH "^VTK-([0-9].[0-9])" VTK_VERSION "${3RDPARTY_VTK_DIR_NAME}")
  set(VTK_VERSION "${CMAKE_MATCH_1}")
  if (NOT 3RDPARTY_VTK_INCLUDE_DIR OR NOT EXISTS "${3RDPARTY_VTK_INCLUDE_DIR}")
    set (3RDPARTY_VTK_INCLUDE_DIR "${3RDPARTY_VTK_DIR}/include/vtk-${VTK_VERSION}" CACHE FILEPATH "The directory containing includes of VTK" FORCE)
  endif()
  if (NOT 3RDPARTY_VTK_LIBRARY_DIR OR NOT EXISTS "${3RDPARTY_VTK_LIBRARY_DIR}")
    if(EXISTS "${3RDPARTY_VTK_DIR}/lib")
      set (3RDPARTY_VTK_LIBRARY_DIR "${3RDPARTY_VTK_DIR}/lib" CACHE FILEPATH "The directory containing libs of VTK" FORCE)
    elseif (EXISTS "${3RDPARTY_VTK_DIR}/lib")
      set (3RDPARTY_VTK_LIBRARY_DIR "${3RDPARTY_VTK_DIR}/lib" CACHE FILEPATH "The directory containing libs of VTK" FORCE)
    endif()
  endif()
  if(3RDPARTY_VTK_LIBRARY_DIR)
    list (APPEND 3RDPARTY_LIBRARY_DIRS "${3RDPARTY_VTK_LIBRARY_DIR}")
  endif()
endif()

# vtk libraries
# lib
set (VTK_LIBRARY_NAMES vtkCommonCore-${VTK_VERSION}.lib vtkCommonDataModel-${VTK_VERSION}.lib vtkCommonExecutionModel-${VTK_VERSION}.lib
                       vtkCommonMath-${VTK_VERSION}.lib vtkCommonTransforms-${VTK_VERSION}.lib vtkRenderingCore-${VTK_VERSION}.lib
                       vtkRenderingOpenGL-${VTK_VERSION}.lib vtkFiltersGeneral-${VTK_VERSION}.lib vtkIOCore-${VTK_VERSION}.lib
                       vtkIOImage-${VTK_VERSION}.lib vtkImagingCore-${VTK_VERSION}.lib vtkInteractionStyle-${VTK_VERSION}.lib )

#dll
set (VTK_DLL_NAMES vtkCommonComputationalGeometry-${VTK_VERSION}.dll
                   vtkCommonCore-${VTK_VERSION}.dll
                   vtkCommonDataModel-${VTK_VERSION}.dll
                   vtkCommonExecutionModel-${VTK_VERSION}.dll
                   vtkCommonMath-${VTK_VERSION}.dll
                   vtkCommonMisc-${VTK_VERSION}.dll
                   vtkCommonSystem-${VTK_VERSION}.dll
                   vtkCommonTransforms-${VTK_VERSION}.dll
                   vtkDICOMParser-${VTK_VERSION}.dll
                   vtkFiltersCore-${VTK_VERSION}.dll
                   vtkFiltersExtraction-${VTK_VERSION}.dll
                   vtkFiltersGeneral-${VTK_VERSION}.dll
                   vtkFiltersGeometry-${VTK_VERSION}.dll
                   vtkFiltersSources-${VTK_VERSION}.dll
                   vtkFiltersStatistics-${VTK_VERSION}.dll
                   vtkIOCore-${VTK_VERSION}.dll
                   vtkIOImage-${VTK_VERSION}.dll
                   vtkImagingCore-${VTK_VERSION}.dll
                   vtkImagingFourier-${VTK_VERSION}.dll
                   vtkImagingHybrid-${VTK_VERSION}.dll
                   vtkInteractionStyle-${VTK_VERSION}.dll
                   vtkRenderingCore-${VTK_VERSION}.dll
                   vtkRenderingOpenGL-${VTK_VERSION}.dll
                   vtkalglib-${VTK_VERSION}.dll
                   vtkjpeg-${VTK_VERSION}.dll
                   vtkmetaio-${VTK_VERSION}.dll
                   vtkpng-${VTK_VERSION}.dll
                   vtksys-${VTK_VERSION}.dll
                   vtktiff-${VTK_VERSION}.dll
                   vtkzlib-${VTK_VERSION}.dll )

# search for dll directory
if (WIN32)
  if (NOT 3RDPARTY_VTK_DLL_DIR OR NOT EXISTS "${3RDPARTY_VTK_DLL_DIR}")
    if(EXISTS "${3RDPARTY_VTK_DIR}/bin")
      set (3RDPARTY_VTK_DLL_DIR "${3RDPARTY_VTK_DIR}/bin" CACHE FILEPATH "The directory containing dll of VTK" FORCE)
    elseif (EXISTS "${3RDPARTY_VTK_DIR}/bind")
      set (3RDPARTY_VTK_DLL_DIR "${3RDPARTY_VTK_DIR}/bind" CACHE FILEPATH "The directory containing dll of VTK" FORCE)
    endif()
  endif()

  if (3RDPARTY_VTK_DLL_DIR OR EXISTS "${3RDPARTY_VTK_DLL_DIR}")
    list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_VTK_DLL_DIR}")
  endif()
endif() 

OCCT_CHECK_AND_UNSET(VTK_DIR)

if (3RDPARTY_VTK_INCLUDE_DIR AND EXISTS "${3RDPARTY_VTK_INCLUDE_DIR}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${3RDPARTY_VTK_INCLUDE_DIR}")
else()
  list (APPEND 3RDPARTY_NOT_INCLUDED 3RDPARTY_VTK_INCLUDE_DIR)
endif()

if (INSTALL_VTK)
  OCCT_MAKE_OS_WITH_BITNESS()
  OCCT_MAKE_COMPILER_SHORT_NAME()
  if (WIN32)
    if(3RDPARTY_VTK_DLL_DIR AND EXISTS "${3RDPARTY_VTK_DLL_DIR}")
      set (CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
      foreach(VTK_DLL_NAME ${VTK_DLL_NAMES})
        set (3RDPARTY_VTK_DLL "3RDPARTY_VTK_DLL-NOTFOUND" CACHE FILEPATH "VTK shared library" FORCE)
        find_library(3RDPARTY_VTK_DLL "${VTK_DLL_NAME}" PATHS "${3RDPARTY_VTK_DLL_DIR}" NO_DEFAULT_PATH)
        if (NOT 3RDPARTY_VTK_DLL OR NOT EXISTS "${3RDPARTY_VTK_DLL}")
          list (APPEND 3RDPARTY_NOT_INCLUDED "${3RDPARTY_VTK_DLL}")
        else()
          install (FILES ${3RDPARTY_VTK_DLL}
                   CONFIGURATIONS Release
                   DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
          install (FILES ${3RDPARTY_VTK_DLL}
                   CONFIGURATIONS RelWithDebInfo
                   DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bini")
          install (FILES ${3RDPARTY_VTK_DLL}
                   CONFIGURATIONS Debug
                   DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")
        endif()
      endforeach()
      OCCT_CHECK_AND_UNSET(3RDPARTY_VTK_DLL)
    endif()
  else ()
    foreach(VTK_DLL_NAME ${VTK_DLL_NAMES})
      string(REPLACE ".dll" ".so.1" VTK_DLL_NAME "${VTK_DLL_NAME}")
      install(FILES "${3RDPARTY_VTK_LIBRARY_DIR}/lib${VTK_DLL_NAME}"
              CONFIGURATIONS Release
              DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib"
              RENAME "lib${VTK_DLL_NAME}")
      install(FILES "${3RDPARTY_VTK_LIBRARY_DIR}/lib${VTK_DLL_NAME}"
              CONFIGURATIONS RelWithDebInfo
              DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libi"
              RENAME "lib${VTK_DLL_NAME}")
      install(FILES "${3RDPARTY_VTK_LIBRARY_DIR}/lib${VTK_DLL_NAME}"
              CONFIGURATIONS Debug
              DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/libd"
              RENAME "lib${VTK_DLL_NAME}")
    endforeach()
  endif()

  set (USED_3RDPARTY_VTK_DIR "")
else()
  # the library directory for using by the executable
  if (WIN32)
    set (USED_3RDPARTY_VTK_DIR ${3RDPARTY_VTK_DLL_DIR})
  else()
    set (USED_3RDPARTY_VTK_DIR ${3RDPARTY_VTK_LIBRARY_DIR})
  endif()
endif()

mark_as_advanced (VTK_INCLUDE_DIRS VTK_LIBRARY_DIRS VTK_DIR)
