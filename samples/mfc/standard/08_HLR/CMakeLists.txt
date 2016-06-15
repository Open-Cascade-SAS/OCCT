cmake_minimum_required (VERSION 2.6)

project (HLR)

add_definitions(-DWINVER=0x0501 -DUNICODE -D_UNICODE)
set (CMAKE_MFC_FLAG 2)

set (HLR_SRC_DIR      ${MFC_STANDARD_SAMPLES_DIR}/08_HLR/src)
set (HLR_HEADER_FILES ${HLR_SRC_DIR}/HLRApp.h
                      ${HLR_SRC_DIR}/HLRDoc.h
                      ${HLR_SRC_DIR}/HLRView2D.h
                      ${HLR_SRC_DIR}/SelectionDialog.h
                      ${HLR_SRC_DIR}/StdAfx.h )
set (HLR_SOURCE_FILES ${HLR_SRC_DIR}/HLRApp.cpp
                      ${HLR_SRC_DIR}/HLRDoc.cpp
                      ${HLR_SRC_DIR}/HLRView2D.cpp
                      ${HLR_SRC_DIR}/SelectionDialog.cpp
                      ${HLR_SRC_DIR}/StdAfx.cpp )

set (HLR_RESOURCE_DIR    ${MFC_STANDARD_SAMPLES_DIR}/08_HLR/res)
set (HLR_RESOURCE_HEADER ${HLR_RESOURCE_DIR}/resource.h)
set (HLR_RESOURCE_FILES  ${HLR_RESOURCE_DIR}/axoviewd.bmp
                         ${HLR_RESOURCE_DIR}/axoviewf.bmp
                         ${HLR_RESOURCE_DIR}/AXOVIEWU.bmp
                         ${HLR_RESOURCE_DIR}/BACKVIEWD.bmp
                         ${HLR_RESOURCE_DIR}/BACKVIEWF.bmp
                         ${HLR_RESOURCE_DIR}/BACKVIEWU.bmp
                         ${HLR_RESOURCE_DIR}/BOTTOMVIEWD.bmp
                         ${HLR_RESOURCE_DIR}/BOTTOMVIEWF.bmp
                         ${HLR_RESOURCE_DIR}/BOTTOMVIEWU.bmp
                         ${HLR_RESOURCE_DIR}/ChildFrame2D.bmp
                         ${HLR_RESOURCE_DIR}/ChildFrame3D.bmp
                         ${HLR_RESOURCE_DIR}/FRONTVIEWD.bmp
                         ${HLR_RESOURCE_DIR}/FRONTVIEWF.bmp
                         ${HLR_RESOURCE_DIR}/FRONTVIEWU.bmp
                         ${HLR_RESOURCE_DIR}/LEFTVIEWD.bmp
                         ${HLR_RESOURCE_DIR}/LEFTVIEWF.bmp
                         ${HLR_RESOURCE_DIR}/LEFTVIEWU.bmp
                         ${HLR_RESOURCE_DIR}/RIGHTVIEWD.bmp
                         ${HLR_RESOURCE_DIR}/RIGHTVIEWF.bmp
                         ${HLR_RESOURCE_DIR}/RIGHTVIEWU.bmp
                         ${HLR_RESOURCE_DIR}/Toolbar.bmp
                         ${HLR_RESOURCE_DIR}/ToolBarObjects.bmp
                         ${HLR_RESOURCE_DIR}/topviewd.bmp
                         ${HLR_RESOURCE_DIR}/topviewf.bmp
                         ${HLR_RESOURCE_DIR}/topviewu.bmp
                         ${HLR_RESOURCE_DIR}/HLR.rc)

# groups in the VS solution
source_group ("Source Files" FILES ${HLR_SOURCE_FILES}
                                   ${COMMON_WINMAIN_FILE})

source_group ("Header Files" FILES ${HLR_HEADER_FILES}
                                   ${HLR_RESOURCE_HEADER})

source_group ("Resource Files" FILES ${HLR_RESOURCE_FILES})

add_executable (HLR WIN32 ${HLR_SOURCE_FILES}
                          ${HLR_HEADER_FILES}
                          ${COMMON_WINMAIN_FILE}
                          ${HLR_RESOURCE_HEADER}
                          ${HLR_RESOURCE_FILES} )

set_property (TARGET HLR PROPERTY FOLDER Samples)

if (SINGLE_GENERATOR)
  install (TARGETS HLR DESTINATION "${INSTALL_DIR_BIN}")
else()
  install (TARGETS HLR
           CONFIGURATIONS Release RelWithDebInfo
           DESTINATION "${INSTALL_DIR_BIN}")
  install (TARGETS HLR
           CONFIGURATIONS Debug
           DESTINATION "${INSTALL_DIR_BIN}d")
endif()

include_directories (${CMAKE_BINARY_DIR}/inc
                     ${HLR_RESOURCE_DIR}
                     ${HLR_SRC_DIR}
                     ${MFC_STANDARD_SAMPLES_DIR}/Common)

target_link_libraries (HLR mfcsample)