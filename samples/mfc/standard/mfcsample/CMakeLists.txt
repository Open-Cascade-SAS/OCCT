cmake_minimum_required (VERSION 2.6)

project (mfcsample)

add_definitions(-DWINVER=0x0501 -D_AFXEXT -DUNICODE -D_UNICODE)
set (CMAKE_MFC_FLAG 2)

# mfcsample
set (mfcsample_SOURCE_FILES ${MFC_STANDARD_SAMPLES_DIR}/mfcsample/src/mfcsample.cpp
                            ${MFC_STANDARD_SAMPLES_DIR}/mfcsample/src/StdAfx.cpp )

SET (MFC_STANDARD_COMMON_SAMPLES_DIR "${MFC_STANDARD_SAMPLES_DIR}/Common")
set (COMMON_SOURCE_FILES ${MFC_STANDARD_COMMON_SAMPLES_DIR}/AISDialogs.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_2dDoc.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_2dChildFrame.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_2dView.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_3dBaseDoc.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_App.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_3dDoc.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_3dChildFrame.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_3dView.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_BaseDoc.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_BaseChildFrame.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_BaseView.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_MainFrame.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_StereoConfigDlg.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/ParamsFacesPage.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/ResultDialog.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/User_Cylinder.cxx
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/ColoredMeshDlg.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/DimensionDlg.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/LengthParamsEdgePage.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/LengthParamsVerticesPage.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/RadiusParamsPage.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/LengthParamsEdgesPage.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/LengthParamsVerticesPage.cpp
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/AngleParamsVerticesPage.cpp)

# Common ImportExport
set (COMMON_IE_DIR          ${MFC_STANDARD_COMMON_SAMPLES_DIR}/ImportExport)
set (COMMON_IE_HEADER_FILES ${COMMON_IE_DIR}/ImportExport.h
                            ${COMMON_IE_DIR}/SaveSTEPDlg.h )
set (COMMON_IE_SOURCE_FILES ${COMMON_IE_DIR}/ImportExport.cpp
                            ${COMMON_IE_DIR}/SaveSTEPDlg.cpp )

# Common ISession2D
set (COMMON_ISESSION2D_DIR          ${MFC_STANDARD_COMMON_SAMPLES_DIR}/ISession2D)
set (COMMON_ISESSION2D_HEADER_FILES ${COMMON_ISESSION2D_DIR}/ISession2D_Shape.h )
set (COMMON_ISESSION2D_SOURCE_FILES ${COMMON_ISESSION2D_DIR}/ISession2D_Shape.cpp )

# Common Primitive
set (COMMON_PRIMITIVE_DIR          ${MFC_STANDARD_COMMON_SAMPLES_DIR}/Primitive)
set (COMMON_PRIMITIVE_HEADER_FILES ${COMMON_PRIMITIVE_DIR}/Sample2D_Face.h
                                   ${COMMON_PRIMITIVE_DIR}/Sample2D_Image.h
                                   ${COMMON_PRIMITIVE_DIR}/Sample2D_Markers.h )
set (COMMON_PRIMITIVE_SOURCE_FILES ${COMMON_PRIMITIVE_DIR}/Sample2D_Face.cpp
                                   ${COMMON_PRIMITIVE_DIR}/Sample2D_Image.cpp
                                   ${COMMON_PRIMITIVE_DIR}/Sample2D_Markers.cpp )

# Common Resource2D
set (COMMON_RESOURCE2D_DIR          ${MFC_STANDARD_COMMON_SAMPLES_DIR}/Resource2D)
set (COMMON_RESOURCE2D_HEADER_FILES ${COMMON_RESOURCE2D_DIR}/CircularGrid.h
                                    ${COMMON_RESOURCE2D_DIR}/RectangularGrid.h )
set (COMMON_RESOURCE2D_SOURCE_FILES ${COMMON_RESOURCE2D_DIR}/CircularGrid.cpp
                                    ${COMMON_RESOURCE2D_DIR}/RectangularGrid.cpp )

# Common headers
set (COMMON_HEADER_FILES ${MFC_STANDARD_COMMON_SAMPLES_DIR}/AISDialogs.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_2dDoc.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_2dChildFrame.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_2dView.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_3dBaseDoc.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_App.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_3dDoc.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_3dChildFrame.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_3dView.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_BaseDoc.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_BaseChildFrame.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_BaseView.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_MainFrame.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/OCC_StereoConfigDlg.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/ParamsFacesPage.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/ResultDialog.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/User_Cylinder.hxx
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/ColoredMeshDlg.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/DimensionDlg.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/LengthParamsEdgePage.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/LengthParamsVerticesPage.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/RadiusParamsPage.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/LengthParamsEdgesPage.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/LengthParamsVerticesPage.h
                         ${MFC_STANDARD_COMMON_SAMPLES_DIR}/AngleParamsVerticesPage.h)


