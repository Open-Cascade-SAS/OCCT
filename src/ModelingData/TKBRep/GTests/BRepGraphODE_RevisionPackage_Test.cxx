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
#include <BRepGraph_Transaction.hxx>
#include <BRepGraphODE_RevisionPackage.hxx>
#include <BRepGraph_Revision.hxx>
#include <BRepGraph_RevisionHash.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Line.hxx>
#include <OSD_Path.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace
{

std::filesystem::path makePackageTestPath(const char* theName)
{
  const uint64_t aStamp =
    static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
  TCollection_AsciiString aName("occt-brepgraph-");
  aName += theName;
  aName += '-';
  aName += static_cast<int>(aStamp & 0x7fffffff);
  return std::filesystem::temp_directory_path() / aName.ToCString();
}

OSD_Path asOSDPath(const std::filesystem::path& thePath)
{
  return OSD_Path(TCollection_AsciiString(thePath.u8string().c_str()));
}

std::filesystem::path asFilesystemPath(const OSD_Path& thePath)
{
  TCollection_AsciiString aPath;
  thePath.SystemName(aPath);
  return std::filesystem::u8path(aPath.ToCString());
}

void expectFreshCoreHashes(const occ::handle<BRepGraph_Revision>& theRevision)
{
  ASSERT_FALSE(theRevision.IsNull());
  const BRepGraph_RevisionHash::Hasher::Result aFresh =
    BRepGraph_RevisionHash::Hasher::Compute(theRevision->Graph());
  EXPECT_EQ(theRevision->SemanticHash(), aFresh.Semantic);
  EXPECT_EQ(theRevision->StorageRootHash(), aFresh.Storage);
}

} // namespace

