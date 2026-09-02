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

#ifndef _BRepGraph_Revision_HeaderFile
#define _BRepGraph_Revision_HeaderFile

#include <BRepGraph_RevisionDiff.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RevisionComponent.hxx>
#include <BRepGraph_RevisionStatus.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_RevisionHash.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_ForwardRange.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>

#include <cstdint>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

class BRepGraph;
class BRepGraphODE_RevisionPackage;
class BRepGraph_Transaction;

//! Complete immutable value of a BRepGraph model.
//!
//! Each revision retains a page-sharing copy of BRepGraphInc_Storage. Revisions are
//! isolated by page-level copy-on-write and detached mutable representations. A
//! transaction may either collect sparse durable-identity changes or edit an
//! isolated complete graph before committing a new revision.
//!
//! Revision identifies model content, not a sequence number or history node.
//! Relationships between revisions are established by their owner or by an
//! explicit diff; this class does not imply ancestry. SchemaVersion() describes
//! only the persistent representation accepted by revision serialization.
class BRepGraph_Revision : public Standard_Transient
{
public:
  DEFINE_STANDARD_ALLOC

  //! First pre-release revision schema.
  //!
  //! This is a persistent contract version, not an implementation iteration
  //! counter. Keep it at 1 until an incompatible stored schema requires an
  //! explicit migration or version increment.
  static constexpr uint32_t THE_SCHEMA_VERSION = 1;

  //! Revision construction options.
  struct Options
  {
    uint32_t SchemaVersion;
    bool     ValidateSource;

    Options()
        : SchemaVersion(THE_SCHEMA_VERSION),
          ValidateSource(true)
    {
    }
  };

  //! A durable vertex record created, modified, or removed by a private edit.
  struct VertexChange
  {
    //! Operation represented by the record.
    enum class Operation
    {
      Create,
      Modify,
      Remove
    };

    Operation               Kind = Operation::Modify;
    BRepGraph_UID           UID;
    BRepGraph_VertexId      LocalId;
    BRepGraphInc::VertexDef Definition;
  };

  //! A durable edge-definition record created, modified, or removed by a private edit.
  struct EdgeChange
  {
    VertexChange::Operation Kind = VertexChange::Operation::Modify;
    BRepGraph_UID           UID;
    BRepGraph_EdgeId        LocalId;
    BRepGraphInc::EdgeDef   Definition;
    occ::handle<Geom_Curve> Curve;
    double                  CurveParamFirst = 0.0;
    double                  CurveParamLast  = 0.0;
    //! True when a modification replaces the edge's curve representation.
    bool HasCurveUpdate = false;
  };

  //! Read-only edge record returned during revision traversal.
  //! The curve pointer remains valid until the iterator advances and must not be retained
  //! beyond the lifetime of the revision.
  struct EdgeView
  {
    VertexChange::Operation Kind = VertexChange::Operation::Modify;
    BRepGraph_UID           UID;
    BRepGraph_EdgeId        LocalId;
    BRepGraphInc::EdgeDef   Definition;
    const Geom_Curve*       Curve           = nullptr;
    double                  CurveParamFirst = 0.0;
    double                  CurveParamLast  = 0.0;
  };

  //! A durable free-coedge usage record created, modified, or removed by a private edit.
  struct CoEdgeChange
  {
    VertexChange::Operation Kind = VertexChange::Operation::Modify;
    BRepGraph_UID           UID;
    BRepGraph_CoEdgeId      LocalId;
    BRepGraphInc::CoEdgeDef Definition;
  };

  //! Role of one ordered wire boundary use on a face.
  enum class BoundaryRole
  {
    Outer,
    Inner
  };

  //! A durable wire-definition record created, modified, or removed by a private edit.
  struct WireChange
  {
    VertexChange::Operation                 Kind = VertexChange::Operation::Modify;
    BRepGraph_UID                           UID;
    BRepGraph_WireId                        LocalId;
    BRepGraphInc::WireDef                   Definition;
    NCollection_LinearVector<BRepGraph_UID> CoEdgeUIDs;
  };

