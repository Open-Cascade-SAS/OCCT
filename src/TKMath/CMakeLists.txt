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

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
