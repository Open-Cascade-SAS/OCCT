cmake_minimum_required (VERSION 2.6)

project (Viewer3d)

add_definitions (-DWINVER=0x0501 -DUNICODE -D_UNICODE)
set (CMAKE_MFC_FLAG 2)

set (Viewer3d_SRC_DIR      ${MFC_STANDARD_SAMPLES_DIR}/04_Viewer3d/src)
set (Viewer3d_RESOURCE_DIR ${MFC_STANDARD_SAMPLES_DIR}/04_Viewer3d/res)
set (Viewer3d_HEADER_FILES ${Viewer3d_SRC_DIR}/BoxRadius.h
                           ${Viewer3d_SRC_DIR}/DlgIsos.h
                           ${Viewer3d_SRC_DIR}/ISession_Curve.h
                           ${Viewer3d_SRC_DIR}/ModelClippingDlg.h
                           ${Viewer3d_SRC_DIR}/OCCDemo_Presentation.h
                           ${Viewer3d_SRC_DIR}/OffsetDlg.h
                           ${Viewer3d_SRC_DIR}/ScaleDlg.h
                           ${Viewer3d_SRC_DIR}/ShadingModelDlg.h
                           ${Viewer3d_SRC_DIR}/StdAfx.h
                           ${Viewer3d_SRC_DIR}/TexturesExt_Presentation.h
                           ${Viewer3d_SRC_DIR}/TrihedronDlg.h
                           ${Viewer3d_SRC_DIR}/Viewer3dApp.h
                           ${Viewer3d_SRC_DIR}/Viewer3dDoc.h
                           ${Viewer3d_SRC_DIR}/Viewer3dView.h
                           ${Viewer3d_SRC_DIR}/State.h
                           ${Viewer3d_RESOURCE_DIR}/resource.h
                           ${Viewer3d_RESOURCE_DIR}/resource.hm)
set (Viewer3d_SOURCE_FILES ${Viewer3d_SRC_DIR}/BoxRadius.cpp
                           ${Viewer3d_SRC_DIR}/DlgIsos.cpp
                           ${Viewer3d_SRC_DIR}/ISession_Curve.cpp
                           ${Viewer3d_SRC_DIR}/ModelClippingDlg.cpp
                           ${Viewer3d_SRC_DIR}/OCCDemo_Presentation.cpp
                           ${Viewer3d_SRC_DIR}/OffsetDlg.cpp
                           ${Viewer3d_SRC_DIR}/ScaleDlg.cpp
                           ${Viewer3d_SRC_DIR}/ShadingModelDlg.cpp
                           ${Viewer3d_SRC_DIR}/StdAfx.cpp
                           ${Viewer3d_SRC_DIR}/TexturesExt_Presentation.cpp
                           ${Viewer3d_SRC_DIR}/TrihedronDlg.cpp
                           ${Viewer3d_SRC_DIR}/Viewer3dApp.cpp
                           ${Viewer3d_SRC_DIR}/Viewer3dDoc.cpp
                           ${Viewer3d_SRC_DIR}/Viewer3dView.cpp)

set (Viewer3d_RESOURCE_FILES ${Viewer3d_RESOURCE_DIR}/Viewer3d.rc
                             ${Viewer3d_RESOURCE_DIR}/AISToolbar.bmp
                             ${Viewer3d_RESOURCE_DIR}/AIS_TB.bmp
                             ${Viewer3d_RESOURCE_DIR}/Toolbar.bmp)

# groups in the VS solution
source_group ("Source Files" FILES ${Viewer3d_SOURCE_FILES}
                                   ${COMMON_WINMAIN_FILE})

source_group ("Header Files" FILES ${Viewer3d_HEADER_FILES})

source_group ("Resource Files" FILES ${Viewer3d_RESOURCE_FILES})

add_executable (Viewer3d WIN32 ${Viewer3d_SOURCE_FILES}
                               ${Viewer3d_HEADER_FILES}
                               ${COMMON_WINMAIN_FILE}
                               ${Viewer3d_RESOURCE_FILES})

set_property (TARGET Viewer3d PROPERTY FOLDER Samples)

if (SINGLE_GENERATOR)
  install (TARGETS Viewer3d DESTINATION "${INSTALL_DIR_BIN}")
else()
  install (TARGETS Viewer3d
           CONFIGURATIONS Release RelWithDebInfo
           DESTINATION "${INSTALL_DIR_BIN}")
  install (TARGETS Viewer3d
           CONFIGURATIONS Debug
           DESTINATION "${INSTALL_DIR_BIN}d")
endif()

include_directories (${CMAKE_BINARY_DIR}/inc
                     ${Viewer3d_SRC_DIR}
                     ${MFC_STANDARD_SAMPLES_DIR}/Common)

target_link_libraries (Viewer3d mfcsample)