TEST(BRepGraph_RevisionTest, EdgeEditPreservesDurableEndpointsAndRelations)
{
  BRepGraph_Transaction aVertexEdit = BRepGraph_Revision::Empty()->BeginTransaction();
  ASSERT_TRUE(aVertexEdit.IsValid());
  const BRepGraph_VertexId aStartVertex = aVertexEdit.AddVertex(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEndVertex  = aVertexEdit.AddVertex(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aStartVertex.IsValid() && anEndVertex.IsValid());
  const BRepGraph_Transaction::CommitResult aVertexCommit = aVertexEdit.Commit();
  ASSERT_TRUE(aVertexCommit.IsOk());
  const BRepGraph_UID aStartUID = aVertexCommit.Revision->Graph().UIDs().Of(aStartVertex);
  const BRepGraph_UID anEndUID  = aVertexCommit.Revision->Graph().UIDs().Of(anEndVertex);

  BRepGraph_Transaction anEdgeEdit = aVertexCommit.Revision->BeginTransaction();
  ASSERT_TRUE(anEdgeEdit.IsValid());
  const occ::handle<Geom_Curve> aCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  ASSERT_FALSE(aCurve.IsNull());
  const BRepGraph_EdgeId anEdge = anEdgeEdit.AddEdge(aStartUID, anEndUID, aCurve, 0.0, 1.0, 2.e-7);
  ASSERT_TRUE(anEdge.IsValid());
  const BRepGraph_Transaction::CommitResult anEdgeCommit = anEdgeEdit.Commit();
  ASSERT_TRUE(anEdgeCommit.IsOk());
  ASSERT_TRUE(anEdgeCommit.Revision->SupportsSparseEdits());
  EXPECT_EQ(anEdgeCommit.Revision->NbVisibleEdges(), 1u);
  EXPECT_EQ(anEdgeCommit.Revision->NbVisibleVertexRefs(), 2u);

  const BRepGraph_UID anEdgeUID               = anEdgeCommit.Revision->Graph().UIDs().Of(anEdge);
  const occ::handle<BRepGraph_Revision> aRead = anEdgeCommit.Revision;
  BRepGraph_Revision::EdgeChange        anEdgeChange;
  ASSERT_TRUE(aRead->ReadEdge(anEdgeUID, anEdgeChange));
  const BRepGraphInc::EdgeDef& anEdgeDefinition = anEdgeChange.Definition;
  EXPECT_EQ(aRead->NbVisibleVertexRefs(), 2u);
  const BRepGraph_RefUID aStartRefUID =
    anEdgeCommit.Revision->Graph().UIDs().Of(anEdgeDefinition.StartVertexRefId);
  BRepGraph_Revision::VertexRefChange aStartRefChange;
  ASSERT_TRUE(aRead->ReadVertexRef(aStartRefUID, aStartRefChange));
  EXPECT_EQ(aStartRefChange.Definition.ParentEdgeId, anEdge);
  const BRepGraph&               aReadGraph = aRead->Graph();
  const BRepGraphInc::VertexRef& aStartRef =
    aReadGraph.Refs().Vertices().Entry(anEdgeDefinition.StartVertexRefId);
  const BRepGraphInc::VertexRef& anEndRef =
    aReadGraph.Refs().Vertices().Entry(anEdgeDefinition.EndVertexRefId);
  EXPECT_EQ(aReadGraph.UIDs().Of(aStartRef.ChildVertexId), aStartUID);
  EXPECT_EQ(aReadGraph.UIDs().Of(anEndRef.ChildVertexId), anEndUID);
  EXPECT_DOUBLE_EQ(anEdgeDefinition.Tolerance, 2.e-7);
  EXPECT_TRUE(anEdgeCommit.Revision->Graph().ValidateRelations());
  EXPECT_EQ(anEdgeCommit.Revision->SemanticHash(),
            BRepGraph_RevisionHash::Hasher::Semantic(anEdgeCommit.Revision->Graph()));
  const BRepGraph_RevisionDiff anEdgeDiff = aVertexCommit.Revision->Diff(*anEdgeCommit.Revision);
  ASSERT_EQ(anEdgeDiff.CreatedUIDs.Size(), 1u);
  EXPECT_EQ(anEdgeDiff.CreatedUIDs.First(), anEdgeUID);
  EXPECT_EQ(anEdgeDiff.CreatedRefUIDs.Size(), 2u);

  const std::filesystem::path           anPackagePath = makePackageTestPath("edge");
  BRepGraphODE_RevisionPackage::Options anPackageOptions;
  const BRepGraphODE_RevisionPackage::Result anPackageWrite =
    BRepGraphODE_RevisionPackage::Write(*anEdgeCommit.Revision,
                                        asOSDPath(anPackagePath),
                                        anPackageOptions);
  ASSERT_TRUE(anPackageWrite.IsOk())
    << (anPackageWrite.Diagnostics.IsEmpty()
          ? ""
          : anPackageWrite.Diagnostics.First().Message.ToCString());
  const BRepGraphODE_RevisionPackage::Result anPackageRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(anPackagePath), anPackageOptions);
  ASSERT_TRUE(anPackageRead.IsOk());
  EXPECT_EQ(anPackageRead.Revision->SemanticHash(), anEdgeCommit.Revision->SemanticHash());
  BRepGraph_Revision::EdgeChange anPackagedEdgeChange;
  EXPECT_TRUE(anPackageRead.Revision->ReadEdge(anEdgeUID, anPackagedEdgeChange));
  std::filesystem::remove_all(anPackagePath);

  BRepGraph_Transaction aToleranceEdit = anEdgeCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aToleranceEdit.IsValid());
  ASSERT_TRUE(aToleranceEdit.SetEdgeTolerance(anEdgeUID, 3.e-7));
  const BRepGraph_Transaction::CommitResult aToleranceCommit = aToleranceEdit.Commit();
  ASSERT_TRUE(aToleranceCommit.IsOk());
  const occ::handle<Geom_Line> aPreservedCurve =
    occ::down_cast<Geom_Line>(aToleranceCommit.Revision->Graph().Topo().Edges().Curve3D(anEdge));
  ASSERT_FALSE(aPreservedCurve.IsNull());
  EXPECT_DOUBLE_EQ(aPreservedCurve->Position().Direction().X(), 1.0);
  EXPECT_DOUBLE_EQ(aPreservedCurve->Position().Direction().Y(), 0.0);

  BRepGraph_Transaction aModifyEdit = aToleranceCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aModifyEdit.IsValid());
  const occ::handle<Geom_Curve> aUpdatedCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  ASSERT_TRUE(aModifyEdit.SetEdgeCurve(anEdgeUID, aUpdatedCurve, -1.0, 2.0));
  const BRepGraph_Transaction::CommitResult aModifyCommit = aModifyEdit.Commit();
  ASSERT_TRUE(aModifyCommit.IsOk());
  expectFreshCoreHashes(aModifyCommit.Revision);
  const occ::handle<Geom_Line> aCopiedCurve =
    occ::down_cast<Geom_Line>(aModifyCommit.Revision->Graph().Topo().Edges().Curve3D(anEdge));
  ASSERT_FALSE(aCopiedCurve.IsNull());
  EXPECT_DOUBLE_EQ(aCopiedCurve->Position().Direction().X(), 0.0);
  EXPECT_DOUBLE_EQ(aCopiedCurve->Position().Direction().Y(), 1.0);
  EXPECT_NE(aModifyCommit.Revision->SemanticHash(), aToleranceCommit.Revision->SemanticHash());
  BRepGraph_Revision::EdgeChange aModifiedChange;
  ASSERT_TRUE(aModifyCommit.Revision->ReadEdge(anEdgeUID, aModifiedChange));
  EXPECT_DOUBLE_EQ(aModifiedChange.Definition.Tolerance, 3.e-7);
  EXPECT_FALSE(aModifyCommit.Diff.ModifiedUIDs.IsEmpty());
  EXPECT_EQ(aModifyCommit.Diff.ModifiedUIDs.First(), anEdgeUID);
  const std::filesystem::path anUpdatedPackagePath = makePackageTestPath("edge-curve-update");
  BRepGraphODE_RevisionPackage::Options anUpdatedPackageOptions;
  ASSERT_TRUE(BRepGraphODE_RevisionPackage::Write(*aModifyCommit.Revision,
                                                  asOSDPath(anUpdatedPackagePath),
                                                  anUpdatedPackageOptions)
                .IsOk());
  const BRepGraphODE_RevisionPackage::Result anUpdatedPackageRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(anUpdatedPackagePath), anUpdatedPackageOptions);
  ASSERT_TRUE(anUpdatedPackageRead.IsOk()) << anUpdatedPackageRead.Diagnostics.Size();
  const occ::handle<Geom_Line> anPackagedCurve = occ::down_cast<Geom_Line>(
    anUpdatedPackageRead.Revision->Graph().Topo().Edges().Curve3D(anEdge));
  ASSERT_FALSE(anPackagedCurve.IsNull());
  EXPECT_DOUBLE_EQ(anPackagedCurve->Position().Direction().X(), 0.0);
  EXPECT_DOUBLE_EQ(anPackagedCurve->Position().Direction().Y(), 1.0);
  std::filesystem::remove_all(anUpdatedPackagePath);

  BRepGraph_Transaction aRemoveEdit = aModifyCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aRemoveEdit.IsValid());
  ASSERT_TRUE(aRemoveEdit.RemoveEdge(anEdgeUID));
  const BRepGraph_Transaction::CommitResult aRemoveCommit = aRemoveEdit.Commit();
  ASSERT_TRUE(aRemoveCommit.IsOk());
  expectFreshCoreHashes(aRemoveCommit.Revision);
  BRepGraph_Revision::EdgeChange aRemovedEdgeChange;
  EXPECT_FALSE(aRemoveCommit.Revision->ReadEdge(anEdgeUID, aRemovedEdgeChange));
  EXPECT_EQ(aRemoveCommit.Revision->NbVisibleEdges(), 0u);
  EXPECT_EQ(aRemoveCommit.Revision->NbVisibleVertexRefs(), 0u);
  EXPECT_TRUE(aRemoveCommit.Revision->Graph().ValidateRelations());
  EXPECT_FALSE(aRemoveCommit.Diff.RemovedRefUIDs.IsEmpty());
}

