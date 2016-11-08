cmake_minimum_required (VERSION 2.6)

project (Geometry)

add_definitions(-DWINVER=0x0501 -DUNICODE -D_UNICODE)
set (CMAKE_MFC_FLAG 2)

set (Geometry_SRC_DIR      ${MFC_STANDARD_SAMPLES_DIR}/01_Geometry/src)
set (Geometry_HEADER_FILES ${Geometry_SRC_DIR}/ChildFrm.h
                           ${Geometry_SRC_DIR}/ChildFrm2d.h
                           ${Geometry_SRC_DIR}/GeoAlgo_Sol.hxx
                           ${Geometry_SRC_DIR}/GeometryApp.h
                           ${Geometry_SRC_DIR}/GeometryDoc.h
                           ${Geometry_SRC_DIR}/GeometryView.h
                           ${Geometry_SRC_DIR}/GeometryView2D.h
                           ${Geometry_SRC_DIR}/GeomSources.h
                           ${Geometry_SRC_DIR}/MainFrm.h
                           ${Geometry_SRC_DIR}/StdAfx.h )
set (Geometry_SOURCE_FILES ${Geometry_SRC_DIR}/ChildFrm.cpp
                           ${Geometry_SRC_DIR}/ChildFrm2d.cpp
                           ${Geometry_SRC_DIR}/GeoAlgo_Sol.cxx
                           ${Geometry_SRC_DIR}/GeometryApp.cpp
                           ${Geometry_SRC_DIR}/GeometryDoc.cpp
                           ${Geometry_SRC_DIR}/GeometryView.cpp
                           ${Geometry_SRC_DIR}/GeometryView2D.cpp
                           ${Geometry_SRC_DIR}/GeomSources.cpp
                           ${Geometry_SRC_DIR}/MainFrm.cpp
                           ${Geometry_SRC_DIR}/StdAfx.cpp )

set (Geometry_ISESSION2D_DIR          ${Geometry_SRC_DIR}/ISession2D)
set (Geometry_ISESSION2D_HEADER_FILES ${Geometry_ISESSION2D_DIR}/ISession_Curve.h
                                      ${Geometry_ISESSION2D_DIR}/ISession_Direction.h
                                      ${Geometry_ISESSION2D_DIR}/ISession_Point.h
                                      ${Geometry_ISESSION2D_DIR}/ISession_Surface.h
                                      ${Geometry_ISESSION2D_DIR}/ISession2D_Curve.h)
set (Geometry_ISESSION2D_SOURCE_FILES ${Geometry_ISESSION2D_DIR}/ISession_Curve.cpp
                                      ${Geometry_ISESSION2D_DIR}/ISession_Direction.cpp
                                      ${Geometry_ISESSION2D_DIR}/ISession_Point.cpp
                                      ${Geometry_ISESSION2D_DIR}/ISession_Surface.cpp
                                      ${Geometry_ISESSION2D_DIR}/ISession2D_Curve.cpp)

set (Geometry_RESOURCE_DIR            ${MFC_STANDARD_SAMPLES_DIR}/01_Geometry/res)
set (Geometry_RESOURCE_HEADER         ${Geometry_RESOURCE_DIR}/resource.h)
set (Geometry_RESOURCE_FILES          ${Geometry_RESOURCE_DIR}/Geometry.rc
                                      ${Geometry_RESOURCE_DIR}/Toolbar1.bmp
                                      ${Geometry_RESOURCE_DIR}/Toolbar2.bmp)

# groups in the VS solution
source_group ("Source Files" FILES ${Geometry_SOURCE_FILES}
                                   ${COMMON_WINMAIN_FILE})

source_group ("Source Files\\ISession2d" FILES ${Geometry_ISESSION2D_SOURCE_FILES})

source_group ("Header Files" FILES ${Geometry_HEADER_FILES}
                                   ${Geometry_ISESSION2D_HEADER_FILES}
                                   ${Geometry_RESOURCE_HEADER})

source_group ("Resource Files" FILES ${Geometry_RESOURCE_FILES})

add_executable (Geometry WIN32 ${Geometry_SOURCE_FILES}
                               ${Geometry_HEADER_FILES}
                               ${COMMON_WINMAIN_FILE}
                               ${Geometry_ISESSION2D_SOURCE_FILES}
                               ${Geometry_ISESSION2D_HEADER_FILES}
                               ${Geometry_RESOURCE_FILES}
                               ${Geometry_RESOURCE_HEADER})

set_property(TARGET Geometry PROPERTY FOLDER Samples)

if (SINGLE_GENERATOR)
  install (TARGETS Geometry DESTINATION "${INSTALL_DIR_BIN}")
else()
  install (TARGETS Geometry
           CONFIGURATIONS Release RelWithDebInfo
           DESTINATION "${INSTALL_DIR_BIN}")
  install (TARGETS Geometry
           CONFIGURATIONS Debug
           DESTINATION "${INSTALL_DIR_BIN}d")
endif()

include_directories (${CMAKE_BINARY_DIR}/inc
                     ${Geometry_SRC_DIR}
                     ${Geometry_ISESSION2D_DIR}
                     ${Geometry_RESOURCE_DIR}
                     ${MFC_STANDARD_SAMPLES_DIR}/Common)

target_link_libraries (Geometry mfcsample)