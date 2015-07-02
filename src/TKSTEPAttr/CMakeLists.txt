project(TKSTEPAttr)

set (TOOLKIT_MODULES
  StepVisual
  RWStepVisual
  StepDimTol
  RWStepDimTol
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
