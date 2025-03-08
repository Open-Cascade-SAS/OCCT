#qt

# Qt is searched manually first (just determine root)
message (STATUS "Processing Qt 3-rd party")

set (USE_QT_FROM_3RDPARTY_DIR TRUE)
if (NOT DEFINED ${3RDPARTY_QT_DIR} AND ${3RDPARTY_QT_DIR} STREQUAL "")
  FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" Qt 3RDPARTY_QT_DIR_NAME)

  if (NOT DEFINED ${3RDPARTY_QT_DIR_NAME} AND ${3RDPARTY_QT_DIR_NAME} STREQUAL "")
    set (3RDPARTY_QT_DIR "" CACHE PATH "The directory containing qt")
    set (USE_QT_FROM_3RDPARTY_DIR FALSE)
  else()
    # Combine directory name with absolute path and show in GUI
    set (3RDPARTY_QT_DIR "${3RDPARTY_DIR}/${3RDPARTY_QT_DIR_NAME}" CACHE PATH "The directory containing Qt" FORCE)
  endif()
endif()

if (${USE_QT_FROM_3RDPARTY_DIR})
  # Now set CMAKE_PREFIX_PATH to point to local Qt installation.
  # Without this setting find_package() will not work
  set(CMAKE_PREFIX_PATH ${3RDPARTY_QT_DIR})
  
  # Now we can apply standard CMake finder for Qt5. We do this mostly
  # to have qt5_wrap_cpp() function available and Qt5_FOUND variable filled
  find_package(Qt5 QUIET COMPONENTS Widgets Quick Xml PATHS ${3RDPARTY_QT_DIR} NO_DEFAULT_PATH)
else()
  find_package(Qt5 QUIET COMPONENTS Widgets Quick Xml)
endif()
if (NOT ${Qt5_FOUND})
  # Now we can apply standard CMake finder for Qt. We do this mostly
  # to have qt4_wrap_cpp() function available
  find_package(Qt4)
elseif(NOT ${USE_QT_FROM_3RDPARTY_DIR} AND WIN32)
  # Qt5_DIR typically points to lib/cmake/Qt5, need to go up to Qt root
  get_filename_component(QT_CMAKE_DIR "${Qt5_DIR}" DIRECTORY)
  get_filename_component(QT_LIB_DIR "${QT_CMAKE_DIR}" DIRECTORY)
  get_filename_component(QT_ROOT_DIR "${QT_LIB_DIR}" DIRECTORY)
  
  # Verify this is indeed the Qt root by checking for bin and packages directories
  if(EXISTS "${QT_ROOT_DIR}/bin")
    set(3RDPARTY_QT_DIR ${QT_ROOT_DIR} CACHE PATH "The directory containing Qt" FORCE)
  else()
    message(WARNING "Found Qt5 at ${Qt5_DIR} but could not determine Qt root directory with bin/ and plugins/ folders")
    set(3RDPARTY_QT_DIR ${Qt5_DIR} CACHE PATH "The directory containing Qt" FORCE)
  endif()
else()
  set(3RDPARTY_QT_DIR ${Qt5_DIR} CACHE PATH "The directory containing Qt" FORCE)
endif()

set (USED_3RDPARTY_QT_DIR "${3RDPARTY_QT_DIR}")

if (3RDPARTY_QT_DIR OR EXISTS "${3RDPARTY_QT_DIR}/bin")
  list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_QT_DIR}/bin")
else()
  list (APPEND 3RDPARTY_NO_DLLS 3RDPARTY_QT_DLL_DIR)
endif()
