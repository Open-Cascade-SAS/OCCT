cmake_minimum_required (VERSION 2.6)

project (Animation)

add_definitions (-DWINVER=0x0501 -DUNICODE -D_UNICODE)
set (CMAKE_MFC_FLAG 2)

set (Animation_SRC_DIR      ${MFC_STANDARD_SAMPLES_DIR}/09_Animation/src)
set (Animation_HEADER_FILES ${Animation_SRC_DIR}/AnimationApp.h
                            ${Animation_SRC_DIR}/AnimationDoc.h
                            ${Animation_SRC_DIR}/AnimationView3D.h
                            ${Animation_SRC_DIR}/Fonc.hxx
                            ${Animation_SRC_DIR}/Sensitivity.h
                            ${Animation_SRC_DIR}/ShadingDialog.h
                            ${Animation_SRC_DIR}/ThreadDialog.h
                            ${Animation_SRC_DIR}/Tune.h
                            ${Animation_SRC_DIR}/StdAfx.h )
set (Animation_SOURCE_FILES ${Animation_SRC_DIR}/AnimationApp.cpp
                            ${Animation_SRC_DIR}/AnimationDoc.cpp
                            ${Animation_SRC_DIR}/AnimationView3D.cpp
                            ${Animation_SRC_DIR}/Fonc.cxx
                            ${Animation_SRC_DIR}/Sensitivity.cpp
                            ${Animation_SRC_DIR}/ShadingDialog.cpp
                            ${Animation_SRC_DIR}/ThreadDialog.cpp
                            ${Animation_SRC_DIR}/Tune.cpp
                            ${Animation_SRC_DIR}/StdAfx.cpp )

set (Animation_RESOURCE_DIR    ${MFC_STANDARD_SAMPLES_DIR}/09_Animation/res)
set (Animation_RESOURCE_HEADER ${Animation_RESOURCE_DIR}/resource.h)
set (Animation_RESOURCE_FILES  ${Animation_RESOURCE_DIR}/child2.bmp
                               ${Animation_RESOURCE_DIR}/Toolbar.bmp
                               ${Animation_RESOURCE_DIR}/Animation.rc)

# groups in the VS solution
source_group ("Source Files"   FILES ${Animation_SOURCE_FILES}
                                     ${COMMON_WINMAIN_FILE})

source_group ("Header Files"   FILES ${Animation_HEADER_FILES})

source_group ("Resource Files" FILES ${Animation_RESOURCE_FILES})

add_executable (Animation WIN32 ${Animation_SOURCE_FILES}
                                ${Animation_HEADER_FILES}
                                ${COMMON_WINMAIN_FILE}
                                ${Animation_RESOURCE_FILES})

set_property (TARGET Animation PROPERTY FOLDER Samples)

if (SINGLE_GENERATOR)
  install (TARGETS Animation DESTINATION "${INSTALL_DIR_BIN}")
else()
  install (TARGETS Animation
           CONFIGURATIONS Release RelWithDebInfo
           DESTINATION "${INSTALL_DIR_BIN}")
  install (TARGETS Animation
           CONFIGURATIONS Debug
           DESTINATION "${INSTALL_DIR_BIN}d")
endif()

include_directories (${CMAKE_BINARY_DIR}/inc
                     ${Animation_RESOURCE_DIR}
                     ${Animation_SRC_DIR}
                     ${MFC_STANDARD_SAMPLES_DIR}/Common)

target_link_libraries (Animation mfcsample TKBO)