TEST(BRepGraph_RevisionTest, FreeCoEdgeEditPreservesUsageIdentity)
{
  BRepGraph_Transaction aVertexEdit = BRepGraph_Revision::Empty()->BeginTransaction();
  ASSERT_TRUE(aVertexEdit.IsValid());
  const BRepGraph_VertexId aStartVertex = aVertexEdit.AddVertex(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEndVertex  = aVertexEdit.AddVertex(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aStartVertex.IsValid() && anEndVertex.IsValid());
  const BRepGraph_Transaction::CommitResult aVertexCommit = aVertexEdit.Commit();
  ASSERT_TRUE(aVertexCommit.IsOk());
  const BRepGraph_UID aStartUID = aVertexCommit.Revision->Graph().UIDs().Of(aStartVertex);
  const BRepGraph_UID anEndUID  = aVertexCommit.Revision->Graph().UIDs().Of(anEndVertex);

  BRepGraph_Transaction anEdgeEdit = aVertexCommit.Revision->BeginTransaction();
  ASSERT_TRUE(anEdgeEdit.IsValid());
  const occ::handle<Geom_Curve> aCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraph_EdgeId anEdge = anEdgeEdit.AddEdge(aStartUID, anEndUID, aCurve, 0.0, 1.0, 2.e-7);
  ASSERT_TRUE(anEdge.IsValid());
  const BRepGraph_Transaction::CommitResult anEdgeCommit = anEdgeEdit.Commit();
  ASSERT_TRUE(anEdgeCommit.IsOk());
  const BRepGraph_UID anEdgeUID = anEdgeCommit.Revision->Graph().UIDs().Of(anEdge);

  BRepGraph_Transaction aCoEdgeEdit = anEdgeCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aCoEdgeEdit.IsValid());
  const BRepGraph_CoEdgeId aCoEdge = aCoEdgeEdit.AddCoEdge(anEdgeUID, TopAbs_FORWARD);
  ASSERT_TRUE(aCoEdge.IsValid());
  const BRepGraph_Transaction::CommitResult aCoEdgeCommit = aCoEdgeEdit.Commit();
  ASSERT_TRUE(aCoEdgeCommit.IsOk());
  ASSERT_EQ(aCoEdgeCommit.Diff.UsageLinkChanges.Size(), 1u);
  EXPECT_EQ(aCoEdgeCommit.Diff.UsageLinkChanges.First().Kind,
            BRepGraph_RevisionDiff::ChangeKind::Created);
  expectFreshCoreHashes(aCoEdgeCommit.Revision);
  ASSERT_TRUE(aCoEdgeCommit.Revision->SupportsSparseEdits());
  EXPECT_EQ(aCoEdgeCommit.Revision->NbVisibleCoEdges(), 1u);

  const BRepGraph_UID aCoEdgeUID              = aCoEdgeCommit.Revision->Graph().UIDs().Of(aCoEdge);
  const occ::handle<BRepGraph_Revision> aRead = aCoEdgeCommit.Revision;
  BRepGraph_Revision::CoEdgeChange      aCoEdgeChange;
  ASSERT_TRUE(aRead->ReadCoEdge(aCoEdgeUID, aCoEdgeChange));
  EXPECT_EQ(aCoEdgeChange.Definition.ChildEdgeId, anEdge);
  EXPECT_EQ(aCoEdgeChange.Definition.Orientation, TopAbs_FORWARD);
  EXPECT_EQ(aRead->Graph().UIDs().Of(aCoEdgeChange.Definition.ChildEdgeId), anEdgeUID);
  EXPECT_TRUE(aCoEdgeCommit.Revision->Graph().ValidateRelations());
  EXPECT_EQ(aCoEdgeCommit.Revision->SemanticHash(),
            BRepGraph_RevisionHash::Hasher::Semantic(aCoEdgeCommit.Revision->Graph()));

  BRepGraph_Transaction anEdgeRemoveWhileUsed = aCoEdgeCommit.Revision->BeginTransaction();
  ASSERT_TRUE(anEdgeRemoveWhileUsed.IsValid());
  EXPECT_FALSE(anEdgeRemoveWhileUsed.RemoveEdge(anEdgeUID));
  anEdgeRemoveWhileUsed.Abort();

  const BRepGraph_RevisionDiff aCoEdgeDiff = anEdgeCommit.Revision->Diff(*aCoEdgeCommit.Revision);
  ASSERT_EQ(aCoEdgeDiff.CreatedUIDs.Size(), 1u);
  EXPECT_EQ(aCoEdgeDiff.CreatedUIDs.First(), aCoEdgeUID);
  ASSERT_EQ(aCoEdgeDiff.UsageLinkChanges.Size(), 1u);
  EXPECT_EQ(aCoEdgeDiff.UsageLinkChanges.First().LinkUID, aCoEdgeUID);
  EXPECT_EQ(aCoEdgeDiff.UsageLinkChanges.First().Kind, BRepGraph_RevisionDiff::ChangeKind::Created);

  const std::filesystem::path           anPackagePath = makePackageTestPath("coedge");
  BRepGraphODE_RevisionPackage::Options anPackageOptions;
  const BRepGraphODE_RevisionPackage::Result anPackageWrite =
    BRepGraphODE_RevisionPackage::Write(*aCoEdgeCommit.Revision,
                                        asOSDPath(anPackagePath),
                                        anPackageOptions);
  ASSERT_TRUE(anPackageWrite.IsOk())
    << (anPackageWrite.Diagnostics.IsEmpty()
          ? ""
          : anPackageWrite.Diagnostics.First().Message.ToCString());
  const BRepGraphODE_RevisionPackage::Result anPackageRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(anPackagePath), anPackageOptions);
  ASSERT_TRUE(anPackageRead.IsOk());
  BRepGraph_Revision::CoEdgeChange anPackagedCoEdgeChange;
  EXPECT_TRUE(anPackageRead.Revision->ReadCoEdge(aCoEdgeUID, anPackagedCoEdgeChange));
  std::filesystem::remove_all(anPackagePath);

  BRepGraph_Transaction aModifyEdit = aCoEdgeCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aModifyEdit.IsValid());
  ASSERT_TRUE(aModifyEdit.SetCoEdgeOrientation(aCoEdgeUID, TopAbs_REVERSED));
  const BRepGraph_Transaction::CommitResult aModifyCommit = aModifyEdit.Commit();
  ASSERT_TRUE(aModifyCommit.IsOk());
  ASSERT_EQ(aModifyCommit.Diff.UsageLinkChanges.Size(), 1u);
  EXPECT_EQ(aModifyCommit.Diff.UsageLinkChanges.First().Kind,
            BRepGraph_RevisionDiff::ChangeKind::Modified);
  expectFreshCoreHashes(aModifyCommit.Revision);
  BRepGraph_Revision::CoEdgeChange aModifiedCoEdgeChange;
  ASSERT_TRUE(aModifyCommit.Revision->ReadCoEdge(aCoEdgeUID, aModifiedCoEdgeChange));
  EXPECT_EQ(aModifiedCoEdgeChange.Definition.Orientation, TopAbs_REVERSED);
  EXPECT_EQ(aCoEdgeCommit.Revision->Diff(*aModifyCommit.Revision).UsageLinkChanges.First().Kind,
            BRepGraph_RevisionDiff::ChangeKind::Modified);

  BRepGraph_Transaction aRemoveEdit = aModifyCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aRemoveEdit.IsValid());
  ASSERT_TRUE(aRemoveEdit.RemoveCoEdge(aCoEdgeUID));
  const BRepGraph_Transaction::CommitResult aRemoveCommit = aRemoveEdit.Commit();
  ASSERT_TRUE(aRemoveCommit.IsOk()) << (aRemoveCommit.Diagnostics.IsEmpty()
                                          ? ""
                                          : aRemoveCommit.Diagnostics.First().Message.ToCString());
  expectFreshCoreHashes(aRemoveCommit.Revision);
  ASSERT_EQ(aRemoveCommit.Diff.UsageLinkChanges.Size(), 1u);
  EXPECT_EQ(aRemoveCommit.Diff.UsageLinkChanges.First().Kind,
            BRepGraph_RevisionDiff::ChangeKind::Removed);
  BRepGraph_Revision::CoEdgeChange aRemovedCoEdgeChange;
  EXPECT_FALSE(aRemoveCommit.Revision->ReadCoEdge(aCoEdgeUID, aRemovedCoEdgeChange));
  EXPECT_EQ(aRemoveCommit.Revision->NbVisibleCoEdges(), 0u);
  EXPECT_TRUE(aRemoveCommit.Revision->Graph().ValidateRelations());
  const BRepGraph_RevisionDiff aRemoveDiff = aModifyCommit.Revision->Diff(*aRemoveCommit.Revision);
  ASSERT_EQ(aRemoveDiff.UsageLinkChanges.Size(), 1u);
  EXPECT_EQ(aRemoveDiff.UsageLinkChanges.First().Kind, BRepGraph_RevisionDiff::ChangeKind::Removed);
}

