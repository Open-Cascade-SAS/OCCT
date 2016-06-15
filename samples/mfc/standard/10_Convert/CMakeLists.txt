cmake_minimum_required (VERSION 2.6)

project (Convert)

add_definitions (-DWINVER=0x0501 -DUNICODE -D_UNICODE)
set (CMAKE_MFC_FLAG 2)

set (Convert_SRC_DIR      ${MFC_STANDARD_SAMPLES_DIR}/10_Convert/src)
set (Convert_HEADER_FILES ${Convert_SRC_DIR}/Convert_Presentation.h
                          ${Convert_SRC_DIR}/OCCDemo_Presentation.h)
set (Convert_SOURCE_FILES ${Convert_SRC_DIR}/Convert_Presentation.cpp
                          ${Convert_SRC_DIR}/OCCDemo_Presentation.cpp )

set (ISESSION_SUBDIR       ${Convert_SRC_DIR}/ISession)
set (ISESSION_HEADER_FILES ${ISESSION_SUBDIR}/ISession_Curve.h)
set (ISESSION_SOURCE_FILES ${ISESSION_SUBDIR}/ISession_Curve.cpp)

set (WNT_SUBDIR       ${Convert_SRC_DIR}/WNT)
set (WNT_HEADER_FILES ${WNT_SUBDIR}/resource.h
                      ${WNT_SUBDIR}/MainFrm.h
                      ${WNT_SUBDIR}/OCCDemo.h
                      ${WNT_SUBDIR}/OCCDemoDoc.h
                      ${WNT_SUBDIR}/OCCDemoView.h
                      ${WNT_SUBDIR}/ResultDialog.h
                      ${WNT_SUBDIR}/StdAfx.h)
set (WNT_SOURCE_FILES ${WNT_SUBDIR}/MainFrm.cpp
                      ${WNT_SUBDIR}/OCCDemo.cpp
                      ${WNT_SUBDIR}/OCCDemoDoc.cpp
                      ${WNT_SUBDIR}/OCCDemoView.cpp
                      ${WNT_SUBDIR}/ResultDialog.cpp
                      ${WNT_SUBDIR}/StdAfx.cpp)

set (WNT_RESOURCE_DIR   ${WNT_SUBDIR}/res)
set (WNT_RESOURCE_FILES ${WNT_RESOURCE_DIR}/OCCDemo.ico
                        ${WNT_RESOURCE_DIR}/occlogo.bmp
                        ${WNT_RESOURCE_DIR}/toolbar.bmp
                        ${WNT_RESOURCE_DIR}/toolbar1.bmp
                        ${WNT_RESOURCE_DIR}/logopen.bmp
                        ${WNT_RESOURCE_DIR}/OCCDemo.rc2
                        ${WNT_SUBDIR}/OCCDemo.rc)

# groups in the VS solution
source_group ("Source Files" FILES ${Convert_SOURCE_FILES}
                                   ${COMMON_WINMAIN_FILE})

source_group ("Source Files\\ISession" FILES ${ISESSION_SOURCE_FILES})
source_group ("Source Files\\WNT" FILES ${WNT_SOURCE_FILES})

source_group ("Header Files" FILES ${Convert_HEADER_FILES} )
source_group ("Header Files\\ISession" FILES ${ISESSION_HEADER_FILES} )
source_group ("Header Files\\WNT" FILES ${WNT_HEADER_FILES})

source_group ("Resource Files" FILES ${WNT_RESOURCE_FILES})

add_executable (Convert WIN32 ${Convert_SOURCE_FILES}
                              ${Convert_HEADER_FILES}
                              ${ISESSION_SOURCE_FILES}
                              ${ISESSION_HEADER_FILES}
                              ${WNT_SOURCE_FILES}
                              ${WNT_HEADER_FILES}
                              ${COMMON_WINMAIN_FILE}
                              ${WNT_RESOURCE_FILES} )

set_property (TARGET Convert PROPERTY FOLDER Samples)

if (SINGLE_GENERATOR)
  install (TARGETS Convert DESTINATION "${INSTALL_DIR_BIN}")
else()
  install (TARGETS Convert
           CONFIGURATIONS Release RelWithDebInfo
           DESTINATION "${INSTALL_DIR_BIN}")
  install (TARGETS Convert
           CONFIGURATIONS Debug
           DESTINATION "${INSTALL_DIR_BIN}d")
endif()

include_directories (${CMAKE_BINARY_DIR}/inc
                     ${Convert_SRC_DIR}
                     ${ISESSION_SUBDIR}
                     ${WNT_SUBDIR}
                     ${MFC_STANDARD_SAMPLES_DIR}/Common)

target_link_libraries (Convert mfcsample)