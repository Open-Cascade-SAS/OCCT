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

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
