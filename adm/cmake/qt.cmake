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
else()
  set (3RDPARTY_QT_DIR ${Qt5_DIR} CACHE PATH "The directory containing Qt" FORCE)
endif()

set (USED_3RDPARTY_QT_DIR "${3RDPARTY_QT_DIR}")

if (3RDPARTY_QT_DIR OR EXISTS "${3RDPARTY_QT_DIR}")
  list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_QT_DIR}/bin")
else()
  list (APPEND 3RDPARTY_NO_DLLS 3RDPARTY_QT_DLL_DIR)
endif()
