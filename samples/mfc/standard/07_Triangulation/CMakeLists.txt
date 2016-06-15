cmake_minimum_required (VERSION 2.6)

project (Triangulation)

add_definitions (-DWINVER=0x0501 -DUNICODE -D_UNICODE)
set (CMAKE_MFC_FLAG 2)

set (Triangulation_SRC_DIR      ${MFC_STANDARD_SAMPLES_DIR}/07_Triangulation/src)
set (Triangulation_HEADER_FILES ${Triangulation_SRC_DIR}/ISession_Curve.h
                                ${Triangulation_SRC_DIR}/OCCDemo_Presentation.h
                                ${Triangulation_SRC_DIR}/Tesselate_Presentation.h
                                ${Triangulation_SRC_DIR}/TriangulationApp.h
                                ${Triangulation_SRC_DIR}/TriangulationDoc.h
                                ${Triangulation_SRC_DIR}/StdAfx.h )
set (Triangulation_SOURCE_FILES ${Triangulation_SRC_DIR}/ISession_Curve.cpp
                                ${Triangulation_SRC_DIR}/OCCDemo_Presentation.cpp
                                ${Triangulation_SRC_DIR}/Tesselate_Presentation.cpp
                                ${Triangulation_SRC_DIR}/TriangulationApp.cpp
                                ${Triangulation_SRC_DIR}/TriangulationDoc.cpp
                                ${Triangulation_SRC_DIR}/StdAfx.cpp)

set (Triangulation_RESOURCE_DIR    ${MFC_STANDARD_SAMPLES_DIR}/07_Triangulation/res)
set (Triangulation_RESOURCE_HEADER ${Triangulation_RESOURCE_DIR}/resource.h)
set (Triangulation_RESOURCE_FILES  ${Triangulation_RESOURCE_DIR}/Toolbar.bmp
                                   ${Triangulation_RESOURCE_DIR}/Triangulation.rc)

# groups in the VS solution
source_group ("Source Files" FILES ${Triangulation_SOURCE_FILES}
                                   ${COMMON_WINMAIN_FILE})

source_group ("Header Files" FILES ${Triangulation_HEADER_FILES}
                                   ${Triangulation_RESOURCE_HEADER})

source_group ("Resource Files" FILES ${Triangulation_RESOURCE_FILES})

add_executable (Triangulation WIN32 ${Triangulation_SOURCE_FILES}
                                    ${Triangulation_HEADER_FILES}
                                    ${COMMON_WINMAIN_FILE}
                                    ${Triangulation_RESOURCE_HEADER}
                                    ${Triangulation_RESOURCE_FILES})

set_property (TARGET Triangulation PROPERTY FOLDER Samples)

if (SINGLE_GENERATOR)
  install (TARGETS Triangulation DESTINATION "${INSTALL_DIR_BIN}")
else()
  install (TARGETS Triangulation
           CONFIGURATIONS Release RelWithDebInfo
           DESTINATION "${INSTALL_DIR_BIN}")
  install (TARGETS Triangulation
           CONFIGURATIONS Debug
           DESTINATION "${INSTALL_DIR_BIN}d")
endif()

include_directories (${CMAKE_BINARY_DIR}/inc
                     ${MFC_STANDARD_SAMPLES_DIR}/07_Triangulation
                     ${Triangulation_SRC_DIR}
                     ${MFC_STANDARD_SAMPLES_DIR}/Common)

target_link_libraries (Triangulation mfcsample TKSTEP209 TKSTEPAttr TKBO)