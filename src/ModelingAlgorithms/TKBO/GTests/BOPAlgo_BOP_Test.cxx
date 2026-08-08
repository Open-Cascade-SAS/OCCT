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

#include <BOPAlgo_ArgumentAnalyzer.hxx>
#include <BOPAlgo_CheckerSI.hxx>
#include <BOPAlgo_CheckStatus.hxx>
#include <BOPDS_DS.hxx>
#include <BRepBndLib.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopExp.hxx>

//=================================================================================================
// Direct BOP Operations Tests (equivalent to bcut, bfuse, bcommon, btuc commands)
//=================================================================================================

class BOPAlgo_DirectOperationsTest : public BOPAlgo_TestBase
{
};

static void ConfigureArgumentAnalyzer(BOPAlgo_ArgumentAnalyzer& theAnalyzer)
{
  theAnalyzer.ArgumentTypeMode()   = true;
  theAnalyzer.SelfInterMode()      = true;
  theAnalyzer.SmallEdgeMode()      = true;
  theAnalyzer.RebuildFaceMode()    = true;
  theAnalyzer.ContinuityMode()     = true;
  theAnalyzer.CurveOnSurfaceMode() = true;
}

static int CountUniqueBOPSubShapes(const TopoDS_Shape& theShape,
                                   const TopAbs_ShapeEnum theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aShapes;
  TopExp::MapShapes(theShape, theType, aShapes);
  return aShapes.Extent();
}

static TopoDS_Shape MakeGridShell()
{
  BRepBuilderAPI_Sewing aSewing;
  const gp_Pln          aPlane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  for (int anX = 0; anX < 3; ++anX)
  {
    for (int anY = 0; anY < 3; ++anY)
    {
      aSewing.Add(BRepBuilderAPI_MakeFace(aPlane,
                                          static_cast<double>(anX),
                                          static_cast<double>(anX + 1),
                                          static_cast<double>(anY),
                                          static_cast<double>(anY + 1)));
    }
  }
  aSewing.Perform();
  return aSewing.SewedShape();
}

static int CountCheckerInterferences(const TopoDS_Shape& theShape, const int theLevel)
{
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(theShape);

  BOPAlgo_CheckerSI aChecker;
  aChecker.SetArguments(anArguments);
  aChecker.SetLevelOfCheck(theLevel);
  aChecker.Perform();
  EXPECT_FALSE(aChecker.HasErrors());

  const BOPDS_DS&                    aData = *aChecker.PDS();
  const NCollection_Map<BOPDS_Pair>& aPairs = aData.Interferences();
  int                               aCount = 0;
  for (NCollection_Map<BOPDS_Pair>::Iterator anIterator(aPairs); anIterator.More();
       anIterator.Next())
  {
    int anIndex1 = -1;
    int anIndex2 = -1;
    anIterator.Value().Indices(anIndex1, anIndex2);
    if (!aData.IsNewShape(anIndex1) && !aData.IsNewShape(anIndex2))
    {
      ++aCount;
    }
  }
  return aCount;
}

// Migrated from tests/bugs/modalg_5/bug24029.  The bopcheck level controls
// which interference families are considered for two overlapping boxes.
TEST(BOPAlgo_CheckerSITest, ModalgBug_24029_CheckLevelControlsInterferences)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 10.0, 10.0, 10.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(5, 5, 5), 10.0, 10.0, 10.0);

  const TopoDS_Shape aCompound = [&]() {
    BRep_Builder  aBuilder;
    TopoDS_Compound aResult;
    aBuilder.MakeCompound(aResult);
    aBuilder.Add(aResult, aBox1);
    aBuilder.Add(aResult, aBox2);
    return aResult;
  }();

  const int aLevel3Count = CountCheckerInterferences(aCompound, 3);
  const int aLevel4Count = CountCheckerInterferences(aCompound, 4);
  const int aLevel9Count = CountCheckerInterferences(aCompound, 9);
  EXPECT_EQ(aLevel3Count, 0);
  EXPECT_GT(aLevel4Count, 0);
  EXPECT_GE(aLevel9Count, aLevel4Count);
}

// Migrated from tests/bugs/modalg_5/bug24213.  The standard sphere must be
// accepted as a valid single Boolean-operation argument.
TEST(BOPAlgo_ArgumentAnalyzerTest, ModalgBug_24213_SphereIsValidBOPArgument)
{
  BOPAlgo_ArgumentAnalyzer anAnalyzer;
  anAnalyzer.SetShape1(BOPTest_Utilities::CreateSphere(gp_Pnt(0, 0, 0), 10.0));
  ConfigureArgumentAnalyzer(anAnalyzer);
  anAnalyzer.Perform();

  EXPECT_FALSE(anAnalyzer.HasFaulty());
  EXPECT_TRUE(anAnalyzer.GetCheckResult().IsEmpty());
}

