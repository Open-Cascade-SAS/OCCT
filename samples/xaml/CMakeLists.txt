cmake_minimum_required(VERSION 3.4.0)
project(uwp)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SOURCE_FILES
  App.xaml.cpp
  MainPage.xaml.cpp
  pch.cpp
  )

set(HEADER_FILES
  App.xaml.h
  MainPage.xaml.h
  pch.h
  )

set(XAML_FILES
  App.xaml
  MainPage.xaml
  )

set(ASSET_FILES
    Assets/LockScreenLogo.scale-200.png
    Assets/SplashScreen.scale-200.png
    Assets/Square150x150Logo.scale-200.png
    Assets/Square44x44Logo.scale-200.png
    Assets/Square44x44Logo.targetsize-24_altform-unplated.png
    Assets/StoreLogo.png
    Assets/Wide310x150Logo.scale-200.png
    )

set(CONTENT_FILES
    Package.appxmanifest
    )

set(RESOURCE_FILES
  ${CONTENT_FILES} ${ASSET_FILES}
  uwp_TemporaryKey.pfx)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set_property(SOURCE ${CONTENT_FILES} PROPERTY VS_DEPLOYMENT_CONTENT 1)
set_property(SOURCE ${ASSET_FILES} PROPERTY VS_DEPLOYMENT_CONTENT 1)
set_property(SOURCE ${ASSET_FILES} PROPERTY VS_DEPLOYMENT_LOCATION "Assets")

set_property(SOURCE "App.xaml" PROPERTY VS_XAML_TYPE "ApplicationDefinition")

source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})
source_group("Resource Files" FILES ${RESOURCE_FILES})
source_group("Xaml Files" FILES ${XAML_FILES})

if (USE_FREETYPE)
  list (APPEND 3RDPARTY_DLLS "${3RDPARTY_FREETYPE_DLL}")
  set_property(SOURCE ${3RDPARTY_DLLS} PROPERTY VS_DEPLOYMENT_CONTENT 1)
endif()

add_executable(uwp WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES} ${XAML_FILES} ${3RDPARTY_DLLS})

set_property(TARGET uwp PROPERTY VS_WINRT_COMPONENT TRUE)

# Set Visual Studio Windows target platform minimum version equal to OS version
string(REGEX MATCH "^([0-9]+\.[0-9]+\.[0-9]+)$" PLATFORM_WO_DEV_VER "${CMAKE_HOST_SYSTEM_VERSION}")
if (PLATFORM_WO_DEV_VER)
  # Add tailing zero to platfrom version 10.0.14393 -> 10.0.14393.0 to have correct version
  set (CMAKE_HOST_SYSTEM_VERSION "${CMAKE_HOST_SYSTEM_VERSION}.0")
endif()
set_property(TARGET uwp PROPERTY VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION "${CMAKE_HOST_SYSTEM_VERSION}")

set_property (TARGET uwp PROPERTY FOLDER Samples)

if (SINGLE_GENERATOR)
  install (TARGETS uwp DESTINATION "${INSTALL_DIR}/${INSTALL_DIR_BIN}")
else()
  install (TARGETS uwp
           CONFIGURATIONS Release RelWithDebInfo
           DESTINATION "${INSTALL_DIR}/${INSTALL_DIR_BIN}")
  install (TARGETS uwp
           CONFIGURATIONS Debug
           DESTINATION "${INSTALL_DIR}/${INSTALL_DIR_BIN}d")
endif()

#include_directories

# OCCT libraries for using
set (uwp_USED_LIBS        TKernel
                          TKMath
                          TKG2d
                          TKG3d
                          TKGeomBase
                          TKBRep
                          TKGeomAlgo
                          TKTopAlgo
                          TKPrim
                          TKShHealing
                          TKBO
                          TKBool
                          TKFillet
                          TKMesh
                          TKFeat
                          TKHLR
                          TKOffset
                          TKXMesh
                          TKIGES
                          TKSTEP
                          TKXSBase
                          TKSTL
                          TKVRML
                          )

target_link_libraries (uwp ${uwp_USED_LIBS})
