project(TKTopTest)

set (TOOLKIT_MODULES
  TestTopOpeDraw
  TestTopOpeTools
  TestTopOpe
  BRepTest
  GeometryTest
  HLRTest
  MeshTest
  GeomliteTest
  DrawFairCurve
  BOPTest
  SWDRAW
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