// Migrated from tests/bugs/modalg_5/bug24492.  A compound containing a solid
// and an isolated vertex must be reported as a faulty BOP argument.
TEST(BOPAlgo_ArgumentAnalyzerTest, ModalgBug_24492_MixedDimensionalCompoundIsFaulty)
{
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 10.0, 10.0, 10.0);
  const TopoDS_Shape aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(3.0, 3.0, 3.0));

  BRep_Builder  aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aVertex);

  BOPAlgo_ArgumentAnalyzer anAnalyzer;
  anAnalyzer.SetShape1(aCompound);
  ConfigureArgumentAnalyzer(anAnalyzer);
  anAnalyzer.Perform();

  ASSERT_TRUE(anAnalyzer.HasFaulty());
  const NCollection_List<BOPAlgo_CheckResult>& aResults = anAnalyzer.GetCheckResult();
  ASSERT_EQ(aResults.Extent(), 1);
  EXPECT_EQ(aResults.First().GetCheckStatus(), BOPAlgo_SelfIntersect);
}

// Migrated from tests/bugs/modalg_5/bug25477_1.  A fuzzy fuse must bridge the
// small gap between the two boxes and produce one solid with the DRAW counts.
TEST(BOPAlgo_BOPTest, ModalgBug_25477_FuzzyFuseBridgesSmallGap)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0.0, 0.0, 0.0),
                                                          10.0,
                                                          10.0,
                                                          10.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(10.00001, 0.0, 0.0),
                                                          10.0,
                                                          10.0,
                                                          10.0);

  BOPAlgo_BOP aFuse;
  aFuse.AddArgument(aBox1);
  aFuse.AddTool(aBox2);
  aFuse.SetOperation(BOPAlgo_FUSE);
  aFuse.SetFuzzyValue(0.00002);
  aFuse.SetRunParallel(false);
  aFuse.SetNonDestructive(false);
  aFuse.Perform();

  ASSERT_FALSE(aFuse.HasErrors());
  const TopoDS_Shape aResult = aFuse.Shape();
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(aResult.ShapeType(), TopAbs_COMPOUND);
  EXPECT_EQ(CountUniqueBOPSubShapes(aResult, TopAbs_VERTEX), 12);
  EXPECT_EQ(CountUniqueBOPSubShapes(aResult, TopAbs_EDGE), 20);
  EXPECT_EQ(CountUniqueBOPSubShapes(aResult, TopAbs_WIRE), 10);
  EXPECT_EQ(CountUniqueBOPSubShapes(aResult, TopAbs_FACE), 10);
  EXPECT_EQ(CountUniqueBOPSubShapes(aResult, TopAbs_SHELL), 1);
  EXPECT_EQ(CountUniqueBOPSubShapes(aResult, TopAbs_SOLID), 1);
  EXPECT_EQ(CountUniqueBOPSubShapes(aResult, TopAbs_COMPOUND), 1);
  EXPECT_EQ(CountUniqueBOPSubShapes(aResult, TopAbs_COMPSOLID), 0);
  // Fuzzy classification slightly changes the bridge volume; it must remain
  // close to the sum of the two box volumes.
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aResult), 2000.0, 0.001);
}

// Migrated from tests/bugs/modalg_5/bug24033.  A common of a sewn planar
// shell and a box must preserve usable, non-INTERNAL orientations.
TEST(BOPAlgo_BOPTest, ModalgBug_24033_CommonResultOrientations)
{
  const TopoDS_Shape aShell = MakeGridShell();
  const TopoDS_Shape aBox   = BOPTest_Utilities::CreateBox(gp_Pnt(-1.0, 1.0, -1.0), 2.0, 1.0, 2.0);
  ASSERT_FALSE(aShell.IsNull());

  BOPAlgo_BOP aCommon;
  aCommon.AddArgument(aShell);
  aCommon.AddTool(aBox);
  aCommon.SetOperation(BOPAlgo_COMMON);
  aCommon.SetRunParallel(false);
  aCommon.SetNonDestructive(false);
  aCommon.Perform();

  ASSERT_FALSE(aCommon.HasErrors());
  const TopoDS_Shape aResult = aCommon.Shape();
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());

  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaces = [&]() {
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
    TopExp::MapShapes(aResult, TopAbs_FACE, aMap);
    return aMap;
  }();
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges = [&]() {
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
    TopExp::MapShapes(aResult, TopAbs_EDGE, aMap);
    return aMap;
  }();
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aVertices = [&]() {
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
    TopExp::MapShapes(aResult, TopAbs_VERTEX, aMap);
    return aMap;
  }();
  ASSERT_GT(aFaces.Extent(), 0);
  ASSERT_GT(anEdges.Extent(), 0);
  ASSERT_GT(aVertices.Extent(), 0);
  EXPECT_NE(TopoDS::Face(aFaces(1)).Orientation(), TopAbs_INTERNAL);
  EXPECT_NE(TopoDS::Edge(anEdges(1)).Orientation(), TopAbs_INTERNAL);
  EXPECT_NE(TopoDS::Vertex(aVertices(1)).Orientation(), TopAbs_INTERNAL);
}

