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

#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Validate.hxx>

#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

namespace
{
TopLoc_Location translationLocation(const double theX, const double theY, const double theZ)
{
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(theX, theY, theZ));
  return TopLoc_Location(aTrsf);
}

TopLoc_Location rotationZLocation(const double theAngle)
{
  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), theAngle);
  return TopLoc_Location(aTrsf);
}

bool locationsEquivalent(const TopLoc_Location& theActual, const TopLoc_Location& theExpected)
{
  const gp_Pnt aProbe(3.0, 5.0, 7.0);
  const gp_Pnt anActualPoint   = aProbe.Transformed(theActual.Transformation());
  const gp_Pnt anExpectedPoint = aProbe.Transformed(theExpected.Transformation());
  return anActualPoint.Distance(anExpectedPoint) <= Precision::Confusion();
}

void expectLocationsEquivalent(const TopLoc_Location& theActual, const TopLoc_Location& theExpected)
{
  const gp_Pnt aProbe(3.0, 5.0, 7.0);
  const gp_Pnt anActualPoint   = aProbe.Transformed(theActual.Transformation());
  const gp_Pnt anExpectedPoint = aProbe.Transformed(theExpected.Transformation());
  EXPECT_NEAR(anActualPoint.X(), anExpectedPoint.X(), Precision::Confusion());
  EXPECT_NEAR(anActualPoint.Y(), anExpectedPoint.Y(), Precision::Confusion());
  EXPECT_NEAR(anActualPoint.Z(), anExpectedPoint.Z(), Precision::Confusion());
}
} // namespace

TEST(BRepGraph_ParentExplorerTest, FaceParents_All_CountAndOrder)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_ParentExplorer anExp(aGraph, BRepGraph_FaceId::Start());
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::Occurrence);

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::Product);

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, FaceParents_TypedSolid_OneResult)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes2 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_ParentExplorer anExp(aGraph, BRepGraph_FaceId::Start(), BRepGraph_NodeId::Kind::Solid);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, FaceParents_DirectParents_StopsAtImmediateShell)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes3 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, FaceParents_DirectParents_ExposeChildAndRef)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes4 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));
  EXPECT_EQ(anExp.CurrentChild(), BRepGraph_NodeId(BRepGraph_FaceId::Start()));
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Reference);

  const BRepGraph_FaceRefId aFaceRefId =
    aGraph.Refs().Faces().IdsOf(BRepGraph_ShellId::Start()).Value(0);
  EXPECT_EQ(anExp.CurrentRef(), BRepGraph_RefId(aFaceRefId));
}

TEST(BRepGraph_ParentExplorerTest, AvoidKind_Solid_PrunesProducts)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes5 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Product,
                                 BRepGraph_NodeId::Kind::Solid,
                                 false);
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, AvoidKind_EmitBoundary_ReturnsSolidInsteadOfProducts)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes6 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Product,
                                 BRepGraph_NodeId::Kind::Solid,
                                 true);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, AvoidKind_SameAsTarget_IsIgnored)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes7 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Solid,
                                 BRepGraph_NodeId::Kind::Solid,
                                 false);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, AllParents_AvoidSolid_PrunesProducts)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes8 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Solid,
                                 false);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, AllParents_AvoidSolidEmitBoundary_ReturnsShellAndSolid)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes9 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_FaceId::Start(),
                                 BRepGraph_NodeId::Kind::Solid,
                                 true);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_ShellId::Start()));

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, SharedProduct_ProductParentsKeepDistinctContexts)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes10 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aPart = aGraph.Editor().Products().Add(BRepGraph_SolidId::Start());
  aGraph.Editor().Products().AppendDocumentRoot(aPart);
  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(anAssembly);
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aT1;
  aT1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  gp_Trsf aT2;
  aT2.SetTranslation(gp_Vec(25.0, 0.0, 0.0));
  ASSERT_TRUE(aGraph.Editor().Products().Append(anAssembly, aPart, TopLoc_Location(aT1)).IsValid());
  ASSERT_TRUE(aGraph.Editor().Products().Append(anAssembly, aPart, TopLoc_Location(aT2)).IsValid());

  int             aPartCount = 0;
  TopLoc_Location aLoc1;
  TopLoc_Location aLoc2;
  for (BRepGraph_ParentExplorer anExp(aGraph,
                                      BRepGraph_SolidId::Start(),
                                      BRepGraph_NodeId::Kind::Product);
       anExp.More();
       anExp.Next())
  {
    if (anExp.Current().DefId != BRepGraph_NodeId(aPart))
    {
      continue;
    }
    if (aPartCount == 0)
    {
      aLoc1 = anExp.Current().Location;
    }
    else if (aPartCount == 1)
    {
      aLoc2 = anExp.Current().Location;
    }
    ++aPartCount;
  }

  ASSERT_EQ(aPartCount, 2);
  EXPECT_FALSE(aLoc1.IsEqual(aLoc2));
}

