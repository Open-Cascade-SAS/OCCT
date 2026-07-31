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

#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <gp_Ax2.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(BRepPrimAPI_MakeConeTest, FullCone)
{
  BRepPrimAPI_MakeCone aCone(gp_Ax2(), 5.0, 0.0, 10.0);
  TopoDS_Shape         aShape = aCone.Shape();

  ASSERT_TRUE(aCone.IsDone()) << "Full cone creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Full cone shape is not valid";
}

TEST(BRepPrimAPI_MakeConeTest, TruncatedCone)
{
  BRepPrimAPI_MakeCone aCone(gp_Ax2(), 5.0, 2.0, 10.0);
  TopoDS_Shape         aShape = aCone.Shape();

  ASSERT_TRUE(aCone.IsDone()) << "Truncated cone creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Truncated cone shape is not valid";
}

TEST(BRepPrimAPI_MakeConeTest, FullConeFaceCount)
{
  BRepPrimAPI_MakeCone aCone(gp_Ax2(), 5.0, 0.0, 10.0);
  TopoDS_Shape         aShape = aCone.Shape();
  ASSERT_TRUE(aCone.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 2) << "Full cone should have 2 faces (lateral + base)";
}

TEST(BRepPrimAPI_MakeConeTest, TruncatedConeFaceCount)
{
  BRepPrimAPI_MakeCone aCone(gp_Ax2(), 5.0, 2.0, 10.0);
  TopoDS_Shape         aShape = aCone.Shape();
  ASSERT_TRUE(aCone.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 3) << "Truncated cone should have 3 faces (lateral + 2 bases)";
}

TEST(BRepPrimAPI_MakeConeTest, TruncatedConeVolume)
{
  const double aR1 = 5.0;
  const double aR2 = 2.0;
  const double aH  = 10.0;

  BRepPrimAPI_MakeCone aCone(gp_Ax2(), aR1, aR2, aH);
  TopoDS_Shape         aShape = aCone.Shape();
  ASSERT_TRUE(aCone.IsDone());

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aShape, aProps);

  // V = PI * H / 3 * (R1^2 + R1*R2 + R2^2)
  const double anExpectedVolume = M_PI * aH / 3.0 * (aR1 * aR1 + aR1 * aR2 + aR2 * aR2);
  EXPECT_NEAR(aProps.Mass(), anExpectedVolume, Precision::Confusion());
}

TEST(BRepPrimAPI_MakeConeTest, PartialCone)
{
  BRepPrimAPI_MakeCone aCone(gp_Ax2(), 5.0, 2.0, 10.0, M_PI);
  TopoDS_Shape         aShape = aCone.Shape();

  ASSERT_TRUE(aCone.IsDone()) << "Partial cone creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Partial cone shape is not valid";

  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_GT(aFaceCount, 3) << "Partial cone should have more than 3 faces";
}
