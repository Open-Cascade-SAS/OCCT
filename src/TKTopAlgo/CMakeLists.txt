project(TKTopAlgo)

set (TOOLKIT_MODULES
  IntCurvesFace
  MAT
  MAT2d
  Bisector
  BRepMAT2d
  BRepCheck
  BRepBndLib
  BRepExtrema
  BRepClass
  BRepClass3d
  BRepLib
  BRepGProp
  BRepIntCurveSurface
  BRepTopAdaptor
  BRepBuilderAPI
  BRepApprox
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
