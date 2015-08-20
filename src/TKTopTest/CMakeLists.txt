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

include(${OCCT_SOURCE_DIR}/adm/cmake/occt_toolkit.cmake)