TEST(BRepGraph_ParentExplorerTest, DeepProductOccurrenceChain_ComposesToTopProduct)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aPart         = BRepGraph_ProductId::Start();
  BRepGraph_ProductId       aChildProduct = aPart;
  double                    anExpectedX   = 0.0;
  BRepGraph_ProductId       aTopProduct;
  for (int aLevel = 1; aLevel <= 6; ++aLevel)
  {
    aTopProduct = aGraph.Editor().Products().Add();
    aGraph.Editor().Products().AppendDocumentRoot(aTopProduct);
    ASSERT_TRUE(aTopProduct.IsValid());

    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(static_cast<double>(aLevel), 0.0, 0.0));
    ASSERT_TRUE(aGraph.Editor()
                  .Products()
                  .Append(aTopProduct, aChildProduct, TopLoc_Location(aTrsf))
                  .IsValid());
    anExpectedX += static_cast<double>(aLevel);
    aChildProduct = aTopProduct;
  }

  bool foundTop = false;
  for (BRepGraph_ParentExplorer anExp(aGraph,
                                      BRepGraph_SolidId::Start(),
                                      BRepGraph_NodeId::Kind::Product);
       anExp.More();
       anExp.Next())
  {
    if (anExp.Current().DefId != BRepGraph_NodeId(aTopProduct))
    {
      continue;
    }
    foundTop = true;
    EXPECT_NEAR(anExp.LeafLocation().Transformation().TranslationPart().X(),
                anExpectedX,
                Precision::Confusion());
  }
  EXPECT_TRUE(foundTop);
}

TEST(BRepGraph_ParentExplorerTest, DeepProductOccurrenceChain_ReachesNestedOccurrences)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_ProductId aPart        = aGraph.Editor().Products().Add();
  const BRepGraph_ProductId aSubAssembly = aGraph.Editor().Products().Add();
  const BRepGraph_ProductId aTopAssembly = aGraph.Editor().Products().Add();
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(aSubAssembly.IsValid());
  ASSERT_TRUE(aTopAssembly.IsValid());

  const BRepGraph_OccurrenceId aPartOccurrence =
    aGraph.Editor().Products().Append(aSubAssembly, aPart, TopLoc_Location());
  const BRepGraph_OccurrenceId aSubOccurrence =
    aGraph.Editor().Products().Append(aTopAssembly, aSubAssembly, TopLoc_Location());
  ASSERT_TRUE(aPartOccurrence.IsValid());
  ASSERT_TRUE(aSubOccurrence.IsValid());

  bool hasPartOccurrence = false;
  bool hasSubOccurrence  = false;
  int  aCount            = 0;
  for (BRepGraph_ParentExplorer anIt(aGraph, aPart, BRepGraph_NodeId::Kind::Occurrence);
       anIt.More();
       anIt.Next())
  {
    hasPartOccurrence |= anIt.Current().DefId == BRepGraph_NodeId(aPartOccurrence);
    hasSubOccurrence |= anIt.Current().DefId == BRepGraph_NodeId(aSubOccurrence);
    ++aCount;
  }

  EXPECT_EQ(aCount, 2);
  EXPECT_TRUE(hasPartOccurrence);
  EXPECT_TRUE(hasSubOccurrence);
}

