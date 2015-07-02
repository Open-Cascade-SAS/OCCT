project(TKDCAF)

set (TOOLKIT_MODULES
  DDF
  DDocStd
  DNaming
  DDataStd
  DPrsStd
  DrawDim
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
