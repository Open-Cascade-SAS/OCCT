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

//==================================================================================================
// Direct BOP Operations Tests (equivalent to bcut, bfuse, bcommon, btuc commands)
//==================================================================================================

class BOPAlgo_DirectOperationsTest : public BOPAlgo_TestBase
{
};

// Test direct cut operation: bcut result sphere box
TEST_F(BOPAlgo_DirectOperationsTest, DirectCut_SphereMinusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformDirectBOP(aSphere, aBox, BOPAlgo_CUT);
  EXPECT_FALSE(aResult.IsNull()) << "Result shape should not be null";

  const Standard_Real aSurfaceArea = BOPTest_Utilities::GetSurfaceArea(aResult);
  EXPECT_GT(aSurfaceArea, 0.0) << "Cut result should have positive surface area";
}

// Test direct fuse operation: bfuse result sphere box
TEST_F(BOPAlgo_DirectOperationsTest, DirectFuse_SpherePlusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformDirectBOP(aSphere, aBox, BOPAlgo_FUSE);
  EXPECT_FALSE(aResult.IsNull()) << "Result shape should not be null";

  const Standard_Real aVolume       = BOPTest_Utilities::GetVolume(aResult);
  const Standard_Real aSphereVolume = BOPTest_Utilities::GetVolume(aSphere);
  const Standard_Real aBoxVolume    = BOPTest_Utilities::GetVolume(aBox);
  EXPECT_GT(aVolume, aSphereVolume) << "Fuse result should be larger than sphere alone";
  EXPECT_GT(aVolume, aBoxVolume) << "Fuse result should be larger than box alone";
}

// Test direct common operation: bcommon result box1 box2
TEST_F(BOPAlgo_DirectOperationsTest, DirectCommon_OverlappingBoxes)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 2.0, 2.0, 2.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(1, 1, 1), 2.0, 2.0, 2.0);

  const TopoDS_Shape aResult = PerformDirectBOP(aBox1, aBox2, BOPAlgo_COMMON);
  ValidateResult(aResult, -1.0, 1.0); // Expected volume = 1.0
}

// Test direct tuc operation: btuc result box1 box2
TEST_F(BOPAlgo_DirectOperationsTest, DirectTUC_IdenticalBoxes)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformDirectBOP(aBox1, aBox2, BOPAlgo_CUT21);
  ValidateResult(aResult, -1.0, -1.0, Standard_True); // Expected empty
}

// Test with NURBS converted shapes
TEST_F(BOPAlgo_DirectOperationsTest, DirectCut_NurbsBoxMinusBox)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1              = BOPTest_Utilities::ConvertToNurbs(aBox1);
  EXPECT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 1, 0), 1.0, 0.5, 1.0);

  const TopoDS_Shape  aResult      = PerformDirectBOP(aBox1, aBox2, BOPAlgo_CUT);
  const Standard_Real aSurfaceArea = BOPTest_Utilities::GetSurfaceArea(aResult);
  EXPECT_GT(aSurfaceArea, 0.0) << "NURBS cut result should have positive surface area";
}

//==================================================================================================
// Two-step BOP Operations Tests (equivalent to bop + bopXXX commands)
//==================================================================================================

class BOPAlgo_TwoStepOperationsTest : public BOPAlgo_TestBase
{
};

// Test two-step cut operation: bop sphere box; bopcut result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepCut_SphereMinusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape  aResult      = PerformTwoStepBOP(aSphere, aBox, BOPAlgo_CUT);
  const Standard_Real aSurfaceArea = BOPTest_Utilities::GetSurfaceArea(aResult);
  EXPECT_GT(aSurfaceArea, 0.0) << "Two-step cut result should have positive surface area";
}

// Test two-step fuse operation: bop sphere box; bopfuse result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepFuse_SpherePlusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformTwoStepBOP(aSphere, aBox, BOPAlgo_FUSE);
  EXPECT_FALSE(aResult.IsNull()) << "Result shape should not be null";

  const Standard_Real aVolume       = BOPTest_Utilities::GetVolume(aResult);
  const Standard_Real aSphereVolume = BOPTest_Utilities::GetVolume(aSphere);
  const Standard_Real aBoxVolume    = BOPTest_Utilities::GetVolume(aBox);
  EXPECT_GT(aVolume, aSphereVolume) << "Two-step fuse result should be larger than sphere alone";
  EXPECT_GT(aVolume, aBoxVolume) << "Two-step fuse result should be larger than box alone";
}

// Test two-step common operation: bop box1 box2; bopcommon result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepCommon_OverlappingBoxes)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 2.0, 2.0, 2.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(1, 1, 1), 2.0, 2.0, 2.0);

  const TopoDS_Shape aResult = PerformTwoStepBOP(aBox1, aBox2, BOPAlgo_COMMON);
  ValidateResult(aResult, -1.0, 1.0); // Expected volume = 1.0
}

// Test two-step tuc operation: bop box1 box2; boptuc result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepTUC_IdenticalBoxes)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformTwoStepBOP(aBox1, aBox2, BOPAlgo_CUT21);
  ValidateResult(aResult, -1.0, -1.0, Standard_True); // Expected empty
}

//==================================================================================================
// Complex Operations Tests
//==================================================================================================

class BOPAlgo_ComplexOperationsTest : public BOPAlgo_TestBase
{
};

// Test multiple intersecting primitives
TEST_F(BOPAlgo_ComplexOperationsTest, MultipleIntersectingPrimitives)
{
  const TopoDS_Shape aSphere   = BOPTest_Utilities::CreateSphere(gp_Pnt(0, 0, 0), 1.5);
  const TopoDS_Shape aCylinder = BOPTest_Utilities::CreateCylinder(0.8, 3.0);
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(-0.5, -0.5, -0.5), 1.0, 1.0, 1.0);

  // First intersect sphere with cylinder
  const TopoDS_Shape aIntermediate = PerformDirectBOP(aSphere, aCylinder, BOPAlgo_COMMON);
  EXPECT_FALSE(aIntermediate.IsNull()) << "Intermediate result should not be null";

  // Then fuse with box
  const TopoDS_Shape  aFinalResult = PerformDirectBOP(aIntermediate, aBox, BOPAlgo_FUSE);
  const Standard_Real aVolume      = BOPTest_Utilities::GetVolume(aFinalResult);
  EXPECT_GT(aVolume, 0.0) << "Complex operation result should have positive volume";
}

// Test comparison between direct and two-step operations
TEST_F(BOPAlgo_ComplexOperationsTest, DirectVsTwoStepComparison)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  // Perform direct operation
  const TopoDS_Shape aDirectResult = PerformDirectBOP(aSphere, aBox, BOPAlgo_FUSE);

  // Perform two-step operation
  const TopoDS_Shape aTwoStepResult = PerformTwoStepBOP(aSphere, aBox, BOPAlgo_FUSE);

  // Results should be equivalent
  const Standard_Real aDirectVolume  = BOPTest_Utilities::GetVolume(aDirectResult);
  const Standard_Real aTwoStepVolume = BOPTest_Utilities::GetVolume(aTwoStepResult);

  EXPECT_NEAR(aDirectVolume, aTwoStepVolume, myTolerance)
    << "Direct and two-step operations should produce equivalent results";
}