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

#include "BOPTest_Utilities.pxx"

#include <cmath>

#include <BOPAlgo_Splitter.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAlgoAPI_Splitter.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

#include <Geom_CylindricalSurface.hxx>
#include <Geom_Curve.hxx>

#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Iterator.hxx>

namespace
{
int CountShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  TopExp::MapShapes(theShape, aMap);

  int aCount = 0;
  for (int anIndex = 1; anIndex <= aMap.Extent(); ++anIndex)
  {
    if (aMap(anIndex).ShapeType() == theType)
    {
      ++aCount;
    }
  }
  return aCount;
}

void ExpectShapeCount(const TopoDS_Shape&    theShape,
                      const TopAbs_ShapeEnum theType,
                      const int              theExpected)
{
  if (!theShape.IsNull())
  {
    EXPECT_TRUE(BRepCheck_Analyzer(theShape).IsValid());
  }
  EXPECT_EQ(CountShapes(theShape, theType), theExpected);
}

TopoDS_Face MakePlaneFace(const double theFirstU,
                          const double theLastU,
                          const double theFirstV,
                          const double theLastV,
                          const double theZ = 0.0)
{
  const gp_Pln aPlane(gp_Pnt(0.0, 0.0, theZ), gp::DZ());
  return BRepBuilderAPI_MakeFace(aPlane, theFirstU, theLastU, theFirstV, theLastV).Face();
}

TopoDS_Face MakeCylinderFace()
{
  const occ::handle<Geom_CylindricalSurface> aSurface =
    new Geom_CylindricalSurface(gp::XOY(), 10.0);
  return BRepBuilderAPI_MakeFace(aSurface,
                                 0.0,
                                 2.0 * 3.14159265358979323846,
                                 -20.0,
                                 20.0,
                                 Precision::Confusion())
    .Face();
}

TopoDS_Compound MakeCylinderIsoEdges()
{
  BRep_Builder    aBuilder;
  TopoDS_Compound anEdges;
  aBuilder.MakeCompound(anEdges);
  const occ::handle<Geom_CylindricalSurface> aSurface =
    new Geom_CylindricalSurface(gp::XOY(), 10.0);

  const double aPi       = 3.14159265358979323846;
  const int    aNbUEdges = 10;
  for (int anIndex = 0; anIndex <= aNbUEdges; ++anIndex)
  {
    const double                       anAngle = 2.0 * aPi * double(anIndex) / double(aNbUEdges);
    const occ::handle<Geom_Curve>       aCurve = aSurface->UIso(anAngle);
    aBuilder.Add(anEdges, BRepBuilderAPI_MakeEdge(aCurve, -25.0, 25.0).Edge());
  }

  const int aNbVEdges = 10;
  for (int anIndex = 0; anIndex <= aNbVEdges; ++anIndex)
  {
    const double aZ = -20.0 + 40.0 * double(anIndex) / double(aNbVEdges);
    const occ::handle<Geom_Curve> aCurve = aSurface->VIso(aZ);
    aBuilder.Add(anEdges, BRepBuilderAPI_MakeEdge(aCurve).Edge());
  }
  return anEdges;
}

TopoDS_Compound MakeCylinderGrid()
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCylinders;
  aBuilder.MakeCompound(aCylinders);
  for (int anX = 0; anX <= 100; anX += 5)
  {
    for (int anY = 0; anY <= 100; anY += 5)
    {
      const gp_Ax2 anAxis(gp_Pnt(double(anX), double(anY), -5.0), gp::DZ());
      aBuilder.Add(aCylinders, BRepPrimAPI_MakeCylinder(anAxis, 1.0, 11.0).Shape());
    }
  }
  return aCylinders;
}

TopoDS_Shape SplitWithBOPAlgo(const TopoDS_Shape& theObject, const TopoDS_Shape& theTool)
{
  BOPAlgo_Splitter aSplitter;
  aSplitter.AddArgument(theObject);
  aSplitter.AddTool(theTool);
  aSplitter.SetRunParallel(false);
  aSplitter.Perform();
  EXPECT_FALSE(aSplitter.HasErrors());
  return aSplitter.Shape();
}

TopoDS_Shape SplitWithBRepAlgoAPI(const TopoDS_Shape& theObject, const TopoDS_Shape& theTool)
{
  BRepAlgoAPI_Splitter           aSplitter;
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(theObject);
  aSplitter.SetArguments(anArguments);

  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(theTool);
  aSplitter.SetTools(aTools);
  aSplitter.SetRunParallel(false);
  aSplitter.Build();
  EXPECT_FALSE(aSplitter.HasErrors());
  return aSplitter.Shape();
}

