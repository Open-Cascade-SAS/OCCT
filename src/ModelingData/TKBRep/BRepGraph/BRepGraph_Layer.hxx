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

#ifndef _BRepGraph_Layer_HeaderFile
#define _BRepGraph_Layer_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RevisionComponent.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

#include <cstdint>
#include <memory>

class BRepGraph_LayerRegistry;
class BRepGraph_LayerSupplementRegistry;

//! @brief Abstract base class for named attribute layers.
//!
//! A layer groups per-node and per-reference metadata under a unique name with
//! lifecycle callbacks. Layers are registered on BRepGraph and automatically
//! notified when nodes or references are removed, remapped (compact), or modified.
//!
//! Derived layers store domain-specific data (names, colors, materials, etc.)
//! in internal maps keyed by BRepGraph_NodeId or BRepGraph_RefId. The lifecycle
//! callbacks ensure data consistency across all graph mutations.
//!
//! ## Node Modification Events
//! Layers subscribe to node modification events by overriding SubscribedKinds()
//! to return a non-zero bitmask of Kind values. When a subscribed node kind is
//! modified, OnNodeModified() (immediate mode) or OnNodesModified() (deferred
//! batch mode) is called. Layers with SubscribedKinds() == 0 (default) incur
//! zero dispatch overhead.
//!
//! ## Reference Modification Events
//! Layers subscribe to reference modification events by overriding
//! SubscribedRefKinds() to return a non-zero bitmask of BRepGraph_RefId::Kind
//! values. When a subscribed ref kind is mutated, OnRefModified() (immediate
//! mode) or OnRefsModified() (deferred batch mode) is called. Removal is always
//! dispatched via OnRefRemoved() regardless of subscription.
//!
//! ## Thread safety
//! Callback dispatch is single-threaded (called from mutation paths).
//! Layers that only provide read access can skip internal locking.
//!
//! @warning All lifecycle callbacks are declared noexcept. Derived
//! implementations that throw will cause std::terminate. This is enforced
//! by C++ language semantics for noexcept virtual overrides.
class BRepGraph_Layer : public Standard_Transient
{
public:
  //! Layer type identity (unique within a graph).
  [[nodiscard]] virtual const Standard_GUID& ID() const = 0;

  //! Layer identity (unique within a graph).
  [[nodiscard]] virtual const TCollection_AsciiString& Name() const = 0;

  //! Describe how this layer participates in immutable revisions.
  //! The default retention is unspecified, so revision creation rejects an
  //! unintegrated layer instead of silently omitting its data.
  [[nodiscard]] Standard_EXPORT virtual BRepGraph_RevisionComponent::ComponentDescriptor
    RevisionDescriptor() const;

  //! Capture detached immutable revision for this persistent layer.
  //! Transient layers return a null handle.
  [[nodiscard]] Standard_EXPORT virtual occ::handle<BRepGraph_RevisionComponent> CaptureRevision(
    const BRepGraph&                       theGraph,
    BRepGraph_RevisionStatus::Diagnostics& theDiagnostics) const;

  //! Called when a node is soft-removed without a replacement.
  //! @param[in] theNode the removed node
  //!            Layers should discard or archive data associated with it.
  //! @warning Layer callbacks must not throw. They are called from noexcept
  //! notification paths (MutGuard destructors, deferred invalidation flush).
  Standard_EXPORT virtual void OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept;

  //! Dispatch a generic item removal to the matching typed removal callback.
  //! This is a non-virtual convenience entry point; typed callbacks remain the
  //! extension points for derived layers.
  //! @param[in] theItem the removed definition or reference
  Standard_EXPORT virtual void OnItemRemoved(const BRepGraph_ItemId theItem) noexcept;

