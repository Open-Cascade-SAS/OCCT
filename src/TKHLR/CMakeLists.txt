project(TKHLR)

set (TOOLKIT_MODULES
  HLRTopoBRep
  HLRBRep
  HLRAlgo
  HLRAppli
  Intrv
  TopBas
  TopCnx
  Contap
)

include(${OCCT_SOURCE_DIR}/adm/cmake/occt_toolkit.cmake)
