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
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_Transaction.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_LayerLock.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Replace.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_RevisionHash.hxx>
#include <BRepGraph_Revision.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Line.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_TriangulationParameters.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <thread>
#include <vector>

namespace
{

void expectFreshCoreHashes(const occ::handle<BRepGraph_Revision>& theRevision)
{
  ASSERT_FALSE(theRevision.IsNull());
  const BRepGraph_RevisionHash::Hasher::Result aFresh =
    BRepGraph_RevisionHash::Hasher::Compute(theRevision->Graph());
  EXPECT_EQ(theRevision->SemanticHash(), aFresh.Semantic);
  EXPECT_EQ(theRevision->StorageRootHash(), aFresh.Storage);
}

occ::handle<Geom_BSplineCurve> makeBSplineCurve(const double theMiddlePoleY)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(0.5, theMiddlePoleY, 0.0));
  aPoles.SetValue(3, gp_Pnt(1.0, 0.0, 0.0));
  NCollection_Array1<double> aKnots(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities.SetValue(1, 3);
  aMultiplicities.SetValue(2, 3);
  return new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
}

BRepGraph_RevisionHash edgeRepresentationHash(const occ::handle<Geom_Curve>&     theCurve,
                                              const occ::handle<Poly_Polygon3D>& thePolygon)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aStart = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEnd  = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aStart, anEnd, theCurve, 0.0, 1.0, 1.e-7);
  if (!anEdge.IsValid())
  {
    return BRepGraph_RevisionHash();
  }
  if (!thePolygon.IsNull())
  {
    aGraph.Editor().Edges().SetPersistentPolygon3D(anEdge, thePolygon);
  }
  return BRepGraph_RevisionHash::Hasher::Semantic(aGraph);
}

BRepGraph_RevisionHash triangulationRepresentationHash(const double theThirdNodeZ,
                                                       const bool   theHasNormals,
                                                       const double theNormalZ = 1.0)
{
  BRepGraph                                  aGraph;
  NCollection_LinearVector<BRepGraph_WireId> anInnerWires;
  const BRepGraph_FaceId aFace = aGraph.Editor().Faces().Add(occ::handle<Geom_Surface>(),
                                                             BRepGraph_WireId(),
                                                             anInnerWires.ToArray1(),
                                                             1.e-7);
  if (!aFace.IsValid())
  {
    return BRepGraph_RevisionHash();
  }

  occ::handle<Poly_Triangulation> aTriangulation =
    new Poly_Triangulation(3, 1, true, theHasNormals);
  aTriangulation->Deflection(0.125);
  aTriangulation->SetMeshPurpose(Poly_MeshPurpose_Calculation);
  aTriangulation->SetNode(1, gp_Pnt(0.0, 0.0, 0.0));
  aTriangulation->SetNode(2, gp_Pnt(1.0, 0.0, 0.0));
  aTriangulation->SetNode(3, gp_Pnt(0.0, 1.0, theThirdNodeZ));
  aTriangulation->SetUVNode(1, gp_Pnt2d(0.0, 0.0));
  aTriangulation->SetUVNode(2, gp_Pnt2d(1.0, 0.0));
  aTriangulation->SetUVNode(3, gp_Pnt2d(0.0, 1.0));
  if (theHasNormals)
  {
    aTriangulation->SetNormal(1, gp_Dir(0.0, 0.0, theNormalZ));
    aTriangulation->SetNormal(2, gp_Dir(0.0, 0.0, theNormalZ));
    aTriangulation->SetNormal(3, gp_Dir(0.0, 0.0, theNormalZ));
  }
  aTriangulation->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aTriangulation->Parameters(new Poly_TriangulationParameters(0.125, 0.5, 0.01));
  aGraph.Editor().Faces().SetPersistentTriangulation(aFace, aTriangulation);
  return BRepGraph_RevisionHash::Hasher::Semantic(aGraph);
}

class BRepGraph_ThrowingCopyLayer : public BRepGraph_Layer
{
public:
  DEFINE_STANDARD_RTTI_INLINE(BRepGraph_ThrowingCopyLayer, BRepGraph_Layer)

  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("96e5a1b9-928f-4ef8-a512-b77a45df7408");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("ThrowingCopyLayer");
    return THE_NAME;
  }

  BRepGraph_RevisionComponent::ComponentDescriptor RevisionDescriptor() const override
  {
    BRepGraph_RevisionComponent::ComponentDescriptor aDescriptor;
    aDescriptor.StableGUID    = GetID();
    aDescriptor.RetentionKind = BRepGraph_RevisionComponent::Retention::Transient;
    return aDescriptor;
  }

  void CopyTo(const BRepGraph_CopyRemap&) const override
  {
    throw Standard_DomainError("intentional layer-copy failure");
  }

  void InvalidateAll() noexcept override {}

  void Clear() noexcept override {}
};

} // namespace

TEST(BRepGraph_RevisionHashTest, IncrementalBytesMatchContiguousBytes)
{
  std::string aBytes(128 * 1024 + 17, '\0');
  for (size_t anIndex = 0; anIndex < aBytes.size(); ++anIndex)
  {
    aBytes[anIndex] = static_cast<char>((anIndex * 37u + 11u) & 0xffu);
  }

  BRepGraph_RevisionHash::Hasher::ByteAccumulator anAccumulator(aBytes.size());
  ASSERT_TRUE(anAccumulator.Append(aBytes.data(), 23));
  ASSERT_TRUE(anAccumulator.Append(aBytes.data() + 23, 64 * 1024));
  ASSERT_TRUE(anAccumulator.Append(aBytes.data() + 23 + 64 * 1024, aBytes.size() - 23 - 64 * 1024));

  const std::optional<BRepGraph_RevisionHash> anIncrementalHash = anAccumulator.Finish();
  ASSERT_TRUE(anIncrementalHash.has_value());
  EXPECT_EQ(*anIncrementalHash,
            BRepGraph_RevisionHash::Hasher::Bytes(aBytes.data(), aBytes.size()));

  BRepGraph_RevisionHash::Hasher::ByteAccumulator aShortAccumulator(8);
  ASSERT_TRUE(aShortAccumulator.Append(aBytes.data(), 7));
  EXPECT_FALSE(aShortAccumulator.Finish().has_value());
}

TEST(BRepGraph_RevisionHashTest, BytesUseCanonicalSHA256Encoding)
{
  const BRepGraph_RevisionHash aHash = BRepGraph_RevisionHash::Hasher::Bytes("abc", 3);
  EXPECT_STREQ(aHash.ToString().ToCString(),
               "3b079107702e383a89b56333530ba65c4df3ddb011647e5d3205404b07220d4c");
}

TEST(BRepGraph_RevisionHashTest, BSplinePoleChangesSemanticHash)
{
  const BRepGraph_RevisionHash aFirst  = edgeRepresentationHash(makeBSplineCurve(0.25), {});
  const BRepGraph_RevisionHash aSecond = edgeRepresentationHash(makeBSplineCurve(0.75), {});
  ASSERT_FALSE(aFirst.IsNull());
  ASSERT_FALSE(aSecond.IsNull());
  EXPECT_NE(aFirst, aSecond);
}