TEST(BRepGraph_RevisionTest, PackagePreservesEdgeAndCoEdgeTombstones)
{
  BRepGraph_Transaction aVertexEdit = BRepGraph_Revision::Empty()->BeginTransaction();
  ASSERT_TRUE(aVertexEdit.IsValid());
  const BRepGraph_VertexId aStartVertex = aVertexEdit.AddVertex(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId anEndVertex  = aVertexEdit.AddVertex(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aStartVertex.IsValid() && anEndVertex.IsValid());
  const BRepGraph_Transaction::CommitResult aVertexCommit = aVertexEdit.Commit();
  ASSERT_TRUE(aVertexCommit.IsOk());
  const BRepGraph_UID aStartUID = aVertexCommit.Revision->Graph().UIDs().Of(aStartVertex);
  const BRepGraph_UID anEndUID  = aVertexCommit.Revision->Graph().UIDs().Of(anEndVertex);

  BRepGraph_Transaction anEdgeEdit = aVertexCommit.Revision->BeginTransaction();
  ASSERT_TRUE(anEdgeEdit.IsValid());
  const occ::handle<Geom_Curve> aCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraph_EdgeId anEdge = anEdgeEdit.AddEdge(aStartUID, anEndUID, aCurve, 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge.IsValid());
  const BRepGraph_Transaction::CommitResult anEdgeCommit = anEdgeEdit.Commit();
  ASSERT_TRUE(anEdgeCommit.IsOk());
  const BRepGraph_UID anEdgeUID = anEdgeCommit.Revision->Graph().UIDs().Of(anEdge);

  BRepGraph_Transaction aCoEdgeEdit = anEdgeCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aCoEdgeEdit.IsValid());
  const BRepGraph_CoEdgeId aCoEdge = aCoEdgeEdit.AddCoEdge(anEdgeUID, TopAbs_FORWARD);
  ASSERT_TRUE(aCoEdge.IsValid());
  const BRepGraph_Transaction::CommitResult aCoEdgeCommit = aCoEdgeEdit.Commit();
  ASSERT_TRUE(aCoEdgeCommit.IsOk());
  const BRepGraph_UID aCoEdgeUID = aCoEdgeCommit.Revision->Graph().UIDs().Of(aCoEdge);

  BRepGraph_Transaction aRemoveEdit = aCoEdgeCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aRemoveEdit.IsValid());
  ASSERT_TRUE(aRemoveEdit.RemoveCoEdge(aCoEdgeUID));
  ASSERT_TRUE(aRemoveEdit.RemoveEdge(anEdgeUID));
  const BRepGraph_Transaction::CommitResult aRemoveCommit = aRemoveEdit.Commit();
  ASSERT_TRUE(aRemoveCommit.IsOk());
  BRepGraph_Revision::EdgeChange   aRemovedEdgeChange;
  BRepGraph_Revision::CoEdgeChange aRemovedCoEdgeChange;
  EXPECT_FALSE(aRemoveCommit.Revision->ReadEdge(anEdgeUID, aRemovedEdgeChange));
  EXPECT_FALSE(aRemoveCommit.Revision->ReadCoEdge(aCoEdgeUID, aRemovedCoEdgeChange));
  EXPECT_TRUE(aRemoveCommit.Revision->Graph().ValidateRelations());

  const BRepGraph_RevisionDiff aDiff = aCoEdgeCommit.Revision->Diff(*aRemoveCommit.Revision);
  EXPECT_TRUE(aDiff.RemovedUIDs.Size() >= 2u);
  EXPECT_TRUE(aDiff.RemovedRefUIDs.Size() >= 2u);
  ASSERT_EQ(aDiff.UsageLinkChanges.Size(), 1u);
  EXPECT_EQ(aDiff.UsageLinkChanges.First().LinkUID, aCoEdgeUID);
  EXPECT_EQ(aDiff.UsageLinkChanges.First().Kind, BRepGraph_RevisionDiff::ChangeKind::Removed);

  const std::filesystem::path aPath = makePackageTestPath("edge-coedge-tombstone");
  ASSERT_TRUE(
    BRepGraphODE_RevisionPackage::Write(*aRemoveCommit.Revision, asOSDPath(aPath)).IsOk());
  const BRepGraphODE_RevisionPackage::Result aRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(aPath));
  ASSERT_TRUE(aRead.IsOk()) << aRead.Diagnostics.Size();
  ASSERT_FALSE(aRead.Revision.IsNull());
  EXPECT_TRUE(aRead.Revision->SupportsSparseEdits());
  EXPECT_TRUE(aRead.Revision->BeginTransaction().IsValid());

  BRepGraph_Transaction         aRestoredEdit = aRead.Revision->BeginTransaction();
  const occ::handle<Geom_Curve> aRestoredCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  ASSERT_TRUE(
    aRestoredEdit.AddEdge(aStartUID, anEndUID, aRestoredCurve, 0.0, 1.0, 1.e-7).IsValid());
  const BRepGraph_Transaction::CommitResult aRestoredCommit = aRestoredEdit.Commit();
  ASSERT_TRUE(aRestoredCommit.IsOk());
  EXPECT_EQ(aRestoredCommit.Revision->NbVisibleEdges(), 1u);
  EXPECT_EQ(aRestoredCommit.Revision->NbVisibleVertexRefs(), 2u);
  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, PackagePublishesAndRestoresEmptyRevision)
{
  const std::filesystem::path           aPath     = makePackageTestPath("empty");
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aRevision.IsNull());

  const BRepGraphODE_RevisionPackage::Result aWrite =
    BRepGraphODE_RevisionPackage::Write(*aRevision, asOSDPath(aPath));
  ASSERT_TRUE(aWrite.IsOk()) << aWrite.Diagnostics.Size()
                             << (aWrite.Diagnostics.IsEmpty()
                                   ? ""
                                   : aWrite.Diagnostics.First().Message.ToCString());
  EXPECT_TRUE(std::filesystem::is_regular_file(aPath / "manifest.json"));
  EXPECT_TRUE(std::filesystem::is_regular_file(aPath / "revision.info"));

  const BRepGraphODE_RevisionPackage::Result aRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(aPath));
  ASSERT_TRUE(aRead.IsOk()) << aRead.Diagnostics.Size();
  ASSERT_FALSE(aRead.Revision.IsNull());
  EXPECT_EQ(aRead.Revision->SemanticHash(), aRevision->SemanticHash());
  EXPECT_TRUE(aRead.Revision->Graph().IsEmpty());

  const BRepGraphODE_RevisionPackage::Result aValidate =
    BRepGraphODE_RevisionPackage::Validate(asOSDPath(aPath));
  EXPECT_TRUE(aValidate.IsOk()) << aValidate.Diagnostics.Size();
  EXPECT_TRUE(aValidate.Revision.IsNull());
  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, PackagePreservesOriginalShapeBindings)
{
  BRepGraph                           aGraph;
  const TopoDS_Shape                  aBox    = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();
  const BRepGraph::ShapesView::Result anAdded = aGraph.Shapes().Add(aBox);
  ASSERT_TRUE(anAdded.IsOk());

  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aRevision.IsNull());
  ASSERT_TRUE(aRevision->Graph().Shapes().HasOriginal(anAdded.TopologyRoot));

  const std::filesystem::path                aPath = makePackageTestPath("original");
  const BRepGraphODE_RevisionPackage::Result aWrite =
    BRepGraphODE_RevisionPackage::Write(*aRevision, asOSDPath(aPath));
  ASSERT_TRUE(aWrite.IsOk()) << aWrite.Diagnostics.Size()
                             << (aWrite.Diagnostics.IsEmpty()
                                   ? ""
                                   : aWrite.Diagnostics.First().Message.ToCString());

  const BRepGraphODE_RevisionPackage::Result aRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(aPath));
  ASSERT_TRUE(aRead.IsOk()) << aRead.Diagnostics.Size();
  ASSERT_FALSE(aRead.Revision.IsNull());
  const BRepGraph_NodeId aRestoredNode =
    aRead.Revision->Graph().UIDs().NodeIdFrom(aRevision->Graph().UIDs().Of(anAdded.TopologyRoot));
  ASSERT_TRUE(aRestoredNode.IsValid());
  EXPECT_TRUE(aRead.Revision->Graph().Shapes().HasOriginal(aRestoredNode));
  EXPECT_EQ(aRead.Revision->SemanticHash(), aRevision->SemanticHash());

  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, PackageRoundTripsCompleteNativeRevision)
{
  BRepGraph_Transaction anEdit = BRepGraph_Revision::Empty()->BeginTransaction();
  ASSERT_TRUE(anEdit.IsValid());
  ASSERT_TRUE(anEdit.AddVertex(gp_Pnt(7.0, 8.0, 9.0), 1.e-7).IsValid());
  const BRepGraph_Transaction::CommitResult aCommit = anEdit.Commit();
  ASSERT_TRUE(aCommit.IsOk());

  const std::filesystem::path           aPath = makePackageTestPath("native-revision");
  BRepGraphODE_RevisionPackage::Options anOptions;
  const BRepGraphODE_RevisionPackage::Result aWrite =
    BRepGraphODE_RevisionPackage::Write(*aCommit.Revision, asOSDPath(aPath), anOptions);
  ASSERT_TRUE(aWrite.IsOk()) << (aWrite.Diagnostics.IsEmpty()
                                   ? ""
                                   : aWrite.Diagnostics.First().Message.ToCString());
  const BRepGraphODE_RevisionPackage::Result aRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(aPath), anOptions);
  ASSERT_TRUE(aRead.IsOk()) << aRead.Diagnostics.Size();
  EXPECT_EQ(aRead.Revision->SemanticHash(), aCommit.Revision->SemanticHash());
  EXPECT_EQ(aRead.Revision->Graph().Topo().Vertices().NbActive(), 1u);
  BRepGraph_Transaction aRestoredEdit = aRead.Revision->BeginTransaction();
  ASSERT_TRUE(aRestoredEdit.IsValid());
  ASSERT_TRUE(aRestoredEdit.AddVertex(gp_Pnt(10.0, 11.0, 12.0), 1.e-7).IsValid());
  const BRepGraph_Transaction::CommitResult aRestoredCommit = aRestoredEdit.Commit();
  ASSERT_TRUE(aRestoredCommit.IsOk());
  EXPECT_EQ(aRestoredCommit.Revision->Graph().Topo().Vertices().NbActive(), 2u);
  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, PackagePreparationIsNotReadableBeforePublish)
{
  const std::filesystem::path           aPath     = makePackageTestPath("prepared");
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aRevision.IsNull());

  const BRepGraphODE_RevisionPackage::Result aPrepared =
    BRepGraphODE_RevisionPackage::Prepare(*aRevision, asOSDPath(aPath));
  ASSERT_TRUE(aPrepared.IsOk()) << aPrepared.Diagnostics.Size();
  EXPECT_FALSE(std::filesystem::exists(aPath));
  EXPECT_TRUE(std::filesystem::is_regular_file(asFilesystemPath(aPrepared.Prepared.StagingPath)
                                               / "manifest.json"));

  const BRepGraphODE_RevisionPackage::Result aBeforePublish =
    BRepGraphODE_RevisionPackage::Read(aPrepared.Prepared.PackagePath);
  EXPECT_FALSE(aBeforePublish.IsOk());
  EXPECT_EQ(aBeforePublish.StatusCode, BRepGraphODE_RevisionPackage::Status::NotFound);

  const BRepGraphODE_RevisionPackage::Result aPublished =
    BRepGraphODE_RevisionPackage::Publish(aPrepared.Prepared);
  ASSERT_TRUE(aPublished.IsOk()) << aPublished.Diagnostics.Size();
  EXPECT_TRUE(std::filesystem::is_regular_file(aPath / "manifest.json"));
  std::filesystem::remove_all(aPath);
  std::filesystem::remove_all(asFilesystemPath(aPrepared.Prepared.StagingPath));
}

