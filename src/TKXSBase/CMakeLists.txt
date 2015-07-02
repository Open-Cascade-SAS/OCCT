project(TKXSBase)

set (TOOLKIT_MODULES
  Interface
  Transfer
  IFGraph
  IFSelect
  TransferBRep
  XSControl
  StepData
  StepFile
  HeaderSection
  RWHeaderSection
  APIHeaderSection
  StepSelect
  UnitsMethods
  XSAlgo
  LibCtl
  MoniTool
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