  //! Called when a node is soft-removed and replaced by another node.
  //! @param[in] theOldNode the removed node
  //! @param[in] theNewNode the node that replaces theOldNode
  //!            Layers that store node-keyed data should migrate from
  //!            theOldNode to theNewNode when the replacement kind is
  //!            compatible. This is a structural lifecycle event, not an
  //!            algorithmic history record.
  //! @warning Layer callbacks must not throw. They are called from noexcept
  //! notification paths (MutGuard destructors, deferred invalidation flush).
  Standard_EXPORT virtual void OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                              const BRepGraph_NodeId theNewNode) noexcept;

  //! Copy this source layer data into another graph.
  //! The source graph is the graph this layer is attached to (Graph()).
  //! @param[in] theCopy source graph, target graph, and source item id -> target item id remap
  //! @note Missing source items were not copied; persistent layers should skip dependent records.
  //! @note For BRepGraph_CopyRemap::Mode::Compact, the layer is being migrated in-place after
  //! structural compaction. UID/ItemUID records and ref/rep entries should be remapped through
  //! the item map. Stale entries (absent from the remap) should be dropped.
  //! @warning This callback may allocate and is intentionally not noexcept.
  Standard_EXPORT virtual void CopyTo(const BRepGraph_CopyRemap& theCopy) const = 0;

  //! Mark all cached values dirty (bulk invalidation).
  virtual void InvalidateAll() noexcept = 0;

  //! Clear all stored data.
  virtual void Clear() noexcept = 0;

  //! Return a bitmask of BRepGraph_NodeId::Kind values this layer subscribes to.
  //! Only modification events matching subscribed kinds are dispatched.
  //! Default: 0 (no subscription - no modification events received).
  //! Override to receive OnNodeModified/OnNodesModified callbacks.
  //! The returned value must be constant for the lifetime of the layer.
  [[nodiscard]] Standard_EXPORT virtual int SubscribedKinds() const;

  //! Called in immediate (non-deferred) mode after a single node is modified.
  //! Only dispatched if the node's kind matches SubscribedKinds().
  //! Default: no-op.
  //! @param[in] theNode the modified node
  Standard_EXPORT virtual void OnNodeModified(const BRepGraph_NodeId theNode) noexcept;

  //! Dispatch a generic item modification to the matching typed modification callback.
  //! This is a non-virtual convenience entry point; typed callbacks remain the
  //! extension points for derived layers.
  //! @param[in] theItem the modified definition or reference
  Standard_EXPORT void OnItemModified(const BRepGraph_ItemId theItem) noexcept;

  //! Called after EndDeferredInvalidation() with all nodes modified during
  //! the deferred scope. Only dispatched if at least one modified node's kind
  //! matches SubscribedKinds(). The array may contain nodes of kinds not
  //! subscribed to - layers should filter internally if needed.
  //! Default: no-op.
  //! @param[in] theModifiedNodes all modified, non-removed nodes
  Standard_EXPORT virtual void OnNodesModified(
    const NCollection_Array1<BRepGraph_NodeId>& theModifiedNodes) noexcept;

  //! Convenience: return bitmask bit for a given Kind.
  static int KindBit(const BRepGraph_NodeId::Kind theKind)
  {
    return 1 << static_cast<int>(theKind);
  }

  //! Return a bitmask of BRepGraph_RefId::Kind values this layer subscribes to.
  //! Only modification events matching subscribed ref kinds are dispatched.
  //! Default: 0 (no subscription). Must be constant for the layer's lifetime.
  [[nodiscard]] Standard_EXPORT virtual int SubscribedRefKinds() const;

  //! Called when a reference is soft-deleted via RemoveRef().
  //! No replacement concept - refs are simply removed (unlike nodes which can have
  //! a replacement during sewing or deduplication). Dispatched to all layers
  //! regardless of SubscribedRefKinds().
  //! Default: no-op.
  //! @param[in] theRef the removed reference
  Standard_EXPORT virtual void OnRefRemoved(const BRepGraph_RefId theRef) noexcept;

  //! Called in immediate (non-deferred) mode after a single ref is mutated.
  //! Only dispatched if the ref's kind matches SubscribedRefKinds().
  //! Default: no-op.
  //! @param[in] theRef the modified reference
  Standard_EXPORT virtual void OnRefModified(const BRepGraph_RefId theRef) noexcept;

  //! Called after EndDeferredInvalidation() with all refs modified during
  //! the deferred scope. Only dispatched if at least one modified ref's kind
  //! matches SubscribedRefKinds(). The array may contain refs of kinds not
  //! subscribed to - layers should filter internally if needed.
  //! Default: no-op.
  //! @param[in] theModifiedRefs all modified, non-removed refs
  Standard_EXPORT virtual void OnRefsModified(
    const NCollection_Array1<BRepGraph_RefId>& theModifiedRefs) noexcept;

  //! Convenience: return bitmask bit for a given RefId::Kind.
  static int RefKindBit(const BRepGraph_RefId::Kind theKind)
  {
    return 1 << static_cast<int>(theKind);
  }

  //! Monotonic generation counter incremented by touch() on every observable
  //! layer change. Consumers compare stored generations to detect staleness in O(1).
  //! Derived layers MUST call touch() from their mutators.
  [[nodiscard]] uint64_t Generation() const noexcept { return myGeneration; }

