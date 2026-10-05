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
#include <BRepGProp.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Map.hxx>
#include <ShapeUpgrade_ShellSewing.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

#include <gtest/gtest.h>

namespace
{
//=================================================================================================

TopoDS_Face makeRectangle(const double theXMin, const double theXMax)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(theXMin, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(theXMax, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(theXMax, 1.0, 0.0));
  aPolygon.Add(gp_Pnt(theXMin, 1.0, 0.0));
  aPolygon.Close();
  return BRepBuilderAPI_MakeFace(aPolygon.Wire()).Face();
}

//=================================================================================================

TopoDS_Shell makeShell(const TopoDS_Face& theFace1, const TopoDS_Face& theFace2)
{
  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aShell, theFace1);
  aBuilder.Add(aShell, theFace2);
  return aShell;
}

//=================================================================================================

size_t countSubShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aSubShapes;
  for (TopExp_Explorer aSubShapeExplorer(theShape, theType); aSubShapeExplorer.More();
       aSubShapeExplorer.Next())
  {
    aSubShapes.Add(aSubShapeExplorer.Current());
  }
  return aSubShapes.Extent();
}

//=================================================================================================

double surfaceArea(const TopoDS_Shape& theShape)
{
  GProp_GProps aProperties;
  BRepGProp::SurfaceProperties(theShape, aProperties);
  return aProperties.Mass();
}
} // namespace

//=================================================================================================

TEST(ShapeUpgrade_ShellSewingTest, NullInputReturnsNull)
{
  ShapeUpgrade_ShellSewing aSewer;
  EXPECT_TRUE(aSewer.ApplySewing(TopoDS_Shape()).IsNull());
}

TEST(ShapeUpgrade_ShellSewingTest, ShapeWithoutShellReturnsNull)
{
  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Face        aFace = makeRectangle(0.0, 1.0);

  EXPECT_TRUE(aSewer.ApplySewing(aFace, 1.0e-6).IsNull());
}

TEST(ShapeUpgrade_ShellSewingTest, EmptyShellReturnsNull)
{
  BRep_Builder aBuilder;
  TopoDS_Shell anEmptyShell;
  aBuilder.MakeShell(anEmptyShell);

  ShapeUpgrade_ShellSewing aSewer;
  EXPECT_TRUE(aSewer.ApplySewing(anEmptyShell, 1.0e-6).IsNull());
}

TEST(ShapeUpgrade_ShellSewingTest, SewsOpenShellWithExplicitTolerance)
{
  const TopoDS_Shell aShell      = makeShell(makeRectangle(0.0, 1.0), makeRectangle(1.0001, 2.0));
  const double       anInputArea = surfaceArea(aShell);

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aShell, 1.0e-3);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 2);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_EDGE), 7);
  EXPECT_NEAR(anInputArea, 1.9999, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), anInputArea, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), 1.9999, 1.0e-12);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeUpgrade_ShellSewingTest, AutomaticTolerancePreservesValidBox)
{
  const TopoDS_Shape aBox        = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();
  const double       anInputArea = surfaceArea(aBox);

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aBox);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_SOLID), 1);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 6);
  EXPECT_NEAR(anInputArea, 52.0, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), anInputArea, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), 52.0, 1.0e-12);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeUpgrade_ShellSewingTest, ExplicitTolerancePreservesValidBox)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aBox, 1.0e-5);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_SOLID), 1);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 6);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeUpgrade_ShellSewingTest, ReorientsInsideOutSolid)
{
  TopoDS_Solid aSolid = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Solid();
  aSolid.Reverse();

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aSolid, 1.0e-6);
  ASSERT_FALSE(aResult.IsNull());
  ASSERT_EQ(countSubShapes(aResult, TopAbs_SOLID), 1);

  const TopoDS_Solid aResultSolid = TopoDS::Solid(TopExp_Explorer(aResult, TopAbs_SOLID).Current());
  BRepClass3d_SolidClassifier aClassifier(aResultSolid);
  aClassifier.PerformInfinitePoint(1.0e-6);
  EXPECT_EQ(aClassifier.State(), TopAbs_OUT);
}

