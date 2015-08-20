project(TKMath)

set (TOOLKIT_MODULES
  math
  ElCLib
  ElSLib
  BSplCLib
  BSplSLib
  PLib
  Precision
  GeomAbs
  Poly
  CSLib
  Convert
  Bnd
  BVH
  gp
  TColgp
  TopLoc
  Expr
  ExprIntrp
)

include(${OCCT_SOURCE_DIR}/adm/cmake/occt_toolkit.cmake)
