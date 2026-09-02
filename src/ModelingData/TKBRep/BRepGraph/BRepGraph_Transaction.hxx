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

#ifndef _BRepGraph_Transaction_HeaderFile
#define _BRepGraph_Transaction_HeaderFile

#include <BRepGraph_RevisionDiff.hxx>
#include <BRepGraph_RevisionStatus.hxx>
#include <BRepGraph_Revision.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_FlatMap.hxx>
#include <gp_Pnt.hxx>

#include <cstdint>
#include <memory>

//! Isolated transaction over one immutable BRepGraph revision.
//!
//! Sparse durable-identity operations avoid copying unchanged core pages for the
//! supported topology subset. Graph() opens an isolated complete graph editor for
//! all other operations. The two edit surfaces cannot be mixed within one transaction.
class BRepGraph_Transaction
{
public:
  //! A contiguous durable-identity allocation range reported by an edit.
  struct UIDRange
  {
    bool                   IsReference = false;
    BRepGraph_NodeId::Kind NodeKind    = BRepGraph_NodeId::Kind::Solid;
    BRepGraph_RefId::Kind  RefKind     = BRepGraph_RefId::Kind::Shell;
    uint32_t               First       = 0;
    uint32_t               Last        = 0;

    [[nodiscard]] bool IsValid() const noexcept { return First != 0 && Last >= First; }
  };

  //! Result of committing the transaction as a new immutable revision.
  struct CommitResult
  {
    BRepGraph_RevisionStatus::Code        Status = BRepGraph_RevisionStatus::Code::InvalidInput;
    occ::handle<BRepGraph_Revision>       Revision;
    BRepGraph_RevisionDiff                Diff;
    NCollection_LinearVector<UIDRange>    AllocatedUIDRanges;
    BRepGraph_RevisionStatus::Diagnostics Diagnostics;

    [[nodiscard]] bool IsOk() const noexcept
    {
      return BRepGraph_RevisionStatus::IsSuccess(Status) && !Revision.IsNull();
    }
  };

  BRepGraph_Transaction() = default;
  Standard_EXPORT ~BRepGraph_Transaction();

  BRepGraph_Transaction(const BRepGraph_Transaction&)            = delete;
  BRepGraph_Transaction& operator=(const BRepGraph_Transaction&) = delete;
  BRepGraph_Transaction(BRepGraph_Transaction&& theOther) noexcept;
  BRepGraph_Transaction& operator=(BRepGraph_Transaction&& theOther) noexcept;

  //! Begin a transaction over a valid immutable revision.
  [[nodiscard]] Standard_EXPORT static BRepGraph_Transaction Begin(
    const occ::handle<BRepGraph_Revision>& theBaseRevision);

  [[nodiscard]] bool IsValid() const noexcept
  {
    return !myBaseRevision.IsNull() && !myFinished
           && myStatus == BRepGraph_RevisionStatus::Code::Ok;
  }

  [[nodiscard]] bool IsFinished() const noexcept { return myFinished; }

  [[nodiscard]] BRepGraph_RevisionStatus::Code Status() const noexcept { return myStatus; }

  [[nodiscard]] const BRepGraph_RevisionStatus::Diagnostics& Diagnostics() const noexcept
  {
    return myDiagnostics;
  }

  //! Return the immutable base revision retained by this transaction.
  [[nodiscard]] const occ::handle<BRepGraph_Revision>& BaseRevision() const noexcept
  {
    return myBaseRevision;
  }

  //! Return the mutable working graph when complete-graph editing was requested.
  //! The pointer remains owned by this transaction and becomes invalid on
  //! Commit(), Abort(), move, or destruction.
  [[nodiscard]] const BRepGraph* Graph() const noexcept { return myWorkingGraph.get(); }

  //! Create or return the isolated mutable working graph.
  //!
  //! This path supports the complete BRepGraph API, including topology above
  //! faces, products, occurrences, representations, layers, and supplemental
  //! stores. It returns null if sparse changes were already recorded or if the
  //! revision cannot be copied.
  [[nodiscard]] Standard_EXPORT BRepGraph* Graph();

