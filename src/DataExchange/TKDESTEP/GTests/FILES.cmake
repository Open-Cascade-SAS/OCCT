# Test source files for TKDESTEP
set(OCCT_TKDESTEP_GTests_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_TKDESTEP_GTests_FILES
    DESTEP_Provider_Test.cxx
    STEPConstruct_RenderingProperties_Test.cxx
    StepData_StepWriter_Test.cxx
    StepTidy_BaseTestFixture.pxx
    StepTidy_Axis2Placement3dReducer_Test.cxx
    StepTidy_CartesianPointReducer_Test.cxx
    StepTidy_CircleReducer_Test.cxx
    StepTidy_DirectionReducer_Test.cxx
    StepTidy_LineReducer_Test.cxx
    StepTidy_PlaneReducer_Test.cxx
    StepTidy_Merger_Test.cxx
    StepTidy_VectorReducer_Test.cxx
    StepTransientReplacements_Test.cxx
)
