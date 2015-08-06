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

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