  //! Return the currently selected immutable component by stable GUID.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_RevisionComponent> Component(
    const Standard_GUID& theGUID) const;

  //! Replace one existing persistent component in a sparse transaction.
  [[nodiscard]] Standard_EXPORT bool ReplaceComponent(
    const occ::handle<BRepGraph_RevisionComponent>& theComponent);

  //! Append a standalone vertex definition to the private edit.
  //! @return a graph-local ID valid in the resulting revision, or invalid on failure
  [[nodiscard]] Standard_EXPORT BRepGraph_VertexId AddVertex(const gp_Pnt& thePoint,
                                                             const double  theTolerance);

  //! Append an edge definition between two visible durable vertices.
  [[nodiscard]] Standard_EXPORT BRepGraph_EdgeId AddEdge(const BRepGraph_UID& theStartVertexUID,
                                                         const BRepGraph_UID& theEndVertexUID,
                                                         const occ::handle<Geom_Curve>& theCurve,
                                                         const double theParamFirst,
                                                         const double theParamLast,
                                                         const double theTolerance);

  //! Append a free coedge usage for an existing durable edge.
  [[nodiscard]] Standard_EXPORT BRepGraph_CoEdgeId
    AddCoEdge(const BRepGraph_UID&                  theEdgeUID,
              const BRepGraphInc::ParityOrientation theOrientation);

  //! Append a wire definition and bind an ordered sequence of visible coedges.
  [[nodiscard]] Standard_EXPORT BRepGraph_WireId
    AddWire(const NCollection_LinearVector<BRepGraph_UID>& theCoEdgeUIDs);

  //! Append a topology-only face with one outer wire and optional inner wires.
  [[nodiscard]] Standard_EXPORT BRepGraph_FaceId
    AddFace(const BRepGraph_UID&                           theOuterWireUID,
            const NCollection_LinearVector<BRepGraph_UID>& theInnerWireUIDs,
            const double                                   theTolerance);

  //! Replace the point of an existing or newly created vertex by durable UID.
  [[nodiscard]] Standard_EXPORT bool SetVertexPoint(const BRepGraph_UID& theUID,
                                                    const gp_Pnt&        thePoint);

  //! Replace the tolerance of an existing or newly created vertex by durable UID.
  [[nodiscard]] Standard_EXPORT bool SetVertexTolerance(const BRepGraph_UID& theUID,
                                                        const double         theTolerance);

  //! Record removal of an existing vertex.
  [[nodiscard]] Standard_EXPORT bool RemoveVertex(const BRepGraph_UID& theUID);

  //! Replace the tolerance of an existing or newly created edge by durable UID.
  [[nodiscard]] Standard_EXPORT bool SetEdgeTolerance(const BRepGraph_UID& theUID,
                                                      const double         theTolerance);

  //! Replace the 3D curve and parameter range of an existing or newly created edge.
  [[nodiscard]] Standard_EXPORT bool SetEdgeCurve(const BRepGraph_UID&           theUID,
                                                  const occ::handle<Geom_Curve>& theCurve,
                                                  const double                   theParamFirst,
                                                  const double                   theParamLast);

  //! Record removal of an existing edge and its endpoint references.
  [[nodiscard]] Standard_EXPORT bool RemoveEdge(const BRepGraph_UID& theUID);

  //! Replace the orientation of an existing or newly created free coedge.
  [[nodiscard]] Standard_EXPORT bool SetCoEdgeOrientation(
    const BRepGraph_UID&                  theUID,
    const BRepGraphInc::ParityOrientation theOrientation);

  //! Record removal of an existing free coedge.
  [[nodiscard]] Standard_EXPORT bool RemoveCoEdge(const BRepGraph_UID& theUID);

  //! Replace the ordered coedge membership of an existing wire.
  [[nodiscard]] Standard_EXPORT bool SetWireCoEdgeOrder(
    const BRepGraph_UID&                           theUID,
    const NCollection_LinearVector<BRepGraph_UID>& theCoEdgeUIDs);

  //! Replace the ordered wire-reference membership of an existing face.
  //! The first reference remains the explicit outer boundary.
  [[nodiscard]] Standard_EXPORT bool SetFaceWireOrder(
    const BRepGraph_UID&                              theUID,
    const NCollection_LinearVector<BRepGraph_RefUID>& theWireRefUIDs);

  //! Replace the orientation of an existing face-to-wire usage.
  [[nodiscard]] Standard_EXPORT bool SetWireRefOrientation(
    const BRepGraph_RefUID&               theUID,
    const BRepGraphInc::ParityOrientation theOrientation);

  //! Remove an existing face-to-wire usage while retaining its definitions.
  [[nodiscard]] Standard_EXPORT bool RemoveWireRef(const BRepGraph_RefUID& theUID);

  //! Record removal of an existing wire with no active usages.
  [[nodiscard]] Standard_EXPORT bool RemoveWire(const BRepGraph_UID& theUID);

  //! Record removal of an existing face with no active usages.
  [[nodiscard]] Standard_EXPORT bool RemoveFace(const BRepGraph_UID& theUID);

  //! Raise the private vertex UID watermark. It never moves backwards.
  [[nodiscard]] Standard_EXPORT bool RaiseVertexUIDWatermark(const uint32_t theNextCounter);

  //! Return the current durable definition for an edited or base vertex.
  [[nodiscard]] Standard_EXPORT bool Vertex(const BRepGraph_UID&     theUID,
                                            BRepGraphInc::VertexDef& theDefinition) const;

  //! Return the current durable definition for an edited or base edge.
  [[nodiscard]] Standard_EXPORT bool Edge(const BRepGraph_UID&   theUID,
                                          BRepGraphInc::EdgeDef& theDefinition) const;

  //! Return the current durable definition and ordered coedge UIDs of a wire.
  [[nodiscard]] Standard_EXPORT bool Wire(const BRepGraph_UID&            theUID,
                                          BRepGraph_Revision::WireChange& theChange) const;

  //! Resolve a wire-local ID allocated by this edit to its durable UID.
  [[nodiscard]] Standard_EXPORT bool WireUID(const BRepGraph_WireId theLocalId,
                                             BRepGraph_UID&         theUID) const;

  //! Return the current durable definition and ordered wire-reference UIDs of a face.
  [[nodiscard]] Standard_EXPORT bool Face(const BRepGraph_UID&            theUID,
                                          BRepGraph_Revision::FaceChange& theChange) const;

  //! Validate the working data and create a new immutable revision.
  [[nodiscard]] Standard_EXPORT CommitResult Commit();

  //! Discard all private data without affecting the base revision.
  Standard_EXPORT void Abort() noexcept;

