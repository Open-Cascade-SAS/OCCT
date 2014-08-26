cmake_minimum_required (VERSION 2.6)

set (PROJECT occtdemo)

project (${PROJECT} CXX)

set( OCC_CASROOT  "" CACHE PATH "OpenCascade Root")
set( OCC_LIB_PATH "" CACHE PATH "OpenCascade LIB PATH")

set( OCCTDEMO_DIR ${OCC_CASROOT}/samples/mfc/occtdemo)

if (DEFINED MSVC70)
  SET(COMPILER vc7)
elseif (DEFINED MSVC80)
  SET(COMPILER vc8)
elseif (DEFINED MSVC90)
  SET(COMPILER vc9)
elseif (DEFINED MSVC10)
  SET(COMPILER vc10)
elseif (DEFINED MSVC11)
  SET(COMPILER vc11)
else()
  SET(COMPILER ${CMAKE_GENERATOR})
endif()


if ( CMAKE_GENERATOR MATCHES "Win64" )
   set ( BITNESS "64" )
else() 
   set ( BITNESS "32" )
endif()

set ( OCC_SAMPLES_OUTPUT_PATH 
         ${CMAKE_BINARY_DIR}/win${BITNESS}/${COMPILER}
         CACHE PATH "MFC Samples libraries path" )

# add OCC headers 
include_directories ( ${OCC_CASROOT}/inc )

# add OCC libraries
link_directories( ${OCC_LIB_PATH} )


set(CMAKE_MFC_FLAG 2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0501")
add_definitions(-D_WIN32_WINNT=0x0501)
add_definitions(/DWNT -wd4996)
add_definitions(-D_AFXDLL)
add_definitions(-D_AFXEXT)


# Common part
set (COMMON_SRC_DIR ${OCCTDEMO_DIR}/Common)
set (COMMON_WNT_DIR ${COMMON_SRC_DIR}/WNT)
set (COMMON_ISESSION_DIR ${COMMON_SRC_DIR}/ISession)

set (COMMON_SRCFILES ${COMMON_SRC_DIR}/OCCDemo_Presentation.cpp
                     ${COMMON_ISESSION_DIR}/ISession_Curve.cpp
                     ${COMMON_WNT_DIR}/MainFrm.cpp
                     ${COMMON_WNT_DIR}/OCCDemo.cpp
                     ${COMMON_WNT_DIR}/OCCDemoDoc.cpp
                     ${COMMON_WNT_DIR}/OCCDemoView.cpp
                     ${COMMON_WNT_DIR}/ResultDialog.cpp
                     ${COMMON_WNT_DIR}/StdAfx.cpp )

set (COMMON_HEADERS  ${COMMON_SRC_DIR}/OCCDemo_Presentation.h
                     ${COMMON_ISESSION_DIR}/ISession_Curve.h
                     ${COMMON_WNT_DIR}/MainFrm.h
                     ${COMMON_WNT_DIR}/OCCDemo.h
                     ${COMMON_WNT_DIR}/OCCDemoDoc.h
                     ${COMMON_WNT_DIR}/OCCDemoView.h
                     ${COMMON_WNT_DIR}/ResultDialog.h
                     ${COMMON_WNT_DIR}/StdAfx.h 
                     ${COMMON_WNT_DIR}/resource.h )

set (COMMON_RESFILES ${COMMON_WNT_DIR}/OCCDemo.rc
                     ${COMMON_WNT_DIR}/res/occ_logo.bmp
                     ${COMMON_WNT_DIR}/res/OCCDemo.ico
                     ${COMMON_WNT_DIR}/res/OCCDemo.rc2
                     ${COMMON_WNT_DIR}/res/toolbar.bmp
                     ${COMMON_WNT_DIR}/res/toolbar1.bmp )
  
# include common headers
include_directories ( ${COMMON_SRC_DIR} )
include_directories ( ${COMMON_ISESSION_DIR} )
include_directories ( ${COMMON_WNT_DIR} )


set ( PROJECTS          Approx
                        Chamfers
                        Convert
                        DCA
                        Extrema
                        GeomConstraints
                        Glue
                        GProps
                        HLR
                        Interpol
                        Intersections
                        LProps
                        Offset2d
                        PointOnCurve
                        Primitives
                        Projection
                        Simplify
                        SplitShape
                        Sweep
                        Tesselate
                        TexturesExt
                        ThruSections
                        Transform
                        Validate )

foreach (Project ${PROJECTS})
  add_subdirectory(${Project} ${CMAKE_BINARY_DIR}/${Project})
endforeach ()

