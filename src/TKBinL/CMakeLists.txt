project(TKBinL)

set (TOOLKIT_MODULES
  BinMDF
  BinMDataStd
  BinMFunction
  BinMDocStd
  BinObjMgt
  BinLDrivers
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
