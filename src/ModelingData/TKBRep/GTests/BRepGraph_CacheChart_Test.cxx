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

// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#include <BRepGraph.hxx>
#include <BRepGraph_CacheChart.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_LinearVector.hxx>

#include <gtest/gtest.h>

namespace
{

BRepGraph_FaceId findPeriodicFace(const BRepGraph& theGraph)
{
  for (BRepGraph_FaceId aFace = BRepGraph_FaceId::Start();
       aFace.IsValid(theGraph.Topo().Faces().Nb());
       ++aFace)
  {
    if (aFace.IsRemoved(theGraph))
    {
      continue;
    }
    const occ::handle<Geom_Surface>& aSurface = theGraph.Topo().Faces().Surface(aFace);
    if (!aSurface.IsNull() && (aSurface->IsUPeriodic() || aSurface->IsVPeriodic()))
    {
      return aFace;
    }
  }
  return BRepGraph_FaceId();
}

NCollection_LinearVector<BRepGraph_FaceId> findPeriodicFaces(const BRepGraph& theGraph)
{
  NCollection_LinearVector<BRepGraph_FaceId> aFaces;
  for (BRepGraph_FaceId aFace = BRepGraph_FaceId::Start();
       aFace.IsValid(theGraph.Topo().Faces().Nb());
       ++aFace)
  {
    if (aFace.IsRemoved(theGraph))
    {
      continue;
    }
    const occ::handle<Geom_Surface>& aSurface = theGraph.Topo().Faces().Surface(aFace);
    if (!aSurface.IsNull() && (aSurface->IsUPeriodic() || aSurface->IsVPeriodic()))
    {
      aFaces.Append(aFace);
    }
  }
  return aFaces;
}

uint32_t countBoundaryKind(const BRepGraph_CacheChart::Result&      theResult,
                           const BRepGraph_CacheChart::BoundaryKind theKind)
{
  uint32_t aCount = 0;
  for (const BRepGraph_CacheChart::Loop& aLoop : theResult.GetLoops())
  {
    for (const BRepGraph_CacheChart::BoundaryUse& aUse : aLoop.Boundary)
    {
      aCount += aUse.Kind == theKind ? 1u : 0u;
    }
  }
  return aCount;
}

} // namespace

TEST(BRepGraph_CacheChartTest, CylinderRetainsSeamAsDerivedChartOccurrence)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape()).IsOk());

  const BRepGraph_FaceId aFace = findPeriodicFace(aGraph);
  ASSERT_TRUE(aFace.IsValid());
  const occ::handle<BRepGraph_CacheChart::Result> aChart =
    BRepGraph_CacheChart::Build(aGraph, aFace);
  ASSERT_FALSE(aChart.IsNull());
  EXPECT_EQ(aChart->GetStatus(), BRepGraph_CacheChart::Status::Ready);
  EXPECT_TRUE(aChart->IsUPeriodic());
  EXPECT_NEAR(aChart->GetUPeriod(), 2.0 * M_PI, 1.0e-12);
  ASSERT_FALSE(aChart->GetTransitions().IsEmpty());
  EXPECT_GT(countBoundaryKind(*aChart, BRepGraph_CacheChart::BoundaryKind::VirtualCut), 0u);
  EXPECT_TRUE(aChart->GetLoops().IsEmpty() || aChart->GetLoops().First().IsClosed);
}

TEST(BRepGraph_CacheChartTest, SphereUsesPeriodicChartWithExplicitBoundaries)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeSphere(5.0).Shape()).IsOk());

  const BRepGraph_FaceId aFace = findPeriodicFace(aGraph);
  ASSERT_TRUE(aFace.IsValid());
  EXPECT_GT(BRepGraph_Tool::Face::NbWires(aGraph, aFace), 0u);

  const occ::handle<BRepGraph_CacheChart::Result> aChart =
    BRepGraph_CacheChart::Build(aGraph, aFace);
  ASSERT_FALSE(aChart.IsNull());
  EXPECT_EQ(aChart->GetStatus(), BRepGraph_CacheChart::Status::Ready);
  EXPECT_TRUE(aChart->IsUPeriodic());
}

