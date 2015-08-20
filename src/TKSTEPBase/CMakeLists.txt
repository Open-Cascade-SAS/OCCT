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

include(${OCCT_SOURCE_DIR}/adm/cmake/occt_toolkit.cmake)
