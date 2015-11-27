cmake_minimum_required (VERSION 2.6)

project (ImportExport)

add_definitions (-DWINVER=0x0501 -DUNICODE -D_UNICODE)
set (CMAKE_MFC_FLAG 2)

set (ImportExport_SRC_DIR      ${MFC_STANDARD_SAMPLES_DIR}/05_ImportExport/src)
set (ImportExport_HEADER_FILES ${ImportExport_SRC_DIR}/ColoredShapes.h
                               ${ImportExport_SRC_DIR}/ImportExportApp.h
                               ${ImportExport_SRC_DIR}/ImportExportDoc.h
                               ${ImportExport_SRC_DIR}/StdAfx.h )
set (ImportExport_SOURCE_FILES ${ImportExport_SRC_DIR}/ColoredShapes.cpp
                               ${ImportExport_SRC_DIR}/ImportExportApp.cpp
                               ${ImportExport_SRC_DIR}/ImportExportDoc.cpp
                               ${ImportExport_SRC_DIR}/StdAfx.cpp)

set (ImportExport_RESOURCE_DIR    ${MFC_STANDARD_SAMPLES_DIR}/05_ImportExport/res)
set (ImportExport_RESOURCE_HEADER ${ImportExport_RESOURCE_DIR}/resource.h)
set (ImportExport_RESOURCE_FILES  ${ImportExport_RESOURCE_DIR}/Toolbar.bmp
                                  ${ImportExport_RESOURCE_DIR}/ImportExport.rc)

# groups in the VS solution
source_group ("Source Files" FILES ${ImportExport_SOURCE_FILES}
                                   ${COMMON_WINMAIN_FILE})

source_group ("Header Files" FILES ${ImportExport_HEADER_FILES}
                                   ${ImportExport_RESOURCE_HEADER})

source_group ("Resource Files" FILES ${ImportExport_RESOURCE_FILES})

add_executable (ImportExport WIN32 ${ImportExport_SOURCE_FILES}
                                   ${ImportExport_HEADER_FILES}
                                   ${COMMON_WINMAIN_FILE}
                                   ${ImportExport_RESOURCE_HEADER}
                                   ${ImportExport_RESOURCE_FILES})

set_property (TARGET ImportExport PROPERTY FOLDER Samples)

install (TARGETS ImportExport
         CONFIGURATIONS Release
         DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
install (TARGETS ImportExport
         CONFIGURATIONS RelWithDebInfo
         DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
install (TARGETS ImportExport
         CONFIGURATIONS Debug
         DESTINATION "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")

include_directories (${CMAKE_BINARY_DIR}/inc
                     ${MFC_STANDARD_SAMPLES_DIR}/05_ImportExport
                     ${ImportExport_SRC_DIR}
                     ${MFC_STANDARD_SAMPLES_DIR}/Common)

target_link_libraries (ImportExport mfcsample)