TEST(BRepGraph_RevisionHashTest, PolygonNodeChangesSemanticHash)
{
  NCollection_Array1<gp_Pnt> aFirstNodes(1, 2);
  aFirstNodes.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aFirstNodes.SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  NCollection_Array1<gp_Pnt> aSecondNodes = aFirstNodes;
  aSecondNodes.SetValue(2, gp_Pnt(1.0, 0.5, 0.0));
  occ::handle<Poly_Polygon3D> aFirstPolygon  = new Poly_Polygon3D(aFirstNodes);
  occ::handle<Poly_Polygon3D> aSecondPolygon = new Poly_Polygon3D(aSecondNodes);
  aFirstPolygon->Deflection(0.01);
  aSecondPolygon->Deflection(0.01);

  const occ::handle<Geom_Curve> aCurve  = makeBSplineCurve(0.25);
  const BRepGraph_RevisionHash  aFirst  = edgeRepresentationHash(aCurve, aFirstPolygon);
  const BRepGraph_RevisionHash  aSecond = edgeRepresentationHash(aCurve, aSecondPolygon);
  ASSERT_FALSE(aFirst.IsNull());
  ASSERT_FALSE(aSecond.IsNull());
  EXPECT_NE(aFirst, aSecond);
}

TEST(BRepGraph_RevisionHashTest, TriangulationNodeChangesSemanticHash)
{
  const BRepGraph_RevisionHash aFirst  = triangulationRepresentationHash(0.0, true);
  const BRepGraph_RevisionHash aSecond = triangulationRepresentationHash(0.25, true);
  ASSERT_FALSE(aFirst.IsNull());
  ASSERT_FALSE(aSecond.IsNull());
  EXPECT_NE(aFirst, aSecond);
}

TEST(BRepGraph_RevisionHashTest, TriangulationNormalsChangeSemanticHash)
{
  const BRepGraph_RevisionHash aWithoutNormals  = triangulationRepresentationHash(0.0, false);
  const BRepGraph_RevisionHash aPositiveNormals = triangulationRepresentationHash(0.0, true, 1.0);
  const BRepGraph_RevisionHash aNegativeNormals = triangulationRepresentationHash(0.0, true, -1.0);
  ASSERT_FALSE(aWithoutNormals.IsNull());
  EXPECT_NE(aWithoutNormals, aPositiveNormals);
  EXPECT_NE(aPositiveNormals, aNegativeNormals);
}

TEST(BRepGraph_RevisionHashTest, OwnershipChangesOnlyPhysicalHashes)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aStart = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEnd  = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const occ::handle<Geom_Curve> aCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraph_EdgeId anEdge =
    aGraph.Editor().Edges().Add(aStart, anEnd, aCurve, 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());
  const BRepGraph_RefId aRef = aGraph.Topo().Edges().Definition(anEdge).StartVertexRefId;

  const BRepGraph_RevisionHash aNodeSemantic = BRepGraph_RevisionHash::Hasher::Node(aGraph, aStart);
  const BRepGraph_RevisionHash aRefSemantic =
    BRepGraph_RevisionHash::Hasher::Reference(aGraph, aRef);
  const BRepGraph_RevisionHash     aStorage = BRepGraph_RevisionHash::Hasher::Storage(aGraph);
  occ::handle<BRepGraph_LayerLock> aLock    = aGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>();
  ASSERT_FALSE(aLock.IsNull());
  const Standard_GUID anOwner("7ad91d25-533c-4f20-b93b-dad463f52ed9");

  aLock->SetOwner(BRepGraph_NodeId(aStart), anOwner);
  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Node(aGraph, aStart), aNodeSemantic);
  EXPECT_NE(BRepGraph_RevisionHash::Hasher::Storage(aGraph), aStorage);
  aLock->UnsetOwner(BRepGraph_NodeId(aStart));
  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Storage(aGraph), aStorage);

  aLock->SetOwner(aRef, anOwner);
  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Reference(aGraph, aRef), aRefSemantic);
  EXPECT_NE(BRepGraph_RevisionHash::Hasher::Storage(aGraph), aStorage);
}

TEST(BRepGraph_RevisionHashTest, TombstoneChangesOnlyPhysicalHash)
{
  BRepGraph                aFirstGraph;
  const BRepGraph_VertexId aFirstVertex =
    aFirstGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 1.e-7);
  ASSERT_TRUE(aFirstVertex.IsValid());
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::FromGraph(aFirstGraph);
  ASSERT_FALSE(aRevision.IsNull());
  BRepGraph aSecondGraph;
  ASSERT_TRUE(aRevision->CopyTo(aSecondGraph));
  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aChange =
      aSecondGraph.Editor().Vertices().Mut(aFirstVertex);
    aSecondGraph.Editor().Vertices().SetPoint(aChange, gp_Pnt(4.0, 5.0, 6.0));
  }
  aFirstGraph.Editor().Gen().RemoveNode(aFirstVertex);
  aSecondGraph.Editor().Gen().RemoveNode(aFirstVertex);

  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Semantic(aFirstGraph),
            BRepGraph_RevisionHash::Hasher::Semantic(aSecondGraph));
  EXPECT_NE(BRepGraph_RevisionHash::Hasher::Storage(aFirstGraph),
            BRepGraph_RevisionHash::Hasher::Storage(aSecondGraph));
}

TEST(BRepGraph_RevisionTest, EmptyRevisionProvidesDirectReads)
{
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aRevision.IsNull());
  EXPECT_TRUE(aRevision->IsEmpty());
  EXPECT_TRUE(aRevision->SupportsSparseEdits());
  EXPECT_FALSE(aRevision->HasContentHashes());
  EXPECT_EQ(aRevision->SemanticHash(),
            BRepGraph_RevisionHash::Hasher::Semantic(aRevision->Graph()));
  EXPECT_TRUE(aRevision->HasContentHashes());

  EXPECT_TRUE(aRevision->Graph().IsEmpty());
  EXPECT_EQ(aRevision->NbVisibleVertices(), 0u);
  const BRepGraph_RevisionDiff aSelfDiff = aRevision->Diff(*aRevision);
  EXPECT_TRUE(aSelfDiff.CreatedUIDs.IsEmpty());
  EXPECT_TRUE(aSelfDiff.ModifiedUIDs.IsEmpty());
  EXPECT_TRUE(aSelfDiff.RemovedUIDs.IsEmpty());
}

TEST(BRepGraph_RevisionTest, VisibleIteratorsYieldResolvedRecordsWithoutUIDCollections)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape()).IsOk());
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aRevision.IsNull());
  EXPECT_FALSE(aRevision->HasContentHashes());

  const auto countVisible = [](auto theIterator) {
    uint32_t aCount = 0;
    for (const auto& aChange : theIterator)
    {
      EXPECT_TRUE(aChange.UID.IsValid());
      EXPECT_NE(aChange.Kind, BRepGraph_Revision::VertexChange::Operation::Remove);
      ++aCount;
    }
    return aCount;
  };

  EXPECT_EQ(countVisible(aRevision->VisibleVertices()), aRevision->NbVisibleVertices());
  EXPECT_EQ(countVisible(aRevision->VisibleEdges()), aRevision->NbVisibleEdges());
  EXPECT_EQ(countVisible(aRevision->VisibleVertexRefs()), aRevision->NbVisibleVertexRefs());
  EXPECT_EQ(countVisible(aRevision->VisibleCoEdges()), aRevision->NbVisibleCoEdges());
  EXPECT_EQ(countVisible(aRevision->VisibleWires()), aRevision->NbVisibleWires());
  EXPECT_EQ(countVisible(aRevision->VisibleFaces()), aRevision->NbVisibleFaces());
  EXPECT_EQ(countVisible(aRevision->VisibleWireRefs()), aRevision->NbVisibleWireRefs());
  EXPECT_FALSE(aRevision->HasContentHashes());

  BRepGraph_Revision::VertexIterator anIterator = aRevision->VisibleVertices();
  ASSERT_TRUE(anIterator.More());
  EXPECT_TRUE(anIterator.Current().LocalId.IsValid());
  anIterator.Next();
}

