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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Precision.hxx>
#include <ShapeExtend_Status.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>

#include <gtest/gtest.h>

namespace
{
// Make a pcurve whose deviation lies between the 23 uniform control parameters.
TopoDS_Face makeFaceWithSubsampleDeviation(TopoDS_Edge& theEdge);

//==================================================================================================

TopoDS_Face makeFaceWithSubsampleDeviation(TopoDS_Edge& theEdge)
{
  BRepBuilderAPI_MakeFace aMakeFace(gp_Pln(gp::XOY()), 0.0, 22.0, 0.0, 22.0);
  const TopoDS_Face       aFace = aMakeFace.Face();
  TopExp_Explorer         anExplorer(aFace, TopAbs_EDGE);
  theEdge = TopoDS::Edge(anExplorer.Current());
  double                          aFirst, aLast;
  const occ::handle<Geom2d_Curve> aPCurve =
    BRep_Tool::CurveOnSurface(theEdge, aFace, aFirst, aLast);
  constexpr int                aNbSpans = 22;
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2 * aNbSpans + 1);
  NCollection_Array1<double>   aKnots(1, aNbSpans + 1);
  NCollection_Array1<int>      aMultiplicities(1, aNbSpans + 1);
  for (int aSpan = 0; aSpan <= aNbSpans; ++aSpan)
  {
    const double aParameter         = aFirst + (aLast - aFirst) * aSpan / aNbSpans;
    aPoles.ChangeAt(2 * aSpan)      = aPCurve->EvalD0(aParameter);
    aKnots.ChangeAt(aSpan)          = aParameter;
    aMultiplicities.ChangeAt(aSpan) = (aSpan == 0 || aSpan == aNbSpans) ? 3 : 2;
    if (aSpan < aNbSpans)
    {
      const double aMiddle = aFirst + (aLast - aFirst) * (aSpan + 0.5) / aNbSpans;
      gp_Pnt2d     aPole   = aPCurve->EvalD0(aMiddle);
      aPole.SetY(aPole.Y() + 0.02);
      aPoles.ChangeAt(2 * aSpan + 1) = aPole;
    }
  }
  const occ::handle<Geom2d_BSplineCurve> aBowedPCurve =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(theEdge, aBowedPCurve, aFace, Precision::Confusion());
  aBuilder.SameParameter(theEdge, true);
  aBuilder.SameRange(theEdge, true);
  return aFace;
}
} // namespace

TEST(ShapeFix_ShapeTest, SameParameterPreservesSourceTopology)
{
  for (const bool toFixWire : {false, true})
  {
    for (const bool toFixShape : {false, true})
    {
      SCOPED_TRACE(toFixWire);
      SCOPED_TRACE(toFixShape);
      TopoDS_Edge       anEdge;
      const TopoDS_Face aFace = makeFaceWithSubsampleDeviation(anEdge);
      TopoDS_Vertex     aFirstVertex, aLastVertex;
      TopExp::Vertices(anEdge, aFirstVertex, aLastVertex);
      const double   anEdgeTolerance = BRep_Tool::Tolerance(anEdge);
      const double   aFirstTolerance = BRep_Tool::Tolerance(aFirstVertex);
      const double   aLastTolerance  = BRep_Tool::Tolerance(aLastVertex);
      ShapeFix_Shape aFixer(aFace);
      aFixer.FixWireTool()->FixSameParameterMode() = toFixWire;
      aFixer.FixSameParameterMode()                = toFixShape;
      aFixer.Perform();
      EXPECT_DOUBLE_EQ(BRep_Tool::Tolerance(anEdge), anEdgeTolerance);
      EXPECT_DOUBLE_EQ(BRep_Tool::Tolerance(aFirstVertex), aFirstTolerance);
      EXPECT_DOUBLE_EQ(BRep_Tool::Tolerance(aLastVertex), aLastTolerance);
      const TopoDS_Shape aResult = aFixer.Shape();
      ASSERT_FALSE(aResult.IsNull());
      if (toFixWire || toFixShape)
      {
        const TopoDS_Shape aReplacement = aFixer.Context()->Apply(anEdge);
        ASSERT_FALSE(aReplacement.IsNull());
        ASSERT_EQ(aReplacement.ShapeType(), TopAbs_EDGE);
        const TopoDS_Edge aResultEdge = TopoDS::Edge(aReplacement);
        EXPECT_FALSE(aResultEdge.IsSame(anEdge));
        EXPECT_NEAR(BRep_Tool::Tolerance(aResultEdge), 0.01, 2.e-7);
        EXPECT_TRUE(BRep_Tool::SameParameter(aResultEdge));
        EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
      }
    }
  }
}

