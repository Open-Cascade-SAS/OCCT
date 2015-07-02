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

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
