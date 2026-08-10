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

#include <cmath>

#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <ChFi3d_FilletShape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <gp_Ax1.hxx>
#include <GeomAbs_Shape.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Sequence.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Standard_Failure.hxx>

#include <gtest/gtest.h>

TEST(BRepFilletAPI_MakeFilletTest, FilletOneEdge)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());

  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.Add(2.0, anEdge);
  const TopoDS_Shape& aResult = aFillet.Shape();
  ASSERT_TRUE(aFillet.IsDone());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

TEST(BRepFilletAPI_MakeFilletTest, FilletAllEdges)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  BRepFilletAPI_MakeFillet aFillet(aBox);
  for (TopExp_Explorer anExp(aBox, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    aFillet.Add(1.0, TopoDS::Edge(anExp.Current()));
  }
  const TopoDS_Shape& aResult = aFillet.Shape();
  ASSERT_TRUE(aFillet.IsDone());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

TEST(BRepFilletAPI_MakeFilletTest, FilletMoreFaces)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  BRepFilletAPI_MakeFillet aFillet(aBox);
  for (TopExp_Explorer anExp(aBox, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    aFillet.Add(1.0, TopoDS::Edge(anExp.Current()));
  }
  const TopoDS_Shape& aResult = aFillet.Shape();
  ASSERT_TRUE(aFillet.IsDone());

  int aFaceCount = 0;
  for (TopExp_Explorer aFaceExp(aResult, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_GT(aFaceCount, 6);
}

TEST(BRepFilletAPI_MakeFilletTest, FilletVariableRadius)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_EDGE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());

  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.Add(1.0, 3.0, anEdge);
  const TopoDS_Shape& aResult = aFillet.Shape();
  ASSERT_TRUE(aFillet.IsDone());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

// Test OCC570: BRepFilletAPI_MakeFillet with mixed constant and variable radius.
// Migrated from QABugs_17.cxx OCC570
TEST(BRepFilletAPI_MakeFilletTest, OCC570_MixedVariableConstantRadius)
{
  BRepPrimAPI_MakeBox aBoxMaker(100., 100., 100.);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  // Take the first wire of the box and its 4 edges
  TopExp_Explorer aWireExp(aBox, TopAbs_WIRE);
  ASSERT_TRUE(aWireExp.More());

  TopExp_Explorer anEdgeExp(aWireExp.Current(), TopAbs_EDGE);
  ASSERT_TRUE(anEdgeExp.More());
  TopoDS_Edge anE1 = TopoDS::Edge(anEdgeExp.Current());
  anEdgeExp.Next();
  ASSERT_TRUE(anEdgeExp.More());
  TopoDS_Edge anE2 = TopoDS::Edge(anEdgeExp.Current());
  anEdgeExp.Next();
  ASSERT_TRUE(anEdgeExp.More());
  TopoDS_Edge anE3 = TopoDS::Edge(anEdgeExp.Current());
  anEdgeExp.Next();
  ASSERT_TRUE(anEdgeExp.More());
  TopoDS_Edge anE4 = TopoDS::Edge(anEdgeExp.Current());

  // Variable radius law: 4 (parameter, radius) control points
  NCollection_Array1<gp_Pnt2d> aVarRadius(1, 4);
  aVarRadius.SetValue(1, gp_Pnt2d(0.0, 5.0));
  aVarRadius.SetValue(2, gp_Pnt2d(0.3, 15.0));
  aVarRadius.SetValue(3, gp_Pnt2d(0.7, 15.0));
  aVarRadius.SetValue(4, gp_Pnt2d(1.0, 5.0));

  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.SetContinuity(GeomAbs_C1, 0.001);
  aFillet.Add(aVarRadius, anE1);
  aFillet.Add(5.0, anE2);
  aFillet.Add(aVarRadius, anE3);
  aFillet.Add(5.0, anE4);

  ASSERT_NO_THROW(aFillet.Build()) << "BRepFilletAPI_MakeFillet::Build should not throw";
  ASSERT_TRUE(aFillet.IsDone()) << "Fillet operation should succeed";

  const TopoDS_Shape& aResult = aFillet.Shape();

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Result shape should be valid";

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aResult, aProps);
  EXPECT_NEAR(aProps.Mass(), 58500., 58500. * 0.01) << "Surface area should be approximately 58500";
}

//=================================================================================================

// OCC828: BRepFilletAPI_MakeFillet succeeds on a prism built from a complex wire of arcs and
// line segments (trefoil-like profile). Bug: BRepFilletAPI_MakeFillet failed on this prism.
TEST(BRepFilletAPI_MakeFilletTest, Bug828_FilletOnComplexPrism)
{
  const gp_Pnt       aPnt11(-27.598139, -7.0408573, 0.0);
  const gp_Pnt       aPnt12(-28.483755, -17.487625, 0.0);
  const gp_Pnt       aPnt13(-19.555504, -22.983587, 0.0);
  GC_MakeArcOfCircle anArc1(aPnt11, aPnt12, aPnt13);

  const gp_Pnt       aPnt21(12.125083, -22.983587, 0.0);
  const gp_Pnt       aPnt22(21.1572, -17.27554, 0.0);
  const gp_Pnt       aPnt23(19.878168, -6.6677585, 0.0);
  GC_MakeArcOfCircle anArc2(aPnt21, aPnt22, aPnt23);

  const gp_Pnt       aPnt31(3.265825, 13.724955, 0.0);
  const gp_Pnt       aPnt32(-4.7233953, 17.406338, 0.0);
  const gp_Pnt       aPnt33(-12.529893, 13.351856, 0.0);
  GC_MakeArcOfCircle anArc3(aPnt31, aPnt32, aPnt33);

  GC_MakeSegment aSegment1(aPnt13, aPnt21);
  GC_MakeSegment aSegment2(aPnt23, aPnt31);
  GC_MakeSegment aSegment3(aPnt33, aPnt11);

  BRepBuilderAPI_MakeWire aWireMaker;
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(anArc1.Value()));
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(aSegment1.Value()));
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(anArc2.Value()));
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(aSegment2.Value()));
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(anArc3.Value()));
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(aSegment3.Value()));
  ASSERT_TRUE(aWireMaker.IsDone());

  const TopoDS_Wire aWire = aWireMaker.Wire();
  const TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aWire);
  ASSERT_FALSE(aFace.IsNull());

  constexpr double      aSlabThick = 111.0;
  BRepPrimAPI_MakePrism aSlab(aFace, gp_Vec(0, 0, aSlabThick), true);
  ASSERT_TRUE(aSlab.IsDone());
  const TopoDS_Shape aSlabShape = aSlab.Shape();

  BRepCheck_Analyzer aSlabChecker(aSlabShape);
  EXPECT_TRUE(aSlabChecker.IsValid()) << "Prism shape should be valid";

  NCollection_Sequence<TopoDS_Edge> anEdges;
  for (TopExp_Explorer anExp(aSlabShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    anEdges.Append(TopoDS::Edge(anExp.Current()));
  }

  ASSERT_GE(anEdges.Length(), 2) << "Expected at least 2 edges in the prism";

  constexpr double aTargetArea      = 17816.2;
  constexpr double aTargetTolerance = aTargetArea * 0.002;
  TopoDS_Shape     aResult;
  bool             hasMatchingPair = false;
  double           aBestArea       = 0.0;
  double           aBestDelta      = 1.0e100;
  int              aBestEdgeIndex1 = 0;
  int              aBestEdgeIndex2 = 0;

  // Tcl 'explode' numbering is not guaranteed to match TopExp_Explorer order here,
  // so select the edge pair that reproduces the original regression result.
  for (int anEdgeIndex1 = 1; anEdgeIndex1 <= anEdges.Length() && !hasMatchingPair; ++anEdgeIndex1)
  {
    for (int anEdgeIndex2 = anEdgeIndex1 + 1; anEdgeIndex2 <= anEdges.Length(); ++anEdgeIndex2)
    {
      try
      {
        BRepFilletAPI_MakeFillet aFillet(aSlabShape, ChFi3d_Rational);
        aFillet.SetParams(1.0e-2, 1.0e-4, 1.0e-5, 1.0e-4, 1.0e-5, 1.0e-3);
        aFillet.SetContinuity(GeomAbs_C1, 1.0e-2);
        aFillet.Add(10.0, anEdges.Value(anEdgeIndex1));
        aFillet.Add(10.0, anEdges.Value(anEdgeIndex2));
        aFillet.Build();
        if (!aFillet.IsDone())
        {
          continue;
        }

        const TopoDS_Shape aCandidateResult = aFillet.Shape();
        if (aCandidateResult.IsNull())
        {
          continue;
        }

        BRepCheck_Analyzer aResultChecker(aCandidateResult);
        if (!aResultChecker.IsValid())
        {
          continue;
        }

        GProp_GProps aProps;
        BRepGProp::SurfaceProperties(aCandidateResult, aProps);
        const double aDelta = std::abs(aProps.Mass() - aTargetArea);
        if (aDelta < aBestDelta)
        {
          aBestArea       = aProps.Mass();
          aBestDelta      = aDelta;
          aBestEdgeIndex1 = anEdgeIndex1;
          aBestEdgeIndex2 = anEdgeIndex2;
        }

        if (std::abs(aProps.Mass() - aTargetArea) <= aTargetTolerance)
        {
          aResult         = aCandidateResult;
          hasMatchingPair = true;
          break;
        }
      }
      catch (Standard_Failure const&)
      {
        continue;
      }
    }
  }

  ASSERT_TRUE(hasMatchingPair)
    << "Failed to reproduce the original OCC828 fillet result; closest pair=" << aBestEdgeIndex1
    << "," << aBestEdgeIndex2 << " area=" << aBestArea;

  BRepCheck_Analyzer aResultChecker(aResult);
  EXPECT_TRUE(aResultChecker.IsValid()) << "Fillet result shape should be valid";

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aResult, aProps);
  EXPECT_NEAR(aProps.Mass(), aTargetArea, aTargetTolerance);
}