TEST(ShapeUpgrade_ShellSewingTest, ProcessesMultipleShellsInCompound)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(10.0, 0.0, 0.0), 2.0, 3.0, 4.0).Shape();
  BRep_Builder       aBuilder;
  TopoDS_Compound    aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);
  const double anInputArea = surfaceArea(aCompound);

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aCompound, 1.0e-6);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_SOLID), 2);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_SHELL), 2);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 12);
  EXPECT_NEAR(anInputArea, 104.0, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), anInputArea, 1.0e-12);
  EXPECT_NEAR(surfaceArea(aResult), 104.0, 1.0e-12);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeUpgrade_ShellSewingTest, InstanceCanProcessSuccessiveShapes)
{
  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape aResult1 = aSewer.ApplySewing(BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape());
  const TopoDS_Shape aResult2 =
    aSewer.ApplySewing(BRepPrimAPI_MakeBox(gp_Pnt(10.0, 0.0, 0.0), 4.0, 5.0, 6.0).Shape());

  ASSERT_FALSE(aResult1.IsNull());
  ASSERT_FALSE(aResult2.IsNull());
  EXPECT_EQ(countSubShapes(aResult2, TopAbs_SOLID), 1);
  EXPECT_EQ(countSubShapes(aResult2, TopAbs_FACE), 6);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult2).IsValid());
}

TEST(ShapeUpgrade_ShellSewingTest, SewsExactlyCoincidentOpenShell)
{
  const TopoDS_Shell aShell = makeShell(makeRectangle(0.0, 1.0), makeRectangle(1.0, 2.0));

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aShell, 1.0e-7);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 2);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_EDGE), 7);
}

TEST(ShapeUpgrade_ShellSewingTest, KeepsEdgesBeyondExplicitToleranceSeparate)
{
  const TopoDS_Shell aShell = makeShell(makeRectangle(0.0, 1.0), makeRectangle(1.01, 2.0));

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aShell, 1.0e-4);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 2);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_EDGE), 8);
}

TEST(ShapeUpgrade_ShellSewingTest, NegativeToleranceUsesAutomaticTolerance)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aBox, -1.0);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_SOLID), 1);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 6);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeUpgrade_ShellSewingTest, CompoundWithoutShellReturnsNull)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, makeRectangle(0.0, 1.0));
  aBuilder.Add(aCompound, makeRectangle(2.0, 3.0));

  ShapeUpgrade_ShellSewing aSewer;
  EXPECT_TRUE(aSewer.ApplySewing(aCompound, 1.0e-6).IsNull());
}

TEST(ShapeUpgrade_ShellSewingTest, PreservesFaceOutsideProcessedShell)
{
  const TopoDS_Shell aShell          = makeShell(makeRectangle(0.0, 1.0), makeRectangle(1.0, 2.0));
  const TopoDS_Face  aStandaloneFace = makeRectangle(10.0, 11.0);
  BRep_Builder       aBuilder;
  TopoDS_Compound    aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aShell);
  aBuilder.Add(aCompound, aStandaloneFace);

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aCompound, 1.0e-6);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 3);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_EDGE), 11);
}

TEST(ShapeUpgrade_ShellSewingTest, HandlesReversedOpenShell)
{
  TopoDS_Shell aShell = makeShell(makeRectangle(0.0, 1.0), makeRectangle(1.0, 2.0));
  aShell.Reverse();

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aShell, 1.0e-6);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 2);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_EDGE), 7);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

TEST(ShapeUpgrade_ShellSewingTest, ReapplyingToSewedShapeIsStable)
{
  const TopoDS_Shell       aShell = makeShell(makeRectangle(0.0, 1.0), makeRectangle(1.0, 2.0));
  ShapeUpgrade_ShellSewing aSewer;

  const TopoDS_Shape aResult1 = aSewer.ApplySewing(aShell, 1.0e-6);
  ASSERT_FALSE(aResult1.IsNull());
  const TopoDS_Shape aResult2 = aSewer.ApplySewing(aResult1, 1.0e-6);

  ASSERT_FALSE(aResult2.IsNull());
  EXPECT_EQ(countSubShapes(aResult2, TopAbs_FACE), 2);
  EXPECT_EQ(countSubShapes(aResult2, TopAbs_EDGE), 7);
}

TEST(ShapeUpgrade_ShellSewingTest, ProcessesSingleFaceShell)
{
  const TopoDS_Face aFace = makeRectangle(0.0, 1.0);
  BRep_Builder      aBuilder;
  TopoDS_Shell      aShell;
  aBuilder.MakeShell(aShell);
  aBuilder.Add(aShell, aFace);

  ShapeUpgrade_ShellSewing aSewer;
  const TopoDS_Shape       aResult = aSewer.ApplySewing(aShell, 1.0e-6);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(countSubShapes(aResult, TopAbs_FACE), 1);
  EXPECT_EQ(countSubShapes(aResult, TopAbs_EDGE), 4);
}