TEST(BRepGraph_RevisionTest, PackagePublishCanRetryAfterTargetConflict)
{
  const std::filesystem::path           aPath     = makePackageTestPath("publish-retry");
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aRevision.IsNull());
  const BRepGraphODE_RevisionPackage::Result aPrepared =
    BRepGraphODE_RevisionPackage::Prepare(*aRevision, asOSDPath(aPath));
  ASSERT_TRUE(aPrepared.IsOk());

  std::filesystem::create_directories(aPath);
  const BRepGraphODE_RevisionPackage::Result aConflict =
    BRepGraphODE_RevisionPackage::Publish(aPrepared.Prepared);
  EXPECT_EQ(aConflict.StatusCode, BRepGraphODE_RevisionPackage::Status::AlreadyExists);
  EXPECT_TRUE(std::filesystem::is_regular_file(asFilesystemPath(aPrepared.Prepared.StagingPath)
                                               / "manifest.json"));

  std::filesystem::remove_all(aPath);
  const BRepGraphODE_RevisionPackage::Result aPublished =
    BRepGraphODE_RevisionPackage::Publish(aPrepared.Prepared);
  ASSERT_TRUE(aPublished.IsOk()) << aPublished.Diagnostics.Size();
  const BRepGraphODE_RevisionPackage::Result aRetried =
    BRepGraphODE_RevisionPackage::Publish(aPrepared.Prepared);
  EXPECT_TRUE(aRetried.IsOk())
    << aRetried.Diagnostics.Size()
    << (aRetried.Diagnostics.IsEmpty() ? "" : aRetried.Diagnostics.First().Code.ToCString())
    << (aRetried.Diagnostics.IsEmpty() ? "" : aRetried.Diagnostics.First().Message.ToCString());
  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, PackagePublishResumesAfterBackupRename)
{
  const std::filesystem::path           aPath     = makePackageTestPath("publish-backup-resume");
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aRevision.IsNull());
  ASSERT_TRUE(BRepGraphODE_RevisionPackage::Write(*aRevision, asOSDPath(aPath)).IsOk());

  BRepGraphODE_RevisionPackage::Options anOptions;
  anOptions.ReplaceExisting = true;
  const BRepGraphODE_RevisionPackage::Result aPrepared =
    BRepGraphODE_RevisionPackage::Prepare(*aRevision, asOSDPath(aPath), anOptions);
  ASSERT_TRUE(aPrepared.IsOk());
  const std::filesystem::path aStage(asFilesystemPath(aPrepared.Prepared.StagingPath));
  std::filesystem::path       aBackup = asFilesystemPath(aPrepared.Prepared.StagingPath);
  aBackup += ".backup";
  std::filesystem::rename(aPath, aBackup);

  const BRepGraphODE_RevisionPackage::Result aPublished =
    BRepGraphODE_RevisionPackage::Publish(aPrepared.Prepared);
  ASSERT_TRUE(aPublished.IsOk())
    << aPublished.Diagnostics.Size()
    << (aPublished.Diagnostics.IsEmpty() ? "" : aPublished.Diagnostics.First().Code.ToCString())
    << (aPublished.Diagnostics.IsEmpty() ? "" : aPublished.Diagnostics.First().Message.ToCString());
  EXPECT_FALSE(std::filesystem::exists(aBackup));
  EXPECT_TRUE(BRepGraphODE_RevisionPackage::Validate(asOSDPath(aPath)).IsOk());
  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, PackageRejectsModifiedRevisionDescription)
{
  const std::filesystem::path           aPath     = makePackageTestPath("manifest-canonical");
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aRevision.IsNull());
  ASSERT_TRUE(BRepGraphODE_RevisionPackage::Write(*aRevision, asOSDPath(aPath)).IsOk());

  const std::filesystem::path aManifestPath = aPath / "revision.info";
  std::ofstream               aManifest(aManifestPath, std::ios::app);
  ASSERT_TRUE(aManifest);
  aManifest << "schema 1\n";
  aManifest.close();

  const BRepGraphODE_RevisionPackage::Result aRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(aPath));
  EXPECT_EQ(aRead.StatusCode, BRepGraphODE_RevisionPackage::Status::DigestMismatch);
  EXPECT_TRUE(aRead.Revision.IsNull());
  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, PackageRejectsCorruptedContentEntry)
{
  const std::filesystem::path           aPath     = makePackageTestPath("corrupt");
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aRevision.IsNull());
  ASSERT_TRUE(BRepGraphODE_RevisionPackage::Write(*aRevision, asOSDPath(aPath)).IsOk());

  std::filesystem::path aCorePath;
  for (const std::filesystem::directory_entry& anEntry :
       std::filesystem::recursive_directory_iterator(aPath / "objects"))
  {
    if (anEntry.path().extension() == ".ode")
    {
      aCorePath = anEntry.path();
      break;
    }
  }
  ASSERT_FALSE(aCorePath.empty());
  std::ofstream aCorruptor(aCorePath, std::ios::binary | std::ios::app);
  ASSERT_TRUE(aCorruptor);
  aCorruptor << 'x';
  aCorruptor.close();

  const BRepGraphODE_RevisionPackage::Result aRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(aPath));
  EXPECT_EQ(aRead.StatusCode, BRepGraphODE_RevisionPackage::Status::DigestMismatch);
  EXPECT_TRUE(aRead.Revision.IsNull());
  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, PackageRejectsSymlinkedContentEntry)
{
  const std::filesystem::path           aPath     = makePackageTestPath("symlink");
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aRevision.IsNull());
  ASSERT_TRUE(BRepGraphODE_RevisionPackage::Write(*aRevision, asOSDPath(aPath)).IsOk());

  std::filesystem::path aCorePath;
  for (const std::filesystem::directory_entry& anEntry :
       std::filesystem::recursive_directory_iterator(aPath / "objects"))
  {
    if (anEntry.path().extension() == ".ode")
    {
      aCorePath = anEntry.path();
      break;
    }
  }
  ASSERT_FALSE(aCorePath.empty());

  const std::filesystem::path anOutsidePath = aPath.string() + "-outside.ode";
  std::error_code             anError;
  std::filesystem::remove(anOutsidePath, anError);
  anError.clear();
  std::filesystem::rename(aCorePath, anOutsidePath, anError);
  ASSERT_FALSE(anError);
  std::filesystem::create_symlink(anOutsidePath, aCorePath, anError);
  if (anError)
  {
    std::filesystem::remove_all(aPath);
    std::filesystem::remove(anOutsidePath);
    GTEST_SKIP() << "Symbolic links are unavailable: " << anError.message();
  }
  ASSERT_TRUE(std::filesystem::is_symlink(std::filesystem::symlink_status(aCorePath)));

  const BRepGraphODE_RevisionPackage::Result aRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(aPath));
  ASSERT_EQ(aRead.StatusCode, BRepGraphODE_RevisionPackage::Status::Incomplete)
    << (aRead.Diagnostics.IsEmpty() ? "" : aRead.Diagnostics.First().Code.ToCString());
  EXPECT_TRUE(aRead.Revision.IsNull());
  std::filesystem::remove_all(aPath);
  std::filesystem::remove(anOutsidePath);
}