//==================================================================================================

TEST(ShapeFix_ShapeTest, SameParameterContextPreservesInputFlags)
{
  for (const bool isSameRange : {false, true})
  {
    for (const bool isSameParameter : {false, true})
    {
      for (const bool toEnforce : {false, true})
      {
        SCOPED_TRACE(isSameRange);
        SCOPED_TRACE(isSameParameter);
        SCOPED_TRACE(toEnforce);
        TopoDS_Edge       anEdge;
        const TopoDS_Face aFace = makeFaceWithSubsampleDeviation(anEdge);
        BRep_Builder      aBuilder;
        aBuilder.SameRange(anEdge, isSameRange);
        aBuilder.SameParameter(anEdge, isSameParameter);
        double                          aFirst, aLast;
        const occ::handle<Geom2d_Curve> aPCurve =
          BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
        NCollection_Array1<gp_Pnt2d> aSourcePoints(23);
        for (size_t i = 0; i < aSourcePoints.Size(); ++i)
        {
          const double aParameter =
            aFirst + (aLast - aFirst) * (double(i) + 0.5) / double(aSourcePoints.Size());
          aSourcePoints.ChangeAt(i) = aPCurve->EvalD0(aParameter);
        }
        const double                    aTolerance = BRep_Tool::Tolerance(anEdge);
        occ::handle<ShapeBuild_ReShape> aContext   = new ShapeBuild_ReShape;
        EXPECT_TRUE(ShapeFix::SameParameter(aFace,
                                            toEnforce,
                                            0.0,
                                            Message_ProgressRange(),
                                            nullptr,
                                            aContext));
        EXPECT_EQ(BRep_Tool::SameRange(anEdge), isSameRange);
        EXPECT_EQ(BRep_Tool::SameParameter(anEdge), isSameParameter);
        EXPECT_DOUBLE_EQ(BRep_Tool::Tolerance(anEdge), aTolerance);
        for (size_t i = 0; i < aSourcePoints.Size(); ++i)
        {
          const double aParameter =
            aFirst + (aLast - aFirst) * (double(i) + 0.5) / double(aSourcePoints.Size());
          EXPECT_DOUBLE_EQ(aSourcePoints.At(i).Distance(aPCurve->EvalD0(aParameter)), 0.0);
        }
        const TopoDS_Shape aResult = aContext->Apply(aFace);
        EXPECT_TRUE(BRepCheck_Analyzer(aResult, true, false, true).IsValid());
      }
    }
  }
}

//==================================================================================================

TEST(ShapeFix_ShapeTest, SameParameterContextKeepsValidEdgesShared)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  TopExp::MapShapes(aBox, TopAbs_EDGE, anEdges);
  for (TopoDS_Shape anEdge : anEdges)
  {
    anEdge.Modified(false);
  }
  occ::handle<ShapeBuild_ReShape> aContext = new ShapeBuild_ReShape;
  EXPECT_TRUE(
    ShapeFix::SameParameter(aBox, false, 0.0, Message_ProgressRange(), nullptr, aContext));
  EXPECT_TRUE(aContext->Apply(aBox).IsSame(aBox));
  for (const TopoDS_Shape& anEdge : anEdges)
  {
    EXPECT_FALSE(aContext->IsRecorded(anEdge));
    EXPECT_FALSE(anEdge.Modified());
    EXPECT_TRUE(aContext->Apply(anEdge).IsSame(anEdge));
  }
}

//==================================================================================================

TEST(ShapeFix_ShapeTest, SameParameterRepairKeepsSharedFaceOccurrences)
{
  TopoDS_Edge       anEdge;
  const TopoDS_Face aFace = makeFaceWithSubsampleDeviation(anEdge);
  TopoDS_Compound   aCompound;
  BRep_Builder      aBuilder;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aFace);
  aBuilder.Add(aCompound, aFace.Reversed());
  ShapeFix_Shape aFixer(aCompound);
  aFixer.Perform();
  const TopoDS_Shape aResult      = aFixer.Shape();
  const TopoDS_Shape aReplacement = aFixer.Context()->Apply(anEdge);
  ASSERT_FALSE(aReplacement.IsNull());
  EXPECT_FALSE(aReplacement.IsSame(anEdge));
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges, aVertices;
  TopExp::MapShapes(aResult, TopAbs_EDGE, anEdges);
  TopExp::MapShapes(aResult, TopAbs_VERTEX, aVertices);
  EXPECT_EQ(anEdges.Size(), 4u);
  EXPECT_EQ(aVertices.Size(), 4u);
  int anOccurrences = 0;
  for (TopExp_Explorer anExplorer(aResult, TopAbs_EDGE); anExplorer.More(); anExplorer.Next())
  {
    if (anExplorer.Current().IsSame(aReplacement))
    {
      ++anOccurrences;
    }
  }
  EXPECT_EQ(anOccurrences, 2);
  EXPECT_DOUBLE_EQ(BRep_Tool::Tolerance(anEdge), Precision::Confusion());
  EXPECT_TRUE(BRepCheck_Analyzer(aResult, true, false, true).IsValid());
}

