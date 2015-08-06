project(TKBinL)

set (TOOLKIT_MODULES
  BinMDF
  BinMDataStd
  BinMFunction
  BinMDocStd
  BinObjMgt
  BinLDrivers
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
