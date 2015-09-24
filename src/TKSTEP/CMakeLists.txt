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
  StepAP242
  RWStepAP242
)

if (WIN32)
  list( APPEND USED_LIBS wsock32.lib )
endif()

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
