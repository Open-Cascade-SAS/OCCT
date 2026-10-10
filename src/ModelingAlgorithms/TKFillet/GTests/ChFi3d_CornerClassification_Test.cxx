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

#include <BRepAdaptor_Curve.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRep_Tool.hxx>
#include <ChFi3d_ChBuilder.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <gtest/gtest.h>

#include <array>
#include <tuple>

namespace
{
// Exercise the existing protected classifier independently of surface walking.
class CornerClassificationBuilder : public ChFi3d_ChBuilder
{
public:
  explicit CornerClassificationBuilder(const TopoDS_Shape& theShape)
      : ChFi3d_ChBuilder(theShape, Precision::Confusion())
  {
  }

  bool Classify(const TopoDS_Vertex&                     theVertex,
                TopoDS_Edge                              theSpineEdge,
                const std::array<ChFiDS_CommonPoint, 4>& thePoints,
                const bool                               theFirst,
                const bool                               theSinglePatch)
  {
    theSpineEdge.Orientation(TopAbs_FORWARD);
    if (TopExp::FirstVertex(theSpineEdge).IsSame(theVertex) != theFirst)
    {
      theSpineEdge.Reverse();
    }
    occ::handle<ChFiDS_Stripe> aStripe = new ChFiDS_Stripe;
    aStripe->ChangeSpine()             = new ChFiDS_Spine(Precision::Confusion());
    aStripe->ChangeSpine()->SetEdges(theSpineEdge);
    aStripe->ChangeSetOfSurfData()     = new NCollection_HSequence<occ::handle<ChFiDS_SurfData>>;
    occ::handle<ChFiDS_SurfData> anEnd = new ChFiDS_SurfData;
    occ::handle<ChFiDS_SurfData> aNeighbor = new ChFiDS_SurfData;
    for (int aSide = 1; aSide <= 2; ++aSide)
    {
      anEnd->ChangeVertex(theFirst, aSide)     = thePoints[aSide - 1];
      aNeighbor->ChangeVertex(theFirst, aSide) = thePoints[aSide + 1];
    }
    if (!theFirst && !theSinglePatch)
    {
      aStripe->ChangeSetOfSurfData()->Append(aNeighbor);
    }
    aStripe->ChangeSetOfSurfData()->Append(anEnd);
    if (theFirst && !theSinglePatch)
    {
      aStripe->ChangeSetOfSurfData()->Append(aNeighbor);
    }
    myVDataMap.Add(theVertex, aStripe);
    return MoreSurfdata(1);
  }
};
} // namespace

class ChFi3d_CornerClassification
    : public testing::TestWithParam<std::tuple<int, int, bool, bool, int>>
{
};

TEST_P(ChFi3d_CornerClassification, MoreSurfdata_PermutedSupports_MatchesSharedRestriction)
{
  const auto [aSharedArc, aNeighborSide, toSwapSupports, isFirst, aScenario] = GetParam();
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10., 10., 10.);
  ASSERT_TRUE(BRepCheck_Analyzer(aBox).IsValid());
  TopExp_Explorer            aFaceIt(aBox, TopAbs_FACE);
  const TopoDS_Face          aFace = TopoDS::Face(aFaceIt.Current());
  TopExp_Explorer            aVertexIt(aFace, TopAbs_VERTEX);
  const TopoDS_Vertex        aVertex = TopoDS::Vertex(aVertexIt.Current());
  std::array<TopoDS_Edge, 2> anArcs;
  int                        anArcCount = 0;
  for (TopExp_Explorer anIt(aFace, TopAbs_EDGE); anIt.More(); anIt.Next())
  {
    const TopoDS_Edge anEdge = TopoDS::Edge(anIt.Current());
    if (TopExp::FirstVertex(anEdge).IsSame(aVertex) || TopExp::LastVertex(anEdge).IsSame(aVertex))
    {
      ASSERT_LT(anArcCount, 2);
      anArcs[anArcCount++] = anEdge;
    }
  }
  ASSERT_EQ(anArcCount, 2);
  TopoDS_Edge aSpine;
  for (TopExp_Explorer anIt(aBox, TopAbs_EDGE); anIt.More(); anIt.Next())
  {
    const TopoDS_Edge anEdge = TopoDS::Edge(anIt.Current());
    if (!anEdge.IsSame(anArcs[0]) && !anEdge.IsSame(anArcs[1])
        && (TopExp::FirstVertex(anEdge).IsSame(aVertex)
            || TopExp::LastVertex(anEdge).IsSame(aVertex)))
    {
      aSpine = anEdge;
      break;
    }
  }
  ASSERT_FALSE(aSpine.IsNull());
  std::array<ChFiDS_CommonPoint, 4> aPoints;
  for (int anArc = 0; anArc < 2; ++anArc)
  {
    const BRepAdaptor_Curve aCurve(anArcs[anArc]);
    const double            aParameter = (aCurve.FirstParameter() + aCurve.LastParameter()) / 2.;
    auto&                   aPoint     = aPoints[toSwapSupports ? 1 - anArc : anArc];
    aPoint.SetPoint(aCurve.Value(aParameter));
    aPoint.SetArc(Precision::Confusion(), anArcs[anArc], aParameter, TopAbs_FORWARD);
  }
  auto& aShared = aPoints[2 + aNeighborSide];
  aShared       = aPoints[toSwapSupports ? 1 - aSharedArc : aSharedArc];
  if (aScenario == 1) // Same position, but no boundary-edge association.
  {
    const gp_Pnt aPoint = aShared.Point();
    aShared.Reset();
    aShared.SetPoint(aPoint);
  }
  else if (aScenario == 2) // Same position associated with an unrelated restriction.
  {
    aShared.SetArc(Precision::Confusion(), aSpine, 0., TopAbs_FORWARD);
  }
  else if (aScenario == 3) // Correct restriction, different point on that restriction.
  {
    const BRepAdaptor_Curve aCurve(anArcs[aSharedArc]);
    aShared.SetPoint(aCurve.Value(aCurve.FirstParameter()));
    aShared.SetArc(Precision::Confusion(),
                   anArcs[aSharedArc],
                   aCurve.FirstParameter(),
                   TopAbs_FORWARD);
  }
  CornerClassificationBuilder aBuilder(aBox);
  EXPECT_EQ(aBuilder.Classify(aVertex, aSpine, aPoints, isFirst, aScenario == 4), aScenario == 0);
}

INSTANTIATE_TEST_SUITE_P(RestrictionOrder,
                         ChFi3d_CornerClassification,
                         testing::Combine(testing::Values(0, 1),
                                          testing::Values(0, 1),
                                          testing::Bool(),
                                          testing::Bool(),
                                          testing::Values(0, 1, 2, 3, 4)));
