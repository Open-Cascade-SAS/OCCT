project(TKSTEPBase)

set (TOOLKIT_MODULES
  StepBasic
  RWStepBasic
  StepRepr
  RWStepRepr
  StepGeom
  RWStepGeom
  StepShape
  RWStepShape
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