TEST(BRepGraph_ParentExplorerTest, DeepCompoundChain_ReachesTopCompound)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_NodeId     aChild = BRepGraph_SolidId::Start();
  BRepGraph_CompoundId aTopCompound;
  for (int aLevel = 0; aLevel < 8; ++aLevel)
  {
    NCollection_LinearVector<BRepGraph_NodeId> aChildren;
    aChildren.Append(aChild);
    aTopCompound = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
    ASSERT_TRUE(aTopCompound.IsValid());
    aChild = aTopCompound;
  }

  int  aCompoundCount = 0;
  bool foundTop       = false;
  for (BRepGraph_ParentExplorer anExp(aGraph,
                                      BRepGraph_SolidId::Start(),
                                      BRepGraph_NodeId::Kind::Compound);
       anExp.More();
       anExp.Next())
  {
    ++aCompoundCount;
    foundTop |= anExp.Current().DefId == BRepGraph_NodeId(aTopCompound);
  }
  EXPECT_EQ(aCompoundCount, 8);
  EXPECT_TRUE(foundTop);
}

TEST(BRepGraph_ParentExplorerTest, VertexChildOfDeepCompoundChain_ReachesTopCompound)
{
  BRepGraph aGraph;
  aGraph.Clear();
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 1.0e-7);
  ASSERT_TRUE(aVertex.IsValid());

  BRepGraph_NodeId     aChild = aVertex;
  BRepGraph_CompoundId aTopCompound;
  double               anExpectedX = 0.0;
  for (int aLevel = 1; aLevel <= 4; ++aLevel)
  {
    NCollection_LinearVector<BRepGraph_NodeId> aChildren;
    aChildren.Append(aChild);
    aTopCompound = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
    ASSERT_TRUE(aTopCompound.IsValid());

    const TopLoc_Location aStepLoc = translationLocation(static_cast<double>(aLevel), 0.0, 0.0);
    const NCollection_LinearVector<BRepGraph_ChildRefId>& aChildRefs =
      aGraph.Refs().Children().IdsOf(aTopCompound);
    ASSERT_EQ(aChildRefs.Size(), 1);
    aGraph.Editor().Gen().SetChildRefLocalLocation(aChildRefs.First(), aStepLoc);
    anExpectedX += static_cast<double>(aLevel);

    aChild = aTopCompound;
  }

  int  aCompoundCount = 0;
  bool foundTop       = false;
  for (BRepGraph_ParentExplorer anExp(aGraph, aVertex, BRepGraph_NodeId::Kind::Compound);
       anExp.More();
       anExp.Next())
  {
    ++aCompoundCount;
    if (anExp.Current().DefId == BRepGraph_NodeId(aTopCompound))
    {
      foundTop = true;
      EXPECT_NEAR(anExp.LeafLocation().Transformation().TranslationPart().X(),
                  anExpectedX,
                  Precision::Confusion());
    }
  }

  EXPECT_EQ(aCompoundCount, 4);
  EXPECT_TRUE(foundTop);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraph_ParentExplorerTest, MixedProductOccurrenceAndCompoundChain_PreservesLocations)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const TopLoc_Location aComp1Loc = translationLocation(2.0, 0.0, 0.0);
  const TopLoc_Location aComp2Loc = rotationZLocation(0.25);
  const TopLoc_Location aComp3Loc = translationLocation(0.0, 3.0, 0.0);
  const TopLoc_Location anOcc1Loc = translationLocation(0.0, 0.0, 5.0);
  const TopLoc_Location anOcc2Loc = rotationZLocation(-0.5);

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  const BRepGraph_CompoundId aComp1 = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aComp1.IsValid());
  ASSERT_EQ(aGraph.Refs().Children().IdsOf(aComp1).Size(), 1);
  aGraph.Editor().Gen().SetChildRefLocalLocation(aGraph.Refs().Children().IdsOf(aComp1).First(),
                                                 aComp1Loc);

  aChildren.Clear();
  aChildren.Append(BRepGraph_NodeId(aComp1));
  const BRepGraph_CompoundId aComp2 = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aComp2.IsValid());
  ASSERT_EQ(aGraph.Refs().Children().IdsOf(aComp2).Size(), 1);
  aGraph.Editor().Gen().SetChildRefLocalLocation(aGraph.Refs().Children().IdsOf(aComp2).First(),
                                                 aComp2Loc);

  aChildren.Clear();
  aChildren.Append(BRepGraph_NodeId(aComp2));
  const BRepGraph_CompoundId aComp3 = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aComp3.IsValid());
  ASSERT_EQ(aGraph.Refs().Children().IdsOf(aComp3).Size(), 1);
  aGraph.Editor().Gen().SetChildRefLocalLocation(aGraph.Refs().Children().IdsOf(aComp3).First(),
                                                 aComp3Loc);

  const BRepGraph_ProductId aPart = aGraph.Editor().Products().Add(aComp3);
  aGraph.Editor().Products().AppendDocumentRoot(aPart);
  const BRepGraph_ProductId aMidAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aMidAssembly);
  const BRepGraph_ProductId aTopAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aTopAssembly);
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(aMidAssembly.IsValid());
  ASSERT_TRUE(aTopAssembly.IsValid());

  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().Append(aMidAssembly, aPart, anOcc1Loc);
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().Append(aTopAssembly, aMidAssembly, anOcc2Loc);
  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());

  const TopLoc_Location aToPart  = anOcc2Loc * anOcc1Loc;
  const TopLoc_Location aToComp3 = aToPart;
  const TopLoc_Location aToComp2 = aToComp3 * aComp3Loc;
  const TopLoc_Location aToComp1 = aToComp2 * aComp2Loc;
  const TopLoc_Location aToSolid = aToComp1 * aComp1Loc;

  bool hasTopAssembly = false;
  bool hasMidAssembly = false;
  bool hasPart        = false;
  bool hasOcc2        = false;
  bool hasOcc1        = false;
  bool hasComp3       = false;
  bool hasComp2       = false;
  bool hasComp1       = false;

  for (BRepGraph_ParentExplorer anExp(aGraph, BRepGraph_SolidId::Start()); anExp.More();
       anExp.Next())
  {
    if (!locationsEquivalent(anExp.LeafLocation(), aToSolid))
    {
      continue;
    }

    const BRepGraph_NodeId aCurrent = anExp.Current().DefId;
    if (aCurrent == BRepGraph_NodeId(aTopAssembly))
    {
      hasTopAssembly = true;
      expectLocationsEquivalent(anExp.Current().Location, TopLoc_Location());
    }
    else if (aCurrent == BRepGraph_NodeId(anOcc2))
    {
      hasOcc2 = true;
      expectLocationsEquivalent(anExp.Current().Location, anOcc2Loc);
    }
    else if (aCurrent == BRepGraph_NodeId(aMidAssembly))
    {
      hasMidAssembly = true;
      expectLocationsEquivalent(anExp.Current().Location, anOcc2Loc);
    }
    else if (aCurrent == BRepGraph_NodeId(anOcc1))
    {
      hasOcc1 = true;
      expectLocationsEquivalent(anExp.Current().Location, aToPart);
    }
    else if (aCurrent == BRepGraph_NodeId(aPart))
    {
      hasPart = true;
      expectLocationsEquivalent(anExp.Current().Location, aToPart);
    }
    else if (aCurrent == BRepGraph_NodeId(aComp3))
    {
      hasComp3 = true;
      expectLocationsEquivalent(anExp.Current().Location, aToComp3);
    }
    else if (aCurrent == BRepGraph_NodeId(aComp2))
    {
      hasComp2 = true;
      expectLocationsEquivalent(anExp.Current().Location, aToComp2);
    }
    else if (aCurrent == BRepGraph_NodeId(aComp1))
    {
      hasComp1 = true;
      expectLocationsEquivalent(anExp.Current().Location, aToComp1);
    }
  }

  EXPECT_TRUE(hasTopAssembly);
  EXPECT_TRUE(hasMidAssembly);
  EXPECT_TRUE(hasPart);
  EXPECT_TRUE(hasOcc2);
  EXPECT_TRUE(hasOcc1);
  EXPECT_TRUE(hasComp3);
  EXPECT_TRUE(hasComp2);
  EXPECT_TRUE(hasComp1);
}