TEST(BRepGraph_RevisionTest, ImmutableRevisionSupportsIndependentConcurrentIterators)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape()).IsOk());
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aRevision.IsNull());

  std::atomic<bool>        isValid{true};
  std::vector<std::thread> aThreads;
  for (int aThreadIndex = 0; aThreadIndex < 4; ++aThreadIndex)
  {
    aThreads.emplace_back([&]() {
      for (int anIteration = 0; anIteration < 32; ++anIteration)
      {
        uint32_t anEdgeCount = 0;
        for (const BRepGraph_Revision::EdgeView& anEdge : aRevision->VisibleEdges())
        {
          if (!anEdge.UID.IsValid() || anEdge.Curve == nullptr)
          {
            isValid = false;
          }
          ++anEdgeCount;
        }
        if (anEdgeCount != aRevision->NbVisibleEdges()
            || aRevision->Graph().Topo().Faces().NbActive() != aRevision->NbVisibleFaces())
        {
          isValid = false;
        }
      }
    });
  }
  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }
  EXPECT_TRUE(isValid);
}

TEST(BRepGraph_RevisionTest, RevisionIsIsolatedFromSourceGraphMutation)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 1.e-7).IsValid());

  const BRepGraph_Revision::CreateResult aCreate = BRepGraph_Revision::Create(aGraph);
  ASSERT_TRUE(aCreate.IsOk());
  EXPECT_EQ(aCreate.Revision->SemanticHash(),
            BRepGraph_RevisionHash::Hasher::Semantic(aCreate.Revision->Graph()));
  ASSERT_EQ(aCreate.Revision->Graph().Topo().Vertices().NbActive(), 1u);

  ASSERT_TRUE(aGraph.Editor().Vertices().Add(gp_Pnt(4.0, 5.0, 6.0), 1.e-7).IsValid());
  EXPECT_EQ(aGraph.Topo().Vertices().NbActive(), 2u);
  EXPECT_EQ(aCreate.Revision->Graph().Topo().Vertices().NbActive(), 1u);
}

TEST(BRepGraph_RevisionTest, CopyToCreatesMutablePageSharedGraph)
{
  BRepGraph                           aSource;
  const BRepGraph::ShapesView::Result aBuilt =
    aSource.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  ASSERT_TRUE(aBuilt.IsOk());
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::FromGraph(aSource);
  ASSERT_FALSE(aRevision.IsNull());

  BRepGraph aWorkingGraph;
  ASSERT_TRUE(aRevision->CopyTo(aWorkingGraph));
  EXPECT_EQ(aWorkingGraph.Topo().Faces().Nb(), aSource.Topo().Faces().Nb());
  const TopoDS_Shape aSourceOriginal  = aSource.Shapes().Original(aBuilt.TopologyRoot);
  const TopoDS_Shape aRootOriginal    = aRevision->Graph().Shapes().Original(aBuilt.TopologyRoot);
  const TopoDS_Shape aWorkingOriginal = aWorkingGraph.Shapes().Original(aBuilt.TopologyRoot);
  ASSERT_FALSE(aSourceOriginal.IsNull());
  ASSERT_FALSE(aRootOriginal.IsNull());
  ASSERT_FALSE(aWorkingOriginal.IsNull());
  EXPECT_FALSE(aRootOriginal.IsSame(aSourceOriginal));
  EXPECT_FALSE(aWorkingOriginal.IsSame(aRootOriginal));
  aWorkingGraph.Editor().Gen().RemoveSubgraph(aBuilt.Product);

  EXPECT_EQ(aWorkingGraph.Topo().Products().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().Occurrences().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().Compounds().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().CompSolids().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().Solids().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().Shells().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().Faces().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().Wires().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().CoEdges().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().Edges().NbActive(), 0u);
  EXPECT_EQ(aWorkingGraph.Topo().Vertices().NbActive(), 0u);
  EXPECT_FALSE(aRevision->IsEmpty());
  EXPECT_EQ(aRevision->Graph().Topo().Faces().Nb(), aSource.Topo().Faces().Nb());
}

TEST(BRepGraph_RevisionTest, CopiedMutableRepresentationDoesNotModifyRevisionOrSource)
{
  BRepGraph                aSource;
  const BRepGraph_VertexId aStart = aSource.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEnd  = aSource.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const occ::handle<Geom_BSplineCurve> aSourceCurve = makeBSplineCurve(0.25);
  const BRepGraph_EdgeId               anEdge =
    aSource.Editor().Edges().Add(aStart, anEnd, aSourceCurve, 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::FromGraph(aSource);
  ASSERT_FALSE(aRevision.IsNull());
  aSourceCurve->SetPole(2, gp_Pnt(0.5, 2.0, 0.0));

  BRepGraph aWorkingGraph;
  ASSERT_TRUE(aRevision->CopyTo(aWorkingGraph));
  const occ::handle<Geom_BSplineCurve> aWorkingCurve =
    occ::down_cast<Geom_BSplineCurve>(aWorkingGraph.Topo().Edges().Curve3D(anEdge));
  ASSERT_FALSE(aWorkingCurve.IsNull());
  EXPECT_DOUBLE_EQ(aWorkingCurve->Pole(2).Y(), 0.25);
  aWorkingCurve->SetPole(2, gp_Pnt(0.5, 3.0, 0.0));

  BRepGraph aFreshGraph;
  ASSERT_TRUE(aRevision->CopyTo(aFreshGraph));
  const occ::handle<Geom_BSplineCurve> aFreshCurve =
    occ::down_cast<Geom_BSplineCurve>(aFreshGraph.Topo().Edges().Curve3D(anEdge));
  ASSERT_FALSE(aFreshCurve.IsNull());
  EXPECT_DOUBLE_EQ(aFreshCurve->Pole(2).Y(), 0.25);
  EXPECT_DOUBLE_EQ(aSourceCurve->Pole(2).Y(), 2.0);
}

TEST(BRepGraph_RevisionTest, BranchRepresentationMutationDoesNotModifyImmutableRevisions)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aStart = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEnd  = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aStart, anEnd, makeBSplineCurve(0.25), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());
  const BRepGraph_UID anEdgeUID = aBase->Graph().UIDs().Of(anEdge);

  BRepGraph_Transaction anEdit = aBase->BeginTransaction();
  ASSERT_TRUE(anEdit.SetEdgeTolerance(anEdgeUID, 2.e-7));
  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());

  BRepGraph aBranchGraph;
  ASSERT_TRUE(aCommit.Revision->CopyTo(aBranchGraph));
  const occ::handle<Geom_BSplineCurve> aBranchCurve =
    occ::down_cast<Geom_BSplineCurve>(aBranchGraph.Topo().Edges().Curve3D(anEdge));
  ASSERT_FALSE(aBranchCurve.IsNull());
  aBranchCurve->SetPole(2, gp_Pnt(0.5, 4.0, 0.0));

  BRepGraph aFreshBase;
  BRepGraph aFreshBranch;
  ASSERT_TRUE(aBase->CopyTo(aFreshBase));
  ASSERT_TRUE(aCommit.Revision->CopyTo(aFreshBranch));
  const occ::handle<Geom_BSplineCurve> aBaseCurve =
    occ::down_cast<Geom_BSplineCurve>(aFreshBase.Topo().Edges().Curve3D(anEdge));
  const occ::handle<Geom_BSplineCurve> aFreshBranchCurve =
    occ::down_cast<Geom_BSplineCurve>(aFreshBranch.Topo().Edges().Curve3D(anEdge));
  ASSERT_FALSE(aBaseCurve.IsNull());
  ASSERT_FALSE(aFreshBranchCurve.IsNull());
  EXPECT_DOUBLE_EQ(aBaseCurve->Pole(2).Y(), 0.25);
  EXPECT_DOUBLE_EQ(aFreshBranchCurve->Pole(2).Y(), 0.25);
  EXPECT_DOUBLE_EQ(aBase->Graph().Topo().Edges().Definition(anEdge).Tolerance, 1.e-7);
  EXPECT_DOUBLE_EQ(aCommit.Revision->Graph().Topo().Edges().Definition(anEdge).Tolerance, 2.e-7);
}

