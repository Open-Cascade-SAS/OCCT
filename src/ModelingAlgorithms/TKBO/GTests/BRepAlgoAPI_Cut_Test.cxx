// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include "BOPTest_Utilities.pxx"

#ifndef M_SQRT2
  #define M_SQRT2 1.41421356237309504880168872420969808
#endif

//==================================================================================================
// BCut Simple Tests - migrating from /tests/boolean/bcut_simple/
//==================================================================================================

class BCutSimpleTest : public BRepAlgoAPI_TestBase
{
};

// Test bcut_simple/A1: psphere s 1; box b 1 1 1; bcut result s b; checkprops result -s 13.3518
TEST_F(BCutSimpleTest, SphereMinusBox_A1)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformCut(aSphere, aBox);
  ValidateResult(aResult, 13.3518);
}

// Test bcut_simple/A2: rotated sphere - box
TEST_F(BCutSimpleTest, RotatedSphereMinusBox_A2)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();

  // Apply standard rotation: Z(-90deg) then Y(-45deg)
  const TopoDS_Shape aRotatedSphere = BOPTest_Utilities::RotateStandard(aSphere);

  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();
  const TopoDS_Shape aResult = PerformCut(aRotatedSphere, aBox);
  ValidateResult(aResult, 13.3517);
}

// Test bcut_simple/A3: box - rotated sphere (reverse operation)
TEST_F(BCutSimpleTest, BoxMinusRotatedSphere_A3)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();

  // Apply standard rotation: Z(-90deg) then Y(-45deg)
  const TopoDS_Shape aRotatedSphere = BOPTest_Utilities::RotateStandard(aSphere);

  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();
  const TopoDS_Shape aResult = PerformCut(aBox, aRotatedSphere); // Note: reversed order
  ValidateResult(aResult, 5.2146);
}

// Test bcut_simple/A4: sphere - Y-rotated box
TEST_F(BCutSimpleTest, SphereMinusRotatedBox_A4)
{
  const TopoDS_Shape aSphere     = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox        = BOPTest_Utilities::CreateUnitBox();
  const TopoDS_Shape aRotatedBox = BOPTest_Utilities::RotateY90(aBox);

  const TopoDS_Shape aResult = PerformCut(aSphere, aRotatedBox);
  ValidateResult(aResult, 13.3517);
}

// Test bcut_simple/A5: Y-rotated box - sphere (reverse operation)
TEST_F(BCutSimpleTest, RotatedBoxMinusSphere_A5)
{
  const TopoDS_Shape aSphere     = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox        = BOPTest_Utilities::CreateUnitBox();
  const TopoDS_Shape aRotatedBox = BOPTest_Utilities::RotateY90(aBox);

  const TopoDS_Shape aResult = PerformCut(aRotatedBox, aSphere);
  ValidateResult(aResult, 5.2146);
}

// Test bcut_simple/A6: identical NURBS box - box (should result in empty)
TEST_F(BCutSimpleTest, IdenticalNurbsBoxMinusBox_A6)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aBox);
  ValidateResult(aResult, -1.0, -1.0, Standard_True); // Expected empty
}

// Test bcut_simple/A7: identical box - NURBS box (should result in empty)
TEST_F(BCutSimpleTest, IdenticalBoxMinusNurbsBox_A7)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformCut(aBox, aNurbsBox);
  ValidateResult(aResult, -1.0, -1.0, Standard_True); // Expected empty
}

// Test bcut_simple/A8: NURBS box - larger box (should result in empty)
TEST_F(BCutSimpleTest, NurbsBoxMinusLargerBox_A8)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aLargerBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aLargerBox);
  ValidateResult(aResult, -1.0, -1.0, Standard_True); // Expected empty
}

// Test bcut_simple/A9: larger box - NURBS box
TEST_F(BCutSimpleTest, LargerBoxMinusNurbsBox_A9)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aLargerBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aLargerBox, aNurbsBox);
  ValidateResult(aResult, 4.0);
}

// Test bcut_simple/B1: nurbs box - box
TEST_F(BCutSimpleTest, NurbsBoxMinusBox_B1)
{
  // Create first box and convert to NURBS
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1              = BOPTest_Utilities::ConvertToNurbs(aBox1);
  EXPECT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";

  // Create second box
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 1, 0), 1.0, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aBox1, aBox2);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/B2: box - NURBS box (reverse B1)
TEST_F(BCutSimpleTest, BoxMinusNurbsBox_B2)
{
  // Create first box and convert to NURBS
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  // Create second box
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 1, 0), 1.0, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aBox, aNurbsBox);
  ValidateResult(aResult, 4.0);
}

// Test bcut_simple/B3: NURBS box - adjacent box
TEST_F(BCutSimpleTest, NurbsBoxMinusAdjacentBox_B3)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aAdjacentBox = BOPTest_Utilities::CreateBox(gp_Pnt(1, 1, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aAdjacentBox);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/B4: adjacent box - NURBS box (reverse B3)
TEST_F(BCutSimpleTest, AdjacentBoxMinusNurbsBox_B4)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aAdjacentBox = BOPTest_Utilities::CreateBox(gp_Pnt(1, 1, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformCut(aAdjacentBox, aNurbsBox);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/B5: NURBS box - smaller contained box
TEST_F(BCutSimpleTest, NurbsBoxMinusSmallerBox_B5)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aSmallerBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 0.5, 1.0, 0.5);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aSmallerBox);
  ValidateResult(aResult, 5.5);
}

// Test bcut_simple/B6: smaller box - NURBS box (should result in empty)
TEST_F(BCutSimpleTest, SmallerBoxMinusNurbsBox_B6)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aSmallerBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 0.5, 1.0, 0.5);

  const TopoDS_Shape aResult = PerformCut(aSmallerBox, aNurbsBox);
  ValidateResult(aResult, -1.0, -1.0, Standard_True); // Expected empty
}