TEST(BRepGraph_ParentExplorerTest, ShapeRootDirectParent_IsOccurrenceNotProductShortcut)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes11 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  // BRepGraph::ShapesView::Add() auto-creates a root Product for the shape root node.
  // Use that product instead of creating a duplicate.
  ASSERT_GT(aGraph.Topo().Products().Nb(), 0);
  const BRepGraph_ProductId aPart(0);
  ASSERT_EQ(aGraph.Topo().Products().NbComponents(aPart), 1);
  const BRepGraph_OccurrenceId aRootOccurrence = aGraph.Topo().Products().Component(aPart, 0);
  ASSERT_TRUE(aRootOccurrence.IsValid());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_SolidId::Start(),
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(aRootOccurrence));
  EXPECT_EQ(anExp.CurrentChild(), BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Structural);
  EXPECT_FALSE(anExp.CurrentRef().IsValid());

  anExp.Next();
  EXPECT_FALSE(anExp.More());

  BRepGraph_ParentExplorer aProductExp(aGraph,
                                       BRepGraph_SolidId::Start(),
                                       BRepGraph_NodeId::Kind::Product);
  ASSERT_TRUE(aProductExp.More());
  EXPECT_EQ(aProductExp.Current().DefId, BRepGraph_NodeId(aPart));
}