//==================================================================================================

TEST(ShapeFix_ShapeTest, SameParameterWithoutContextChangesInput)
{
  TopoDS_Edge       anEdge;
  const TopoDS_Face aFace = makeFaceWithSubsampleDeviation(anEdge);
  ShapeFix_Edge     aFixer;
  EXPECT_TRUE(aFixer.FixSameParameter(anEdge, aFace));
  EXPECT_NEAR(BRep_Tool::Tolerance(anEdge), 0.01, 2.e-7);
  EXPECT_TRUE(BRepCheck_Analyzer(aFace, true, false, true).IsValid());
}

//==================================================================================================

TEST(ShapeFix_ShapeTest, SameParameterContextPreservesDegeneratedSource)
{
  BRepPrimAPI_MakeSphere aSphere(1.0);
  const TopoDS_Shape     aShape = aSphere.Shape();
  ASSERT_TRUE(aSphere.IsDone());
  TopoDS_Edge anEdge;
  for (TopExp_Explorer anExplorer(aShape, TopAbs_EDGE); anExplorer.More(); anExplorer.Next())
  {
    const TopoDS_Edge aCandidate = TopoDS::Edge(anExplorer.Current());
    if (BRep_Tool::Degenerated(aCandidate))
    {
      anEdge = aCandidate;
      break;
    }
  }
  ASSERT_FALSE(anEdge.IsNull());
  BRep_Builder aBuilder;
  aBuilder.SameParameter(anEdge, false);
  aBuilder.SameRange(anEdge, false);
  const double                          aTolerance = BRep_Tool::Tolerance(anEdge);
  const occ::handle<ShapeBuild_ReShape> aContext   = new ShapeBuild_ReShape;
  ShapeFix_Edge                         aFixer;
  aFixer.SetContext(aContext);
  EXPECT_FALSE(aFixer.FixSameParameter(anEdge));
  EXPECT_FALSE(BRep_Tool::SameParameter(anEdge));
  EXPECT_FALSE(BRep_Tool::SameRange(anEdge));
  EXPECT_DOUBLE_EQ(BRep_Tool::Tolerance(anEdge), aTolerance);
  const TopoDS_Edge aResult = TopoDS::Edge(aContext->Apply(anEdge));
  EXPECT_FALSE(aResult.IsSame(anEdge));
  EXPECT_TRUE(BRep_Tool::SameParameter(aResult));
  EXPECT_TRUE(BRep_Tool::SameRange(aResult));
}

TEST(ShapeFix_ShapeTest, FixValidBox)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone());

  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aBox);
  aFixer->Perform();
  const TopoDS_Shape aResult = aFixer->Shape();
  ASSERT_FALSE(aResult.IsNull());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

TEST(ShapeFix_ShapeTest, StatusAfterFix)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone());

  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aBox);
  aFixer->Perform();

  // A valid box should not require any failure-level fixes
  EXPECT_FALSE(aFixer->Status(ShapeExtend_FAIL));
}

TEST(ShapeFix_ShapeTest, SetPrecision)
{
  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape();

  const double aPrecision    = 0.01;
  const double aMinTolerance = 0.001;
  const double aMaxTolerance = 0.1;

  aFixer->SetPrecision(aPrecision);
  aFixer->SetMinTolerance(aMinTolerance);
  aFixer->SetMaxTolerance(aMaxTolerance);

  EXPECT_NEAR(aFixer->Precision(), aPrecision, Precision::Confusion());
  EXPECT_NEAR(aFixer->MinTolerance(), aMinTolerance, Precision::Confusion());
  EXPECT_NEAR(aFixer->MaxTolerance(), aMaxTolerance, Precision::Confusion());
}