TopoDS_Shape SplitCylinderFaceWithBOPAlgo(const TopoDS_Shape& theObject,
                                          const TopoDS_Shape& theTools)
{
  BOPAlgo_Splitter aSplitter;
  aSplitter.AddArgument(theObject);
  for (TopoDS_Iterator anIterator(theTools); anIterator.More(); anIterator.Next())
  {
    aSplitter.AddTool(anIterator.Value());
  }
  aSplitter.SetRunParallel(false);
  aSplitter.Perform();
  EXPECT_FALSE(aSplitter.HasErrors());
  return aSplitter.Shape();
}

TopoDS_Shape SplitCylinderFaceWithBRepAlgoAPI(const TopoDS_Shape& theObject,
                                              const TopoDS_Shape& theTools)
{
  BRepAlgoAPI_Splitter           aSplitter;
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(theObject);
  aSplitter.SetArguments(anArguments);
  NCollection_List<TopoDS_Shape> aTools;
  for (TopoDS_Iterator anIterator(theTools); anIterator.More(); anIterator.Next())
  {
    aTools.Append(anIterator.Value());
  }
  aSplitter.SetTools(aTools);
  aSplitter.SetRunParallel(false);
  aSplitter.Build();
  EXPECT_FALSE(aSplitter.HasErrors());
  return aSplitter.Shape();
}
} // namespace

// Equivalent to tests/boolean/splitter/A1.
TEST(BRepAlgoAPI_SplitterTest, A1_BoxSplitByCrossingBoxes_ProducesNineSolids)
{
  const TopoDS_Shape aBox   = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aToolX = BRepPrimAPI_MakeBox(gp_Pnt(4.0, -2.0, -2.0), 2.0, 14.0, 14.0).Shape();
  const TopoDS_Shape aToolY = BRepPrimAPI_MakeBox(gp_Pnt(-2.0, 4.0, -2.0), 14.0, 2.0, 14.0).Shape();

  BOPAlgo_Splitter aSplitter;
  aSplitter.AddArgument(aBox);
  aSplitter.AddTool(aToolX);
  aSplitter.AddTool(aToolY);
  aSplitter.SetRunParallel(false);
  aSplitter.Perform();

  ASSERT_FALSE(aSplitter.HasErrors());
  const TopoDS_Shape& aResult = aSplitter.Shape();
  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCount(aResult, TopAbs_SOLID, 9);
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aResult), 1000.0, Precision::Confusion());
}

// Equivalent to tests/boolean/splitter/A2.
TEST(BRepAlgoAPI_SplitterTest, A2_BoxSplitBySolidAndFace_ProducesFourSolids)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aToolBox =
    BRepPrimAPI_MakeBox(gp_Pnt(5.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aToolFace = MakePlaneFace(-20.0, 20.0, -20.0, 20.0, 5.0);

  BOPAlgo_Splitter aSplitter;
  aSplitter.AddArgument(aBox);
  aSplitter.AddTool(aToolBox);
  aSplitter.AddTool(aToolFace);
  aSplitter.SetRunParallel(false);
  aSplitter.Perform();
  ASSERT_FALSE(aSplitter.HasErrors());

  const TopoDS_Shape& aSplitResult = aSplitter.Shape();
  ASSERT_FALSE(aSplitResult.IsNull());
  ExpectShapeCount(aSplitResult, TopAbs_SOLID, 4);
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aSplitResult), 1000.0, Precision::Confusion());
}

// Equivalent to tests/boolean/splitter/A3.
TEST(BRepAlgoAPI_SplitterTest, A3_PlaneSplitBySphere_ProducesTwoFaces)
{
  const TopoDS_Shape aFace   = MakePlaneFace(-20.0, 20.0, -20.0, 20.0);
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(5.0).Shape();
  const TopoDS_Shape aResult = SplitWithBOPAlgo(aFace, aSphere);

  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCount(aResult, TopAbs_FACE, 2);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aResult), 1600.0, Precision::Confusion());
}