TEST(BRepGraph_ParentExplorerTest, TargetProductAvoidCompound_StopsAtCompound)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, BRepPrimAPI_MakeBox(10, 20, 30).Shape());

  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes = aGraph.Shapes().Add(aCompound);
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Topo().Products().Nb(), 0);

  BRepGraph_ParentExplorer anExp(aGraph,
                                 BRepGraph_SolidId::Start(),
                                 BRepGraph_NodeId::Kind::Product,
                                 BRepGraph_NodeId::Kind::Compound,
                                 false);
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ParentExplorerTest, OccurrenceParent_ExposeOccurrenceRef)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes12 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aPart = aGraph.Editor().Products().Add(BRepGraph_SolidId::Start());
  aGraph.Editor().Products().AppendDocumentRoot(aPart);
  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(anAssembly);
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  const BRepGraph_OccurrenceId anOccurrence =
    aGraph.Editor().Products().Append(anAssembly, aPart, TopLoc_Location());
  ASSERT_TRUE(anOccurrence.IsValid());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 anOccurrence,
                                 BRepGraph_NodeId::Kind::Product,
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(anAssembly));
  EXPECT_EQ(anExp.CurrentChild(), BRepGraph_NodeId(anOccurrence));
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Reference);

  const BRepGraph_OccurrenceRefId anOccurrenceRefId =
    aGraph.Refs().Occurrences().IdsOf(anAssembly).Value(0);
  EXPECT_EQ(anExp.CurrentRef(), BRepGraph_RefId(anOccurrenceRefId));
}

TEST(BRepGraph_ParentExplorerTest, OccurrenceParents_SetRefOccurrenceDefRejectsSharing)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aPart = aGraph.Editor().Products().Add(BRepGraph_SolidId::Start());
  aGraph.Editor().Products().AppendDocumentRoot(aPart);
  const BRepGraph_ProductId anAssembly1 = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(anAssembly1);
  const BRepGraph_ProductId anAssembly2 = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(anAssembly2);
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly1.IsValid());
  ASSERT_TRUE(anAssembly2.IsValid());

  BRepGraph_OccurrenceRefId    anOccRef1;
  BRepGraph_OccurrenceRefId    anOccRef2;
  const BRepGraph_OccurrenceId anOcc1 = aGraph.Editor().Products().Append(anAssembly1,
                                                                          aPart,
                                                                          TopLoc_Location(),
                                                                          BRepGraph_OccurrenceId(),
                                                                          &anOccRef1);
  const BRepGraph_OccurrenceId anOcc2 = aGraph.Editor().Products().Append(anAssembly2,
                                                                          aPart,
                                                                          TopLoc_Location(),
                                                                          BRepGraph_OccurrenceId(),
                                                                          &anOccRef2);
  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());
  ASSERT_TRUE(anOccRef1.IsValid());
  ASSERT_TRUE(anOccRef2.IsValid());

  aGraph.Editor().Occurrences().SetRefChildOccurrenceId(anOccRef2, anOcc1);
  EXPECT_EQ(aGraph.Refs().Occurrences().Entry(anOccRef2).ChildOccurrenceId, anOcc2);
  EXPECT_TRUE(aGraph.ValidateRelations());

  const BRepGraph_Validate::Result anAuditResult =
    BRepGraph_Validate::Perform(aGraph, BRepGraph_Validate::Options::Audit());
  EXPECT_TRUE(anAuditResult.IsValid());
}