TEST(BRepGraph_RevisionTest, CommitCreatesPageSharedRevision)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aBaseVertex =
    aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aBaseVertex.IsValid());
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());

  BRepGraph_Transaction anEdit = aBase->BeginTransaction();
  ASSERT_TRUE(anEdit.IsValid()) << anEdit.Diagnostics().Size();
  const BRepGraph_VertexId aCreated = anEdit.AddVertex(gp_Pnt(10.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aCreated.IsValid());

  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  ASSERT_FALSE(aCommit.Revision.IsNull());
  EXPECT_NE(aCommit.Revision->StorageRootHash(), aBase->StorageRootHash());
  expectFreshCoreHashes(aCommit.Revision);
  EXPECT_EQ(aBase->Graph().Topo().Vertices().NbActive(), 1u);
  EXPECT_EQ(aCommit.Revision->Graph().Topo().Vertices().NbActive(), 2u);
  const BRepGraph_UID                   aCreatedUID = aCommit.Revision->Graph().UIDs().Of(aCreated);
  const occ::handle<BRepGraph_Revision> aRead       = aCommit.Revision;
  BRepGraph_Revision::VertexChange      aCreatedChange;
  ASSERT_TRUE(aRead->ReadVertex(aCreatedUID, aCreatedChange));
  EXPECT_DOUBLE_EQ(aCreatedChange.Definition.Point.X(), 10.0);
  EXPECT_EQ(aRead->NbVisibleVertices(), 2u);
  EXPECT_FALSE(aCommit.AllocatedUIDRanges.IsEmpty());
}

TEST(BRepGraph_RevisionTest, BulkEdgeCreationKeepsSequentialLocalIdsAndFreshHashes)
{
  BRepGraph_Transaction aVertexEdit = BRepGraph_Revision::Empty()->BeginTransaction();
  ASSERT_TRUE(aVertexEdit.IsValid());
  const BRepGraph_VertexId aStart = aVertexEdit.AddVertex(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEnd  = aVertexEdit.AddVertex(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aStart.IsValid() && anEnd.IsValid());
  const BRepGraph_Transaction::CommitResult aVertexCommit = aVertexEdit.Commit();
  ASSERT_TRUE(aVertexCommit.IsOk());

  const BRepGraph_UID           aStartUID = aVertexCommit.Revision->Graph().UIDs().Of(aStart);
  const BRepGraph_UID           anEndUID  = aVertexCommit.Revision->Graph().UIDs().Of(anEnd);
  const occ::handle<Geom_Curve> aCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  BRepGraph_Transaction anEdgeEdit     = aVertexCommit.Revision->BeginTransaction();
  constexpr uint32_t    THE_EDGE_COUNT = 128;
  for (uint32_t anIndex = 0; anIndex < THE_EDGE_COUNT; ++anIndex)
  {
    EXPECT_EQ(anEdgeEdit.AddEdge(aStartUID, anEndUID, aCurve, 0.0, 1.0, 1.e-7),
              BRepGraph_EdgeId(anIndex));
  }

  const BRepGraph_Transaction::CommitResult anEdgeCommit = anEdgeEdit.Commit();
  ASSERT_TRUE(anEdgeCommit.IsOk());
  EXPECT_EQ(anEdgeCommit.Revision->NbVisibleEdges(), THE_EDGE_COUNT);
  EXPECT_EQ(anEdgeCommit.Revision->NbVisibleVertexRefs(), 2u * THE_EDGE_COUNT);
  expectFreshCoreHashes(anEdgeCommit.Revision);
}

TEST(BRepGraph_RevisionTest, CommitReplacesVertexRecordByDurableUID)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());
  const BRepGraph_UID aUID = aBase->Graph().UIDs().Of(aVertex);

  BRepGraph_Transaction anEdit = aBase->BeginTransaction();
  ASSERT_TRUE(anEdit.IsValid());
  ASSERT_TRUE(anEdit.SetVertexPoint(aUID, gp_Pnt(1.0, 2.0, 3.0)));
  ASSERT_TRUE(anEdit.SetVertexTolerance(aUID, 2.e-6));

  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  expectFreshCoreHashes(aCommit.Revision);
  const occ::handle<BRepGraph_Revision> aRead = aCommit.Revision;
  BRepGraph_Revision::VertexChange      aReadChange;
  ASSERT_TRUE(aRead->ReadVertex(aUID, aReadChange));
  EXPECT_DOUBLE_EQ(aReadChange.Definition.Point.X(), 1.0);
  EXPECT_DOUBLE_EQ(aReadChange.Definition.Tolerance, 2.e-6);
  EXPECT_NE(aCommit.Revision->SemanticHash(), aBase->SemanticHash());
  EXPECT_EQ(aCommit.Revision->SemanticHash(),
            BRepGraph_RevisionHash::Hasher::Semantic(aCommit.Revision->Graph()));
  const gp_Pnt& aChangedPoint =
    aCommit.Revision->Graph().Topo().Vertices().Definition(aVertex).Point;
  EXPECT_DOUBLE_EQ(aChangedPoint.X(), 1.0);
  EXPECT_DOUBLE_EQ(aChangedPoint.Y(), 2.0);
  EXPECT_DOUBLE_EQ(aChangedPoint.Z(), 3.0);
  EXPECT_DOUBLE_EQ(aCommit.Revision->Graph().Topo().Vertices().Definition(aVertex).Tolerance,
                   2.e-6);
  const gp_Pnt& aBasePoint = aBase->Graph().Topo().Vertices().Definition(aVertex).Point;
  EXPECT_DOUBLE_EQ(aBasePoint.X(), 0.0);
  EXPECT_DOUBLE_EQ(aBasePoint.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aBasePoint.Z(), 0.0);
  EXPECT_FALSE(aCommit.Diff.ModifiedUIDs.IsEmpty());
  EXPECT_EQ(aCommit.Diff.ModifiedUIDs.First(), aUID);
}