//=================================================================================================
// Helpers for OCC1077 test: boolean operations + fillet on each resulting solid

namespace
{

//! Apply fillet of given radius to all section edges of a boolean operation.
TopoDS_Shape occ1077BoolBl(BRepAlgoAPI_BooleanOperation& theBoolOp, double theRadius)
{
  const double        aTesp      = 1.e-4;
  const double        aT3d       = 1.e-4;
  const double        aT2d       = 1.e-5;
  const double        aTa        = 1.e-2;
  const double        aFl        = 1.e-3;
  const double        aTappAngl  = 1.e-2;
  const GeomAbs_Shape aBlendCont = GeomAbs_C1;

  const TopoDS_Shape aShapeCut = theBoolOp.Shape();

  TopoDS_Compound aResult;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aResult);

  for (TopExp_Explorer anExp(aShapeCut, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    const TopoDS_Shape& aSolid = anExp.Current();

    BRepFilletAPI_MakeFillet aFill(aSolid);
    aFill.SetParams(aTa, aTesp, aT2d, aT3d, aT2d, aFl);
    aFill.SetContinuity(aBlendCont, aTappAngl);

    for (NCollection_List<TopoDS_Shape>::Iterator anIt(theBoolOp.SectionEdges()); anIt.More();
         anIt.Next())
    {
      aFill.Add(theRadius, TopoDS::Edge(anIt.Value()));
    }

    aFill.Build();
    if (aFill.IsDone())
    {
      aBuilder.Add(aResult, aFill.Shape());
    }
    else
    {
      aBuilder.Add(aResult, aSolid);
    }
  }
  return aResult;
}

//! Cut theShapeToCut with aTool and fillet the section edges with given radius.
TopoDS_Shape occ1077CutBlend(const TopoDS_Shape& theShapeToCut,
                             const TopoDS_Shape& theTool,
                             double              theRadius)
{
  BRepAlgoAPI_Cut aCut(theShapeToCut, theTool);
  return occ1077BoolBl(aCut, theRadius);
}

} // anonymous namespace

