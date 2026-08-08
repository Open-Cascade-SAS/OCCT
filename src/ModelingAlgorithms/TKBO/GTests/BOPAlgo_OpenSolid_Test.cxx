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

#include <gtest/gtest.h>

#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_CheckerSI.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pln.hxx>

namespace
{
struct ShapeExpectation
{
  int    myVertices;
  int    myEdges;
  int    myWires;
  int    myFaces;
  int    myShells;
  int    mySolids;
  double mySurface;
  double myVolume;
};

static int CountShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
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

static void ExpectNoSelfInterference(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
  {
    return;
  }

  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(theShape);
  BOPAlgo_CheckerSI aChecker;
  aChecker.SetArguments(anArguments);
  aChecker.SetLevelOfCheck(9);
  aChecker.SetFuzzyValue(Precision::Confusion());
  aChecker.SetRunParallel(false);
  aChecker.Perform();
  EXPECT_FALSE(aChecker.HasErrors());
}

static void ExpectShape(const TopoDS_Shape& theShape, const ShapeExpectation& theExpected)
{
  // Open-solid results are intentionally not closed. DRAW checkshape reports
  // BRepCheck_NotClosed for these results, while bopcheck is the actual
  // self-interference validation used by the test.
  ExpectNoSelfInterference(theShape);
  EXPECT_EQ(CountShapes(theShape, TopAbs_VERTEX), theExpected.myVertices);
  EXPECT_EQ(CountShapes(theShape, TopAbs_EDGE), theExpected.myEdges);
  EXPECT_EQ(CountShapes(theShape, TopAbs_WIRE), theExpected.myWires);
  EXPECT_EQ(CountShapes(theShape, TopAbs_FACE), theExpected.myFaces);
  EXPECT_EQ(CountShapes(theShape, TopAbs_SHELL), theExpected.myShells);
  EXPECT_EQ(CountShapes(theShape, TopAbs_SOLID), theExpected.mySolids);

  GProp_GProps aSurfaceProperties;
  BRepGProp::SurfaceProperties(theShape, aSurfaceProperties);
  EXPECT_NEAR(aSurfaceProperties.Mass(), theExpected.mySurface, 1.0e-6);
  if (theExpected.myVolume >= 0.0)
  {
    GProp_GProps aVolumeProperties;
    BRepGProp::VolumeProperties(theShape, aVolumeProperties);
    EXPECT_NEAR(aVolumeProperties.Mass(), theExpected.myVolume, 1.0e-6);
  }
}

static TopoDS_Solid MakeOpenSolid(const TopoDS_Shape& theSolid,
                                  const int           theFirstFace,
                                  const int           theLastFace,
                                  const TopoDS_Face&  theExtraFace = TopoDS_Face())
{
  BRep_Builder aBuilder;
  TopoDS_Shell aShell;
  aBuilder.MakeShell(aShell);
  int anIndex = 0;
  for (TopExp_Explorer anExplorer(theSolid, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    if (anIndex >= theFirstFace && anIndex <= theLastFace)
    {
      aBuilder.Add(aShell, anExplorer.Current());
    }
    ++anIndex;
  }

  TopoDS_Solid aResult;
  aBuilder.MakeSolid(aResult);
  aBuilder.Add(aResult, aShell);
  if (!theExtraFace.IsNull())
  {
    TopoDS_Shell anInternalShell;
    aBuilder.MakeShell(anInternalShell);
    aBuilder.Add(anInternalShell, theExtraFace);
    aBuilder.Add(aResult, anInternalShell);
  }
  return aResult;
}

static void CheckBOP(const NCollection_List<TopoDS_Shape>& theObjects,
                     const NCollection_List<TopoDS_Shape>& theTools,
                     const BOPAlgo_Operation               theOperation,
                     const ShapeExpectation&               theExpected)
{
  BOPAlgo_BOP anAlgorithm;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theObjects); anIterator.More();
       anIterator.Next())
  {
    anAlgorithm.AddArgument(anIterator.Value());
  }
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theTools); anIterator.More();
       anIterator.Next())
  {
    anAlgorithm.AddTool(anIterator.Value());
  }
  anAlgorithm.SetOperation(theOperation);
  anAlgorithm.SetRunParallel(false);
  anAlgorithm.Perform();
  ASSERT_FALSE(anAlgorithm.HasErrors());
  ExpectShape(anAlgorithm.Shape(), theExpected);
}