// Test bcut_simple/B7: NURBS box - partially overlapping box
TEST_F(BCutSimpleTest, NurbsBoxMinusPartiallyOverlappingBox_B7)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aPartialBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 0.5, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aPartialBox);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/B8: partially overlapping box - NURBS box
TEST_F(BCutSimpleTest, PartiallyOverlappingBoxMinusNurbsBox_B8)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aPartialBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 0.5, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aPartialBox, aNurbsBox);
  ValidateResult(aResult, 2.5);
}

// Test bcut_simple/B9: NURBS box - extended overlapping box
TEST_F(BCutSimpleTest, NurbsBoxMinusExtendedBox_B9)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aExtendedBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 0.5, 1.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aExtendedBox);
  ValidateResult(aResult, 4.0);
}

// Test bcut_simple/C1: extended box - NURBS box (from B8 setup)
TEST_F(BCutSimpleTest, ExtendedBoxMinusNurbsBox_C1)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aExtendedBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 0.5, 1.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aExtendedBox, aNurbsBox);
  ValidateResult(aResult, 2.5);
}

// Test bcut_simple/C2: NURBS box - shifted overlapping box
TEST_F(BCutSimpleTest, NurbsBoxMinusShiftedBox_C2)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aShiftedBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0.5, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aShiftedBox);
  ValidateResult(aResult, 4.0);
}

// Test bcut_simple/C3: shifted box - NURBS box (reverse C2)
TEST_F(BCutSimpleTest, ShiftedBoxMinusNurbsBox_C3)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aShiftedBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0.5, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformCut(aShiftedBox, aNurbsBox);
  ValidateResult(aResult, 4.0);
}

// Test bcut_simple/C4: NURBS box - narrow contained box
TEST_F(BCutSimpleTest, NurbsBoxMinusNarrowBox_C4)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aNarrowBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0.25, 0), 1.0, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aNarrowBox);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/C5: narrow box - NURBS box (should result in empty)
TEST_F(BCutSimpleTest, NarrowBoxMinusNurbsBox_C5)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aNarrowBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0.25, 0), 1.0, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNarrowBox, aNurbsBox);
  ValidateResult(aResult, -1.0, -1.0, Standard_True); // Expected empty
}

// Test bcut_simple/C6: NURBS box - small corner cube
TEST_F(BCutSimpleTest, NurbsBoxMinusCornerCube_C6)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aCornerCube = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 0.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aCornerCube);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/C7: small corner cube - NURBS box (should result in empty)
TEST_F(BCutSimpleTest, CornerCubeMinusNurbsBox_C7)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aCornerCube = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 0.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aCornerCube, aNurbsBox);
  ValidateResult(aResult, -1.0, -1.0, Standard_True); // Expected empty
}

// Test bcut_simple/C8: NURBS box - offset small cube
TEST_F(BCutSimpleTest, NurbsBoxMinusOffsetCube_C8)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetCube = BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 0.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aOffsetCube);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/C9: offset small cube - NURBS box
TEST_F(BCutSimpleTest, OffsetCubeMinusNurbsBox_C9)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetCube = BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 0.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aOffsetCube, aNurbsBox);
  ValidateResult(aResult, 1.5);
}

// Test bcut_simple/D1: NURBS box - offset corner cube
TEST_F(BCutSimpleTest, NurbsBoxMinusOffsetCornerCube_D1)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetCornerCube =
    BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, -0.5), 0.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aOffsetCornerCube);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/D2: offset corner cube - NURBS box
TEST_F(BCutSimpleTest, OffsetCornerCubeMinusNurbsBox_D2)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetCornerCube =
    BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, -0.5), 0.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aOffsetCornerCube, aNurbsBox);
  ValidateResult(aResult, 1.5);
}

// Test bcut_simple/D3: NURBS box - shifted corner cube
TEST_F(BCutSimpleTest, NurbsBoxMinusShiftedCornerCube_D3)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aShiftedCornerCube =
    BOPTest_Utilities::CreateBox(gp_Pnt(-0.5, -0.5, -0.5), 0.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aShiftedCornerCube);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/D4: shifted corner cube - NURBS box
TEST_F(BCutSimpleTest, ShiftedCornerCubeMinusNurbsBox_D4)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aShiftedCornerCube =
    BOPTest_Utilities::CreateBox(gp_Pnt(-0.5, -0.5, -0.5), 0.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aShiftedCornerCube, aNurbsBox);
  ValidateResult(aResult, 1.5);
}

// Test bcut_simple/D5: NURBS box - extended X box
TEST_F(BCutSimpleTest, NurbsBoxMinusExtendedXBox_D5)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aExtendedXBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aExtendedXBox);
  ValidateResult(aResult, 5.5);
}

// Test bcut_simple/D6: extended X box - NURBS box
TEST_F(BCutSimpleTest, ExtendedXBoxMinusNurbsBox_D6)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aExtendedXBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aExtendedXBox, aNurbsBox);
  ValidateResult(aResult, 1.5);
}

// Test bcut_simple/D7: NURBS box - offset extended X box
TEST_F(BCutSimpleTest, NurbsBoxMinusOffsetExtendedXBox_D7)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetExtendedXBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 1.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aOffsetExtendedXBox);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/D8: offset extended X box - NURBS box
TEST_F(BCutSimpleTest, OffsetExtendedXBoxMinusNurbsBox_D8)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetExtendedXBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0, -0.5, 0), 1.5, 0.5, 0.5);

  const TopoDS_Shape aResult = PerformCut(aOffsetExtendedXBox, aNurbsBox);
  ValidateResult(aResult, 3.5);
}

// Test bcut_simple/D9: NURBS box - shifted narrow box
TEST_F(BCutSimpleTest, NurbsBoxMinusShiftedNarrowBox_D9)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aShiftedNarrowBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.25, 0, 0), 0.5, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aShiftedNarrowBox);
  ValidateResult(aResult, 6.5);
}

