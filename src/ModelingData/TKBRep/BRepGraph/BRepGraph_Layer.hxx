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

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DynamicArray.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

#include <cstdint>

class BRepGraph;
class BRepGraph_LayerRegistry;

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

  //! Called when a node is soft-removed.
  //! @param[in] theNode        the removed node
  //! @param[in] theReplacement if valid, the node that replaces theNode
  //!            (e.g., sewing edge merge, deduplicate). If invalid, pure deletion.
  //!            Layers should migrate data from theNode to theReplacement when valid,
  //!            otherwise discard or archive removed-node data.
  //!            Implementations must validate theReplacement before dereferencing
  //!            graph data through it.
  //! @warning Layer callbacks must not throw. They are called from noexcept
  //! notification paths (MutGuard destructors, deferred invalidation flush).
  virtual void OnNodeRemoved(const BRepGraph_NodeId theNode,
                             const BRepGraph_NodeId theReplacement) noexcept = 0;

  //! Called after Compact with a unified old->new remap map.
  //! Layer must remap all internal NodeId references using this map.
  //! The map covers all node kinds (Vertex through CompSolid and future extensions).
  //! Nodes absent from the map were removed during compaction - layers should
  //! drop data associated with those nodes.
  //! @param[in] theRemapMap maps old NodeId to new NodeId for all surviving nodes
  virtual void OnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept = 0;

  //! Mark all cached values dirty (bulk invalidation).
  virtual void InvalidateAll() noexcept = 0;

  //! Clear all stored data.
  virtual void Clear() noexcept = 0;

  // --- Modification event subscription ---

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

  //! Called after EndDeferredInvalidation() with all nodes modified during
  //! the deferred scope. Only dispatched if at least one modified node's kind
  //! matches SubscribedKinds(). The vector may contain nodes of kinds not
  //! subscribed to - layers should filter internally if needed.
  //! Default: no-op.
  //! @param[in] theModifiedNodes all modified, non-removed nodes
  Standard_EXPORT virtual void OnNodesModified(
    const NCollection_DynamicArray<BRepGraph_NodeId>& theModifiedNodes) noexcept;

  //! Convenience: return bitmask bit for a given Kind.
  static int KindBit(const BRepGraph_NodeId::Kind theKind)
  {
    return 1 << static_cast<int>(theKind);
  }

  // --- Reference modification event subscription ---

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
  //! matches SubscribedRefKinds(). The vector may contain refs of kinds not
  //! subscribed to - layers should filter internally if needed.
  //! Default: no-op.
  //! @param[in] theModifiedRefs     all modified, non-removed refs
  //! @param[in] theModifiedRefKindsMask bitwise OR of all modified ref kinds
  Standard_EXPORT virtual void OnRefsModified(
    const NCollection_DynamicArray<BRepGraph_RefId>& theModifiedRefs,
    const int                                  theModifiedRefKindsMask) noexcept;

  //! Convenience: return bitmask bit for a given RefId::Kind.
  static int RefKindBit(const BRepGraph_RefId::Kind theKind)
  {
    return 1 << static_cast<int>(theKind);
  }

  // --- Revision + owning-graph access ---

  //! Monotonic revision counter incremented by touch() on every observable
  //! state change. Consumers compare stored revisions to detect staleness in O(1).
  //! Derived layers MUST call touch() from their mutators.
  [[nodiscard]] uint64_t Revision() const noexcept { return myRevision; }

  //! Owning graph, set by the registry on RegisterLayer() and cleared on Unregister().
  //! Nullptr before registration or after unregistration.
  [[nodiscard]] const BRepGraph* OwningGraph() const noexcept { return myOwningGraph; }

  //! Mutable accessor for layers that drive graph mutations (e.g. meshing).
  [[nodiscard]] BRepGraph* OwningMutableGraph() const noexcept
  {
    return const_cast<BRepGraph*>(myOwningGraph);
  }

protected:
  //! Bump the revision counter.
  void touch() noexcept { ++myRevision; }

private:
  friend class ::BRepGraph_LayerRegistry;

  void setOwningGraph(const BRepGraph* theGraph) noexcept { myOwningGraph = theGraph; }

  const BRepGraph* myOwningGraph = nullptr;
  uint64_t         myRevision    = 0;

public:
  DEFINE_STANDARD_RTTIEXT(BRepGraph_Layer, Standard_Transient)
};

#endif // _BRepGraph_Layer_HeaderFile