  //! A durable face-definition record created, modified, or removed by a private edit.
  struct FaceChange
  {
    VertexChange::Operation                    Kind = VertexChange::Operation::Modify;
    BRepGraph_UID                              UID;
    BRepGraph_FaceId                           LocalId;
    BRepGraphInc::FaceDef                      Definition;
    NCollection_LinearVector<BRepGraph_RefUID> WireRefUIDs;
  };

  //! A durable face-to-wire usage record changed by a private edit.
  struct WireRefChange
  {
    VertexChange::Operation Kind = VertexChange::Operation::Modify;
    BRepGraph_RefUID        UID;
    BRepGraph_WireRefId     LocalId;
    BRepGraphInc::WireRef   Definition;
    BoundaryRole            Role = BoundaryRole::Inner;
  };

  //! A durable edge-bound vertex-reference record changed by a private edit.
  struct VertexRefChange
  {
    VertexChange::Operation Kind = VertexChange::Operation::Modify;
    BRepGraph_RefUID        UID;
    BRepGraph_VertexRefId   LocalId;
    BRepGraphInc::VertexRef Definition;
  };

private:
  //! Iterator over visible records without building a temporary container.
  //! The revision must outlive the iterator.
  //! Mutable representations exposed by a record are detached from the immutable revision.
  template <typename TheChangeType>
  class VisibleIterator
  {
    using Resolver = bool (BRepGraph_Revision::*)(uint32_t, TheChangeType&) const;

  public:
    //! Return true while a visible record is available.
    [[nodiscard]] bool More() const noexcept { return myIndex < myCount; }

    //! Advance to the next visible record.
    void Next()
    {
      ++myIndex;
      advance();
    }

    //! Return the current resolved record.
    //! The reference remains valid until this iterator is advanced or destroyed.
    [[nodiscard]] const TheChangeType& Current() const noexcept { return myCurrent; }

    //! Return an STL-compatible iterator adapter for range-based traversal.
    NCollection_ForwardRangeIterator<VisibleIterator> begin()
    {
      return NCollection_ForwardRangeIterator<VisibleIterator>(this);
    }

    //! Return the sentinel terminating range-based traversal.
    NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

  private:
    friend class BRepGraph_Revision;

    VisibleIterator(const BRepGraph_Revision& theRevision,
                    const uint32_t            theCount,
                    const Resolver            theResolver)
        : myRevision(&theRevision),
          myCount(theCount),
          myResolver(theResolver)
    {
      advance();
    }

    void advance()
    {
      while (myIndex < myCount)
      {
        TheChangeType aChange;
        if ((myRevision->*myResolver)(myIndex, aChange) && aChange.UID.IsValid()
            && aChange.Kind != VertexChange::Operation::Remove)
        {
          myCurrent = std::move(aChange);
          return;
        }
        ++myIndex;
      }
    }

    const BRepGraph_Revision* myRevision = nullptr;
    uint32_t                  myIndex    = 0;
    uint32_t                  myCount    = 0;
    Resolver                  myResolver;
    TheChangeType             myCurrent;
  };

public:
  using VertexIterator    = VisibleIterator<VertexChange>;
  using VertexRefIterator = VisibleIterator<VertexRefChange>;
  using CoEdgeIterator    = VisibleIterator<CoEdgeChange>;
  using WireIterator      = VisibleIterator<WireChange>;
  using FaceIterator      = VisibleIterator<FaceChange>;
  using WireRefIterator   = VisibleIterator<WireRefChange>;

  //! Iterator over immutable edge views without copying curve geometry.
  class EdgeIterator
  {
  public:
    [[nodiscard]] bool More() const noexcept { return myIndex < myCount; }

    void Next()
    {
      ++myIndex;
      advance();
    }

    [[nodiscard]] const EdgeView& Current() const noexcept { return myCurrent; }

    NCollection_ForwardRangeIterator<EdgeIterator> begin()
    {
      return NCollection_ForwardRangeIterator<EdgeIterator>(this);
    }

    NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

  private:
    friend class BRepGraph_Revision;

    explicit EdgeIterator(const BRepGraph_Revision& theRevision)
        : myRevision(&theRevision),
          myCount(theRevision.EdgeCount())
    {
      advance();
    }

    void advance()
    {
      while (myIndex < myCount)
      {
        if (myRevision->ResolveEdgeLocal(myIndex, myResolved)
            && myResolved.Kind != VertexChange::Operation::Remove)
        {
          myCurrent.Kind            = myResolved.Kind;
          myCurrent.UID             = myResolved.UID;
          myCurrent.LocalId         = myResolved.LocalId;
          myCurrent.Definition      = myResolved.Definition;
          myCurrent.Curve           = myResolved.Curve.get();
          myCurrent.CurveParamFirst = myResolved.CurveParamFirst;
          myCurrent.CurveParamLast  = myResolved.CurveParamLast;
          return;
        }
        ++myIndex;
      }
      myResolved.Curve.Nullify();
      myCurrent.Curve = nullptr;
    }

    const BRepGraph_Revision* myRevision = nullptr;
    uint32_t                  myIndex    = 0;
    uint32_t                  myCount    = 0;
    EdgeChange                myResolved;
    EdgeView                  myCurrent;
  };

  //! Iterator over immutable extension revisions without exposing their container.
  //! The graph revision must outlive the iterator.
  class ComponentIterator
  {
  public:
    [[nodiscard]] bool More() const noexcept
    {
      return myComponents != nullptr && myIndex < myComponents->Size();
    }

    void Next() noexcept { ++myIndex; }

    //! Return the current component revision.
    //! The reference remains valid until the graph revision is destroyed.
    [[nodiscard]] const occ::handle<BRepGraph_RevisionComponent>& Current() const
    {
      return myComponents->Value(myIndex);
    }

    [[nodiscard]] const occ::handle<BRepGraph_RevisionComponent>& Value() const
    {
      return Current();
    }

    NCollection_ForwardRangeIterator<ComponentIterator> begin()
    {
      return NCollection_ForwardRangeIterator<ComponentIterator>(this);
    }

    NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

  private:
    friend class BRepGraph_Revision;

    explicit ComponentIterator(
      const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents)
        : myComponents(&theComponents)
    {
    }

    const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>* myComponents =
      nullptr;
    size_t myIndex = 0;
  };

  //! Result of constructing a revision from a mutable graph.
  struct CreateResult
  {
    BRepGraph_RevisionStatus::Code        Status = BRepGraph_RevisionStatus::Code::InvalidInput;
    occ::handle<BRepGraph_Revision>       Revision;
    BRepGraph_RevisionStatus::Diagnostics Diagnostics;

    [[nodiscard]] bool IsOk() const noexcept
    {
      return BRepGraph_RevisionStatus::IsSuccess(Status) && !Revision.IsNull();
    }
  };

  //! Create an immutable revision from a validated graph.
  [[nodiscard]] Standard_EXPORT static CreateResult Create(const BRepGraph& theGraph,
                                                           const Options&   theOptions = Options());

  //! Convenience factory returning null on a typed construction failure.
  [[nodiscard]] Standard_EXPORT static occ::handle<BRepGraph_Revision> FromGraph(
    const BRepGraph& theGraph);

  //! Create an immutable empty revision.
  [[nodiscard]] Standard_EXPORT static occ::handle<BRepGraph_Revision> Empty();

  //! Return a cached detached graph copy of this revision.
  [[nodiscard]] Standard_EXPORT const BRepGraph& Graph() const;

  //! Return a cached detached graph copy, or null when copying fails.
  [[nodiscard]] Standard_EXPORT const BRepGraph* TryGraph() const;