TEST(BRepGraph_RevisionTest, PackageDoesNotPersistRuntimeFingerprints)
{
  const std::filesystem::path           aPath     = makePackageTestPath("runtime-fingerprint");
  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::Empty();
  ASSERT_FALSE(aRevision.IsNull());
  ASSERT_TRUE(BRepGraphODE_RevisionPackage::Write(*aRevision, asOSDPath(aPath)).IsOk());

  const std::filesystem::path aManifestPath = aPath / "revision.info";
  std::ifstream               aInput(aManifestPath);
  ASSERT_TRUE(aInput);
  std::string aManifest;
  std::string aLine;
  while (std::getline(aInput, aLine))
  {
    aManifest += aLine;
    aManifest += '\n';
  }
  EXPECT_EQ(aManifest.find("\nsemantic "), std::string::npos);
  EXPECT_EQ(aManifest.find("\nstorage "), std::string::npos);
  EXPECT_EQ(aManifest.find(aRevision->SemanticHash().ToString().ToCString()), std::string::npos);
  EXPECT_EQ(aManifest.find(aRevision->StorageRootHash().ToString().ToCString()), std::string::npos);
  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, PackagedTombstoneRevisionRetainsSparseEditability)
{
  BRepGraph                aGraph;
  const BRepGraph_VertexId aVertex = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  ASSERT_TRUE(aVertex.IsValid());
  aGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aVertex));
  aGraph.Editor().Gen().CleanupRemovedReferences();

  const occ::handle<BRepGraph_Revision> aRevision = BRepGraph_Revision::FromGraph(aGraph);
  ASSERT_FALSE(aRevision.IsNull());
  ASSERT_TRUE(aRevision->SupportsSparseEdits());

  const std::filesystem::path aPath = makePackageTestPath("tombstone");
  ASSERT_TRUE(BRepGraphODE_RevisionPackage::Write(*aRevision, asOSDPath(aPath)).IsOk());
  const BRepGraphODE_RevisionPackage::Result aRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(aPath));
  ASSERT_TRUE(aRead.IsOk()) << aRead.Diagnostics.Size();
  ASSERT_FALSE(aRead.Revision.IsNull());
  EXPECT_TRUE(aRead.Revision->SupportsSparseEdits());
  EXPECT_TRUE(aRead.Revision->BeginTransaction().IsValid());
  std::filesystem::remove_all(aPath);
}