// Test bcut_simple/E1: shifted narrow box - NURBS box (should result in empty)
TEST_F(BCutSimpleTest, ShiftedNarrowBoxMinusNurbsBox_E1)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aShiftedNarrowBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.25, 0, 0), 0.5, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aShiftedNarrowBox, aNurbsBox);
  ValidateResult(aResult, -1.0, -1.0, Standard_True); // Expected empty
}

// Test bcut_simple/E2: NURBS box - offset shifted narrow box
TEST_F(BCutSimpleTest, NurbsBoxMinusOffsetShiftedNarrowBox_E2)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetShiftedNarrowBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.25, -0.5, 0), 0.5, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aOffsetShiftedNarrowBox);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/E3: offset shifted narrow box - NURBS box
TEST_F(BCutSimpleTest, OffsetShiftedNarrowBoxMinusNurbsBox_E3)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetShiftedNarrowBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.25, -0.5, 0), 0.5, 0.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aOffsetShiftedNarrowBox, aNurbsBox);
  ValidateResult(aResult, 2.5);
}

// Test bcut_simple/E4: NURBS box - extended Y shifted narrow box
TEST_F(BCutSimpleTest, NurbsBoxMinusExtendedYShiftedNarrowBox_E4)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aExtendedYShiftedNarrowBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.25, 0, 0), 0.5, 1.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aExtendedYShiftedNarrowBox);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/E5: extended Y shifted narrow box - NURBS box
TEST_F(BCutSimpleTest, ExtendedYShiftedNarrowBoxMinusNurbsBox_E5)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aExtendedYShiftedNarrowBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.25, 0, 0), 0.5, 1.5, 1.0);

  const TopoDS_Shape aResult = PerformCut(aExtendedYShiftedNarrowBox, aNurbsBox);
  ValidateResult(aResult, 2.5);
}

// Test bcut_simple/E6: NURBS box - right half box
TEST_F(BCutSimpleTest, NurbsBoxMinusRightHalfBox_E6)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aRightHalfBox = BOPTest_Utilities::CreateBox(gp_Pnt(0.5, 0, 0), 1.0, 1.0, 0.5);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aRightHalfBox);
  ValidateResult(aResult, 5.5);
}

// Test bcut_simple/E7: right half box - NURBS box
TEST_F(BCutSimpleTest, RightHalfBoxMinusNurbsBox_E7)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aRightHalfBox = BOPTest_Utilities::CreateBox(gp_Pnt(0.5, 0, 0), 1.0, 1.0, 0.5);

  const TopoDS_Shape aResult = PerformCut(aRightHalfBox, aNurbsBox);
  ValidateResult(aResult, 2.5);
}

// Test bcut_simple/E8: NURBS box - offset right half box
TEST_F(BCutSimpleTest, NurbsBoxMinusOffsetRightHalfBox_E8)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetRightHalfBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.5, 0, -0.5), 1.0, 1.0, 0.5);

  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aOffsetRightHalfBox);
  ValidateResult(aResult, 6.0);
}

// Test bcut_simple/E9: offset right half box - NURBS box
TEST_F(BCutSimpleTest, OffsetRightHalfBoxMinusNurbsBox_E9)
{
  TopoDS_Shape aNurbsBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox              = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  EXPECT_FALSE(aNurbsBox.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aOffsetRightHalfBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.5, 0, -0.5), 1.0, 1.0, 0.5);

  const TopoDS_Shape aResult = PerformCut(aOffsetRightHalfBox, aNurbsBox);
  ValidateResult(aResult, 4.0);
}

// Test bcut_simple/F1: NURBS box - rotated rectangular box (sqrt(2) x sqrt(2)/2 x 1, 45deg)
TEST_F(BCutSimpleTest, NurbsBoxMinusRotatedRectangularBox_F1)
{
  TopoDS_Shape aNurbsBox       = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox                    = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  constexpr Standard_Real r    = M_SQRT2;
  TopoDS_Shape aRectangularBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), r, r / 2.0, 1.0);
  aRectangularBox              = BOPTest_Utilities::RotateShape(aRectangularBox,
                                                   gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                                   45.0 * M_PI / 180.0);
  const TopoDS_Shape aResult   = PerformCut(aNurbsBox, aRectangularBox);
  ValidateResult(aResult, 4.41421);
}

// Test bcut_simple/F2: rotated rectangular box - NURBS box
TEST_F(BCutSimpleTest, RotatedRectangularBoxMinusNurbsBox_F2)
{
  TopoDS_Shape aNurbsBox       = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox                    = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  constexpr Standard_Real r    = M_SQRT2;
  TopoDS_Shape aRectangularBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), r, r / 2.0, 1.0);
  aRectangularBox              = BOPTest_Utilities::RotateShape(aRectangularBox,
                                                   gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                                   45.0 * M_PI / 180.0);
  const TopoDS_Shape aResult   = PerformCut(aRectangularBox, aNurbsBox);
  ValidateResult(aResult, 5.82843);
}

// Test bcut_simple/F3: NURBS box - rotated square box (sqrt(2)/2 x sqrt(2)/2 x 1, 45deg)
TEST_F(BCutSimpleTest, NurbsBoxMinusRotatedSquareBox_F3)
{
  TopoDS_Shape aNurbsBox         = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox                      = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  const Standard_Real r          = sqrt(2.0) / 2.0;
  TopoDS_Shape        aSquareBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), r, r, 1.0);
  aSquareBox                     = BOPTest_Utilities::RotateShape(aSquareBox,
                                              gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                              45.0 * M_PI / 180.0);
  const TopoDS_Shape aResult     = PerformCut(aNurbsBox, aSquareBox);
  ValidateResult(aResult, 5.91421);
}

