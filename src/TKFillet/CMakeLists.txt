project(TKFillet)

set (TOOLKIT_MODULES
  ChFiDS
  ChFi2d
  ChFi3d
  ChFiKPart
  Blend
  BRepBlend
  BlendFunc
  BRepFilletAPI
  FilletSurf
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