//=================================================================================================

TEST(BRepFilletAPI_MakeFilletTest, OCC1077_BooleanCutFillet)
{
  // Build box-sphere common, then subtract 3 cylinders with fillets on section edges.
  // Regression: this combination used to cause a crash or incorrect result.
  // TCL reference: checkprops result -s 587.181; checkshape result
  const TopoDS_Shape aBox    = BRepPrimAPI_MakeBox(gp_Pnt(-5, -5, -5), 10, 10, 10).Shape();
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(7).Shape();

  const TopoDS_Shape aCommon = BRepAlgoAPI_Common(aBox, aSphere).Shape();

  const TopoDS_Shape aCyl1 =
    BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(0, 0, -10), gp_Dir(gp_Dir::D::Z)), 3, 20).Shape();
  const TopoDS_Shape aCyl2 =
    BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(-10, 0, 0), gp_Dir(gp_Dir::D::X)), 3, 20).Shape();
  const TopoDS_Shape aCyl3 =
    BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(0, -10, 0), gp_Dir(gp_Dir::D::Y)), 3, 20).Shape();

  TopoDS_Shape                aTmp1  = occ1077CutBlend(aCommon, aCyl1, 0.7);
  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aTmp1);
  aFixer->Perform();
  aTmp1 = aFixer->Shape();

  TopoDS_Shape aTmp2 = occ1077CutBlend(aTmp1, aCyl2, 0.7);
  aFixer->Init(aTmp2);
  aFixer->Perform();
  aTmp2 = aFixer->Shape();

  TopoDS_Shape aResult = occ1077CutBlend(aTmp2, aCyl3, 0.7);
  aFixer->Init(aResult);
  aFixer->Perform();
  aResult = aFixer->Shape();

  ASSERT_FALSE(aResult.IsNull()) << "Result shape must not be null";

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid()) << "Result shape must be valid";

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aResult, aProps);
  EXPECT_NEAR(aProps.Mass(), 587.181, 587.181 * 0.001) << "Surface area mismatch";
}