// Test direct cut operation: bcut result sphere box
TEST_F(BOPAlgo_DirectOperationsTest, DirectCut_SphereMinusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformDirectBOP(aSphere, aBox, BOPAlgo_CUT);
  EXPECT_FALSE(aResult.IsNull()) << "Result shape should not be null";

  const double aSurfaceArea = BOPTest_Utilities::GetSurfaceArea(aResult);
  EXPECT_GT(aSurfaceArea, 0.0) << "Cut result should have positive surface area";
}

// Test direct fuse operation: bfuse result sphere box
TEST_F(BOPAlgo_DirectOperationsTest, DirectFuse_SpherePlusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformDirectBOP(aSphere, aBox, BOPAlgo_FUSE);
  EXPECT_FALSE(aResult.IsNull()) << "Result shape should not be null";

  const double aVolume       = BOPTest_Utilities::GetVolume(aResult);
  const double aSphereVolume = BOPTest_Utilities::GetVolume(aSphere);
  const double aBoxVolume    = BOPTest_Utilities::GetVolume(aBox);
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
  ValidateResult(aResult, -1.0, -1.0, true); // Expected empty
}

// Test with NURBS converted shapes
TEST_F(BOPAlgo_DirectOperationsTest, DirectCut_NurbsBoxMinusBox)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1              = BOPTest_Utilities::ConvertToNurbs(aBox1);
  EXPECT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 1, 0), 1.0, 0.5, 1.0);

  const TopoDS_Shape aResult      = PerformDirectBOP(aBox1, aBox2, BOPAlgo_CUT);
  const double       aSurfaceArea = BOPTest_Utilities::GetSurfaceArea(aResult);
  EXPECT_GT(aSurfaceArea, 0.0) << "NURBS cut result should have positive surface area";
}

//=================================================================================================
// Two-step BOP Operations Tests (equivalent to bop + bopXXX commands)
//=================================================================================================

class BOPAlgo_TwoStepOperationsTest : public BOPAlgo_TestBase
{
};

// Test two-step cut operation: bop sphere box; bopcut result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepCut_SphereMinusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult      = PerformTwoStepBOP(aSphere, aBox, BOPAlgo_CUT);
  const double       aSurfaceArea = BOPTest_Utilities::GetSurfaceArea(aResult);
  EXPECT_GT(aSurfaceArea, 0.0) << "Two-step cut result should have positive surface area";
}

// Test two-step fuse operation: bop sphere box; bopfuse result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepFuse_SpherePlusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformTwoStepBOP(aSphere, aBox, BOPAlgo_FUSE);
  EXPECT_FALSE(aResult.IsNull()) << "Result shape should not be null";

  const double aVolume       = BOPTest_Utilities::GetVolume(aResult);
  const double aSphereVolume = BOPTest_Utilities::GetVolume(aSphere);
  const double aBoxVolume    = BOPTest_Utilities::GetVolume(aBox);
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
  ValidateResult(aResult, -1.0, -1.0, true); // Expected empty
}

//=================================================================================================
// Complex Operations Tests
//=================================================================================================

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
  const TopoDS_Shape aFinalResult = PerformDirectBOP(aIntermediate, aBox, BOPAlgo_FUSE);
  const double       aVolume      = BOPTest_Utilities::GetVolume(aFinalResult);
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
  const double aDirectVolume  = BOPTest_Utilities::GetVolume(aDirectResult);
  const double aTwoStepVolume = BOPTest_Utilities::GetVolume(aTwoStepResult);

  EXPECT_NEAR(aDirectVolume, aTwoStepVolume, myTolerance)
    << "Direct and two-step operations should produce equivalent results";
}

//=================================================================================================
// Degenerate thin-tool tests
//=================================================================================================

class BOPAlgo_DegenerateToolTest : public BOPAlgo_TestBase
{
};

TEST_F(BOPAlgo_DegenerateToolTest, Cut_AxisAlignedThinTool_NearlyPreservesBoxVolume)
{
  const TopoDS_Shape aBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.0, 0.0, 0.0), 100.0, 100.0, 100.0);
  const TopoDS_Shape aThin =
    BOPTest_Utilities::CreateBox(gp_Pnt(-500.0, 25.0, -500.0), 1500.0, 1.0e-6, 1500.0);

  const TopoDS_Shape aRes      = PerformDirectBOP(aBox, aThin, BOPAlgo_CUT);
  const double       aBoxVol   = BOPTest_Utilities::GetVolume(aBox);
  const double       aOverlapV = 100.0 * 1.0e-6 * 100.0;
  ASSERT_FALSE(aRes.IsNull());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aRes), aBoxVol - aOverlapV, 1.0e-4);
}