// Test bcut_simple/F4: rotated square box - NURBS box
TEST_F(BCutSimpleTest, RotatedSquareBoxMinusNurbsBox_F4)
{
  TopoDS_Shape aNurbsBox         = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox                      = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  const Standard_Real r          = sqrt(2.0) / 2.0;
  TopoDS_Shape        aSquareBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), r, r, 1.0);
  aSquareBox                     = BOPTest_Utilities::RotateShape(aSquareBox,
                                              gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                              45.0 * M_PI / 180.0);
  const TopoDS_Shape aResult     = PerformCut(aSquareBox, aNurbsBox);
  ValidateResult(aResult, 2.91421);
}

// Test bcut_simple/F5: NURBS box - rotated thin box (sqrt(2) x 0.25 x 1, 45deg)
TEST_F(BCutSimpleTest, NurbsBoxMinusRotatedThinBox_F5)
{
  TopoDS_Shape aNurbsBox           = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox                        = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  constexpr Standard_Real r        = M_SQRT2;
  TopoDS_Shape            aThinBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), r, 0.25, 1.0);
  aThinBox                         = BOPTest_Utilities::RotateShape(aThinBox,
                                            gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                            45.0 * M_PI / 180.0);
  const TopoDS_Shape aResult       = PerformCut(aNurbsBox, aThinBox);
  ValidateResult(aResult, 7.03921);
}

// Test bcut_simple/F6: rotated thin box - NURBS box
TEST_F(BCutSimpleTest, RotatedThinBoxMinusNurbsBox_F6)
{
  TopoDS_Shape aNurbsBox           = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox                        = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  constexpr Standard_Real r        = M_SQRT2;
  TopoDS_Shape            aThinBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), r, 0.25, 1.0);
  aThinBox                         = BOPTest_Utilities::RotateShape(aThinBox,
                                            gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                            45.0 * M_PI / 180.0);
  const TopoDS_Shape aResult       = PerformCut(aThinBox, aNurbsBox);
  ValidateResult(aResult, 1.83211);
}

// Test bcut_simple/F7: NURBS box - rotated narrow box (sqrt(31)/4 x 0.25 x 1, 34.73deg)
TEST_F(BCutSimpleTest, NurbsBoxMinusRotatedNarrowBox_F7)
{
  TopoDS_Shape aNurbsBox     = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox                  = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  constexpr Standard_Real r  = 5.5677643628300219; // sqrt(31.0)
  TopoDS_Shape aNarrowBox    = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), r / 4.0, 0.25, 1.0);
  aNarrowBox                 = BOPTest_Utilities::RotateShape(aNarrowBox,
                                              gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                              34.73 * M_PI / 180.0);
  const TopoDS_Shape aResult = PerformCut(aNurbsBox, aNarrowBox);
  ValidateResult(aResult, 7.21677);
}

// Test bcut_simple/F8: rotated narrow box - NURBS box
TEST_F(BCutSimpleTest, RotatedNarrowBoxMinusNurbsBox_F8)
{
  TopoDS_Shape aNurbsBox     = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aNurbsBox                  = BOPTest_Utilities::ConvertToNurbs(aNurbsBox);
  constexpr Standard_Real r  = 5.5677643628300219;
  TopoDS_Shape aNarrowBox    = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), r / 4.0, 0.25, 1.0);
  aNarrowBox                 = BOPTest_Utilities::RotateShape(aNarrowBox,
                                              gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                              34.73 * M_PI / 180.0);
  const TopoDS_Shape aResult = PerformCut(aNarrowBox, aNurbsBox);
  ValidateResult(aResult, 1.54631);
}

// Test bcut_simple/F9: sphere - box
TEST_F(BCutSimpleTest, SphereMinusBox_F9)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateSphere(gp_Pnt(0, 0, 0), 1.0);
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aResult = PerformCut(aSphere, aBox);
  ValidateResult(aResult, 13.3518);
}

// Test bcut_simple/G1: box - sphere
TEST_F(BCutSimpleTest, BoxMinusSphere_G1)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateSphere(gp_Pnt(0, 0, 0), 1.0);
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aResult = PerformCut(aBox, aSphere);
  ValidateResult(aResult, 5.2146);
}

// Test bcut_simple/G2: rotated sphere - box
TEST_F(BCutSimpleTest, RotatedSphereMinusBox_G2)
{
  TopoDS_Shape aSphere       = BOPTest_Utilities::CreateSphere(gp_Pnt(0, 0, 0), 1.0);
  aSphere                    = BOPTest_Utilities::RotateShape(aSphere,
                                           gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                           -90.0 * M_PI / 180.0);
  aSphere                    = BOPTest_Utilities::RotateShape(aSphere,
                                           gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Y)),
                                           -45.0 * M_PI / 180.0);
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aResult = PerformCut(aSphere, aBox);
  ValidateResult(aResult, 13.3517);
}

// Test bcut_simple/G3: box - rotated sphere
TEST_F(BCutSimpleTest, BoxMinusRotatedSphere_G3)
{
  TopoDS_Shape aSphere       = BOPTest_Utilities::CreateSphere(gp_Pnt(0, 0, 0), 1.0);
  aSphere                    = BOPTest_Utilities::RotateShape(aSphere,
                                           gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)),
                                           -90.0 * M_PI / 180.0);
  aSphere                    = BOPTest_Utilities::RotateShape(aSphere,
                                           gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Y)),
                                           -45.0 * M_PI / 180.0);
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aResult = PerformCut(aBox, aSphere);
  ValidateResult(aResult, 5.2146);
}

// Test bcut_simple/G4: sphere - rotated box
TEST_F(BCutSimpleTest, SphereMinusRotatedBox_G4)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateSphere(gp_Pnt(0, 0, 0), 1.0);
  TopoDS_Shape       aBox    = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox                       = BOPTest_Utilities::RotateShape(aBox,
                                        gp_Ax1(gp_Pnt(0, 0, 1), gp_Dir(gp_Dir::D::Y)),
                                        90.0 * M_PI / 180.0);
  const TopoDS_Shape aResult = PerformCut(aSphere, aBox);
  ValidateResult(aResult, 13.3517);
}