private:
  friend class BRepGraph_Revision;

  explicit BRepGraph_Transaction(const occ::handle<BRepGraph_Revision>& theBaseRevision);

  //! Return whether this session can mutate the sparse native topology.
  [[nodiscard]] bool canEditTopology() const noexcept
  {
    return IsValid() && myWorkingGraph == nullptr && myBaseRevision->SupportsSparseEdits();
  }

  //! Resolve a UID through the base graph and this edit's private records.
  [[nodiscard]] bool resolveVertex(const BRepGraph_UID&     theUID,
                                   BRepGraphInc::VertexDef& theDefinition) const;

  //! Resolve the complete persistent record through the base and private changes.
  [[nodiscard]] bool resolveVertexChange(const BRepGraph_UID&              theUID,
                                         BRepGraph_Revision::VertexChange& theChange) const;

  //! Resolve the complete persistent edge record through the base and private changes.
  [[nodiscard]] bool resolveEdgeChange(const BRepGraph_UID&            theUID,
                                       BRepGraph_Revision::EdgeChange& theChange) const;

  //! Resolve an edge-owned vertex reference through the base and private changes.
  [[nodiscard]] bool resolveVertexRefChange(const BRepGraph_RefUID&              theUID,
                                            BRepGraph_Revision::VertexRefChange& theChange) const;

  //! Resolve a free-coedge usage through the base and private changes.
  [[nodiscard]] bool resolveCoEdgeChange(const BRepGraph_UID&              theUID,
                                         BRepGraph_Revision::CoEdgeChange& theChange) const;
  [[nodiscard]] bool resolveWireChange(const BRepGraph_UID&            theUID,
                                       BRepGraph_Revision::WireChange& theChange) const;
  [[nodiscard]] bool resolveFaceChange(const BRepGraph_UID&            theUID,
                                       BRepGraph_Revision::FaceChange& theChange) const;
  [[nodiscard]] bool resolveWireRefChange(const BRepGraph_RefUID&            theUID,
                                          BRepGraph_Revision::WireRefChange& theChange) const;

  //! Find a pending record by durable UID.
  [[nodiscard]] BRepGraph_Revision::VertexChange*    changeOf(const BRepGraph_UID& theUID);
  [[nodiscard]] BRepGraph_Revision::EdgeChange*      edgeChangeOf(const BRepGraph_UID& theUID);
  [[nodiscard]] BRepGraph_Revision::VertexRefChange* vertexRefChangeOf(
    const BRepGraph_RefUID& theUID);
  [[nodiscard]] BRepGraph_Revision::CoEdgeChange*  coEdgeChangeOf(const BRepGraph_UID& theUID);
  [[nodiscard]] BRepGraph_Revision::WireChange*    wireChangeOf(const BRepGraph_UID& theUID);
  [[nodiscard]] BRepGraph_Revision::FaceChange*    faceChangeOf(const BRepGraph_UID& theUID);
  [[nodiscard]] BRepGraph_Revision::WireRefChange* wireRefChangeOf(const BRepGraph_RefUID& theUID);

  //! Append or replace one private record.
  void recordChange(const BRepGraph_Revision::VertexChange& theChange);
  void recordEdgeChange(const BRepGraph_Revision::EdgeChange& theChange);
  void recordVertexRefChange(const BRepGraph_Revision::VertexRefChange& theChange);
  void recordCoEdgeChange(const BRepGraph_Revision::CoEdgeChange& theChange);
  void recordWireChange(const BRepGraph_Revision::WireChange& theChange);
  void recordFaceChange(const BRepGraph_Revision::FaceChange& theChange);
  void recordWireRefChange(const BRepGraph_Revision::WireRefChange& theChange);

  void recordAllocation(const UIDRange& theRange);
  void clearChangeIndices() noexcept;
  void clearAllocations() noexcept;
  void finish() noexcept;

  //! Build the directional result directly from the authoritative sparse records.
  [[nodiscard]] BRepGraph_RevisionDiff sparseDiff(
    const BRepGraph_Revision& theResultRevision) const;

  [[nodiscard]] bool hasSparseChanges() const noexcept;

  occ::handle<BRepGraph_Revision>                                    myBaseRevision;
  std::unique_ptr<BRepGraph>                                         myWorkingGraph;
  NCollection_LinearVector<BRepGraph_Revision::VertexChange>         myChanges;
  NCollection_LinearVector<BRepGraph_Revision::EdgeChange>           myEdgeChanges;
  NCollection_LinearVector<BRepGraph_Revision::VertexRefChange>      myVertexRefChanges;
  NCollection_LinearVector<BRepGraph_Revision::CoEdgeChange>         myCoEdgeChanges;
  NCollection_LinearVector<BRepGraph_Revision::WireChange>           myWireChanges;
  NCollection_LinearVector<BRepGraph_Revision::FaceChange>           myFaceChanges;
  NCollection_LinearVector<BRepGraph_Revision::WireRefChange>        myWireRefChanges;
  NCollection_FlatDataMap<BRepGraph_UID, uint32_t>                   myVertexChangeIndices;
  NCollection_FlatDataMap<BRepGraph_UID, uint32_t>                   myEdgeChangeIndices;
  NCollection_FlatDataMap<BRepGraph_RefUID, uint32_t>                myVertexRefChangeIndices;
  NCollection_FlatDataMap<BRepGraph_UID, uint32_t>                   myCoEdgeChangeIndices;
  NCollection_FlatDataMap<BRepGraph_UID, uint32_t>                   myWireChangeIndices;
  NCollection_FlatDataMap<BRepGraph_UID, uint32_t>                   myFaceChangeIndices;
  NCollection_FlatDataMap<BRepGraph_RefUID, uint32_t>                myWireRefChangeIndices;
  NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>> myComponents;
  NCollection_FlatDataMap<Standard_GUID, uint32_t>                    myComponentIndices;
  NCollection_FlatMap<Standard_GUID>                                 myChangedComponentGUIDs;
  NCollection_LinearVector<UIDRange>                                 myAllocatedRanges;
  BRepGraph_RevisionStatus::Diagnostics                              myDiagnostics;
  BRepGraph_RevisionStatus::Code myStatus            = BRepGraph_RevisionStatus::Code::InvalidInput;
  uint32_t                       myNextVertexCounter = 1;
  uint32_t                       myNextEdgeCounter   = 1;
  uint32_t                       myNextVertexRefCounter = 1;
  uint32_t                       myNextCoEdgeCounter    = 1;
  uint32_t                       myNextWireCounter      = 1;
  uint32_t                       myNextFaceCounter      = 1;
  uint32_t                       myNextWireRefCounter   = 1;
  uint32_t                       myNextVertexIndex      = 0;
  uint32_t                       myNextEdgeIndex        = 0;
  uint32_t                       myNextVertexRefIndex   = 0;
  uint32_t                       myNextCoEdgeIndex      = 0;
  uint32_t                       myNextWireIndex        = 0;
  uint32_t                       myNextFaceIndex        = 0;
  uint32_t                       myNextWireRefIndex     = 0;
  bool                           myFinished             = true;
  bool                           myComponentsChanged    = false;
};

#endif // _BRepGraph_Transaction_HeaderFile