# Common resources
set (COMMON_RESOURCE_DIR   ${MFC_STANDARD_COMMON_SAMPLES_DIR}/res)
set (COMMON_RESOURCE_FILES ${COMMON_RESOURCE_DIR}/2dChildFrameTB.bmp
                           ${COMMON_RESOURCE_DIR}/2dType.ico
                           ${COMMON_RESOURCE_DIR}/3dChildFrameTB.bmp
                           ${COMMON_RESOURCE_DIR}/3dType.ico
                           ${COMMON_RESOURCE_DIR}/AIS_TB.bmp
                           ${COMMON_RESOURCE_DIR}/coloredm.bmp
                           ${COMMON_RESOURCE_DIR}/MainFrame.ico
                           ${COMMON_RESOURCE_DIR}/occ_logo.bmp
                           ${COMMON_RESOURCE_DIR}/OCC_Resource.rc)

# groups in the VS solution
source_group ( "Header Files" FILES   ${COMMON_HEADER_FILES}
                                      ${COMMON_IE_HEADER_FILES}
                                      ${COMMON_PRIMITIVE_HEADER_FILES}
                                      ${COMMON_RESOURCE2D_HEADER_FILES}
                                      ${COMMON_ISESSION2D_HEADER_FILES})

source_group ("Source Files" FILES    ${mfcsample_SOURCE_FILES}
                                      ${COMMON_SOURCE_FILES}
                                      ${COMMON_IE_SOURCE_FILES}
                                      ${COMMON_PRIMITIVE_SOURCE_FILES}
                                      ${COMMON_RESOURCE2D_SOURCE_FILES}
                                      ${COMMON_ISESSION2D_SOURCE_FILES})

source_group ( "Resource Files" FILES ${COMMON_RESOURCE_FILES})

# mfcsample library
add_library ( mfcsample SHARED ${mfcsample_SOURCE_FILES}
                               ${COMMON_HEADER_FILES}
                               ${COMMON_SOURCE_FILES}
                               ${COMMON_IE_HEADER_FILES}
                               ${COMMON_IE_SOURCE_FILES}
                               ${COMMON_PRIMITIVE_HEADER_FILES}
                               ${COMMON_PRIMITIVE_SOURCE_FILES}
                               ${COMMON_RESOURCE2D_HEADER_FILES}
                               ${COMMON_RESOURCE2D_SOURCE_FILES}
                               ${COMMON_ISESSION2D_HEADER_FILES}
                               ${COMMON_ISESSION2D_SOURCE_FILES}
                               ${COMMON_RESOURCE_FILES})

set_property (TARGET mfcsample PROPERTY FOLDER Samples)

if (SINGLE_GENERATOR)
  install (TARGETS mfcsample
           RUNTIME DESTINATION "${INSTALL_DIR_BIN}"
           ARCHIVE DESTINATION "${INSTALL_DIR_LIB}"
           LIBRARY DESTINATION "${INSTALL_DIR_LIB}")
else()
  install (TARGETS mfcsample
           CONFIGURATIONS Release RelWithDebInfo
           RUNTIME DESTINATION "${INSTALL_DIR_BIN}"
           ARCHIVE DESTINATION "${INSTALL_DIR_LIB}"
           LIBRARY DESTINATION "${INSTALL_DIR_LIB}")
  install (TARGETS mfcsample
           CONFIGURATIONS Debug
           RUNTIME DESTINATION "${INSTALL_DIR_BIN}d"
           ARCHIVE DESTINATION "${INSTALL_DIR_LIB}d"
           LIBRARY DESTINATION "${INSTALL_DIR_LIB}d")
endif()

include_directories( ${CMAKE_BINARY_DIR}/inc
                     ${MFC_STANDARD_COMMON_SAMPLES_DIR}
                     ${COMMON_IE_DIR}
                     ${COMMON_ISESSION2D_DIR}
                     ${COMMON_PRIMITIVE_DIR}
                     ${COMMON_RESOURCE2D_DIR})

# OCCT libraries for using
set (mfcsample_USED_LIBS  TKVRML
                          TKSTL
                          TKBRep
                          TKIGES
                          TKShHealing
                          TKSTEP
                          TKXSBase
                          TKBool
                          TKCAF
                          TKCDF
                          TKernel
                          TKFeat
                          TKFillet
                          TKG2d
                          TKG3d
                          TKGeomAlgo
                          TKGeomBase
                          TKHLR
                          TKMath
                          TKOffset
                          TKOpenGl
                          TKPrim
                          TKService
                          TKTopAlgo
                          TKMesh
                          TKV3d)

target_link_libraries (mfcsample ${mfcsample_USED_LIBS})