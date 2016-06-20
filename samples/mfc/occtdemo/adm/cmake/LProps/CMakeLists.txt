cmake_minimum_required (VERSION 2.6)

set (PROJECT LProps)
project (${PROJECT} CXX)

set (PROJECT_DIR ${OCCTDEMO_DIR}/${PROJECT})


# List of used sources
set (USED_SRCFILES 
         ${PROJECT_DIR}/${PROJECT}_Presentation.cpp )

# List of used headers
set (USED_HEADERFILES         
         ${PROJECT_DIR}/${PROJECT}_Presentation.h )


# include headers
include_directories( ${PROJECT_DIR} )

# Specify folders in the VS solution
source_group ( "Source Files" FILES                      
                     ${USED_SRCFILES} )

source_group ( "Header Files" FILES                      
                     ${USED_HEADERFILES} )

source_group ( "Common\\Source Files" FILES                      
                     ${COMMON_SRCFILES} )

source_group ( "Common\\Header Files" FILES                      
                     ${COMMON_HEADERS} )

source_group ( "Common\\Resource Files" FILES 
                     ${COMMON_RESFILES})

# lib to include:
set ( LINK_LIBRARIES         
         TKBool.lib
         TKCAF.lib
         TKCDF.lib
         TKDraw.lib
         TKernel.lib
         TKFeat.lib
         TKFillet.lib
         TKGeomAlgo.lib
         TKHLR.lib
         TKMath.lib
         TKOffset.lib
         TKPCAF.lib
         TKPrim.lib
         TKPShape.lib
         TKService.lib
         TKTopAlgo.lib
         TKV3d.lib
         TKBRep.lib
         TKG2d.lib
         TKG3d.lib
         TKGeomBase.lib
         TKBO.lib)

add_executable ( ${PROJECT} WIN32 
                            ${USED_SRCFILES}
                            ${COMMON_SRCFILES}
                            ${USED_HEADERFILES}
                            ${COMMON_HEADERS}
                            ${COMMON_RESFILES} )

target_link_libraries( ${PROJECT} "${LINK_LIBRARIES}" )

set_target_properties( ${PROJECT} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OCC_SAMPLES_OUTPUT_PATH}/bind 
ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OCC_SAMPLES_OUTPUT_PATH}/bind 
LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OCC_SAMPLES_OUTPUT_PATH}/bind 
RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OCC_SAMPLES_OUTPUT_PATH}/bind 
ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OCC_SAMPLES_OUTPUT_PATH}/bind 
LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OCC_SAMPLES_OUTPUT_PATH}/bind )