static void CheckBOPWithFiller(const NCollection_List<TopoDS_Shape>& theObjects,
                               const NCollection_List<TopoDS_Shape>& theTools,
                               const BOPAlgo_Operation               theOperation,
                               const ShapeExpectation&               theExpected)
{
  NCollection_List<TopoDS_Shape> anArguments = theObjects;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theTools); anIterator.More();
       anIterator.Next())
  {
    anArguments.Append(anIterator.Value());
  }

  BOPAlgo_PaveFiller aFiller;
  aFiller.SetArguments(anArguments);
  aFiller.SetFuzzyValue(Precision::Confusion());
  aFiller.SetRunParallel(false);
  aFiller.SetNonDestructive(false);
  aFiller.Perform();
  ASSERT_FALSE(aFiller.HasErrors());

  BOPAlgo_BOP anAlgorithm;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theObjects); anIterator.More();
       anIterator.Next())
  {
    anAlgorithm.AddArgument(anIterator.Value());
  }
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theTools); anIterator.More();
       anIterator.Next())
  {
    anAlgorithm.AddTool(anIterator.Value());
  }
  anAlgorithm.SetOperation(theOperation);
  anAlgorithm.SetRunParallel(false);
  anAlgorithm.SetCheckInverted(true);
  anAlgorithm.PerformWithFiller(aFiller);
  ASSERT_FALSE(anAlgorithm.HasErrors());
  ExpectShape(anAlgorithm.Shape(), theExpected);
}

static void CheckBuildBOP(const NCollection_List<TopoDS_Shape>& theArguments,
                          const NCollection_List<TopoDS_Shape>& theObjects,
                          const NCollection_List<TopoDS_Shape>& theTools,
                          const BOPAlgo_Operation               theOperation,
                          const ShapeExpectation&               theExpected,
                          const char*                           theLabel)
{
  SCOPED_TRACE(theLabel);
  BOPAlgo_PaveFiller aFiller;
  aFiller.SetArguments(theArguments);
  aFiller.SetFuzzyValue(Precision::Confusion());
  aFiller.SetRunParallel(false);
  aFiller.SetNonDestructive(false);
  aFiller.Perform();
  ASSERT_FALSE(aFiller.HasErrors());

  BOPAlgo_Builder aBuilder;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theArguments); anIterator.More();
       anIterator.Next())
  {
    aBuilder.AddArgument(anIterator.Value());
  }
  aBuilder.SetRunParallel(false);
  aBuilder.SetCheckInverted(true);
  aBuilder.PerformWithFiller(aFiller);
  ASSERT_FALSE(aBuilder.HasErrors());

  aBuilder.BuildBOP(theObjects, theTools, theOperation, Message_ProgressRange());
  ASSERT_FALSE(aBuilder.HasErrors());
  ExpectShape(aBuilder.Shape(), theExpected);
}

static void CheckTwoShapeBOP(const TopoDS_Shape&     theObject,
                             const TopoDS_Shape&     theTool,
                             const ShapeExpectation& theCommon,
                             const ShapeExpectation& theFuse,
                             const ShapeExpectation& theCut,
                             const ShapeExpectation& theTuc)
{
  NCollection_List<TopoDS_Shape> anObject;
  anObject.Append(theObject);
  NCollection_List<TopoDS_Shape> aTool;
  aTool.Append(theTool);
  CheckBOP(anObject, aTool, BOPAlgo_COMMON, theCommon);
  CheckBOP(anObject, aTool, BOPAlgo_FUSE, theFuse);
  CheckBOP(anObject, aTool, BOPAlgo_CUT, theCut);
  CheckBOP(aTool, anObject, BOPAlgo_CUT, theTuc);
}

static TopoDS_Shape MakeOpenBox(const gp_Pnt& theOrigin,
                                const double  theDx,
                                const double  theDy,
                                const double  theDz,
                                const int     theFirstFace = 0,
                                const int     theLastFace  = 4)
{
  return MakeOpenSolid(BRepPrimAPI_MakeBox(theOrigin, theDx, theDy, theDz).Shape(),
                       theFirstFace,
                       theLastFace);
}

