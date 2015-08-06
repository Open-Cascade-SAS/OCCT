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

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
