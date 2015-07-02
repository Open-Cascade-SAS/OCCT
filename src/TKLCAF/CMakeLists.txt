project(TKLCAF)

set (TOOLKIT_MODULES
  TDF
  TDataStd
  TFunction
  TDocStd
  AppStdL
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