  //! Copy this revision to a mutable page-shared graph.
  //! The destination is replaced only after all persistent components have
  //! restored successfully. Core pages remain shared until changed;
  //! independently mutable geometry, mesh, and TopoDS representations are detached.
  //! @param[out] theGraph graph receiving the complete revision
  //! @return true when the revision was copied successfully
  [[nodiscard]] Standard_EXPORT bool CopyTo(BRepGraph& theGraph) const;

  //! Begin a transaction. Sparse convenience operations are available when
  //! SupportsSparseEdits() is true; Transaction::Graph() supports every valid revision.
  [[nodiscard]] Standard_EXPORT BRepGraph_Transaction BeginTransaction() const;

  //! Compute a durable-keyed directional diff from this revision to another revision.
  //! @param[in] theResultRevision immutable comparison revision
  //! @return changes required to transform this revision into the result revision
  [[nodiscard]] Standard_EXPORT BRepGraph_RevisionDiff
    Diff(const BRepGraph_Revision& theResultRevision) const;

  //! True when this object owns a usable immutable revision.
  [[nodiscard]] bool IsValid() const noexcept { return myCoreGraph != nullptr; }

  //! True when the revision contains no active definitions.
  [[nodiscard]] Standard_EXPORT bool IsEmpty() const;

  //! True when optimized sparse topology operations are available.
  [[nodiscard]] bool SupportsSparseEdits() const noexcept { return mySupportsSparseEdits; }

  //! Number of visible vertex records in the persistent component.
  [[nodiscard]] Standard_EXPORT uint32_t NbVisibleVertices() const noexcept;

  //! Number of visible edge records in the persistent topology component.
  [[nodiscard]] Standard_EXPORT uint32_t NbVisibleEdges() const noexcept;

  //! Number of visible edge-bound vertex references.
  [[nodiscard]] Standard_EXPORT uint32_t NbVisibleVertexRefs() const noexcept;

  //! Number of visible free-coedge usage records.
  [[nodiscard]] Standard_EXPORT uint32_t NbVisibleCoEdges() const noexcept;

  //! Number of visible wire definitions.
  [[nodiscard]] Standard_EXPORT uint32_t NbVisibleWires() const noexcept;

  //! Number of visible face definitions.
  [[nodiscard]] Standard_EXPORT uint32_t NbVisibleFaces() const noexcept;

  //! Number of visible face-to-wire usage records.
  [[nodiscard]] Standard_EXPORT uint32_t NbVisibleWireRefs() const noexcept;

  //! Resolve one visible vertex record directly from the retained revision.
  [[nodiscard]] Standard_EXPORT bool ReadVertex(const BRepGraph_UID& theUID,
                                                VertexChange&        theChange) const;

  //! Resolve one visible edge record directly from the retained revision.
  [[nodiscard]] Standard_EXPORT bool ReadEdge(const BRepGraph_UID& theUID,
                                              EdgeChange&          theChange) const;

  //! Resolve one visible edge-bound vertex reference directly from the retained revision.
  [[nodiscard]] Standard_EXPORT bool ReadVertexRef(const BRepGraph_RefUID& theUID,
                                                   VertexRefChange&        theChange) const;

  //! Resolve one visible free-coedge usage directly from the retained revision.
  [[nodiscard]] Standard_EXPORT bool ReadCoEdge(const BRepGraph_UID& theUID,
                                                CoEdgeChange&        theChange) const;

  //! Resolve one visible wire definition directly from the retained revision.
  [[nodiscard]] Standard_EXPORT bool ReadWire(const BRepGraph_UID& theUID,
                                              WireChange&          theChange) const;

  //! Resolve one visible face definition directly from the retained revision.
  [[nodiscard]] Standard_EXPORT bool ReadFace(const BRepGraph_UID& theUID,
                                              FaceChange&          theChange) const;

  //! Resolve one visible face-to-wire usage directly from the retained revision.
  [[nodiscard]] Standard_EXPORT bool ReadWireRef(const BRepGraph_RefUID& theUID,
                                                 WireRefChange&          theChange) const;