// Test bcut_simple/G5: rotated box - sphere
TEST_F(BCutSimpleTest, RotatedBoxMinusSphere_G5)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateSphere(gp_Pnt(0, 0, 0), 1.0);
  TopoDS_Shape       aBox    = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox                       = BOPTest_Utilities::RotateShape(aBox,
                                        gp_Ax1(gp_Pnt(0, 0, 1), gp_Dir(gp_Dir::D::Y)),
                                        90.0 * M_PI / 180.0);
  const TopoDS_Shape aResult = PerformCut(aBox, aSphere);
  ValidateResult(aResult, 5.2146);
}

// Test bcut_simple/G6: Complex profile/revol operation (pro5363 bug)
TEST_F(BCutSimpleTest, ComplexProfileRevolOperation_G6)
{
  // Create box: "box b 100 100 40"
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 100.0, 100.0, 40.0);

  // Create exact G6 profile: "profile rev S b_4 F 50 20 Y 50 C 10 180 Y -50 C 10 180"
  gp_Pln aPlane(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z)); // Face b_4 of box (top face at Z=40)
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, 20.0),  // F 50 20
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 50.0),        // Y 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::C, 10.0, 180.0), // C 10 180
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -50.0),       // Y -50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::C, 10.0, 180.0)  // C 10 180
  };
  const TopoDS_Shape aProfile = BOPTest_Utilities::CreateProfile(aPlane, aProfileOps);

  // Create revolution: "revol rev2 rev 0 0 50 0 1 0 360" (around Y-axis at Z=50)
  const TopoDS_Shape aRevolution =
    BOPTest_Utilities::CreateRevolution(aProfile,
                                        gp_Ax1(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Y)),
                                        2.0 * M_PI);

  // Perform boolean cut operation: "bcut result b rev2"
  const TopoDS_Shape aResult = PerformCut(aBox, aRevolution);
  ValidateResult(aResult, 41187.4);
}

// Test bcut_simple/G7: Box minus translated box (buc40054 bug)
TEST_F(BCutSimpleTest, BoxMinusTranslatedBox_G7)
{
  const TopoDS_Shape aBox1   = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 3.0, 3.0, 3.0);
  const TopoDS_Shape aBox2   = BOPTest_Utilities::CreateBox(gp_Pnt(0, 3, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aResult = PerformCut(aBox1, aBox2);
  ValidateResult(aResult, 54.0);
}

// Test bcut_simple/G8: Box minus prism (BUC40188 bug)
TEST_F(BCutSimpleTest, BoxMinusPrism_G8)
{
  // Create box mb (-0.5, -0.5, -0.5, 1, 1, 1)
  const TopoDS_Shape aBoxMb = BOPTest_Utilities::CreateBox(gp_Pnt(-0.5, -0.5, -0.5), 1.0, 1.0, 1.0);

  // Get face mb_1 (approximated as first face)
  const TopoDS_Face aFace = BOPTest_Utilities::GetFaceByIndex(aBoxMb, 1);

  // Create semi-infinite prism (simulated with very large prism: prism pryz mb_1 1 0 0 SemiInf)
  // Semi-infinite prism approximated as large prism extending 1000 units in X direction
  const TopoDS_Shape aSemiInfPrism =
    BOPTest_Utilities::CreatePrism(aFace, gp_Vec(1000.0, 0.0, 0.0));

  // Create box ab (0, -1, -1, 2, 2, 2)
  const TopoDS_Shape aBoxAb = BOPTest_Utilities::CreateBox(gp_Pnt(0, -1, -1), 2.0, 2.0, 2.0);

  // Perform boolean cut operation: bcut result ab pryz
  const TopoDS_Shape aResult = PerformCut(aBoxAb, aSemiInfPrism);
  ValidateResult(aResult, 30.0);
}

// Test bcut_simple/G9: Complex cylinder/cone fuse minus translated cylinder
TEST_F(BCutSimpleTest, ComplexCylinderConeOperation_G9)
{
  // Create cylinder and cone, fuse them
  const TopoDS_Shape aCylinder = BOPTest_Utilities::CreateCylinder(9.0, 3.0);
  const TopoDS_Shape aCone     = BOPTest_Utilities::CreateCone(7.0, 6.0, 4.0);

  BRepAlgoAPI_Fuse aFuseOp(aCylinder, aCone);
  EXPECT_TRUE(aFuseOp.IsDone()) << "Fuse operation failed";
  const TopoDS_Shape aBody = aFuseOp.Shape();

  // Create small cylinder and translate it
  TopoDS_Shape aSmallCylinder = BOPTest_Utilities::CreateCylinder(1.0, 9.0);
  aSmallCylinder = BOPTest_Utilities::TranslateShape(aSmallCylinder, gp_Vec(5.0, 0.0, -2.0));

  const TopoDS_Shape aResult = PerformCut(aBody, aSmallCylinder);
  ValidateResult(aResult, 727.481);
}

// Test bcut_simple/H1: Complex custom polygon prism minus box (MFA bug collection)
TEST_F(BCutSimpleTest, ComplexPolygonPrismMinusBox_H1)
{
  // Create custom polygon from H1 test vertices: v1(0,0,0) v2(1,0,0) v3(1,3,0) v4(2,3,0) v5(2,0,0)
  // v6(3,0,0) v7(3,5,0) v8(0,5,0)
  std::vector<gp_Pnt> aPoints;
  aPoints.push_back(gp_Pnt(0, 0, 0)); // v1
  aPoints.push_back(gp_Pnt(1, 0, 0)); // v2
  aPoints.push_back(gp_Pnt(1, 3, 0)); // v3
  aPoints.push_back(gp_Pnt(2, 3, 0)); // v4
  aPoints.push_back(gp_Pnt(2, 0, 0)); // v5
  aPoints.push_back(gp_Pnt(3, 0, 0)); // v6
  aPoints.push_back(gp_Pnt(3, 5, 0)); // v7
  aPoints.push_back(gp_Pnt(0, 5, 0)); // v8

  // Create wire and extrude to solid (prism sol p 0 0 2)
  TopoDS_Wire        aWire  = BOPTest_Utilities::CreatePolygonWire(aPoints, Standard_True);
  TopoDS_Shape       aFace  = BOPTest_Utilities::CreateFaceFromWire(aWire);
  const TopoDS_Shape aSolid = BOPTest_Utilities::CreatePrism(aFace, gp_Vec(0, 0, 2));

  // Create box: box b -1 2 1 5 1 3 (corner at (-1,2,1), dimensions 5x1x3)
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(-1, 2, 1), 5.0, 1.0, 3.0);

  // Perform boolean cut: bcut result sol b
  const TopoDS_Shape aResult = PerformCut(aSolid, aBox);
  ValidateResult(aResult, 68.0);
}

