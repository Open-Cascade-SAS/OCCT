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
#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRBRep_TypeOfResultingEdge.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>

#include <gtest/gtest.h>

#include <array>

namespace
{
struct HLRResultCounts
{
  int VisibleEdges = 0;
  int HiddenEdges  = 0;
};

TopoDS_Face makeRectangularFace(const double theZ,
                                const double theUMin,
                                const double theUMax,
                                const double theVMin,
                                const double theVMax)
{
  return BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0.0, 0.0, theZ), gp::DZ()),
                                 theUMin,
                                 theUMax,
                                 theVMin,
                                 theVMax)
    .Face();
}

int countResultEdges(const occ::handle<HLRBRep_Algo>& theAlgorithm, const bool theVisible)
{
  HLRBRep_HLRToShape aResultBuilder(theAlgorithm);
  int                aNbEdges = 0;
  for (int aType = HLRBRep_IsoLine; aType <= HLRBRep_Sharp; ++aType)
  {
    const TopoDS_Shape aResult =
      aResultBuilder.CompoundOfEdges(static_cast<HLRBRep_TypeOfResultingEdge>(aType),
                                     theVisible,
                                     false);
    for (TopExp_Explorer anEdges(aResult, TopAbs_EDGE); anEdges.More(); anEdges.Next())
    {
      ++aNbEdges;
    }
  }
  return aNbEdges;
}

occ::handle<HLRBRep_Algo> performExactHLR(const TopoDS_Shape& theShape)
{
  occ::handle<HLRBRep_Algo> anAlgorithm = new HLRBRep_Algo();
  anAlgorithm->Projector(HLRAlgo_Projector(gp_Ax2(gp::Origin(), gp::DZ())));
  anAlgorithm->Add(theShape);
  anAlgorithm->Update();
  anAlgorithm->Hide();
  return anAlgorithm;
}

HLRResultCounts performExactHLR(const TopoDS_Face& theFirstFace, const TopoDS_Face& theSecondFace)
{
  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, theFirstFace);
  aBuilder.Add(aCompound, theSecondFace);

  const occ::handle<HLRBRep_Algo> anAlgorithm = performExactHLR(aCompound);

  return {countResultEdges(anAlgorithm, true), countResultEdges(anAlgorithm, false)};
}
} // namespace

TEST(HLRBRep_DataTest, ZeroSizedStorageCanBeDestroyedRepeatedly)
{
  occ::handle<HLRBRep_Data> aData = new HLRBRep_Data(0, 0, 0);
  EXPECT_EQ(aData->NbVertices(), 0);
  EXPECT_EQ(aData->NbEdges(), 0);
  EXPECT_EQ(aData->NbFaces(), 0);

  aData->Destroy();
  aData->Destroy();
}

TEST(HLRBRep_DataTest, IntersectionCacheIsStableAcrossEdgeOrdering)
{
  const TopoDS_Face aFrontFace = makeRectangularFace(2.0, -4.0, 4.0, -3.0, 3.0);
  const TopoDS_Face aBackFace  = makeRectangularFace(0.0, -1.0, 7.0, -5.0, 1.0);

  const HLRResultCounts aForwardOrder = performExactHLR(aFrontFace, aBackFace);
  const HLRResultCounts aReverseOrder = performExactHLR(aBackFace, aFrontFace);

  EXPECT_EQ(aForwardOrder.VisibleEdges, 8);
  EXPECT_EQ(aForwardOrder.HiddenEdges, 2);
  EXPECT_EQ(aReverseOrder.VisibleEdges, aForwardOrder.VisibleEdges);
  EXPECT_EQ(aReverseOrder.HiddenEdges, aForwardOrder.HiddenEdges);
}

TEST(HLRBRep_DataTest, RejectionBitsCrossWordBoundary)
{
  std::array<TopoDS_Face, 9> aFaces;
  TopoDS_Compound            aForwardCompound;
  TopoDS_Compound            aReverseCompound;
  BRep_Builder               aBuilder;
  aBuilder.MakeCompound(aForwardCompound);
  aBuilder.MakeCompound(aReverseCompound);

  for (size_t anIndex = 0; anIndex < aFaces.size(); ++anIndex)
  {
    const double anOffset = static_cast<double>(anIndex) * 0.25;
    aFaces[anIndex]       = makeRectangularFace(static_cast<double>(anIndex),
                                                -4.0 + anOffset,
                                                4.0 + anOffset,
                                                -3.0 + anOffset,
                                                3.0 + anOffset);
    aBuilder.Add(aForwardCompound, aFaces[anIndex]);
  }
  for (size_t anIndex = aFaces.size(); anIndex > 0; --anIndex)
  {
    aBuilder.Add(aReverseCompound, aFaces[anIndex - 1]);
  }

  const occ::handle<HLRBRep_Algo> aForwardAlgorithm = performExactHLR(aForwardCompound);
  const occ::handle<HLRBRep_Algo> aReverseAlgorithm = performExactHLR(aReverseCompound);
  ASSERT_FALSE(aForwardAlgorithm->DataStructure().IsNull());
  ASSERT_FALSE(aReverseAlgorithm->DataStructure().IsNull());
  EXPECT_EQ(aForwardAlgorithm->DataStructure()->NbEdges(), 36);
  EXPECT_EQ(aReverseAlgorithm->DataStructure()->NbEdges(), 36);
  const HLRResultCounts aForwardCounts = {countResultEdges(aForwardAlgorithm, true),
                                          countResultEdges(aForwardAlgorithm, false)};
  const HLRResultCounts aReverseCounts = {countResultEdges(aReverseAlgorithm, true),
                                          countResultEdges(aReverseAlgorithm, false)};
  EXPECT_EQ(aForwardCounts.VisibleEdges, 36);
  EXPECT_EQ(aForwardCounts.HiddenEdges, 16);
  EXPECT_EQ(aReverseCounts.VisibleEdges, aForwardCounts.VisibleEdges);
  EXPECT_EQ(aReverseCounts.HiddenEdges, aForwardCounts.HiddenEdges);
}