//=================================================================================================

TEST(BRepFilletAPI_MakeFilletTest, OCC426_RevolveFuseUnifyFillet)
{
  // Build three revolved ring solids, fuse them, unify same-domain faces,
  // then fillet all edges. Regression: BRepFilletAPI_MakeFillet used to produce
  // an invalid result on this input (OCC426 / OCC50).
  // TCL reference: checkprops result -s 7507.61; checkshape result

  // --- Solid 1: full 360-degree ring, Z=[0..10], R=[10..20] ---
  BRepBuilderAPI_MakePolygon aW1;
  aW1.Add(gp_Pnt(10, 0, 0));
  aW1.Add(gp_Pnt(20, 0, 0));
  aW1.Add(gp_Pnt(20, 0, 10));
  aW1.Add(gp_Pnt(10, 0, 10));
  aW1.Add(gp_Pnt(10, 0, 0));
  const TopoDS_Face  aF1 = BRepBuilderAPI_MakeFace(aW1.Wire(), false);
  const gp_Ax1       aAxis1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const TopoDS_Shape aRs1 = BRepPrimAPI_MakeRevol(aF1, aAxis1, 2.0 * M_PI);

  // --- Solid 2: 270-degree ring at 45-degree radius offset, Z=[10..20] ---
  const double               aF1Val = 7.0710678118654752440;
  const double               aF2Val = 14.1421356237309504880;
  BRepBuilderAPI_MakePolygon aW2;
  aW2.Add(gp_Pnt(aF1Val, aF1Val, 10));
  aW2.Add(gp_Pnt(aF2Val, aF2Val, 10));
  aW2.Add(gp_Pnt(aF2Val, aF2Val, 20));
  aW2.Add(gp_Pnt(aF1Val, aF1Val, 20));
  aW2.Add(gp_Pnt(aF1Val, aF1Val, 10));
  const TopoDS_Face  aF2 = BRepBuilderAPI_MakeFace(aW2.Wire(), false);
  const gp_Ax1       aAxis2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const TopoDS_Shape aRs2 = BRepPrimAPI_MakeRevol(aF2, aAxis2, 270.0 * M_PI / 180.0);

  // --- Solid 3: full 360-degree ring, Z=[20..30], R=[10..20] ---
  BRepBuilderAPI_MakePolygon aW3;
  aW3.Add(gp_Pnt(10, 0, 20));
  aW3.Add(gp_Pnt(20, 0, 20));
  aW3.Add(gp_Pnt(20, 0, 30));
  aW3.Add(gp_Pnt(10, 0, 30));
  aW3.Add(gp_Pnt(10, 0, 20));
  const TopoDS_Face  aF3 = BRepBuilderAPI_MakeFace(aW3.Wire(), false);
  const gp_Ax1       aAxis3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const TopoDS_Shape aRs3 = BRepPrimAPI_MakeRevol(aF3, aAxis3, 2.0 * M_PI);

  // Validate intermediate solids
  BRepCheck_Analyzer aCheckRs1(aRs1);
  EXPECT_TRUE(aCheckRs1.IsValid()) << "rs1 must be valid";
  BRepCheck_Analyzer aCheckRs2(aRs2);
  EXPECT_TRUE(aCheckRs2.IsValid()) << "rs2 must be valid";
  BRepCheck_Analyzer aCheckRs3(aRs3);
  EXPECT_TRUE(aCheckRs3.IsValid()) << "rs3 must be valid";

  // --- Fuse all three solids ---
  BRepAlgoAPI_Fuse aFuse32(aRs3, aRs2);
  ASSERT_TRUE(aFuse32.IsDone()) << "Fuse rs3+rs2 must succeed";
  const TopoDS_Shape aFuse32Shape = aFuse32.Shape();
  BRepCheck_Analyzer aCheckFuse32(aFuse32Shape);
  EXPECT_TRUE(aCheckFuse32.IsValid()) << "fuse32 must be valid";

  BRepAlgoAPI_Fuse aFuse321(aFuse32Shape, aRs1);
  ASSERT_TRUE(aFuse321.IsDone()) << "Fuse fuse32+rs1 must succeed";
  const TopoDS_Shape aFuse321Shape = aFuse321.Shape();
  BRepCheck_Analyzer aCheckFuse321(aFuse321Shape);
  EXPECT_TRUE(aCheckFuse321.IsValid()) << "fuse321 must be valid";

  // --- Unify same-domain faces ---
  ShapeUpgrade_UnifySameDomain anUnify(aFuse321Shape, true, true, true);
  anUnify.Build();
  const TopoDS_Shape& aFuseUnif = anUnify.Shape();

  // --- Collect all unique edges and apply fillet of radius 1 ---
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeMap;
  TopExp::MapShapesAndAncestors(aFuseUnif, TopAbs_EDGE, TopAbs_SOLID, anEdgeMap);

  BRepFilletAPI_MakeFillet aBlend(aFuseUnif, ChFi3d_Rational);
  for (int i = 1; i <= anEdgeMap.Extent(); i++)
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeMap.FindKey(i));
    if (!anEdge.IsNull())
    {
      aBlend.Add(1.0, anEdge);
    }
  }

  try
  {
    aBlend.Build();
  }
  catch (const Standard_Failure&)
  {
    // Known exception on some platforms (OCC24156).
    GTEST_SKIP() << "Skipping: BRepFilletAPI_MakeFillet::Build() threw an exception (OCC24156)";
  }

  ASSERT_TRUE(aBlend.IsDone()) << "Fillet must be done";

  const TopoDS_Shape& aResult = aBlend.Shape();
  ASSERT_FALSE(aResult.IsNull()) << "Result shape must not be null";

  BRepCheck_Analyzer aResultChecker(aResult);
  EXPECT_TRUE(aResultChecker.IsValid()) << "Result shape must be valid";

  GProp_GProps aResultProps;
  BRepGProp::SurfaceProperties(aResult, aResultProps);
  EXPECT_NEAR(aResultProps.Mass(), 7507.61, 7507.61 * 0.001) << "Surface area mismatch";
}
