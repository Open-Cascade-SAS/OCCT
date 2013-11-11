cmake_minimum_required (VERSION 2.6)

project (Viewer3d)

add_definitions (-DWINVER=0x0500)

set (Viewer3d_SRC_DIR      ${MFC_STANDARD_SAMPLES_DIR}/04_Viewer3d/src)
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
                           ${Viewer3d_SRC_DIR}/ZClippingDlg.h
                           ${Viewer3d_SRC_DIR}/ZCueingDlg.h
                           ${Viewer3d_SRC_DIR}/State.h
                           ${Viewer3d_SRC_DIR}/resource.h
                           ${Viewer3d_SRC_DIR}/resource.hm)
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
                           ${Viewer3d_SRC_DIR}/Viewer3dView.cpp
                           ${Viewer3d_SRC_DIR}/ZClippingDlg.cpp
                           ${Viewer3d_SRC_DIR}/ZCueingDlg.cpp)

set (Viewer3d_RESOURCE_DIR   ${MFC_STANDARD_SAMPLES_DIR}/04_Viewer3d/res)
set (Viewer3d_RESOURCE_FILES ${Viewer3d_SRC_DIR}/Viewer3d.rc
                             ${Viewer3d_SRC_DIR}/AISToolbar.bmp
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

install (TARGETS Viewer3d RUNTIME DESTINATION "${INSTALL_DIR}/bin"
                          ARCHIVE DESTINATION "${INSTALL_DIR}/lib"
                          LIBRARY DESTINATION "${INSTALL_DIR}/lib")

include_directories (${OCCT_ROOT}/inc
                     ${MFC_STANDARD_SAMPLES_DIR}/Common
                     ${Viewer3d_SRC_DIR})

target_link_libraries (Viewer3d mfcsample)