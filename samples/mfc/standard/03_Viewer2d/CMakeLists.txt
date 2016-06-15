cmake_minimum_required (VERSION 2.6)

project (Viewer2d)

add_definitions (-DWINVER=0x0501 -DUNICODE -D_UNICODE)
set (CMAKE_MFC_FLAG 2)

set (Viewer2d_SOURCE_DIR   ${MFC_STANDARD_SAMPLES_DIR}/03_Viewer2d/src)
set (Viewer2d_HEADER_FILES ${Viewer2d_SOURCE_DIR}/Viewer2dApp.h
                           ${Viewer2d_SOURCE_DIR}/Viewer2dDoc.h
                           ${Viewer2d_SOURCE_DIR}/StdAfx.h )
set (Viewer2d_SOURCE_FILES ${Viewer2d_SOURCE_DIR}/Viewer2dApp.cpp
                           ${Viewer2d_SOURCE_DIR}/Viewer2dDoc.cpp
                           ${Viewer2d_SOURCE_DIR}/StdAfx.cpp )

set (Viewer2d_RESOURCE_DIR    ${MFC_STANDARD_SAMPLES_DIR}/03_Viewer2d/res)
set (Viewer2d_RESOURCE_HEADER ${Viewer2d_RESOURCE_DIR}/resource.h)
set (Viewer2d_RESOURCE_FILES  ${Viewer2d_RESOURCE_DIR}/Toolbar.bmp
                              ${Viewer2d_RESOURCE_DIR}/Viewer2d.rc)

# groups in the VS solution
source_group ("Source Files" FILES ${Viewer2d_SOURCE_FILES}
                                   ${COMMON_WINMAIN_FILE})

source_group ("Header Files" FILES ${Viewer2d_HEADER_FILES}
                                   ${Viewer2d_RESOURCE_HEADER})

source_group ("Resource Files" FILES ${Viewer2d_RESOURCE_FILES})

add_executable (Viewer2d WIN32 ${Viewer2d_SOURCE_FILES}
                               ${Viewer2d_HEADER_FILES}
                               ${COMMON_WINMAIN_FILE}
                               ${Viewer2d_RESOURCE_HEADER}
                               ${Viewer2d_RESOURCE_FILES})

set_property (TARGET Viewer2d PROPERTY FOLDER Samples)

if (SINGLE_GENERATOR)
  install (TARGETS Viewer2d DESTINATION "${INSTALL_DIR_BIN}")
else()
  install (TARGETS Viewer2d
           CONFIGURATIONS Release RelWithDebInfo
           DESTINATION "${INSTALL_DIR_BIN}")
  install (TARGETS Viewer2d
           CONFIGURATIONS Debug
           DESTINATION "${INSTALL_DIR_BIN}d")
endif()

include_directories (${CMAKE_BINARY_DIR}/inc
                     ${MFC_STANDARD_SAMPLES_DIR}/03_Viewer2d
                     ${Viewer2d_SOURCE_DIR}
                     ${MFC_STANDARD_SAMPLES_DIR}/Common)

target_link_libraries (Viewer2d mfcsample)