#include "BOPTest_Utilities.pxx"

//==================================================================================================
// BFuse Simple Tests - migrating from /tests/boolean/bfuse_simple/
//==================================================================================================

class BFuseSimpleTest : public BRepAlgoAPI_TestBase
{
};

// Test bfuse_simple/A1: psphere s 1; box b 1 1 1; bfuse result s b; checkprops result -s 14.6394
TEST_F(BFuseSimpleTest, SpherePlusBox_A1)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformFuse(aSphere, aBox);
  ValidateResult(aResult, 14.6394);
}

// Test bfuse_simple/A2: rotated sphere + box
TEST_F(BFuseSimpleTest, RotatedSpherePlusBox_A2)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();

  // Apply standard rotation: Z(-90°) then Y(-45°)
  const TopoDS_Shape aRotatedSphere = BOPTest_Utilities::RotateStandard(aSphere);

  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();
  const TopoDS_Shape aResult = PerformFuse(aRotatedSphere, aBox);
  ValidateResult(aResult, 14.6393);
}

// Test bfuse_simple/B1: nurbs box + box
TEST_F(BFuseSimpleTest, NurbsBoxPlusBox_B1)
{
  // Create first box and convert to NURBS
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1              = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";

  // Create second box
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 0.5, 1.0, 0.5);

  const TopoDS_Shape aResult = PerformFuse(aBox2, aBox1);
  ValidateResult(aResult, 6.0);
}

// Test bfuse_simple/B2: box + nurbs box 
TEST_F(BFuseSimpleTest, BoxPlusNurbsBox_B2)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 0.5, 0.5, 1.0);
  
  TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox2 = BOPTest_Utilities::ConvertToNurbs(aBox2);
  ASSERT_FALSE(aBox2.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 7.5);
}

// Test bfuse_simple/B3: box + nurbs box (adjacent)
TEST_F(BFuseSimpleTest, NurbsBoxPlusAdjacentBox_B3)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 0.5, 1.5, 1.0);
  
  const TopoDS_Shape aResult = PerformFuse(aBox2, aBox1);
  ValidateResult(aResult, 7.5);
}

// Test bfuse_simple/B4: adjacent box + nurbs box
TEST_F(BFuseSimpleTest, AdjacentBoxPlusNurbsBox_B4)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0.5, 0), 1.0, 1.0, 1.0);
  
  const TopoDS_Shape aResult = PerformFuse(aBox2, aBox1);
  ValidateResult(aResult, 8.0);
}

// Test bfuse_simple/B5: nurbs box + smaller box
TEST_F(BFuseSimpleTest, NurbsBoxPlusSmallerBox_B5)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0.25, 0.25, 0.25), 0.5, 0.5, 0.5);
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.0);
}

// Test bfuse_simple/B6: smaller box + nurbs box
TEST_F(BFuseSimpleTest, SmallerBoxPlusNurbsBox_B6)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0.25, 0.25, 0.25), 0.5, 0.5, 0.5);
  
  TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox2 = BOPTest_Utilities::ConvertToNurbs(aBox2);
  ASSERT_FALSE(aBox2.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.0);
}

// Test bfuse_simple/B7: nurbs box + partially overlapping box
TEST_F(BFuseSimpleTest, NurbsBoxPlusPartiallyOverlappingBox_B7)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0.5, 0.5, 0.5), 1.0, 1.0, 1.0);
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 7.75);
}

// Test bfuse_simple/B8: partially overlapping box + nurbs box
TEST_F(BFuseSimpleTest, PartiallyOverlappingBoxPlusNurbsBox_B8)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0.5, 0.5, 0.5), 1.0, 1.0, 1.0);
  
  TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox2 = BOPTest_Utilities::ConvertToNurbs(aBox2);
  ASSERT_FALSE(aBox2.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 7.75);
}

// Test bfuse_simple/B9: nurbs box + extended box
TEST_F(BFuseSimpleTest, NurbsBoxPlusExtendedBox_B9)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(-0.5, -0.5, -0.5), 2.0, 2.0, 2.0);
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 24.0);
}

// Test bfuse_simple/C1: extended box + nurbs box
TEST_F(BFuseSimpleTest, ExtendedBoxPlusNurbsBox_C1)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.5, 0.5, 0.5);
  
  const TopoDS_Shape aResult = PerformFuse(aBox2, aBox1);
  ValidateResult(aResult, 7.0);
}

// Test bfuse_simple/C2: nurbs box + shifted extended box
TEST_F(BFuseSimpleTest, NurbsBoxPlusShiftedBox_C2)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 1.5, 0.5, 0.5);
  
  const TopoDS_Shape aResult = PerformFuse(aBox2, aBox1);
  ValidateResult(aResult, 8.5);
}

// Test bfuse_simple/C3: shifted box + nurbs box
TEST_F(BFuseSimpleTest, ShiftedBoxPlusNurbsBox_C3)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0.25, 0, 0), 0.5, 0.5, 1.0);
  
  const TopoDS_Shape aResult = PerformFuse(aBox2, aBox1);
  ValidateResult(aResult, 6.0);
}