TEST(BRepGraph_RevisionTest, TopologyOnlyWireFaceEditPreservesOrderedMembership)
{
  BRepGraph_Transaction aVertexEdit = BRepGraph_Revision::Empty()->BeginTransaction();
  ASSERT_TRUE(aVertexEdit.IsValid());
  const BRepGraph_VertexId aVertex0 = aVertexEdit.AddVertex(gp_Pnt(0.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aVertex1 = aVertexEdit.AddVertex(gp_Pnt(1.0, 0.0, 0.0), 1.e-7);
  const BRepGraph_VertexId aVertex2 = aVertexEdit.AddVertex(gp_Pnt(0.0, 1.0, 0.0), 1.e-7);
  ASSERT_TRUE(aVertex0.IsValid() && aVertex1.IsValid() && aVertex2.IsValid());
  const BRepGraph_Transaction::CommitResult aVertexCommit = aVertexEdit.Commit();
  ASSERT_TRUE(aVertexCommit.IsOk());

  const BRepGraph_UID   aVertexUID0 = aVertexCommit.Revision->Graph().UIDs().Of(aVertex0);
  const BRepGraph_UID   aVertexUID1 = aVertexCommit.Revision->Graph().UIDs().Of(aVertex1);
  const BRepGraph_UID   aVertexUID2 = aVertexCommit.Revision->Graph().UIDs().Of(aVertex2);
  BRepGraph_Transaction anEdgeEdit  = aVertexCommit.Revision->BeginTransaction();
  ASSERT_TRUE(anEdgeEdit.IsValid());
  const occ::handle<Geom_Curve> aCurve =
    new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraph_EdgeId anEdge0 =
    anEdgeEdit.AddEdge(aVertexUID0, aVertexUID1, aCurve, 0.0, 1.0, 1.e-7);
  const BRepGraph_EdgeId anEdge1 =
    anEdgeEdit.AddEdge(aVertexUID1, aVertexUID2, aCurve, 0.0, 1.0, 1.e-7);
  const BRepGraph_EdgeId anEdge2 =
    anEdgeEdit.AddEdge(aVertexUID2, aVertexUID0, aCurve, 0.0, 1.0, 1.e-7);
  ASSERT_TRUE(anEdge0.IsValid() && anEdge1.IsValid() && anEdge2.IsValid());
  const BRepGraph_Transaction::CommitResult anEdgeCommit = anEdgeEdit.Commit();
  ASSERT_TRUE(anEdgeCommit.IsOk());

  BRepGraph_Transaction aCoEdgeEdit = anEdgeCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aCoEdgeEdit.IsValid());
  const BRepGraph_UID      anEdgeUID0 = anEdgeCommit.Revision->Graph().UIDs().Of(anEdge0);
  const BRepGraph_UID      anEdgeUID1 = anEdgeCommit.Revision->Graph().UIDs().Of(anEdge1);
  const BRepGraph_UID      anEdgeUID2 = anEdgeCommit.Revision->Graph().UIDs().Of(anEdge2);
  const BRepGraph_CoEdgeId aCoEdge0   = aCoEdgeEdit.AddCoEdge(anEdgeUID0, TopAbs_FORWARD);
  const BRepGraph_CoEdgeId aCoEdge1   = aCoEdgeEdit.AddCoEdge(anEdgeUID1, TopAbs_FORWARD);
  const BRepGraph_CoEdgeId aCoEdge2   = aCoEdgeEdit.AddCoEdge(anEdgeUID2, TopAbs_FORWARD);
  ASSERT_TRUE(aCoEdge0.IsValid() && aCoEdge1.IsValid() && aCoEdge2.IsValid());
  const BRepGraph_Transaction::CommitResult aCoEdgeCommit = aCoEdgeEdit.Commit();
  ASSERT_TRUE(aCoEdgeCommit.IsOk());

  NCollection_LinearVector<BRepGraph_UID> aCoEdgeUIDs;
  aCoEdgeUIDs.Append(aCoEdgeCommit.Revision->Graph().UIDs().Of(aCoEdge0));
  aCoEdgeUIDs.Append(aCoEdgeCommit.Revision->Graph().UIDs().Of(aCoEdge1));
  aCoEdgeUIDs.Append(aCoEdgeCommit.Revision->Graph().UIDs().Of(aCoEdge2));
  BRepGraph_Transaction aTopologyEdit = aCoEdgeCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aTopologyEdit.IsValid());
  const BRepGraph_WireId aWire = aTopologyEdit.AddWire(aCoEdgeUIDs);
  ASSERT_TRUE(aWire.IsValid());
  BRepGraph_UID aWireUID;
  ASSERT_TRUE(aTopologyEdit.WireUID(aWire, aWireUID));
  NCollection_LinearVector<BRepGraph_UID> anEmptyInnerWires;
  const BRepGraph_FaceId aFace = aTopologyEdit.AddFace(aWireUID, anEmptyInnerWires, 2.e-7);
  ASSERT_TRUE(aFace.IsValid());

  const BRepGraph_Transaction::CommitResult aTopologyCommit = aTopologyEdit.Commit();
  ASSERT_TRUE(aTopologyCommit.IsOk())
    << (aTopologyCommit.Diagnostics.IsEmpty()
          ? ""
          : aTopologyCommit.Diagnostics.First().Message.ToCString());
  expectFreshCoreHashes(aTopologyCommit.Revision);
  ASSERT_TRUE(aTopologyCommit.Revision->SupportsSparseEdits());
  EXPECT_EQ(aTopologyCommit.Revision->NbVisibleWires(), 1u);
  EXPECT_EQ(aTopologyCommit.Revision->NbVisibleFaces(), 1u);
  EXPECT_EQ(aTopologyCommit.Revision->NbVisibleWireRefs(), 1u);
  EXPECT_TRUE(aTopologyCommit.Revision->Graph().ValidateRelations());

  const BRepGraph_UID aFaceUID                = aTopologyCommit.Revision->Graph().UIDs().Of(aFace);
  const occ::handle<BRepGraph_Revision> aRead = aTopologyCommit.Revision;
  BRepGraph_Revision::WireChange        aWireChange;
  ASSERT_TRUE(aRead->ReadWire(aWireUID, aWireChange));
  ASSERT_EQ(aWireChange.CoEdgeUIDs.Size(), aCoEdgeUIDs.Size());
  for (size_t anIndex = 0; anIndex < aCoEdgeUIDs.Size(); ++anIndex)
  {
    EXPECT_EQ(aWireChange.CoEdgeUIDs.Value(anIndex), aCoEdgeUIDs.Value(anIndex));
  }
  BRepGraph_Revision::FaceChange aFaceChange;
  ASSERT_TRUE(aRead->ReadFace(aFaceUID, aFaceChange));
  ASSERT_EQ(aFaceChange.WireRefUIDs.Size(), 1u);
  BRepGraph_Revision::WireRefChange aWireRefChange;
  ASSERT_TRUE(aRead->ReadWireRef(aFaceChange.WireRefUIDs.First(), aWireRefChange));
  EXPECT_EQ(aWireRefChange.Role, BRepGraph_Revision::BoundaryRole::Outer);
  EXPECT_EQ(aTopologyCommit.Revision->SemanticHash(),
            BRepGraph_RevisionHash::Hasher::Semantic(aTopologyCommit.Revision->Graph()));

  const BRepGraph_RevisionDiff aDiff = aCoEdgeCommit.Revision->Diff(*aTopologyCommit.Revision);
  EXPECT_NE(std::find(aDiff.CreatedUIDs.begin(), aDiff.CreatedUIDs.end(), aWireUID),
            aDiff.CreatedUIDs.end());
  EXPECT_NE(std::find(aDiff.CreatedUIDs.begin(), aDiff.CreatedUIDs.end(), aFaceUID),
            aDiff.CreatedUIDs.end());
  EXPECT_EQ(aDiff.CreatedRefUIDs.Size(), 1u);

  BRepGraph_Transaction aBoundCoEdgeRemoval = aTopologyCommit.Revision->BeginTransaction();
  ASSERT_TRUE(aBoundCoEdgeRemoval.IsValid());
  EXPECT_FALSE(aBoundCoEdgeRemoval.RemoveCoEdge(aCoEdgeUIDs.First()));
  aBoundCoEdgeRemoval.Abort();

  const std::filesystem::path           anPackagePath = makePackageTestPath("wire-face");
  BRepGraphODE_RevisionPackage::Options anPackageOptions;
  const BRepGraphODE_RevisionPackage::Result anPackageWrite =
    BRepGraphODE_RevisionPackage::Write(*aTopologyCommit.Revision,
                                        asOSDPath(anPackagePath),
                                        anPackageOptions);
  ASSERT_TRUE(anPackageWrite.IsOk())
    << (anPackageWrite.Diagnostics.IsEmpty()
          ? ""
          : anPackageWrite.Diagnostics.First().Message.ToCString());
  const BRepGraphODE_RevisionPackage::Result anPackageRead =
    BRepGraphODE_RevisionPackage::Read(asOSDPath(anPackagePath), anPackageOptions);
  ASSERT_TRUE(anPackageRead.IsOk()) << anPackageRead.Diagnostics.Size();
  EXPECT_EQ(anPackageRead.Revision->SemanticHash(), aTopologyCommit.Revision->SemanticHash());
  BRepGraph_Revision::FaceChange anPackagedFaceChange;
  EXPECT_TRUE(anPackageRead.Revision->ReadFace(aFaceUID, anPackagedFaceChange));

  BRepGraph_Transaction aRestoredEdit = anPackageRead.Revision->BeginTransaction();
  ASSERT_TRUE(aRestoredEdit.IsValid());
  ASSERT_TRUE(aRestoredEdit.AddVertex(gp_Pnt(2.0, 2.0, 0.0), 1.e-7).IsValid());
  const BRepGraph_Transaction::CommitResult aRestoredCommit = aRestoredEdit.Commit();
  ASSERT_TRUE(aRestoredCommit.IsOk());
  BRepGraph_Revision::FaceChange aRestoredFaceChange;
  EXPECT_TRUE(aRestoredCommit.Revision->ReadFace(aFaceUID, aRestoredFaceChange));
  std::filesystem::remove_all(anPackagePath);
}
