project(TKBin)

set (TOOLKIT_MODULES
  BinDrivers
  BinMDataXtd
  BinMPrsStd
  BinMNaming
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