TEST(BRepGraph_RevisionTest, ImmutableRevisionReadDoesNotShareEdgeCurve)
{
  BRepGraph_Transaction aVertexEdit = BRepGraph_Revision::Empty()->BeginTransaction();
  ASSERT_TRUE(aVertexEdit.IsValid());
  const BRepGraph_VertexId aStartVertex = aVertexEdit.AddVertex(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEndVertex  = aVertexEdit.AddVertex(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aStartVertex.IsValid() && anEndVertex.IsValid());
  const BRepGraph_Transaction::CommitResult aVertexCommit = aVertexEdit.Commit();
  ASSERT_TRUE(aVertexCommit.IsOk());

  const BRepGraph_UID   aStartUID  = aVertexCommit.Revision->Graph().UIDs().Of(aStartVertex);
  const BRepGraph_UID   anEndUID   = aVertexCommit.Revision->Graph().UIDs().Of(anEndVertex);
  BRepGraph_Transaction anEdgeEdit = aVertexCommit.Revision->BeginTransaction();
  ASSERT_TRUE(anEdgeEdit.IsValid());
  const occ::handle<Geom_Curve> aCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraph_EdgeId anEdge = anEdgeEdit.AddEdge(aStartUID, anEndUID, aCurve, 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());
  const BRepGraph_Transaction::CommitResult anEdgeCommit = anEdgeEdit.Commit();
  ASSERT_TRUE(anEdgeCommit.IsOk());
  const BRepGraph_UID anEdgeUID = anEdgeCommit.Revision->Graph().UIDs().Of(anEdge);

  BRepGraph_Revision::EdgeChange aReadChange;
  ASSERT_TRUE(anEdgeCommit.Revision->ReadEdge(anEdgeUID, aReadChange));
  ASSERT_FALSE(aReadChange.Curve.IsNull());
  aReadChange.Curve->Reverse();

  BRepGraph_Revision::EdgeChange aFreshChange;
  ASSERT_TRUE(anEdgeCommit.Revision->ReadEdge(anEdgeUID, aFreshChange));
  ASSERT_FALSE(aFreshChange.Curve.IsNull());
  EXPECT_DOUBLE_EQ(aFreshChange.Curve->Value(1.0).X(), 1.0);

  BRepGraph_Revision::EdgeIterator anIterator = anEdgeCommit.Revision->VisibleEdges();
  ASSERT_TRUE(anIterator.More());
  ASSERT_NE(anIterator.Current().Curve, nullptr);
  EXPECT_DOUBLE_EQ(anIterator.Current().Curve->Value(1.0).X(), 1.0);
  ASSERT_TRUE(anEdgeCommit.Revision->ReadEdge(anEdgeUID, aFreshChange));
  EXPECT_DOUBLE_EQ(aFreshChange.Curve->Value(1.0).X(), 1.0);

  const BRepGraph&               aGraphCopy   = anEdgeCommit.Revision->Graph();
  const occ::handle<Geom_Curve>& aCopiedCurve = aGraphCopy.Topo().Edges().Curve3D(anEdge);
  ASSERT_FALSE(aCopiedCurve.IsNull());
  aCopiedCurve->Reverse();
  ASSERT_TRUE(anEdgeCommit.Revision->ReadEdge(anEdgeUID, aFreshChange));
  EXPECT_DOUBLE_EQ(aFreshChange.Curve->Value(1.0).X(), 1.0);
}

TEST(BRepGraph_RevisionTest, AbortDoesNotChangeBaseRevision)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());
  const BRepGraph_RevisionHash aHash = aBase->SemanticHash();

  BRepGraph_Transaction anEdit = aBase->BeginTransaction();
  ASSERT_TRUE(anEdit.IsValid());
  const BRepGraph_UID aUID = aBase->Graph().UIDs().Of(aVertex);
  ASSERT_TRUE(anEdit.SetVertexPoint(aUID, gp_Pnt(2.0, 0.0, 0.0)));
  anEdit.Abort();

  EXPECT_TRUE(anEdit.IsFinished());
  EXPECT_EQ(aBase->SemanticHash(), aHash);
  const gp_Pnt& aBasePoint = aBase->Graph().Topo().Vertices().Definition(aVertex).Point;
  EXPECT_DOUBLE_EQ(aBasePoint.X(), 0.0);
  EXPECT_DOUBLE_EQ(aBasePoint.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aBasePoint.Z(), 0.0);
}

TEST(BRepGraph_RevisionTest, RemovalCreatesTombstoneAndRetainsBaseRecord)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(4.0, 5.0, 6.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());
  const BRepGraph_UID aUID = aBase->Graph().UIDs().Of(aVertex);

  BRepGraph_Transaction anEdit = aBase->BeginTransaction();
  ASSERT_TRUE(anEdit.IsValid());
  ASSERT_TRUE(anEdit.RemoveVertex(aUID));
  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());

  BRepGraph_Revision::VertexChange aVertexChange;
  EXPECT_TRUE(aBase->ReadVertex(aUID, aVertexChange));
  EXPECT_FALSE(aCommit.Revision->ReadVertex(aUID, aVertexChange));
  EXPECT_EQ(aCommit.Revision->NbVisibleVertices(), 0u);
  EXPECT_FALSE(aCommit.Revision->VisibleVertices().More());
  EXPECT_FALSE(aCommit.Diff.RemovedUIDs.IsEmpty());
  EXPECT_EQ(aCommit.Diff.RemovedUIDs.First(), aUID);
}