  //! Iterate visible vertices in local-address order without a temporary container.
  [[nodiscard]] VertexIterator VisibleVertices() const
  {
    return VertexIterator(*this, VertexCount(), &BRepGraph_Revision::ResolveVertexLocal);
  }

  //! Iterate visible edges in local-address order without a temporary container.
  [[nodiscard]] EdgeIterator VisibleEdges() const { return EdgeIterator(*this); }

  //! Iterate visible edge-bound vertex references in local-address order.
  [[nodiscard]] VertexRefIterator VisibleVertexRefs() const
  {
    return VertexRefIterator(*this, VertexRefCount(), &BRepGraph_Revision::ResolveVertexRefLocal);
  }

  //! Iterate visible free coedges in local-address order.
  [[nodiscard]] CoEdgeIterator VisibleCoEdges() const
  {
    return CoEdgeIterator(*this, CoEdgeCount(), &BRepGraph_Revision::ResolveCoEdgeLocal);
  }

  //! Iterate visible wires in local-address order without a temporary container.
  [[nodiscard]] WireIterator VisibleWires() const
  {
    return WireIterator(*this, WireCount(), &BRepGraph_Revision::ResolveWireLocal);
  }

  //! Iterate visible faces in local-address order without a temporary container.
  [[nodiscard]] FaceIterator VisibleFaces() const
  {
    return FaceIterator(*this, FaceCount(), &BRepGraph_Revision::ResolveFaceLocal);
  }

  //! Iterate visible face-to-wire references in local-address order.
  [[nodiscard]] WireRefIterator VisibleWireRefs() const
  {
    return WireRefIterator(*this, WireRefCount(), &BRepGraph_Revision::ResolveWireRefLocal);
  }

  [[nodiscard]] uint32_t SchemaVersion() const noexcept { return mySchemaVersion; }

  //! Return the persistent identity of the retained graph.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& GraphGUID() const;

  [[nodiscard]] Standard_EXPORT const BRepGraph_RevisionHash& SemanticHash() const;

  [[nodiscard]] Standard_EXPORT const BRepGraph_RevisionHash& StorageRootHash() const;

  //! Iterate immutable revisions of persistent extensions without a temporary container.
  [[nodiscard]] ComponentIterator Components() const noexcept
  {
    return ComponentIterator(myComponents);
  }

  //! Return the number of persistent extension revisions.
  [[nodiscard]] size_t NbComponents() const noexcept { return myComponents.Size(); }

  //! Find an immutable persistent component by stable GUID.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_RevisionComponent> FindComponent(
    const Standard_GUID& theGUID) const;

  //! Return the semantic hash captured with a persistent component.
  //! The returned value is revision-owned and does not observe later mutation
  //! of an incorrectly implemented component object.
  [[nodiscard]] Standard_EXPORT BRepGraph_RevisionHash
    ComponentSemanticHash(const Standard_GUID& theGUID) const;