// Test bfuse_simple/C4: nurbs box + narrow box
TEST_F(BFuseSimpleTest, NurbsBoxPlusNarrowBox_C4)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0.25, 0.25, 0), 0.5, 0.5, 1.5);
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.75);
}

// Test bfuse_simple/C5: narrow box + nurbs box
TEST_F(BFuseSimpleTest, NarrowBoxPlusNurbsBox_C5)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0.25, 0.25, 0), 0.5, 0.5, 1.5);
  
  TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox2 = BOPTest_Utilities::ConvertToNurbs(aBox2);
  ASSERT_FALSE(aBox2.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.75);
}

// Test bfuse_simple/C6: nurbs box + corner cube
TEST_F(BFuseSimpleTest, NurbsBoxPlusCornerCube_C6)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0.5, 0.5, 0.5), 0.5, 0.5, 0.5);
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.0);
}

// Test bfuse_simple/C7: corner cube + nurbs box
TEST_F(BFuseSimpleTest, CornerCubePlusNurbsBox_C7)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0.5, 0.5, 0.5), 0.5, 0.5, 0.5);
  
  TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox2 = BOPTest_Utilities::ConvertToNurbs(aBox2);
  ASSERT_FALSE(aBox2.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.0);
}

// Test bfuse_simple/C8: nurbs box + offset cube
TEST_F(BFuseSimpleTest, NurbsBoxPlusOffsetCube_C8)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(1, 1, 0), 0.5, 0.5, 1.0);
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.25);
}

// Test bfuse_simple/C9: offset cube + nurbs box
TEST_F(BFuseSimpleTest, OffsetCubePlusNurbsBox_C9)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(1, 1, 0), 0.5, 0.5, 1.0);
  
  TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox2 = BOPTest_Utilities::ConvertToNurbs(aBox2);
  ASSERT_FALSE(aBox2.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.25);
}

// Test bfuse_simple/A3: box + rotated sphere
TEST_F(BFuseSimpleTest, BoxPlusRotatedSphere_A3)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  
  // Apply standard rotation: Z(-90°) then Y(-45°)
  const TopoDS_Shape aRotatedSphere = BOPTest_Utilities::RotateStandard(aSphere);
  
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateUnitBox();
  const TopoDS_Shape aResult = PerformFuse(aBox, aRotatedSphere);
  ValidateResult(aResult, 14.6393);
}

// Test bfuse_simple/A4: sphere + rotated box
TEST_F(BFuseSimpleTest, SpherePlusRotatedBox_A4)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateUnitBox();
  
  // Apply Y rotation: 90°
  const TopoDS_Shape aRotatedBox = BOPTest_Utilities::RotateY(aBox, 90.0);
  
  const TopoDS_Shape aResult = PerformFuse(aSphere, aRotatedBox);
  ValidateResult(aResult, 14.6393);
}

// Test bfuse_simple/A5: rotated box + sphere 
TEST_F(BFuseSimpleTest, RotatedBoxPlusSphere_A5)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateUnitBox();
  
  // Apply Y rotation: 90°
  const TopoDS_Shape aRotatedBox = BOPTest_Utilities::RotateY(aBox, 90.0);
  
  const TopoDS_Shape aResult = PerformFuse(aRotatedBox, aSphere);
  ValidateResult(aResult, 14.6393);
}

// Test bfuse_simple/A6: nurbs box + identical box (should be 6.0 surface area)
TEST_F(BFuseSimpleTest, IdenticalNurbsBoxPlusBox_A6)
{
  // Create first box and convert to NURBS
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.0);
}

// Test bfuse_simple/A7: box + nurbs box (identical)
TEST_F(BFuseSimpleTest, IdenticalBoxPlusNurbsBox_A7)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  
  TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox2 = BOPTest_Utilities::ConvertToNurbs(aBox2);
  ASSERT_FALSE(aBox2.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 6.0);
}

// Test bfuse_simple/A8: nurbs box + larger box
TEST_F(BFuseSimpleTest, NurbsBoxPlusLargerBox_A8)
{
  // Create first box and convert to NURBS
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1 = BOPTest_Utilities::ConvertToNurbs(aBox1);
  ASSERT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(-0.5, -0.5, -0.5), 2.0, 2.0, 2.0);
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 24.0);
}

// Test bfuse_simple/A9: larger box + nurbs box
TEST_F(BFuseSimpleTest, LargerBoxPlusNurbsBox_A9)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(-0.5, -0.5, -0.5), 2.0, 2.0, 2.0);
  
  TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox2 = BOPTest_Utilities::ConvertToNurbs(aBox2);
  ASSERT_FALSE(aBox2.IsNull()) << "Failed to convert to NURBS";
  
  const TopoDS_Shape aResult = PerformFuse(aBox1, aBox2);
  ValidateResult(aResult, 24.0);
}

//==================================================================================================
// BOPFuse Simple Tests - migrating from /tests/boolean/bopfuse_simple/
//==================================================================================================

class BOPFuseSimpleTest : public BOPAlgo_TestBase
{
};

// Test bopfuse_simple/A1: identical boxes
TEST_F(BOPFuseSimpleTest, IdenticalBoxes_A1)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformDirectBOP(aBox1, aBox2, BOPAlgo_FUSE);
  ValidateResult(aResult, 6.0);
}