TEST(BRepGraph_RevisionTest, CompleteGraphTransactionUsesExistingEditorAPI)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape()).IsOk());
  const occ::handle<BRepGraph_Revision> aBaseRevision = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBaseRevision.IsNull());
  ASSERT_FALSE(aBaseRevision->SupportsSparseEdits());
  const uint32_t aBaseVertexCount = aBaseRevision->Graph().Topo().Vertices().NbActive();

  BRepGraph_Transaction aTransaction = aBaseRevision->BeginTransaction();
  ASSERT_TRUE(aTransaction.IsValid());
  BRepGraph* aWorkingGraph = aTransaction.Graph();
  ASSERT_NE(aWorkingGraph, nullptr);
  const BRepGraph_VertexId aVertex =
    aWorkingGraph->Editor().Vertices().Add(gp_Pnt(5.0, 6.0, 7.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  const BRepGraph_UID aVertexUID = aWorkingGraph->UIDs().Of(aVertex);
  ASSERT_TRUE(aVertexUID.IsValid());

  const BRepGraph_Transaction::CommitResult aCommit = aTransaction.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  ASSERT_FALSE(aCommit.Revision.IsNull());
  EXPECT_NE(aCommit.Revision, aBaseRevision);
  EXPECT_EQ(aBaseRevision->Graph().Topo().Vertices().NbActive(), aBaseVertexCount);
  EXPECT_EQ(aCommit.Revision->Graph().Topo().Vertices().NbActive(), aBaseVertexCount + 1);
  EXPECT_EQ(aCommit.Diff.CreatedUIDs.Size(), 1u);
  EXPECT_EQ(aCommit.Diff.CreatedUIDs.First(), aVertexUID);
}

TEST(BRepGraph_RevisionTest, CompleteGraphCommitDetachesExposedRepresentationHandles)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aStart = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEnd  = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_EdgeId   anEdge =
    aGraph.Editor().Edges().Add(aStart, anEnd, makeBSplineCurve(1.0), 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());

  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());
  BRepGraph_Transaction anEdit        = aBase->BeginTransaction();
  BRepGraph*            aWorkingGraph = anEdit.Graph();
  ASSERT_NE(aWorkingGraph, nullptr);
  aWorkingGraph->Editor().Vertices().SetPoint(aStart, gp_Pnt(-1.0, 0.0, 0.0));
  occ::handle<Geom_BSplineCurve> anExposedCurve =
    occ::down_cast<Geom_BSplineCurve>(aWorkingGraph->Topo().Edges().Curve3D(anEdge));
  ASSERT_FALSE(anExposedCurve.IsNull());

  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  ASSERT_NE(aCommit.Revision, aBase);
  const BRepGraph_RevisionHash aHashBefore = aCommit.Revision->SemanticHash();

  anExposedCurve->SetPole(2, gp_Pnt(0.5, 42.0, 0.0));
  const occ::handle<Geom_BSplineCurve> aCommittedCurve =
    occ::down_cast<Geom_BSplineCurve>(aCommit.Revision->Graph().Topo().Edges().Curve3D(anEdge));
  ASSERT_FALSE(aCommittedCurve.IsNull());
  EXPECT_DOUBLE_EQ(aCommittedCurve->Pole(2).Y(), 1.0);
  EXPECT_EQ(aCommit.Revision->SemanticHash(), aHashBefore);
}

TEST(BRepGraph_RevisionTest, CompleteGraphWatermarkOnlyCommitIsNotDiscarded)
{
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aBase.IsNull());
  BRepGraph_Transaction anEdit        = aBase->BeginTransaction();
  BRepGraph*            aWorkingGraph = anEdit.Graph();
  ASSERT_NE(aWorkingGraph, nullptr);
  ASSERT_TRUE(aWorkingGraph->RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::Vertex, 100u));

  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  EXPECT_TRUE(aCommit.Diff.IsEmpty());
  EXPECT_NE(aCommit.Revision, aBase);
  EXPECT_EQ(aCommit.Revision->SemanticHash(), aBase->SemanticHash());
  EXPECT_NE(aCommit.Revision->StorageRootHash(), aBase->StorageRootHash());
  EXPECT_EQ(aCommit.Revision->Graph().NextNodeUIDCounter(BRepGraph_NodeId::Kind::Vertex), 100u);
}

TEST(BRepGraph_RevisionTest, CompleteGraphTransactionDiffReportsModifyNoOpAndRemove)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());
  const BRepGraph_UID aUID = aBase->Graph().UIDs().Of(aVertex);

  BRepGraph_Transaction anEdit        = aBase->BeginTransaction();
  BRepGraph*            aWorkingGraph = anEdit.Graph();
  ASSERT_NE(aWorkingGraph, nullptr);
  aWorkingGraph->Editor().Vertices().SetPoint(aVertex, gp_Pnt(4.0, 5.0, 6.0));
  const BRepGraph_Transaction::CommitResult aModified = anEdit.Commit();
  ASSERT_TRUE(aModified.IsOk());
  ASSERT_EQ(aModified.Diff.ModifiedUIDs.Size(), 1u);
  EXPECT_EQ(aModified.Diff.ModifiedUIDs.First(), aUID);

  BRepGraph_Transaction aNoOpEdit  = aModified.Revision->BeginTransaction();
  BRepGraph*            aNoOpGraph = aNoOpEdit.Graph();
  ASSERT_NE(aNoOpGraph, nullptr);
  aNoOpGraph->Editor().Vertices().SetPoint(aVertex, gp_Pnt(4.0, 5.0, 6.0));
  const BRepGraph_Transaction::CommitResult aNoOp = aNoOpEdit.Commit();
  ASSERT_TRUE(aNoOp.IsOk());
  EXPECT_TRUE(aNoOp.Diff.IsEmpty());
  EXPECT_EQ(aNoOp.Revision, aModified.Revision);

  BRepGraph_Transaction aRemoveEdit  = aModified.Revision->BeginTransaction();
  BRepGraph*            aRemoveGraph = aRemoveEdit.Graph();
  ASSERT_NE(aRemoveGraph, nullptr);
  aRemoveGraph->Editor().Gen().RemoveNode(aVertex);
  const BRepGraph_Transaction::CommitResult aRemoved = aRemoveEdit.Commit();
  ASSERT_TRUE(aRemoved.IsOk());
  ASSERT_EQ(aRemoved.Diff.RemovedUIDs.Size(), 1u);
  EXPECT_EQ(aRemoved.Diff.RemovedUIDs.First(), aUID);
}

TEST(BRepGraph_RevisionTest, UIDWatermarksOnlyRaiseAndReportReservedRanges)
{
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aBase.IsNull());

  BRepGraph_Transaction anEdit = aBase->BeginTransaction();
  ASSERT_TRUE(anEdit.IsValid());
  EXPECT_TRUE(anEdit.RaiseVertexUIDWatermark(100u));
  EXPECT_FALSE(anEdit.RaiseVertexUIDWatermark(50u));

  const BRepGraph_VertexId aVertex = anEdit.AddVertex(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());

  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  ASSERT_EQ(aCommit.AllocatedUIDRanges.Size(), 2);
  EXPECT_EQ(aCommit.AllocatedUIDRanges.First().First, 1u);
  EXPECT_EQ(aCommit.AllocatedUIDRanges.First().Last, 99u);
  EXPECT_EQ(aCommit.AllocatedUIDRanges.Last().First, 100u);
  EXPECT_EQ(aCommit.AllocatedUIDRanges.Last().Last, 100u);
  EXPECT_EQ(aCommit.Revision->Graph().UIDs().Of(aVertex).Counter, 100u);
}

TEST(BRepGraph_RevisionTest, WatermarkOnlyCommitRetainsCounterAdvance)
{
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aBase.IsNull());

  BRepGraph_Transaction anEdit = aBase->BeginTransaction();
  ASSERT_TRUE(anEdit.IsValid());
  ASSERT_TRUE(anEdit.RaiseVertexUIDWatermark(100u));
  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  ASSERT_FALSE(aCommit.Revision.IsNull());
  EXPECT_NE(aCommit.Revision, aBase);
  EXPECT_EQ(aCommit.Revision->SemanticHash(), aBase->SemanticHash());
  EXPECT_NE(aCommit.Revision->StorageRootHash(), aBase->StorageRootHash());
  ASSERT_EQ(aCommit.AllocatedUIDRanges.Size(), 1u);
  EXPECT_EQ(aCommit.AllocatedUIDRanges.First().First, 1u);
  EXPECT_EQ(aCommit.AllocatedUIDRanges.First().Last, 99u);

  BRepGraph_Transaction aFollowup = aCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aFollowup.IsValid());
  const BRepGraph_VertexId aVertex = aFollowup.AddVertex(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  const BRepGraph_Transaction::CommitResult aFollowupCommit = aFollowup.Commit();
  ASSERT_TRUE(aFollowupCommit.IsOk());
  EXPECT_EQ(aFollowupCommit.Revision->Graph().UIDs().Of(aVertex).Counter, 100u);
}

