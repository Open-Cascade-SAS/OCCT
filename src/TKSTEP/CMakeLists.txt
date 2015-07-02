project(TKSTEP)

set (TOOLKIT_MODULES
  StepAP214
  RWStepAP214
  StepAP203
  RWStepAP203
  STEPConstruct
  STEPEdit
  GeomToStep
  StepToGeom
  StepToTopoDS
  TopoDSToStep
  STEPControl
  STEPSelections
  StepAP209
)

if (WIN32)
  list( APPEND USED_LIBS wsock32.lib )
endif()

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
