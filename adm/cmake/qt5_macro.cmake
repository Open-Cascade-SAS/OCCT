#qt

macro (FIND_QT5_PACKAGE PROJECT_LIBRARIES_DEBUG PROJECT_LIBRARIES_RELEASE PROJECT_INCLUDES)

  if ("${3RDPARTY_QT_DIR}" STREQUAL "")
    message (FATAL_ERROR "Empty Qt dir")
  endif()

  # Now set CMAKE_PREFIX_PATH to point to local Qt installation.
  # Without this setting find_package() will not work
  set(CMAKE_PREFIX_PATH ${3RDPARTY_QT_DIR})
  if (USE_QT4)
    # Now we can apply standard CMake finder for Qt. We do this mostly
    # to have qt5_wrap_cpp() function available
    find_package(Qt4)
    #message (STATUS "Qt cmake configuration at directory ${Qt4DIR}")

    set(PROJECT_INCLUDES ${QT_INCLUDES})
    if (WIN32)
      set(PROJECT_LIBRARIES_DEBUG "${3RDPARTY_QT_DIR}/lib/QtCored4.lib;${3RDPARTY_QT_DIR}/lib/QtGuid4.lib")
      set(PROJECT_LIBRARIES_RELEASE "${3RDPARTY_QT_DIR}/lib/QtCore4.lib;${3RDPARTY_QT_DIR}/lib/QtGui4.lib")
    else()
      set(PROJECT_LIBRARIES_DEBUG "${3RDPARTY_QT_DIR}/lib/libQtCore.so;${3RDPARTY_QT_DIR}/lib/libQtGui.so")
      set(PROJECT_LIBRARIES_RELEASE "${3RDPARTY_QT_DIR}/lib/libQtCore.so;${3RDPARTY_QT_DIR}/lib/libQtGui.so")
    endif(WIN32)
  else()
    # Now we can apply standard CMake finder for Qt4. We do this mostly
    # to have qt5_wrap_cpp() function available
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    #message (STATUS "Qt cmake configuration at directory ${Qt5DIR}")

    set(PROJECT_INCLUDES "${Qt5Widgets_INCLUDE_DIRS}")
    set(PROJECT_LIBRARIES_DEBUG "${Qt5Widgets_LIBRARIES}")
    set(PROJECT_LIBRARIES_RELEASE "${Qt5Widgets_LIBRARIES}")
  endif()
endmacro()


macro (FIND_AND_WRAP_MOC_FILES SOURCE_FILES)
  SET(CMAKE_AUTOMOC ON)

  foreach (FILE ${SOURCE_FILES})
    set (src_files ${src_files} ${FILE})
    unset (MOC_FILE)
    if (USE_QT4)
      qt4_wrap_cpp(MOC_FILE ${FILE})
    else()
      qt5_wrap_cpp(MOC_FILE ${FILE})
    endif()
    #message (STATUS "... Info: next MOC file ${MOC_FILE}")

  endforeach (FILE)

endmacro()

macro (FIND_AND_WRAP_RESOURCE_FILE RESOURCE_FILE_NAME RCC_FILES)
  if(EXISTS "${RESOURCE_FILE_NAME}")
    if (USE_QT4)
      qt4_add_resources(RCC_FILES "${RESOURCE_FILE_NAME}")
      # suppress some GCC warnings coming from source files generated from .qrc resources
      if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
      endif()
    else()
      qt5_add_resources(RCC_FILES "${RESOURCE_FILE_NAME}")
    endif()
  endif()
endmacro()
