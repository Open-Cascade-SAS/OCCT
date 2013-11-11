cmake_minimum_required (VERSION 2.6)

project (Modeling)

add_definitions(-DWINVER=0x0500)

set (Modeling_SRC_DIR      ${MFC_STANDARD_SAMPLES_DIR}/02_Modeling/src)
set (Modeling_HEADER_FILES ${Modeling_SRC_DIR}/ISession_Direction.h
                           ${Modeling_SRC_DIR}/ModelingApp.h
                           ${Modeling_SRC_DIR}/ModelingDoc.h
                           ${Modeling_SRC_DIR}/StdAfx.h
                           ${Modeling_SRC_DIR}/State.h )
set (Modeling_SOURCE_FILES ${Modeling_SRC_DIR}/ISession_Direction.cpp
                           ${Modeling_SRC_DIR}/ModelingApp.cpp
                           ${Modeling_SRC_DIR}/ModelingDoc.cpp
                           ${Modeling_SRC_DIR}/StdAfx.cpp )

set (Modeling_RESOURCE_DIR    ${MFC_STANDARD_SAMPLES_DIR}/02_Modeling/res)
set (Modeling_RESOURCE_HEADER ${Modeling_RESOURCE_DIR}/resource.h)
set (Modeling_RESOURCE_FILES  ${Modeling_RESOURCE_DIR}/fram2.bmp
                              ${Modeling_RESOURCE_DIR}/Toolbar.bmp
                              ${Modeling_RESOURCE_DIR}/Modeling.rc)

# groups in the VS solution
source_group ("Source Files" FILES ${Modeling_SOURCE_FILES}
                                   ${COMMON_WINMAIN_FILE})

source_group ("Header Files" FILES ${Modeling_HEADER_FILES}
                                   ${Modeling_RESOURCE_HEADER})

source_group ("Resource Files" FILES ${Modeling_RESOURCE_FILES})

add_executable ( Modeling WIN32 ${Modeling_SOURCE_FILES}
                                ${Modeling_HEADER_FILES}
                                ${COMMON_WINMAIN_FILE}
                                ${Modeling_RESOURCE_HEADER}
                                ${Modeling_RESOURCE_FILES})

set_property (TARGET Modeling PROPERTY FOLDER Samples)

install (TARGETS Modeling RUNTIME DESTINATION "${INSTALL_DIR}/bin"
                          ARCHIVE DESTINATION "${INSTALL_DIR}/lib"
                          LIBRARY DESTINATION "${INSTALL_DIR}/lib")

include_directories (${OCCT_ROOT}/inc
                     ${MFC_STANDARD_SAMPLES_DIR}/Common
                     ${MFC_STANDARD_SAMPLES_DIR}/02_Modeling
                     ${Modeling_SRC_DIR})

target_link_libraries (Modeling mfcsample TKSTEP209 TKSTEPAttr TKSTEPBase TKBO)