// Test bcut_simple/H2: Complex custom polygon prism minus box (pro13306 bug)
TEST_F(BCutSimpleTest, ComplexPolygonPrismMinusBox_H2)
{
  // H2 is identical to H1 - same polygon and same box
  // Create custom polygon from H2 test vertices (same as H1): v1(0,0,0) v2(1,0,0) v3(1,3,0)
  // v4(2,3,0) v5(2,0,0) v6(3,0,0) v7(3,5,0) v8(0,5,0)
  std::vector<gp_Pnt> aPoints;
  aPoints.push_back(gp_Pnt(0, 0, 0)); // v1
  aPoints.push_back(gp_Pnt(1, 0, 0)); // v2
  aPoints.push_back(gp_Pnt(1, 3, 0)); // v3
  aPoints.push_back(gp_Pnt(2, 3, 0)); // v4
  aPoints.push_back(gp_Pnt(2, 0, 0)); // v5
  aPoints.push_back(gp_Pnt(3, 0, 0)); // v6
  aPoints.push_back(gp_Pnt(3, 5, 0)); // v7
  aPoints.push_back(gp_Pnt(0, 5, 0)); // v8

  // Create wire and extrude to solid (prism sol p 0 0 2)
  TopoDS_Wire        aWire  = BOPTest_Utilities::CreatePolygonWire(aPoints, Standard_True);
  TopoDS_Shape       aFace  = BOPTest_Utilities::CreateFaceFromWire(aWire);
  const TopoDS_Shape aSolid = BOPTest_Utilities::CreatePrism(aFace, gp_Vec(0, 0, 2));

  // Create box: box b -1 2 1 5 1 3 (corner at (-1,2,1), dimensions 5x1x3)
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(-1, 2, 1), 5.0, 1.0, 3.0);

  // Perform boolean cut: bcut result sol b
  const TopoDS_Shape aResult = PerformCut(aSolid, aBox);
  ValidateResult(aResult, 68.0);
}

// Test bcut_simple/H3: Complex cylinder/cone fuse minus translated cylinder (pro13307 bug)
TEST_F(BCutSimpleTest, ComplexCylinderConeOperationPro13307_H3)
{
  // This is similar to G9 but with a different bug reference
  // Create cylinder and cone, fuse them
  const TopoDS_Shape aCylinder = BOPTest_Utilities::CreateCylinder(9.0, 3.0);
  const TopoDS_Shape aCone     = BOPTest_Utilities::CreateCone(7.0, 6.0, 4.0);

  BRepAlgoAPI_Fuse aFuseOp(aCylinder, aCone);
  EXPECT_TRUE(aFuseOp.IsDone()) << "Fuse operation failed";
  const TopoDS_Shape aBody = aFuseOp.Shape();

  // Create small cylinder and translate it (x direction as per comment)
  TopoDS_Shape aSmallCylinder = BOPTest_Utilities::CreateCylinder(1.0, 9.0);
  aSmallCylinder = BOPTest_Utilities::TranslateShape(aSmallCylinder, gp_Vec(5.0, 0.0, -2.0));

  const TopoDS_Shape aResult = PerformCut(aBody, aSmallCylinder);
  ValidateResult(aResult, 727.481);
}

// Test bcut_simple/H4: Complex profile operation FORWARD FORWARD
TEST_F(BCutSimpleTest, ComplexProfileForwardForward_H4)
{
  // Create profile p1: "profile p1 o 0 0 40 x 150 y 200 x -150"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                    // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0), // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 200.0), // y 200
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -150.0) // x -150
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: "profile p2 o 0 0 50 f 25 25 y 100 x 75 y -100"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, 25.0), // f 25 25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)      // y -100
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  // First cut operation: bcut po1 pr1 pr2
  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: "profile p3 p 0 0 -1 1 0 0 o 0 0 50 f 50 -75 x 75 y -100 x -75"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, -75.0), // f 50 -75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  // Final cut operation: bcut result po1 pr3
  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 98000.0);
}

// Test bcut_simple/H5: Complex profile operation FORWARD FORWARD (variation)
TEST_F(BCutSimpleTest, ComplexProfileForwardForwardVariation_H5)
{
  // H5 has same profiles as H4, but p3 has different sequence: "f 50 -75 y -100 x 75 y 100"

  // Create profile p1: same as H4 "profile p1 o 0 0 40 x 150 y 200 x -150"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                    // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0), // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 200.0), // y 200
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -150.0) // x -150
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: same as H4 "profile p2 o 0 0 50 f 25 25 y 100 x 75 y -100"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, 25.0), // f 25 25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)      // y -100
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: H5 variation "profile p3 p 0 0 -1 1 0 0 o 0 0 50 f 50 -75 y -100 x 75 y 100"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, -75.0), // f 50 -75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0)        // y 100
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 98000.0);
}