TEST(BRepGraph_ParentExplorerTest, ProductParents_ImmediateOccurrence_IsStructural)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes13 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aPart = aGraph.Editor().Products().Add(BRepGraph_SolidId::Start());
  aGraph.Editor().Products().AppendDocumentRoot(aPart);
  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(anAssembly);
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  const BRepGraph_OccurrenceId anOccurrence =
    aGraph.Editor().Products().Append(anAssembly, aPart, TopLoc_Location());
  ASSERT_TRUE(anOccurrence.IsValid());

  BRepGraph_ParentExplorer anExp(aGraph,
                                 aPart,
                                 BRepGraph_ParentExplorer::TraversalMode::DirectParents);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(anOccurrence));
  EXPECT_EQ(anExp.CurrentChild(), BRepGraph_NodeId(aPart));
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Structural);
  EXPECT_FALSE(anExp.CurrentRef().IsValid());
}

TEST(BRepGraph_ParentExplorerTest, CoEdgeParents_ImmediateWireIsVisible)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes14 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefIds =
    aGraph.Refs().Wires().IdsOf(BRepGraph_FaceId::Start());
  ASSERT_GT(aWireRefIds.Size(), 0);
  const BRepGraph_WireId aWireId = aGraph.Refs().Wires().Entry(aWireRefIds.Value(0)).ChildWireId;

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
    aGraph.Topo().Wires().Relations(aWireId).CoEdgeIds;
  ASSERT_GT(aCoEdgeIds.Size(), 0);
  const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIds.Value(0);

  BRepGraph_ParentExplorer anExp(aGraph, aCoEdgeId);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(aWireId));
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Structural);
  EXPECT_FALSE(anExp.CurrentRef().IsValid());

  anExp.Next();
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_NodeId(BRepGraph_FaceId::Start()));
}

TEST(BRepGraph_ParentExplorerTest, CoEdgeChildOfCompound_ImmediateCompoundIsVisible)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_WireId aWireId =
    aGraph.Refs()
      .Wires()
      .Entry(aGraph.Refs().Wires().IdsOf(BRepGraph_FaceId::Start()).First())
      .ChildWireId;
  const BRepGraph_CoEdgeId aCoEdgeId = aGraph.Topo().Wires().Relations(aWireId).CoEdgeIds.First();

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(aCoEdgeId);
  const BRepGraph_CompoundId aCompound = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());

  bool hasWire     = false;
  bool hasCompound = false;
  for (BRepGraph_ParentExplorer anExp(aGraph,
                                      aCoEdgeId,
                                      BRepGraph_ParentExplorer::TraversalMode::DirectParents);
       anExp.More();
       anExp.Next())
  {
    if (anExp.Current().DefId == BRepGraph_NodeId(aWireId))
    {
      hasWire = true;
    }
    if (anExp.Current().DefId == BRepGraph_NodeId(aCompound))
    {
      hasCompound = true;
      EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ParentExplorer::LinkKind::Reference);
      EXPECT_TRUE(anExp.CurrentRef().IsValid());
    }
  }

  EXPECT_TRUE(hasWire);
  EXPECT_TRUE(hasCompound);
}

TEST(BRepGraph_ParentExplorerTest, ProductRoot_HasNoParents)
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes15 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ProductId aRootProduct = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aRootProduct);
  ASSERT_TRUE(aRootProduct.IsValid());

  BRepGraph_ParentExplorer anExp(aGraph, aRootProduct);
  EXPECT_FALSE(anExp.More());
}