TEST(BRepGraph_CacheChartTest, CacheIsFreshPerFaceGenerationAndBoundsExplicitCuts)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape()).IsOk());
  const BRepGraph_FaceId aFace = findPeriodicFace(aGraph);
  ASSERT_TRUE(aFace.IsValid());

  const occ::handle<BRepGraph_CacheChart> aCache =
    aGraph.CacheRegistry().Ensure<BRepGraph_CacheChart>();
  ASSERT_FALSE(aCache.IsNull());

  const occ::handle<BRepGraph_CacheChart::Result> aFirst  = aCache->Get(aFace);
  const occ::handle<BRepGraph_CacheChart::Result> aSecond = aCache->Get(aFace);
  EXPECT_EQ(aFirst, aSecond);
  EXPECT_EQ(aCache->NbEntries(), 1u);

  BRepGraph_CacheChart::Policy anExplicit;
  anExplicit.Selection = BRepGraph_CacheChart::Policy::CutSelection::Explicit;
  anExplicit.ExplicitU = 0.25;
  const occ::handle<BRepGraph_CacheChart::Result> anAtQuarter = aCache->Get(aFace, anExplicit);
  anExplicit.ExplicitU                                        = 1.25;
  const occ::handle<BRepGraph_CacheChart::Result> anAtOneAndQuarter =
    aCache->Get(aFace, anExplicit);
  EXPECT_NE(anAtQuarter, anAtOneAndQuarter);
  EXPECT_EQ(aCache->NbEntries(), 2u);

  aGraph.Editor().Faces().SetTolerance(aFace, 2.0e-6);
  const occ::handle<BRepGraph_CacheChart::Result> aAfterEdit = aCache->Get(aFace);
  EXPECT_NE(aFirst, aAfterEdit);
  EXPECT_EQ(aCache->NbEntries(), 2u);
}

TEST(BRepGraph_CacheChartTest, CacheBoundsPolicyVariantsPerFace)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape()).IsOk());
  const BRepGraph_FaceId aFace = findPeriodicFace(aGraph);
  ASSERT_TRUE(aFace.IsValid());

  const occ::handle<BRepGraph_CacheChart> aCache =
    aGraph.CacheRegistry().Ensure<BRepGraph_CacheChart>();
  ASSERT_FALSE(aCache.IsNull());

  for (int anIndex = 0; anIndex < 16; ++anIndex)
  {
    BRepGraph_CacheChart::Policy aPolicy;
    aPolicy.Tolerances.ToleranceU = 1.0e-9 * static_cast<double>(anIndex + 1);
    EXPECT_FALSE(aCache->Get(aFace, aPolicy).IsNull());
  }
  EXPECT_LE(aCache->NbEntries(), 8u);
}

TEST(BRepGraph_CacheChartTest, CacheBoundsVariantsIndependentlyForEachFace)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape()).IsOk());
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeCylinder(7.0, 12.0).Shape()).IsOk());

  const NCollection_LinearVector<BRepGraph_FaceId> aFaces = findPeriodicFaces(aGraph);
  ASSERT_GE(aFaces.Size(), 2u);

  const occ::handle<BRepGraph_CacheChart> aCache =
    aGraph.CacheRegistry().Ensure<BRepGraph_CacheChart>();
  ASSERT_FALSE(aCache.IsNull());

  const occ::handle<BRepGraph_CacheChart::Result> aFirstFace  = aCache->Get(aFaces.Value(0));
  const occ::handle<BRepGraph_CacheChart::Result> aSecondFace = aCache->Get(aFaces.Value(1));
  for (size_t aFaceIndex = 0; aFaceIndex < 2; ++aFaceIndex)
  {
    for (int aVariant = 1; aVariant < 8; ++aVariant)
    {
      BRepGraph_CacheChart::Policy aPolicy;
      aPolicy.Tolerances.ToleranceU = 1.0e-9 * static_cast<double>(aVariant + 1);
      EXPECT_FALSE(aCache->Get(aFaces.Value(aFaceIndex), aPolicy).IsNull());
    }
  }

  EXPECT_EQ(aCache->NbEntries(), 16u);
  EXPECT_EQ(aCache->Get(aFaces.Value(0)), aFirstFace);
  EXPECT_EQ(aCache->Get(aFaces.Value(1)), aSecondFace);

  BRepGraph_CacheChart::Policy anAdditionalPolicy;
  anAdditionalPolicy.Tolerances.ToleranceU = 9.0e-9;
  EXPECT_FALSE(aCache->Get(aFaces.Value(0), anAdditionalPolicy).IsNull());
  EXPECT_EQ(aCache->NbEntries(), 16u);
  EXPECT_EQ(aCache->Get(aFaces.Value(1)), aSecondFace);
}
