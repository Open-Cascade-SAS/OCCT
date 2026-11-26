// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRep_Builder.hxx>
#include <BRepCheck_Solid.hxx>
#include <BRepCheck_Status.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Solid.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

TEST(BRepCheck_SolidTest, InitializesAllShellsAfterEnclosedRegion)
{
  BRep_Builder aBuilder;
  TopoDS_Solid aSolid;
  aBuilder.MakeSolid(aSolid);

  // All four shells are outward oriented and nested from largest to smallest.
  // Pairwise comparisons therefore reach the third and fourth shells.
  for (int aHalfSize = 4; aHalfSize >= 1; --aHalfSize)
  {
    const gp_Pnt aCorner(-aHalfSize, -aHalfSize, -aHalfSize);
    BRepPrimAPI_MakeBox aBox(aCorner, 2.0 * aHalfSize, 2.0 * aHalfSize, 2.0 * aHalfSize);
    TopExp_Explorer anExplorer(aBox.Solid(), TopAbs_SHELL);
    ASSERT_TRUE(anExplorer.More());
    aBuilder.Add(aSolid, anExplorer.Current());
  }

  BRepCheck_Solid aCheck(aSolid);
  aCheck.Minimum();
  int anEnclosedRegionCount = 0;
  for (const BRepCheck_Status aStatus : aCheck.Status())
  {
    if (aStatus == BRepCheck_EnclosedRegion)
    {
      ++anEnclosedRegionCount;
    }
    EXPECT_NE(BRepCheck_SubshapeNotInShape, aStatus);
  }
  EXPECT_EQ(1, anEnclosedRegionCount);
}