private:
  friend class BRepGraphODE_RevisionPackage;
  friend class BRepGraph_Transaction;

  struct HashState;

  struct ComponentState
  {
    BRepGraph_RevisionComponent::ComponentDescriptor Descriptor;
    BRepGraph_RevisionHash                           SemanticHash;
    BRepGraph_RevisionHash                           StorageHash;
  };

  BRepGraph_Revision(
    std::shared_ptr<const BRepGraph> theGraph,
    const uint32_t                   theSchemaVersion,
    const bool                       theSupportsSparseEdits,
    const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents = {},
    std::shared_ptr<const HashState>                                          theHashState  = {});

  [[nodiscard]] const HashState&                 ensureHashState() const;
  [[nodiscard]] std::shared_ptr<const HashState> hashStateIfAvailable() const noexcept;

  //! Apply sparse core changes and return a new immutable revision.
  [[nodiscard]] static occ::handle<BRepGraph_Revision> FromNativeChanges(
    const occ::handle<BRepGraph_Revision>&           theBaseRevision,
    const NCollection_LinearVector<VertexChange>&    theChanges,
    const NCollection_LinearVector<EdgeChange>&      theEdgeChanges,
    const NCollection_LinearVector<VertexRefChange>& theVertexRefChanges,
    const NCollection_LinearVector<CoEdgeChange>&    theCoEdgeChanges,
    const NCollection_LinearVector<WireChange>&      theWireChanges,
    const NCollection_LinearVector<FaceChange>&      theFaceChanges,
    const NCollection_LinearVector<WireRefChange>&   theWireRefChanges,
    const uint32_t                                   theNextVertexCounter,
    const uint32_t                                   theNextEdgeCounter,
    const uint32_t                                   theNextVertexRefCounter,
    const uint32_t                                   theNextCoEdgeCounter,
    const uint32_t                                   theNextWireCounter,
    const uint32_t                                   theNextFaceCounter,
    const uint32_t                                   theNextWireRefCounter,
    const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
    BRepGraph_RevisionStatus::Diagnostics&                                    theDiagnostics);

  //! Construct a revision that shares unchanged core data and replaces only components.
  [[nodiscard]] static occ::handle<BRepGraph_Revision> FromComponents(
    const occ::handle<BRepGraph_Revision>&                                    theBaseRevision,
    const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
    BRepGraph_RevisionStatus::Diagnostics&                                    theDiagnostics);

  //! Construct a revision from an isolated core graph.
  [[nodiscard]] static occ::handle<BRepGraph_Revision> FromCoreGraph(
    std::shared_ptr<const BRepGraph> theGraph,
    const uint32_t                   theSchemaVersion);

  [[nodiscard]] static CreateResult FromTransactionGraph(std::unique_ptr<BRepGraph> theGraph,
                                                         const uint32_t theSchemaVersion);

  //! Construct directly from a decoded core and decoded immutable components.
  //! This trusted ODE boundary never restores components into a mutable graph.
  [[nodiscard]] static occ::handle<BRepGraph_Revision> FromDecodedCore(
    std::shared_ptr<const BRepGraph>                                          theGraph,
    const uint32_t                                                            theSchemaVersion,
    const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
    BRepGraph_RevisionStatus::Diagnostics&                                    theDiagnostics);

  //! Reject extension revision that cannot be stored exactly.
  [[nodiscard]] Standard_EXPORT static bool ValidateComponents(
    const BRepGraph&                       theGraph,
    BRepGraph_RevisionStatus::Diagnostics& theDiagnostics);

  //! Capture and validate immutable revision for all persistent extensions.
  [[nodiscard]] static bool CaptureComponents(
    const BRepGraph&                                                    theGraph,
    NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
    BRepGraph_RevisionStatus::Diagnostics&                              theDiagnostics);

  //! Copy the core graph and restore persistent components into the copy.
  [[nodiscard]] std::unique_ptr<BRepGraph> copyGraph() const;

  //! Replay one set of sparse core changes into a private native graph fork.
  [[nodiscard]] static bool ApplyCoreChanges(
    BRepGraph&                                       theGraph,
    const NCollection_LinearVector<VertexChange>&    theChanges,
    const NCollection_LinearVector<EdgeChange>&      theEdgeChanges,
    const NCollection_LinearVector<VertexRefChange>& theVertexRefChanges,
    const NCollection_LinearVector<CoEdgeChange>&    theCoEdgeChanges,
    const NCollection_LinearVector<WireChange>&      theWireChanges,
    const NCollection_LinearVector<FaceChange>&      theFaceChanges,
    const NCollection_LinearVector<WireRefChange>&   theWireRefChanges,
    const uint32_t                                   theNextVertexCounter,
    const uint32_t                                   theNextEdgeCounter,
    const uint32_t                                   theNextVertexRefCounter,
    const uint32_t                                   theNextCoEdgeCounter,
    const uint32_t                                   theNextWireCounter,
    const uint32_t                                   theNextFaceCounter,
    const uint32_t                                   theNextWireRefCounter);

  //! Validate the retained core graph relations.
  [[nodiscard]] bool ValidateRelations() const;

  //! Return the retained core graph for trusted internal consumers.
  [[nodiscard]] const BRepGraph& CoreGraph() const;

  //! Return the retained core graph, or null when this revision is invalid.
  [[nodiscard]] const BRepGraph* TryCoreGraph() const;

  //! Resolve one persistent vertex record from the retained core graph.
  [[nodiscard]] bool ResolveVertex(const BRepGraph_UID& theUID, VertexChange& theChange) const;
  [[nodiscard]] bool ResolveVertexLocal(const uint32_t theIndex, VertexChange& theChange) const;
  [[nodiscard]] bool ResolveEdge(const BRepGraph_UID& theUID, EdgeChange& theChange) const;
  [[nodiscard]] bool ResolveEdgeLocal(const uint32_t theIndex, EdgeChange& theChange) const;
  [[nodiscard]] bool ResolveVertexRef(const BRepGraph_RefUID& theUID,
                                      VertexRefChange&        theChange) const;
  [[nodiscard]] bool ResolveVertexRefLocal(const uint32_t   theIndex,
                                           VertexRefChange& theChange) const;
  [[nodiscard]] bool ResolveCoEdge(const BRepGraph_UID& theUID, CoEdgeChange& theChange) const;
  [[nodiscard]] bool ResolveCoEdgeLocal(const uint32_t theIndex, CoEdgeChange& theChange) const;
  [[nodiscard]] bool ResolveWire(const BRepGraph_UID& theUID, WireChange& theChange) const;
  [[nodiscard]] bool ResolveWireLocal(const uint32_t theIndex, WireChange& theChange) const;
  [[nodiscard]] bool ResolveFace(const BRepGraph_UID& theUID, FaceChange& theChange) const;
  [[nodiscard]] bool ResolveFaceLocal(const uint32_t theIndex, FaceChange& theChange) const;
  [[nodiscard]] bool ResolveWireRef(const BRepGraph_RefUID& theUID, WireRefChange& theChange) const;
  [[nodiscard]] bool ResolveWireRefLocal(const uint32_t theIndex, WireRefChange& theChange) const;

  //! Return native record counts and durable allocation counters.
  [[nodiscard]] uint32_t VertexCount() const noexcept;
  [[nodiscard]] uint32_t NextVertexCounter() const noexcept;
  [[nodiscard]] uint32_t EdgeCount() const noexcept;
  [[nodiscard]] uint32_t NextEdgeCounter() const noexcept;
  [[nodiscard]] uint32_t VertexRefCount() const noexcept;
  [[nodiscard]] uint32_t NextVertexRefCounter() const noexcept;
  [[nodiscard]] uint32_t CoEdgeCount() const noexcept;
  [[nodiscard]] uint32_t NextCoEdgeCounter() const noexcept;
  [[nodiscard]] uint32_t WireCount() const noexcept;
  [[nodiscard]] uint32_t NextWireCounter() const noexcept;
  [[nodiscard]] uint32_t FaceCount() const noexcept;
  [[nodiscard]] uint32_t NextFaceCounter() const noexcept;
  [[nodiscard]] uint32_t WireRefCount() const noexcept;
  [[nodiscard]] uint32_t NextWireRefCounter() const noexcept;

  std::shared_ptr<const BRepGraph>         myCoreGraph;
  mutable std::unique_ptr<BRepGraph>       myCachedGraph;
  mutable std::once_flag                   myGraphOnce;
  mutable std::once_flag                   myHashOnce;
  mutable std::shared_ptr<const HashState> myHashState;
  uint32_t                                 mySchemaVersion       = THE_SCHEMA_VERSION;
  bool                                     mySupportsSparseEdits = false;
  NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>> myComponents;
  NCollection_LinearVector<ComponentState>                           myComponentStates;
  NCollection_FlatDataMap<Standard_GUID, uint32_t>                   myComponentIndices;

public:
  DEFINE_STANDARD_RTTIEXT(BRepGraph_Revision, Standard_Transient)
};

#endif // _BRepGraph_Revision_HeaderFile