static TopoDS_Face MakeInternalFace(const double theX)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(theX, 1.0, 1.0));
  aPolygon.Add(gp_Pnt(theX, 9.0, 1.0));
  aPolygon.Add(gp_Pnt(theX, 9.0, 9.0));
  aPolygon.Add(gp_Pnt(theX, 1.0, 9.0));
  aPolygon.Close();
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aPolygon.Wire()).Face();
  aFace.Orientation(TopAbs_INTERNAL);
  return aFace;
}

static ShapeExpectation OpenResult(const int    theVertices,
                                   const int    theEdges,
                                   const int    theWires,
                                   const int    theFaces,
                                   const int    theShells,
                                   const int    theSolids,
                                   const double theSurface)
{
  return {theVertices, theEdges, theWires, theFaces, theShells, theSolids, theSurface, -1.0};
}
} // namespace

// Equivalent to tests/boolean/opensolid/A1.
TEST(BOPAlgo_OpenSolidTest, A1_TwoOpenBoxes)
{
  const TopoDS_Shape aBox1 = MakeOpenBox(gp_Pnt(0.0, 0.0, 0.0), 10.0, 10.0, 10.0);
  const TopoDS_Shape aBox2 = MakeOpenBox(gp_Pnt(5.0, 0.0, 0.0), 10.0, 10.0, 10.0);
  CheckTwoShapeBOP(aBox1,
                   aBox2,
                   OpenResult(8, 12, 5, 5, 1, 1, 350.0),
                   OpenResult(16, 26, 11, 11, 1, 1, 650.0),
                   OpenResult(8, 12, 5, 5, 1, 1, 350.0),
                   OpenResult(8, 12, 5, 5, 1, 1, 350.0));
}

// Equivalent to tests/boolean/opensolid/A2.
TEST(BOPAlgo_OpenSolidTest, A2_OpenBoxesWithPartialIntersection)
{
  const TopoDS_Shape aBox1 = MakeOpenBox(gp_Pnt(0.0, 0.0, 0.0), 10.0, 10.0, 10.0);
  const TopoDS_Shape aBox2 = MakeOpenBox(gp_Pnt(2.5, 0.0, 0.0), 5.0, 10.0, 5.0);
  CheckTwoShapeBOP(aBox1,
                   aBox2,
                   OpenResult(8, 12, 5, 5, 1, 1, 200.0),
                   OpenResult(16, 24, 9, 9, 1, 1, 500.0),
                   OpenResult(16, 24, 8, 8, 1, 1, 500.0),
                   OpenResult(0, 0, 0, 0, 0, 0, 0.0));
}

// Equivalent to tests/boolean/opensolid/A3.
TEST(BOPAlgo_OpenSolidTest, A3_OpenBoxesWithDifferentOrientation)
{
  const TopoDS_Shape aBox1 = MakeOpenBox(gp_Pnt(-15.0, 0.0, 0.0), 20.0, 5.0, 5.0, 1, 5);
  const TopoDS_Shape aBox2 = MakeOpenBox(gp_Pnt(0.0, 0.0, 0.0), 5.0, 5.0, 20.0);
  CheckTwoShapeBOP(aBox1,
                   aBox2,
                   OpenResult(8, 12, 6, 6, 1, 1, 150.0),
                   OpenResult(16, 28, 12, 12, 1, 1, 700.0),
                   OpenResult(8, 12, 5, 5, 1, 1, 325.0),
                   OpenResult(8, 12, 5, 5, 1, 1, 325.0));
}

