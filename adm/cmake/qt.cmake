#qt

#looking for 3RDPARTY_QT_DIR variable used later in qt_macro.cmake
SET(CSF_QtCore "QtCore")
THIRDPARTY_PRODUCT("QT" "" "CSF_QtCore" "d")

list (APPEND 3RDPARTY_DLL_DIRS "${3RDPARTY_QT_DIR}/bin")

list (REMOVE_ITEM 3RDPARTY_NOT_INCLUDED "3RDPARTY_QT_INCLUDE_DIR")
list (REMOVE_ITEM 3RDPARTY_NO_LIBS "3RDPARTY_QT_LIBRARY_DIR")
list (REMOVE_ITEM 3RDPARTY_NO_DLLS "3RDPARTY_QT_DLL_DIR")

UNSET (${3RDPARTY_QT_DLL} CACHE)
UNSET (${3RDPARTY_QT_DLL_DIR} CACHE)
UNSET (${3RDPARTY_QT_INCLUDE_DIR} CACHE)
UNSET (${3RDPARTY_QT_LIBRARY} CACHE)
UNSET (${3RDPARTY_QT_LIBRARY_DIR} CACHE)

set (USED_3RDPARTY_QT_DIR "${3RDPARTY_QT_DIR}")
message (STATUS "Info: Qt is used from folder: ${3RDPARTY_QT_DIR}")

# Now set CMAKE_PREFIX_PATH to point to local Qt installation.
# Without this setting find_package() will not work
set(CMAKE_PREFIX_PATH ${3RDPARTY_QT_DIR})

# Now we can apply standard CMake finder for Qt5. We do this mostly
# to have qt5_wrap_cpp() function available and Qt5_FOUND variable filled
find_package(Qt5 QUIET COMPONENTS Widgets Quick Xml PATHS ${3RDPARTY_QT_DIR} NO_DEFAULT_PATH)
if (NOT ${Qt5_FOUND})
  # Now we can apply standard CMake finder for Qt. We do this mostly
  # to have qt4_wrap_cpp() function available
  find_package(Qt4)
  #message (STATUS "Qt4 cmake configuration")
else()
  #message (STATUS "Qt5 cmake configuration")
endif()
