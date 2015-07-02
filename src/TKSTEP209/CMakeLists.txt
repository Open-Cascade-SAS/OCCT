project(TKSTEP209)

set (TOOLKIT_MODULES
  StepElement
  StepFEA
  RWStepElement
  RWStepFEA
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