// Equivalent to tests/boolean/opensolid/A4.
TEST(BOPAlgo_OpenSolidTest, A4_MultiObjectOpenSolidOperations)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(40.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(0.0, 0.0, 30.0), 40.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox3 = BRepPrimAPI_MakeBox(10.0, 10.0, 40.0).Shape();
  const TopoDS_Shape aBox4 = BRepPrimAPI_MakeBox(gp_Pnt(30.0, 0.0, 0.0), 10.0, 10.0, 40.0).Shape();
  const ShapeExpectation aFuse         = {32, 64, 32, 32, 1, 1, 4800.0, 12000.0};
  const ShapeExpectation aCut          = {16, 24, 12, 12, 2, 2, 2000.0, 4000.0};
  const ShapeExpectation aSingleCut    = {8, 12, 6, 6, 1, 1, 1000.0, 2000.0};
  const ShapeExpectation aCommon       = {32, 48, 24, 24, 4, 4, 2400.0, 4000.0};
  const ShapeExpectation aSingleCommon = {16, 24, 12, 12, 2, 2, 1200.0, 2000.0};

  NCollection_List<TopoDS_Shape> anAll;
  anAll.Append(aBox1);
  anAll.Append(aBox2);
  anAll.Append(aBox3);
  anAll.Append(aBox4);

  NCollection_List<TopoDS_Shape> anObjects;
  anObjects.Append(aBox1);
  anObjects.Append(aBox2);
  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(aBox3);
  aTools.Append(aBox4);
  NCollection_List<TopoDS_Shape> anAllTools;
  anAllTools.Append(aBox2);
  anAllTools.Append(aBox3);
  anAllTools.Append(aBox4);
  CheckBuildBOP(anAll, anAll, NCollection_List<TopoDS_Shape>(), BOPAlgo_FUSE, aFuse, "all fuse");
  CheckBuildBOP(anAll, anObjects, aTools, BOPAlgo_FUSE, aFuse, "objects/tools fuse");
  CheckBuildBOP(anAll, anObjects, aTools, BOPAlgo_CUT, aCut, "objects/tools cut");
  CheckBuildBOP(anAll, aTools, anObjects, BOPAlgo_CUT, aCut, "tools/objects cut");
  CheckBuildBOP(anAll, anObjects, aTools, BOPAlgo_COMMON, aCommon, "objects/tools common");

  NCollection_List<TopoDS_Shape> aSingleObject;
  aSingleObject.Append(aBox1);
  NCollection_List<TopoDS_Shape> aSecondObject;
  aSecondObject.Append(aBox2);
  CheckBuildBOP(anAll, aSingleObject, anAllTools, BOPAlgo_FUSE, aFuse, "single object/tools fuse");
  CheckBuildBOP(anAll, aSingleObject, aTools, BOPAlgo_CUT, aSingleCut, "single object/tools cut");
  CheckBuildBOP(anAll, aSecondObject, aTools, BOPAlgo_CUT, aSingleCut, "second object/tools cut");
  CheckBuildBOP(anAll, aSingleObject, aTools, BOPAlgo_COMMON, aSingleCommon,
                "first object/tools common");
  CheckBuildBOP(anAll, aSecondObject, aTools, BOPAlgo_COMMON, aSingleCommon,
                "second object/tools common");
}

// Equivalent to tests/boolean/opensolid/A5.
TEST(BOPAlgo_OpenSolidTest, A5_InvertedToolOpenSolidOperations)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 2.0, 2.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(2.0, 0.5, -3.0), 1.0, 1.0, 8.0).Shape();
  TopoDS_Shape       aBox3 = BRepPrimAPI_MakeBox(gp_Pnt(7.0, 0.5, -3.0), 1.0, 1.0, 8.0).Shape();
  aBox3.Reverse();
  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(aBox2);
  aTools.Append(aBox3);
  NCollection_List<TopoDS_Shape> anObject;
  anObject.Append(aBox1);
  CheckBOP(anObject, aTools, BOPAlgo_COMMON, {24, 36, 20, 16, 2, 2, 110.0, 38.0});
  CheckBOP(anObject, aTools, BOPAlgo_FUSE, {16, 24, 12, 12, 2, 2, 28.0, -6.0});
  CheckBOP(anObject, aTools, BOPAlgo_CUT, {8, 12, 6, 6, 1, 1, 10.0, 2.0});
  CheckBOP(aTools, anObject, BOPAlgo_CUT, {40, 60, 32, 28, 3, 3, 164.0, -46.0});
}

// Equivalent to tests/boolean/opensolid/A6.
TEST(BOPAlgo_OpenSolidTest, A6_NestedOpenSolidOperations)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 2.0, 2.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(5.0, 0.0, 0.0), 5.0, 5.0, 5.0).Shape();
  const TopoDS_Shape aBox3 = BRepPrimAPI_MakeBox(gp_Pnt(6.0, 0.0, 0.0), 1.0, 1.0, 1.0).Shape();
  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(aBox2);
  aTools.Append(aBox3);
  NCollection_List<TopoDS_Shape> anObject;
  anObject.Append(aBox1);
  CheckBOP(anObject, aTools, BOPAlgo_COMMON, {16, 25, 12, 12, 2, 2, 56.0, 20.0});
  CheckBOP(anObject, aTools, BOPAlgo_FUSE, {24, 38, 16, 16, 1, 1, 190.0, 145.0});
  CheckBOP(anObject, aTools, BOPAlgo_CUT, {8, 12, 6, 6, 1, 1, 48.0, 20.0});
  CheckBOP(aTools, anObject, BOPAlgo_CUT, {12, 18, 8, 8, 1, 1, 142.0, 105.0});
}