TEST(BRepGraph_RevisionTest, CommitRemovalSupersedesPriorModification)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());
  const BRepGraph_UID aUID = aBase->Graph().UIDs().Of(aVertex);

  BRepGraph_Transaction anEdit = aBase->BeginTransaction();
  ASSERT_TRUE(anEdit.SetVertexPoint(aUID, gp_Pnt(1.0, 2.0, 3.0)));
  ASSERT_TRUE(anEdit.RemoveVertex(aUID));
  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  EXPECT_TRUE(aCommit.Diff.ModifiedUIDs.IsEmpty());
  ASSERT_EQ(aCommit.Diff.RemovedUIDs.Size(), 1u);
  EXPECT_EQ(aCommit.Diff.RemovedUIDs.First(), aUID);
}

TEST(BRepGraph_RevisionTest, SparseCommitOmitsNoOpModificationAndReleasesState)
{
  BRepGraph                aGraph;
  const gp_Pnt             aPoint(1.0, 2.0, 3.0);
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(aPoint, 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());
  const BRepGraph_UID aUID = aBase->Graph().UIDs().Of(aVertex);

  BRepGraph_Transaction anEdit = aBase->BeginTransaction();
  ASSERT_TRUE(anEdit.SetVertexPoint(aUID, aPoint));
  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  EXPECT_TRUE(aCommit.Diff.IsEmpty());
  EXPECT_EQ(aCommit.Revision->SemanticHash(), aBase->SemanticHash());
  EXPECT_TRUE(anEdit.IsFinished());
  EXPECT_TRUE(anEdit.BaseRevision().IsNull());
  EXPECT_EQ(anEdit.Graph(), nullptr);
}

TEST(BRepGraph_RevisionTest, FailedCommitReleasesTransactionState)
{
  BRepGraph aGraph;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape()).IsOk());
  const occ::handle<BRepGraph_Revision> aBase = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBase.IsNull());
  ASSERT_FALSE(aBase->SupportsSparseEdits());

  BRepGraph_VertexIterator aVertices(aGraph);
  ASSERT_TRUE(aVertices.More());
  BRepGraph_Transaction anEdit        = aBase->BeginTransaction();
  BRepGraph*            aWorkingGraph = anEdit.Graph();
  ASSERT_NE(aWorkingGraph, nullptr);
  aWorkingGraph->Editor().Gen().RemoveNode(aVertices.CurrentId());
  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  EXPECT_EQ(aCommit.Status, BRepGraph_RevisionStatus::Code::ValidationFailed);
  EXPECT_TRUE(anEdit.IsFinished());
  EXPECT_TRUE(anEdit.BaseRevision().IsNull());
  EXPECT_EQ(anEdit.Graph(), nullptr);
}

TEST(BRepGraph_RevisionTest, CompactionPreservesSemanticHashButChangesPhysicalHash)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aRemoved  = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aSurvivor = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aRemoved.IsValid() && aSurvivor.IsValid());

  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aRemoved));
  aGraph.Editor().Gen().CleanupRemovedReferences();
  const occ::handle<BRepGraph_Revision> aBefore = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aBefore.IsNull());

  const BRepGraph_Compact::Result aCompactResult = BRepGraph_Compact::Perform(aGraph);
  EXPECT_EQ(aCompactResult.NbRemovedVertices, 1u);
  const occ::handle<BRepGraph_Revision> aAfter = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aAfter.IsNull());

  EXPECT_EQ(aBefore->SemanticHash(), aAfter->SemanticHash());
  EXPECT_NE(aBefore->StorageRootHash(), aAfter->StorageRootHash());
}

TEST(BRepGraph_RevisionTest, DiffReportsEdgeCurveDifference)
{
  BRepGraph                aBaseGraph;
  const BRepGraph_VertexId aBaseStart =
    aBaseGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aBaseEnd =
    aBaseGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aBaseStart.IsValid() && aBaseEnd.IsValid());
  const occ::handle<Geom_Curve> aBaseCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  ASSERT_TRUE(
    aBaseGraph.Editor().Edges().Add(aBaseStart, aBaseEnd, aBaseCurve, 0.0, 1.0, 1.e-7).IsValid());

  BRepGraph                aResultGraph;
  const BRepGraph_VertexId aResultStart =
    aResultGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aResultEnd =
    aResultGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aResultStart.IsValid() && aResultEnd.IsValid());
  const occ::handle<Geom_Curve> aResultCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  ASSERT_TRUE(aResultGraph.Editor()
                .Edges()
                .Add(aResultStart, aResultEnd, aResultCurve, 0.0, 1.0, 1.e-7)
                .IsValid());

  const occ::handle<BRepGraph_Revision> aBase   = BRepGraph_Revision::FromGraph(aBaseGraph);
  const occ::handle<BRepGraph_Revision> aResult = BRepGraph_Revision::FromGraph(aResultGraph);
  ASSERT_FALSE(aBase.IsNull());
  ASSERT_FALSE(aResult.IsNull());
  const BRepGraph_RevisionDiff aDiff = aBase->Diff(*aResult);
  ASSERT_EQ(aDiff.ModifiedUIDs.Size(), 1u);
  EXPECT_EQ(aDiff.ModifiedUIDs.First().Kind, BRepGraph_NodeId::Kind::Edge);
}

TEST(BRepGraph_RevisionTest, DiffReportsEdgeParameterDifference)
{
  BRepGraph                aBaseGraph;
  const BRepGraph_VertexId aBaseStart =
    aBaseGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aBaseEnd =
    aBaseGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const occ::handle<Geom_Curve> aBaseCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  ASSERT_TRUE(
    aBaseGraph.Editor().Edges().Add(aBaseStart, aBaseEnd, aBaseCurve, 0.0, 1.0, 1.e-7).IsValid());

  BRepGraph                aResultGraph;
  const BRepGraph_VertexId aResultStart =
    aResultGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aResultEnd =
    aResultGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const occ::handle<Geom_Curve> aResultCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  ASSERT_TRUE(aResultGraph.Editor()
                .Edges()
                .Add(aResultStart, aResultEnd, aResultCurve, 0.25, 1.0, 1.e-7)
                .IsValid());

  const occ::handle<BRepGraph_Revision> aBase   = BRepGraph_Revision::FromGraph(aBaseGraph);
  const occ::handle<BRepGraph_Revision> aResult = BRepGraph_Revision::FromGraph(aResultGraph);
  ASSERT_FALSE(aBase.IsNull());
  ASSERT_FALSE(aResult.IsNull());
  const BRepGraph_RevisionDiff aDiff = aBase->Diff(*aResult);
  ASSERT_EQ(aDiff.ModifiedUIDs.Size(), 1u);
  EXPECT_EQ(aDiff.ModifiedUIDs.First().Kind, BRepGraph_NodeId::Kind::Edge);
}