// Test bcut_simple/H6: Complex profile operation FORWARD REVERSED
TEST_F(BCutSimpleTest, ComplexProfileForwardReversed_H6)
{
  // Create profile p1: same as H4/H5 "profile p1 o 0 0 40 x 150 y 200 x -150"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                    // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0), // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 200.0), // y 200
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -150.0) // x -150
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: same as H4/H5 "profile p2 o 0 0 50 f 25 25 y 100 x 75 y -100"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, 25.0), // f 25 25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)      // y -100
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: H6 variation "profile p3 o 0 0 50 f 50 75 x 75 y 100 x -75"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, 75.0), // f 50 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)       // x -75
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 98000.0);
}

// Test bcut_simple/H7: Complex profile operation FORWARD REVERSED (variation)
TEST_F(BCutSimpleTest, ComplexProfileForwardReversedVariation_H7)
{
  // Create profile p1: same as other H tests "profile p1 o 0 0 40 x 150 y 200 x -150"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                    // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0), // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 200.0), // y 200
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -150.0) // x -150
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: same as other H tests "profile p2 o 0 0 50 f 25 25 y 100 x 75 y -100"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, 25.0), // f 25 25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)      // y -100
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: H7 variation "profile p3 o 0 0 50 f 50 75 y 100 x 75 y -100"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, 75.0), // f 50 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)      // y -100
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 98000.0);
}

// Test bcut_simple/H8: Complex profile operation REVERSED FORWARD
TEST_F(BCutSimpleTest, ComplexProfileReversedForward_H8)
{
  // Create profile p1: "profile p1 p 0 0 -1 1 0 0 o 0 0 40 y -200 x 150 y 200"
  gp_Pln aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z)); // Use same approach as working H4/H5
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                     // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -200.0), // y -200
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),  // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 200.0)   // y 200
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: "profile p2 p 0 0 -1 1 0 0 o 0 0 50 f 25 -25 x 75 y -100 x -75"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, -25.0), // f 25 -25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: "profile p3 p 0 0 -1 1 0 0 o 0 0 50 f 50 -75 x 75 y -100 x -75"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, -75.0), // f 50 -75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 98000.0);
}

// Test bcut_simple/H9: Complex profile operation REVERSED FORWARD (variation)
TEST_F(BCutSimpleTest, ComplexProfileReversedForwardVariation_H9)
{
  // Create profile p1: same as H8 "profile p1 p 0 0 -1 1 0 0 o 0 0 40 y -200 x 150 y 200"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                     // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -200.0), // y -200
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),  // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 200.0)   // y 200
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: same as H8 "profile p2 p 0 0 -1 1 0 0 o 0 0 50 f 25 -25 x 75 y -100 x -75"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, -25.0), // f 25 -25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: H9 variation "profile p3 p 0 0 -1 1 0 0 o 0 0 50 f 50 -75 y -100 x 75 y 100"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, -75.0), // f 50 -75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0)        // y 100
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 98000.0);
}

// Test bcut_simple/I1: Complex profile operation REVERSED REVERSED
TEST_F(BCutSimpleTest, ComplexProfileReversedReversed_I1)
{
  // Create profile p1: same as H8/H9 "profile p1 p 0 0 -1 1 0 0 o 0 0 40 y -200 x 150 y 200"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                     // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -200.0), // y -200
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),  // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 200.0)   // y 200
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: same as H8/H9 "profile p2 p 0 0 -1 1 0 0 o 0 0 50 f 25 -25 x 75 y -100 x
  // -75"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, -25.0), // f 25 -25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: I1 "profile p3 o 0 0 50 f 50 75 x 75 y 100 x -75" (similar to H6)
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, 75.0), // f 50 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)       // x -75
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 98000.0);
}

// Test bcut_simple/I2: Complex profile operation REVERSED REVERSED (variation)
TEST_F(BCutSimpleTest, ComplexProfileReversedReversedVariation_I2)
{
  // Create profile p1: same as I1 "profile p1 p 0 0 -1 1 0 0 o 0 0 40 y -200 x 150 y 200"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                     // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -200.0), // y -200
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),  // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 200.0)   // y 200
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: same as I1 "profile p2 p 0 0 -1 1 0 0 o 0 0 50 f 25 -25 x 75 y -100 x -75"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, -25.0), // f 25 -25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: I2 "profile p3 o 0 0 50 f 50 75 y 100 x 75 y -100" (similar to H7)
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, 75.0), // f 50 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)      // y -100
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 98000.0);
}

// Test bcut_simple/I3: Complex profile operation FORWARD FORWARD (variation)
TEST_F(BCutSimpleTest, ComplexProfileForwardForwardVariation2_I3)
{
  // Create profile p1: "profile p1 o 0 0 40 f 0 50 x 150 y 100 x -150"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                        // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 0.0, 50.0), // f 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),     // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),     // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -150.0)     // x -150
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: same as I1/I2 but different plane "profile p2 o 0 0 50 f 25 25 y 100 x 75 y
  // -100"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, 25.0), // f 25 25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)      // y -100
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: same as H8 "profile p3 p 0 0 -1 1 0 0 o 0 0 50 f 50 -75 x 75 y -100 x -75"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, -75.0), // f 50 -75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 52000.0);
}

// Test bcut_simple/I4: Complex profile operation FORWARD FORWARD (variation 2)
TEST_F(BCutSimpleTest, ComplexProfileForwardForwardVariation3_I4)
{
  // Create profile p1: same as I3 "profile p1 o 0 0 40 f 0 50 x 150 y 100 x -150"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                        // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 0.0, 50.0), // f 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),     // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),     // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -150.0)     // x -150
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: same as I3 "profile p2 o 0 0 50 f 25 25 y 100 x 75 y -100"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, 25.0), // f 25 25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)      // y -100
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: I4 "profile p3 p 0 0 -1 1 0 0 o 0 0 50 f 50 -75 y -100 x 75 y 100" with
  // prism height -3
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, -75.0), // f 50 -75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0)        // y 100
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3 =
    BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -3)); // Note: -3, not -30!

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 53000.0);
}