TEST_F(BOPAlgo_DegenerateToolTest, Fuse_AxisAlignedThinTool_AddsNonOverlappingSlice)
{
  const TopoDS_Shape aBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.0, 0.0, 0.0), 100.0, 100.0, 100.0);
  const TopoDS_Shape aThin =
    BOPTest_Utilities::CreateBox(gp_Pnt(-500.0, 25.0, -500.0), 1500.0, 1.0e-6, 1500.0);

  const TopoDS_Shape aRes      = PerformDirectBOP(aBox, aThin, BOPAlgo_FUSE);
  const double       aBoxVol   = BOPTest_Utilities::GetVolume(aBox);
  const double       aThinVol  = 1500.0 * 1.0e-6 * 1500.0;
  const double       aOverlapV = 100.0 * 1.0e-6 * 100.0;
  ASSERT_FALSE(aRes.IsNull());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aRes), aBoxVol + aThinVol - aOverlapV, 1.0e-4);
}

TEST_F(BOPAlgo_DegenerateToolTest, Cut_LegitimateThinSlab_NotTreatedAsEmpty)
{
  const TopoDS_Shape aBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.0, 0.0, 0.0), 100.0, 100.0, 100.0);
  const TopoDS_Shape aSlab =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.0, 0.0, 50.0), 100.0, 100.0, 1.0);

  const TopoDS_Shape aRes = PerformDirectBOP(aBox, aSlab, BOPAlgo_CUT);
  ASSERT_FALSE(aRes.IsNull());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aRes),
              BOPTest_Utilities::GetVolume(aBox) - BOPTest_Utilities::GetVolume(aSlab),
              myTolerance);
}

TEST_F(BOPAlgo_DegenerateToolTest, Common_SolidAndPlanarFace_Unaffected)
{
  const TopoDS_Shape aBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.0, 0.0, 0.0), 100.0, 100.0, 100.0);
  const gp_Pln            aPln(gp_Pnt(0.0, 0.0, 50.0), gp_Dir(0.0, 0.0, 1.0));
  BRepBuilderAPI_MakeFace aFaceMaker(aPln, -200.0, 200.0, -200.0, 200.0);
  ASSERT_TRUE(aFaceMaker.IsDone());
  const TopoDS_Shape aFace = aFaceMaker.Face();

  const TopoDS_Shape aRes = PerformDirectBOP(aBox, aFace, BOPAlgo_COMMON);
  ASSERT_FALSE(aRes.IsNull());
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aRes), 1.0e4, 1.0);
}

TEST_F(BOPAlgo_DegenerateToolTest, Cut_BySemiInfinitePrism_Unaffected)
{
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(0.0, -1.0, -1.0), 2.0, 2.0, 2.0);
  const gp_Pln       aPln(gp_Pnt(-0.5, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  BRepBuilderAPI_MakeFace aFaceMaker(aPln, -0.5, 0.5, -0.5, 0.5);
  ASSERT_TRUE(aFaceMaker.IsDone());
  BRepPrimAPI_MakePrism aPrismMaker(aFaceMaker.Face(), gp_Dir(1.0, 0.0, 0.0), true);
  ASSERT_TRUE(aPrismMaker.IsDone());

  const TopoDS_Shape aRes = PerformDirectBOP(aBox, aPrismMaker.Shape(), BOPAlgo_CUT);
  ASSERT_FALSE(aRes.IsNull());
  EXPECT_GT(BOPTest_Utilities::GetVolume(aRes), 1.0);
}

TEST_F(BOPAlgo_DegenerateToolTest, Common_SolidAndHalfspace_Unaffected)
{
  const TopoDS_Shape aBox =
    BOPTest_Utilities::CreateBox(gp_Pnt(0.0, 0.0, -30.0), 150.0, 200.0, 200.0);
  const gp_Pln            aPln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  BRepBuilderAPI_MakeFace aFaceMaker(aPln, -250.0, 250.0, -250.0, 250.0);
  ASSERT_TRUE(aFaceMaker.IsDone());
  BRepPrimAPI_MakeHalfSpace aHSMaker(aFaceMaker.Face(), gp_Pnt(0.0, 0.0, -100.0));
  const TopoDS_Shape        aHalfSpace = aHSMaker.Solid();

  const TopoDS_Shape aRes = PerformDirectBOP(aBox, aHalfSpace, BOPAlgo_COMMON);
  ASSERT_FALSE(aRes.IsNull());
  EXPECT_GT(BOPTest_Utilities::GetVolume(aRes), 1.0);
}