// Equivalent to tests/boolean/opensolid/A7.
TEST(BOPAlgo_OpenSolidTest, A7_ThreeOpenSolidOperations)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(1.0, 1.0, 1.0), 8.0, 8.0, 8.0).Shape();
  const TopoDS_Shape aBox3 = BRepPrimAPI_MakeBox(gp_Pnt(-2.0, 3.0, 3.0), 14.0, 4.0, 4.0).Shape();
  NCollection_List<TopoDS_Shape> anObjects;
  anObjects.Append(aBox1);
  anObjects.Append(aBox2);
  anObjects.Append(aBox3);
  NCollection_List<TopoDS_Shape> aOne;
  aOne.Append(aBox3);
  NCollection_List<TopoDS_Shape> aTwo;
  aTwo.Append(aBox1);
  aTwo.Append(aBox2);
  CheckBOP(aOne, aTwo, BOPAlgo_COMMON, {16, 28, 16, 16, 3, 3, 256.0, 160.0});
  CheckBOP(aOne, aTwo, BOPAlgo_FUSE, {24, 36, 18, 16, 1, 1, 664.0, 1064.0});
  CheckBOP(aOne, aTwo, BOPAlgo_CUT, {16, 24, 12, 12, 2, 2, 128.0, 64.0});
  CheckBOP(aTwo, aOne, BOPAlgo_CUT, {32, 52, 28, 24, 2, 2, 1432.0, 840.0});
  (void)anObjects;
}

// Equivalent to tests/boolean/opensolid/A8.
TEST(BOPAlgo_OpenSolidTest, A8_OverlappingOpenSolidOperations)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(-2.0, -2.0, 2.0), 7.0, 14.0, 6.0).Shape();
  const TopoDS_Shape aBox3 = BRepPrimAPI_MakeBox(gp_Pnt(5.0, -2.0, 2.0), 7.0, 14.0, 6.0).Shape();
  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(aBox3);
  aTools.Append(aBox2);
  NCollection_List<TopoDS_Shape> anObject;
  anObject.Append(aBox1);
  CheckBOP(anObject, aTools, BOPAlgo_COMMON, {12, 20, 11, 11, 2, 2, 560.0, 600.0});
  CheckBOP(anObject, aTools, BOPAlgo_FUSE, {32, 50, 20, 20, 1, 1, 888.0, 1576.0});
  CheckBOP(anObject, aTools, BOPAlgo_CUT, {20, 30, 14, 14, 2, 2, 560.0, 400.0});
  CheckBOP(aTools, anObject, BOPAlgo_CUT, {24, 40, 18, 18, 2, 2, 816.0, 576.0});
}

// Equivalent to tests/boolean/opensolid/A9.
TEST(BOPAlgo_OpenSolidTest, A9_OpenSolidWithInternalShell)
{
  const TopoDS_Shape aBox1  = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2  = BRepPrimAPI_MakeBox(gp_Pnt(5.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aOpen1 = MakeOpenSolid(aBox1, 0, 4, MakeInternalFace(1.0));
  const TopoDS_Shape aOpen2 = MakeOpenSolid(aBox2, 0, 4, MakeInternalFace(14.0));
  NCollection_List<TopoDS_Shape> anObject;
  anObject.Append(aOpen1);
  NCollection_List<TopoDS_Shape> aTool;
  aTool.Append(aOpen2);
  CheckBOPWithFiller(anObject, aTool, BOPAlgo_COMMON, OpenResult(8, 12, 5, 5, 1, 1, 350.0));
  CheckBOPWithFiller(anObject, aTool, BOPAlgo_FUSE, OpenResult(16, 26, 11, 11, 1, 1, 650.0));
  CheckBOPWithFiller(anObject, aTool, BOPAlgo_CUT, OpenResult(12, 16, 6, 6, 2, 1, 414.0));
  CheckBOPWithFiller(aTool, anObject, BOPAlgo_CUT, OpenResult(12, 16, 6, 6, 2, 1, 414.0));
}