TEST(ShapeFix_ShapeTest, FixCompound)
{
  BRepPrimAPI_MakeBox aMakeBox1(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox1 = aMakeBox1.Shape();
  ASSERT_TRUE(aMakeBox1.IsDone());

  BRepPrimAPI_MakeBox aMakeBox2(gp_Pnt(20.0, 0.0, 0.0), 5.0, 5.0, 5.0);
  const TopoDS_Shape& aBox2 = aMakeBox2.Shape();
  ASSERT_TRUE(aMakeBox2.IsDone());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aCompound);
  aFixer->Perform();
  const TopoDS_Shape aResult = aFixer->Shape();
  ASSERT_FALSE(aResult.IsNull());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

// Healing a prism built from a self-intersecting ("bowtie") face must not crash:
// the cap is split into a compound and ShapeFix_Face must not cast it to a face.
TEST(ShapeFix_ShapeTest, HealPrismFromSelfIntersectingFace)
{
  // Bowtie quad: vertices ordered so the two diagonals cross -> the wire self-intersects.
  BRepBuilderAPI_MakePolygon aPoly;
  aPoly.Add(gp_Pnt(0.0, 0.0, 0.0));
  aPoly.Add(gp_Pnt(1.0, 1.0, 0.0));
  aPoly.Add(gp_Pnt(1.0, 0.0, 0.0));
  aPoly.Add(gp_Pnt(0.0, 1.0, 0.0));
  aPoly.Close();
  ASSERT_TRUE(aPoly.IsDone());

  BRepBuilderAPI_MakeFace aMakeFace(aPoly.Wire(), true);
  ASSERT_TRUE(aMakeFace.IsDone());

  BRepPrimAPI_MakePrism aMakePrism(aMakeFace.Face(), gp_Vec(0.0, 0.0, 1.0));
  ASSERT_TRUE(aMakePrism.IsDone());

  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aMakePrism.Shape());
  aFixer->Perform();
  ASSERT_FALSE(aFixer->Shape().IsNull());

  BRepCheck_Analyzer anAnalyzer(aFixer->Shape());
  EXPECT_TRUE(anAnalyzer.IsValid());
}

// Migrated from tests/bugs/heal/bug24881.  A valid box must pass the same
// reorder/gap-fixing sequence used by the DRAW OCC24881 command without
// reporting a failure status for either 3D or 2D gaps.
TEST(ShapeFix_ShapeTest, OCC24881_ValidBoxHasNoWireGapFailures)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();
  ASSERT_FALSE(aBox.IsNull());

  occ::handle<ShapeFix_Wire> aWireFix = new ShapeFix_Wire;
  int                        aNbWires = 0;
  for (TopExp_Explorer aFaceExplorer(aBox, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next())
  {
    const TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
    for (TopoDS_Iterator aWireIterator(aFace); aWireIterator.More(); aWireIterator.Next())
    {
      const TopoDS_Wire aWire = TopoDS::Wire(aWireIterator.Value());
      aWireFix->Load(aWire);
      aWireFix->SetFace(aFace);
      EXPECT_NO_THROW(aWireFix->FixReorder());

      bool aFixed3d = false;
      EXPECT_NO_THROW(aFixed3d = aWireFix->FixGaps3d());
      EXPECT_FALSE(aWireFix->StatusGaps3d(ShapeExtend_FAIL));

      bool aFixed2d = false;
      EXPECT_NO_THROW(aFixed2d = aWireFix->FixGaps2d());
      EXPECT_FALSE(aWireFix->StatusGaps2d(ShapeExtend_FAIL));
      (void)aFixed3d;
      (void)aFixed2d;
      ++aNbWires;
    }
  }

  EXPECT_EQ(aNbWires, 6);
}

// Migrated from tests/bugs/heal/bug25014.  The DRAW stwire +r path must be
// safe even when the input wire contains no edges.
TEST(ShapeFix_ShapeTest, OCC25014_EmptyWireReorderDoesNotThrow)
{
  BRep_Builder aBuilder;
  TopoDS_Wire  anEmptyWire;
  aBuilder.MakeWire(anEmptyWire);

  occ::handle<ShapeExtend_WireData> aWireData  = new ShapeExtend_WireData;
  occ::handle<ShapeAnalysis_Wire>   anAnalyzer = new ShapeAnalysis_Wire;
  anAnalyzer->Load(aWireData);

  ShapeFix_Wire aFixer;
  aFixer.Init(anAnalyzer);
  ShapeAnalysis_WireOrder anOrder(true, Precision::Confusion());
  EXPECT_NO_THROW(anAnalyzer->CheckOrder(anOrder));
  EXPECT_NO_THROW(aFixer.FixReorder(anOrder));
  EXPECT_EQ(anOrder.NbEdges(), 0);
  EXPECT_EQ(anEmptyWire.ShapeType(), TopAbs_WIRE);
}
