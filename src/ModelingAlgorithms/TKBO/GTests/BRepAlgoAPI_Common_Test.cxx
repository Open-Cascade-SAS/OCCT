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

//=================================================================================================
// BOPCommon Simple Tests - migrating from /tests/boolean/bopcommon_simple/
//=================================================================================================

class BOPCommonSimpleTest : public BRepAlgoAPI_TestBase
{
};

// Test bopcommon_simple/A1: box b1 0 0 0 1 1 1; box b2 0 0 0 1 1 1; bop b1 b2; bopcommon result;
// checkprops result -s 6
TEST_F(BOPCommonSimpleTest, IdenticalBoxes_A1)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformCommon(aBox1, aBox2);
  ValidateResult(aResult, 6.0);
}

// Migrated from tests/bugs/modalg_5/bug23855.  The common of two coincident
// spheres must preserve the complete solid, not produce a partial shell.
TEST_F(BOPCommonSimpleTest, OCC23855_CoincidentSpheresPreserveSolid)
{
  const TopoDS_Shape aSphere1 = BRepPrimAPI_MakeSphere(10.0).Shape();
  const TopoDS_Shape aSphere2 = BRepPrimAPI_MakeSphere(10.0).Shape();
  const TopoDS_Shape aResult  = PerformCommon(aSphere1, aSphere2);
  ASSERT_FALSE(aResult.IsNull());

  int aNbSolids = 0;
  int aNbFaces  = 0;
  for (TopExp_Explorer anExplorer(aResult, TopAbs_SOLID); anExplorer.More();
       anExplorer.Next())
  {
    ++aNbSolids;
  }
  for (TopExp_Explorer anExplorer(aResult, TopAbs_FACE); anExplorer.More();
       anExplorer.Next())
  {
    ++aNbFaces;
  }
  EXPECT_EQ(aNbSolids, 1);
  EXPECT_EQ(aNbFaces, 1);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aResult), 4.0 * M_PI * 100.0, 1.e-6);
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aResult), 4.0 * M_PI * 1000.0 / 3.0, 1.e-6);
}