protected:
  Standard_EXPORT BRepGraph_Layer();

  //! Bump the generation counter.
  void touch() noexcept { ++myGeneration; }

  //! True while this layer is registered in a live graph registry.
  [[nodiscard]] bool IsAttached() const noexcept { return myGraph != nullptr; }

  //! Attached graph for read-only layer services. Raises Standard_ProgramError if detached.
  [[nodiscard]] Standard_EXPORT const BRepGraph& Graph() const;

  //! Attached mutable graph for graph-owned service layers. Returns null if detached.
  [[nodiscard]] BRepGraph* AttachedGraph() const noexcept { return myGraph; }

  //! Return the target generic node only when it remains a node of the same kind.
  [[nodiscard]] static BRepGraph_NodeId RemappedItem(const BRepGraph_CopyRemap& theCopy,
                                                     const BRepGraph_NodeId     theId)
  {
    if (!theId.IsValid())
    {
      return BRepGraph_NodeId();
    }
    const BRepGraph_ItemId aMapped = theCopy.TargetItem(BRepGraph_ItemId(theId));
    if (!aMapped.IsNode())
    {
      return BRepGraph_NodeId();
    }
    const BRepGraph_NodeId aNode = aMapped.NodeId();
    return aNode.NodeKind == theId.NodeKind ? aNode : BRepGraph_NodeId();
  }

  //! Return the target generic reference only when it remains a ref of the same kind.
  [[nodiscard]] static BRepGraph_RefId RemappedItem(const BRepGraph_CopyRemap& theCopy,
                                                    const BRepGraph_RefId      theId)
  {
    if (!theId.IsValid())
    {
      return BRepGraph_RefId();
    }
    const BRepGraph_ItemId aMapped = theCopy.TargetItem(BRepGraph_ItemId(theId));
    if (!aMapped.IsReference())
    {
      return BRepGraph_RefId();
    }
    const BRepGraph_RefId aRef = aMapped.RefId();
    return aRef.RefKind == theId.RefKind ? aRef : BRepGraph_RefId();
  }

  template <BRepGraph_NodeId::Kind TheKind>
  [[nodiscard]] static BRepGraph_NodeId::Typed<TheKind> RemappedItem(
    const BRepGraph_CopyRemap&             theCopy,
    const BRepGraph_NodeId::Typed<TheKind> theId)
  {
    if (!theId.IsValid())
    {
      return BRepGraph_NodeId::Typed<TheKind>();
    }
    const BRepGraph_ItemId aMapped = theCopy.TargetItem(BRepGraph_ItemId(theId));
    if (!aMapped.IsNode())
    {
      return BRepGraph_NodeId::Typed<TheKind>();
    }
    return BRepGraph_NodeId::Typed<TheKind>::FromNodeId(aMapped.NodeId());
  }

  template <BRepGraph_RefId::Kind TheKind>
  [[nodiscard]] static BRepGraph_RefId::Typed<TheKind> RemappedItem(
    const BRepGraph_CopyRemap&            theCopy,
    const BRepGraph_RefId::Typed<TheKind> theId)
  {
    if (!theId.IsValid())
    {
      return BRepGraph_RefId::Typed<TheKind>();
    }
    const BRepGraph_ItemId aMapped = theCopy.TargetItem(BRepGraph_ItemId(theId));
    if (!aMapped.IsReference())
    {
      return BRepGraph_RefId::Typed<TheKind>();
    }
    return BRepGraph_RefId::Typed<TheKind>::FromRefId(aMapped.RefId());
  }

  //! Called after the layer is attached to a graph registry.
  Standard_EXPORT virtual void OnAttached() noexcept;

  //! Called before the layer is detached from a graph registry.
  Standard_EXPORT virtual void OnDetached() noexcept;

private:
  friend class ::BRepGraph_LayerRegistry;
  friend class ::BRepGraph_LayerSupplementRegistry;

  Standard_EXPORT void attachGraph(BRepGraph* theGraph) noexcept;

  //! Update graph owner after wrapper relocation without lifecycle callbacks.
  void relocateGraph(BRepGraph* theGraph) noexcept { myGraph = theGraph; }

  Standard_EXPORT void detachContext() noexcept;

  BRepGraph* myGraph      = nullptr;
  uint64_t   myGeneration = 0;

public:
  DEFINE_STANDARD_RTTIEXT(BRepGraph_Layer, Standard_Transient)
};

#endif // _BRepGraph_Layer_HeaderFile
