# Test source files for TKFillet
set(OCCT_TKFillet_GTests_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_TKFillet_GTests_FILES
  BRepFilletAPI_MakeChamfer_Test.cxx
  BRepFilletAPI_MakeFillet_OppositeEdges_Test.cxx
  BRepFilletAPI_MakeFillet_Test.cxx
  BRepFilletAPI_MakeFillet_WiderThanFace_Test.cxx
  ChFi3d_Builder_0_Test.cxx
  ChFi3d_Hatching_Test.cxx
  BRepFilletAPI_ShallowLoft_Test.cxx
  BlendFunc_InvTolerance_Test.cxx
  BRepBlend_AppFunc_Test.cxx
  BRepBlend_Line_Test.cxx
  ChFi3d_FilBuilder_Test.cxx
  ChFi3d_IsTangentFaces_Test.cxx
)