// Equivalent to tests/boolean/splitter/B1.
TEST(BRepAlgoAPI_SplitterTest, B1_ApiBoxSplitByCrossingBoxes_ProducesNineSolids)
{
  const TopoDS_Shape aBox   = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aToolX = BRepPrimAPI_MakeBox(gp_Pnt(4.0, -2.0, -2.0), 2.0, 14.0, 14.0).Shape();
  const TopoDS_Shape aToolY = BRepPrimAPI_MakeBox(gp_Pnt(-2.0, 4.0, -2.0), 14.0, 2.0, 14.0).Shape();

  BRepAlgoAPI_Splitter           aSplitter;
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(aBox);
  aSplitter.SetArguments(anArguments);
  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(aToolX);
  aTools.Append(aToolY);
  aSplitter.SetTools(aTools);
  aSplitter.SetRunParallel(false);
  aSplitter.Build();

  ASSERT_FALSE(aSplitter.HasErrors());
  const TopoDS_Shape& aResult = aSplitter.Shape();
  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCount(aResult, TopAbs_SOLID, 9);
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aResult), 1000.0, Precision::Confusion());
}

// Equivalent to tests/boolean/splitter/B2.
TEST(BRepAlgoAPI_SplitterTest, B2_ApiBoxSplitBySolidAndFace_ProducesFourSolids)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aToolBox =
    BRepPrimAPI_MakeBox(gp_Pnt(5.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aToolFace = MakePlaneFace(-20.0, 20.0, -20.0, 20.0, 5.0);

  BRepAlgoAPI_Splitter           aSplitter;
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(aBox);
  aSplitter.SetArguments(anArguments);
  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(aToolBox);
  aTools.Append(aToolFace);
  aSplitter.SetTools(aTools);
  aSplitter.SetRunParallel(false);
  aSplitter.Build();
  ASSERT_FALSE(aSplitter.HasErrors());

  const TopoDS_Shape& aSplitResult = aSplitter.Shape();
  ASSERT_FALSE(aSplitResult.IsNull());
  ExpectShapeCount(aSplitResult, TopAbs_SOLID, 4);
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aSplitResult), 1000.0, Precision::Confusion());
}

// Equivalent to tests/boolean/splitter/B3.
TEST(BRepAlgoAPI_SplitterTest, B3_ApiPlaneSplitBySphere_ProducesTwoFaces)
{
  const TopoDS_Shape aFace   = MakePlaneFace(-20.0, 20.0, -20.0, 20.0);
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(5.0).Shape();
  const TopoDS_Shape aResult = SplitWithBRepAlgoAPI(aFace, aSphere);

  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCount(aResult, TopAbs_FACE, 2);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aResult), 1600.0, Precision::Confusion());
}

// Equivalent to tests/boolean/splitter/A4.
TEST(BRepAlgoAPI_SplitterTest, A4_CylinderFaceSplitByIsoEdges_ProducesHundredFaces)
{
  const TopoDS_Shape aFace   = MakeCylinderFace();
  const TopoDS_Shape anEdges = MakeCylinderIsoEdges();
  const TopoDS_Shape aResult = SplitCylinderFaceWithBOPAlgo(aFace, anEdges);

  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCount(aResult, TopAbs_FACE, 100);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aResult), 2513.27, 0.01);
}

// Equivalent to tests/boolean/splitter/B4.
TEST(BRepAlgoAPI_SplitterTest, B4_ApiCylinderFaceSplitByIsoEdges_ProducesHundredFaces)
{
  const TopoDS_Shape aFace   = MakeCylinderFace();
  const TopoDS_Shape anEdges = MakeCylinderIsoEdges();
  const TopoDS_Shape aResult = SplitCylinderFaceWithBRepAlgoAPI(aFace, anEdges);

  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCount(aResult, TopAbs_FACE, 100);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aResult), 2513.27, 0.01);
}

// Equivalent to tests/boolean/splitter/A6.
TEST(BRepAlgoAPI_SplitterTest, A6_PlateSplitByCylinderGrid_ProducesFourHundredFortyTwoSolids)
{
  const TopoDS_Shape aPlate     = BRepPrimAPI_MakeBox(100.0, 100.0, 1.0).Shape();
  const TopoDS_Shape aCylinders = MakeCylinderGrid();
  const TopoDS_Shape aResult    = SplitWithBOPAlgo(aPlate, aCylinders);

  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCount(aResult, TopAbs_SOLID, 442);
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aResult), 10000.0, Precision::Confusion());
}

// Equivalent to tests/boolean/splitter/B6.
TEST(BRepAlgoAPI_SplitterTest, B6_ApiPlateSplitByCylinderGrid_ProducesFourHundredFortyTwoSolids)
{
  const TopoDS_Shape aPlate     = BRepPrimAPI_MakeBox(100.0, 100.0, 1.0).Shape();
  const TopoDS_Shape aCylinders = MakeCylinderGrid();
  const TopoDS_Shape aResult    = SplitWithBRepAlgoAPI(aPlate, aCylinders);

  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCount(aResult, TopAbs_SOLID, 442);
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aResult), 10000.0, Precision::Confusion());
}