TEST(BRepGraph_RevisionTest, FaceWireRefRemovalPreservesOuterBoundaryInvariant)
{
  BRepGraph_Transaction anEdit = BRepGraph_Revision::Empty()->BeginTransaction();
  ASSERT_TRUE(anEdit.IsValid());
  NCollection_LinearVector<BRepGraph_UID> anEmptyCoEdges;
  const BRepGraph_WireId                  anOuterWire = anEdit.AddWire(anEmptyCoEdges);
  const BRepGraph_WireId                  anInnerWire = anEdit.AddWire(anEmptyCoEdges);
  ASSERT_TRUE(anOuterWire.IsValid() && anInnerWire.IsValid());

  BRepGraph_UID anOuterWireUID;
  BRepGraph_UID anInnerWireUID;
  ASSERT_TRUE(anEdit.WireUID(anOuterWire, anOuterWireUID));
  ASSERT_TRUE(anEdit.WireUID(anInnerWire, anInnerWireUID));
  NCollection_LinearVector<BRepGraph_UID> anInnerWires;
  anInnerWires.Append(anInnerWireUID);
  const BRepGraph_FaceId aFace = anEdit.AddFace(anOuterWireUID, anInnerWires, 1.e-7);
  ASSERT_TRUE(aFace.IsValid());
  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());

  const BRepGraph_UID            aFaceUID = aCommit.Revision->Graph().UIDs().Of(aFace);
  BRepGraph_Revision::FaceChange aFaceChange;
  ASSERT_TRUE(aCommit.Revision->ReadFace(aFaceUID, aFaceChange));
  ASSERT_EQ(aFaceChange.WireRefUIDs.Size(), 2u);
  BRepGraph_Revision::WireRefChange anOuterWireRefChange;
  BRepGraph_Revision::WireRefChange anInnerWireRefChange;
  ASSERT_TRUE(aCommit.Revision->ReadWireRef(aFaceChange.WireRefUIDs.First(), anOuterWireRefChange));
  ASSERT_TRUE(
    aCommit.Revision->ReadWireRef(aFaceChange.WireRefUIDs.Value(1), anInnerWireRefChange));
  EXPECT_EQ(anOuterWireRefChange.Role, BRepGraph_Revision::BoundaryRole::Outer);
  EXPECT_EQ(anInnerWireRefChange.Role, BRepGraph_Revision::BoundaryRole::Inner);

  BRepGraph_Transaction anInnerRemoval = aCommit.Revision->BeginTransaction();
  ASSERT_TRUE(anInnerRemoval.IsValid());
  ASSERT_TRUE(anInnerRemoval.RemoveWireRef(aFaceChange.WireRefUIDs.Value(1)));
  EXPECT_TRUE(anInnerRemoval.Commit().IsOk());

  BRepGraph_Transaction anOuterRemoval = aCommit.Revision->BeginTransaction();
  ASSERT_TRUE(anOuterRemoval.IsValid());
  EXPECT_FALSE(anOuterRemoval.RemoveWireRef(aFaceChange.WireRefUIDs.First()));
}

TEST(BRepGraph_RevisionTest, WireRemovalRejectsPendingFaceReference)
{
  BRepGraph_Transaction anEdit = BRepGraph_Revision::Empty()->BeginTransaction();
  ASSERT_TRUE(anEdit.IsValid());

  NCollection_LinearVector<BRepGraph_UID> anEmptyCoEdges;
  const BRepGraph_WireId                  aWire = anEdit.AddWire(anEmptyCoEdges);
  ASSERT_TRUE(aWire.IsValid());

  BRepGraph_UID aWireUID;
  ASSERT_TRUE(anEdit.WireUID(aWire, aWireUID));
  NCollection_LinearVector<BRepGraph_UID> anEmptyInnerWires;
  ASSERT_TRUE(anEdit.AddFace(aWireUID, anEmptyInnerWires, 1.e-7).IsValid());

  EXPECT_FALSE(anEdit.RemoveWire(aWireUID));
  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());
  EXPECT_TRUE(aCommit.Revision->Graph().ValidateRelations());
}

TEST(BRepGraph_ReplaceTest, RestoresPersistentRevisionInStableWrapper)
{
  BRepGraph                      aGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.Parallel = false;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape(), anOptions).IsOk());
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aRevision.IsNull());

  BRepGraph_VertexId aVertex = BRepGraph_VertexId::Start();
  ASSERT_TRUE(aVertex.IsValid(aGraph.Topo().Vertices().Nb()));
  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aChange = aGraph.Editor().Vertices().Mut(aVertex);
    const gp_Pnt                                aPoint  = aChange->Point;
    aGraph.Editor().Vertices().SetPoint(aChange, gp_Pnt(aPoint.X() + 5.0, aPoint.Y(), aPoint.Z()));
  }
  EXPECT_NE(BRepGraph_RevisionHash::Hasher::Semantic(aGraph), aRevision->SemanticHash());

  BRepGraph aReplacement;
  ASSERT_TRUE(aRevision->CopyTo(aReplacement));
  BRepGraph* const                aStableAddress = &aGraph;
  const BRepGraph_Replace::Result aResult =
    BRepGraph_Replace::Perform(aGraph, std::move(aReplacement));
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(&aGraph, aStableAddress);
  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Semantic(aGraph), aRevision->SemanticHash());
}

TEST(BRepGraph_ReplaceTest, RejectsDifferentGraphIdentityWithoutMutation)
{
  BRepGraph                      aGraph;
  BRepGraph                      aForeignGraph;
  BRepGraph::ShapesView::Options anOptions;
  anOptions.Parallel = false;
  ASSERT_TRUE(aGraph.Shapes().Add(BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape(), anOptions).IsOk());
  ASSERT_TRUE(
    aForeignGraph.Shapes().Add(BRepPrimAPI_MakeBox(4.0, 5.0, 6.0).Shape(), anOptions).IsOk());
  const BRepGraph_RevisionHash aBefore = BRepGraph_RevisionHash::Hasher::Semantic(aGraph);

  const BRepGraph_Replace::Result aResult =
    BRepGraph_Replace::Perform(aGraph, std::move(aForeignGraph));
  EXPECT_EQ(aResult.StatusCode, BRepGraph_Replace::Status::DifferentGraphIdentity);
  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Semantic(aGraph), aBefore);
}

TEST(BRepGraph_ReplaceTest, LayerMigrationFailureKeepsSourceGraphAndServices)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 2.0, 3.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aRevision.IsNull());

  const occ::handle<BRepGraph_ThrowingCopyLayer> aLayer = new BRepGraph_ThrowingCopyLayer();
  aGraph.LayerRegistry().RegisterLayer(aLayer);
  const BRepGraph_RevisionHash aBefore = BRepGraph_RevisionHash::Hasher::Semantic(aGraph);
  BRepGraph                    aReplacement;
  ASSERT_TRUE(aRevision->CopyTo(aReplacement));

  const BRepGraph_Replace::Result aResult =
    BRepGraph_Replace::Perform(aGraph, std::move(aReplacement));
  EXPECT_EQ(aResult.StatusCode, BRepGraph_Replace::Status::MigrationFailed);
  EXPECT_EQ(BRepGraph_RevisionHash::Hasher::Semantic(aGraph), aBefore);
  EXPECT_EQ(aGraph.Topo().Vertices().Definition(aVertex).Point.X(), 1.0);
  EXPECT_EQ(aGraph.LayerRegistry().FindLayer(BRepGraph_ThrowingCopyLayer::GetID()), aLayer);
}