// Test bcut_simple/I5: Complex profile operation FORWARD REVERSED (variation 2)
TEST_F(BCutSimpleTest, ComplexProfileForwardReversedVariation2_I5)
{
  // Create profile p1: same as I3/I4 "profile p1 o 0 0 40 f 0 50 x 150 y 100 x -150"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                        // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 0.0, 50.0), // f 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),     // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),     // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -150.0)     // x -150
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: same as I3/I4 "profile p2 o 0 0 50 f 25 25 y 100 x 75 y -100"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, 25.0), // f 25 25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)      // y -100
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: I5 "profile p3 o 0 0 50 f 50 75 x 75 y 100 x -75" (similar to H6/I1)
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, 75.0), // f 50 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)       // x -75
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 52000.0);
}

// Test bcut_simple/I6: Complex profile operation FORWARD REVERSED (variation 3)
TEST_F(BCutSimpleTest, ComplexProfileForwardReversedVariation3_I6)
{
  // Same profiles as I5 but different p3 - "profile p3 o 0 0 50 f 50 75 y 100 x 75 y -100"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O, 0.0, 0.0, 40.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 0.0, 50.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -150.0)};
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O, 0.0, 0.0, 50.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, 25.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)};
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O, 0.0, 0.0, 50.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, 75.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0)};
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 52000.0);
}

// Test bcut_simple/I7: Complex profile operation REVERSED FORWARD (variation 2)
TEST_F(BCutSimpleTest, ComplexProfileReversedForwardVariation2_I7)
{
  // Create profile p1: "profile p1 p 0 0 -1 1 0 0 o 0 0 40 f 0 -50 y -100 x 150 y 100"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                         // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 0.0, -50.0), // f 0 -50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),     // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),      // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0)       // y 100
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: "profile p2 p 0 0 -1 1 0 0 o 0 0 50 f 25 -25 x 75 y -100 x -75"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, -25.0), // f 25 -25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: same as H8 "profile p3 p 0 0 -1 1 0 0 o 0 0 50 f 50 -75 x 75 y -100 x -75"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, -75.0), // f 50 -75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 52000.0);
}

// Test bcut_simple/I8: Complex profile operation REVERSED FORWARD (variation 3)
TEST_F(BCutSimpleTest, ComplexProfileReversedForwardVariation3_I8)
{
  // Create profile p1: "profile p1 p 0 0 -1 1 0 0 o 0 0 40 f 0 -50 y -100 x 150 y 100"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                         // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 0.0, -50.0), // f 0 -50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),     // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),      // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0)       // y 100
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: "profile p2 p 0 0 -1 1 0 0 o 0 0 50 f 25 -25 x 75 y -100 x -75"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, -25.0), // f 25 -25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: "profile p3 p 0 0 -1 1 0 0 o 0 0 50 f 50 -75 y -100 x 75 y 100"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, -75.0), // f 50 -75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0)        // y 100
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 52000.0);
}

// Test bcut_simple/I9: Complex profile operation REVERSED REVERSED (variation 2)
TEST_F(BCutSimpleTest, ComplexProfileReversedReversedVariation2_I9)
{
  // Create profile p1: "profile p1 p 0 0 -1 1 0 0 o 0 0 40 f 0 -50 y -100 x 150 y 100"
  gp_Pln                                           aPlane1(gp_Pnt(0, 0, 40), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps1 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        40.0),                                         // o 0 0 40
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 0.0, -50.0), // f 0 -50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),     // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 150.0),      // x 150
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0)       // y 100
  };
  const TopoDS_Shape aProfile1 = BOPTest_Utilities::CreateProfile(aPlane1, aProfileOps1);
  const TopoDS_Shape aPrism1   = BOPTest_Utilities::CreatePrism(aProfile1, gp_Vec(0, 0, -40));

  // Create profile p2: "profile p2 p 0 0 -1 1 0 0 o 0 0 50 f 25 -25 x 75 y -100 x -75"
  gp_Pln                                           aPlane2(gp_Pnt(0, 0, 50), gp_Dir(gp_Dir::D::Z));
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps2 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                          // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 25.0, -25.0), // f 25 -25
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),        // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, -100.0),      // y -100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)        // x -75
  };
  const TopoDS_Shape aProfile2 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps2);
  const TopoDS_Shape aPrism2   = BOPTest_Utilities::CreatePrism(aProfile2, gp_Vec(0, 0, -30));

  BRepAlgoAPI_Cut aCutOp1(aPrism1, aPrism2);
  EXPECT_TRUE(aCutOp1.IsDone()) << "First cut operation failed";
  const TopoDS_Shape aIntermediate = aCutOp1.Shape();

  // Create profile p3: "profile p3 o 0 0 50 f 50 75 x 75 y 100 x -75"
  std::vector<BOPTest_Utilities::ProfileOperation> aProfileOps3 = {
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::O,
                                        0.0,
                                        0.0,
                                        50.0),                                         // o 0 0 50
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::F, 50.0, 75.0), // f 50 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, 75.0),       // x 75
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::Y, 100.0),      // y 100
    BOPTest_Utilities::ProfileOperation(BOPTest_Utilities::ProfileCmd::X, -75.0)       // x -75
  };
  const TopoDS_Shape aProfile3 = BOPTest_Utilities::CreateProfile(aPlane2, aProfileOps3);
  const TopoDS_Shape aPrism3   = BOPTest_Utilities::CreatePrism(aProfile3, gp_Vec(0, 0, -30));

  const TopoDS_Shape aResult = PerformCut(aIntermediate, aPrism3);
  ValidateResult(aResult, 52000.0);
}
